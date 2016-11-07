/*****************************************************************************/
// File: image_out.cpp [scope = APPS/IMAGE-IO]
// Version: Kakadu, V2.2.1
// Author: David Taubman
// Last Revised: 5 July, 2001
/*****************************************************************************/
// Copyright 2001, David Taubman, The University of New South Wales (UNSW)
// The copyright owner is Unisearch Ltd, Australia (commercial arm of UNSW)
// Neither this copyright statement, nor the licensing details below
// may be removed from this file or dissociated from its contents.
/*****************************************************************************/
// Licensee: Book Owner
// License number: 99999
// The Licensee has been granted a NON-COMMERCIAL license to the contents of
// this source file, said Licensee being the owner of a copy of the book,
// "JPEG2000: Image Compression Fundamentals, Standards and Practice," by
// Taubman and Marcellin (Kluwer Academic Publishers, 2001).  A brief summary
// of the license appears below.  This summary is not to be relied upon in
// preference to the full text of the license agreement, which was accepted
// upon breaking the seal of the compact disc accompanying the above-mentioned
// book.
// 1. The Licensee has the right to Non-Commercial Use of the Kakadu software,
//    Version 2.2, including distribution of one or more Applications built
//    using the software, provided such distribution is not for financial
//    return.
// 2. The Licensee has the right to personal use of the Kakadu software,
//    Version 2.2.
// 3. The Licensee has the right to distribute Reusable Code (including
//    source code and dynamically or statically linked libraries) to a Third
//    Party, provided the Third Party possesses a license to use the Kakadu
//    software, Version 2.2, and provided such distribution is not for
//    financial return.
/******************************************************************************
Description:
   Implements image file writing for a variety of different file formats:
currently BMP, PGM, PPM and RAW only.  Readily extendible to include other file
formats without affecting the rest of the system.
******************************************************************************/

// System includes
#include <string.h>
#include <math.h>
#include <assert.h>
// Core includes
#include "kdu_messaging.h"
#include "kdu_sample_processing.h"
// Image includes
#include "kdu_image.h"
#include "image_local.h"

/* ========================================================================= */
/*                             Internal Functions                            */
/* ========================================================================= */

static void
  to_little_endian(kdu_int32 * words, int num_words)
{
  kdu_int32 test = 1;
  kdu_byte *first_byte = (kdu_byte *) &test;
  if (*first_byte)
    return; // Machine uses little-endian architecture already.
  kdu_int32 tmp;
  for (; num_words--; words++)
    {
      tmp = *words;
      *words = ((tmp >> 24) & 0x000000FF) +
               ((tmp >> 8)  & 0x0000FF00) +
               ((tmp << 8)  & 0x00FF0000) +
               ((tmp << 24) & 0xFF000000);
    }
}

/*****************************************************************************/
/* INLINE                      from_little_endian                            */
/*****************************************************************************/

static inline void
  from_little_endian(kdu_int32 * words, int num_words)
{
  to_little_endian(words,num_words);
}

/*****************************************************************************/
/* STATIC                    convert_floats_to_bytes                         */
/*****************************************************************************/

static void
  convert_floats_to_bytes(kdu_sample32 *src, kdu_byte *dest, int num,
                          int precision, int sample_gap=1)
{
  float scale16 = (float)(1<<16);
  kdu_int32 val;

  if (precision >= 8)
    {
      for (; num > 0; num--, src++, dest+=sample_gap)
        {
          val = (kdu_int32)(src->fval*scale16);
          val = (val+128)>>8; // Often faster than true rounding from floats.
          val += 128;
          if (val & ((-1)<<8))
            val = (val<0)?0:255;
          *dest = (kdu_byte) val;
        }
    }
  else
    { // Need to force zeros into one or more least significant bits.
      kdu_int32 downshift = 16-precision;
      kdu_int32 upshift = 8-precision;
      kdu_int32 offset = 1<<(downshift-1);

      for (; num > 0; num--, src++, dest+=sample_gap)
        {
          val = (kdu_int32)(src->fval*scale16);
          val = (val+offset)>>downshift;
          val <<= upshift;
          val += 128;
          if (val & ((-1)<<8))
            val = (val<0)?0:(256-(1<<upshift));
          *dest = (kdu_byte) val;
        }
    }
}

/*****************************************************************************/
/* STATIC                   convert_fixpoint_to_bytes                        */
/*****************************************************************************/

static void
  convert_fixpoint_to_bytes(kdu_sample16 *src, kdu_byte *dest, int num,
                            int precision, int sample_gap=1)
{
  kdu_int16 val;

  if (precision >= 8)
    {
      kdu_int16 downshift = KDU_FIX_POINT-8;
      if (downshift < 0)
        { kdu_error e; e << "Cannot use 16-bit representation with high "
          "bit-depth data"; }
      kdu_int16 offset = (1<<downshift)>>1;
      for (; num > 0; num--, src++, dest+=sample_gap)
        {
          val = src->ival;
          val = (val + offset) >> (KDU_FIX_POINT-8);
          val += 128;
          if (val & ((-1)<<8))
            val = (val<0)?0:255;
          *dest = (kdu_byte) val;
        }
    }
  else
    { // Need to force zeros into one or more least significant bits.
      kdu_int16 downshift = KDU_FIX_POINT-precision;
      if (downshift < 0)
        { kdu_error e; e << "Cannot use 16-bit representation with high "
          "bit-depth data"; }
      kdu_int16 upshift = 8-precision;
      kdu_int16 offset = (1<<downshift)>>1;

      for (; num > 0; num--, src++, dest+=sample_gap)
        {
          val = src->ival;
          val = (val+offset)>>downshift;
          val <<= upshift;
          val += 128;
          if (val & ((-1)<<8))
            val = (val<0)?0:(256-(1<<upshift));
          *dest = (kdu_byte) val;
        }
    }
}

/*****************************************************************************/
/* STATIC                     convert_ints_to_bytes                          */
/*****************************************************************************/

static void
  convert_ints_to_bytes(kdu_sample32 *src, kdu_byte *dest, int num,
                        int precision, int sample_gap=1)
{
  kdu_int32 val;

  if (precision >= 8)
    {
      kdu_int32 downshift = precision-8;
      kdu_int32 offset = (1<<downshift)>>1;

      for (; num > 0; num--, src++, dest+=sample_gap)
        {
          val = src->ival;
          val = (val+offset)>>downshift;
          val += 128;
          if (val & ((-1)<<8))
            val = (val<0)?0:255;
          *dest = (kdu_byte) val;
        }
    }
  else
    {
      kdu_int32 upshift = 8-precision;

      for (; num > 0; num--, src++, dest+=sample_gap)
        {
          val = src->ival;
          val <<= upshift;
          val += 128;
          if (val & ((-1)<<8))
            val = (val<0)?0:(256-(1<<upshift));
          *dest = (kdu_byte) val;
        }
    }
}

/*****************************************************************************/
/* STATIC                    convert_shorts_to_bytes                         */
/*****************************************************************************/

static void
  convert_shorts_to_bytes(kdu_sample16 *src, kdu_byte *dest, int num,
                          int precision, int sample_gap=1)
{
  kdu_int16 val;

  if (precision >= 8)
    {
      kdu_int16 downshift = precision-8;
      kdu_int16 offset = (1<<downshift)>>1;

      for (; num > 0; num--, src++, dest+=sample_gap)
        {
          val = src->ival;
          val = (val+offset)>>downshift;
          val += 128;
          if (val & ((-1)<<8))
            val = (val<0)?0:255;
          *dest = (kdu_byte) val;
        }
    }
  else
    {
      kdu_int16 upshift = 8-precision;

      for (; num > 0; num--, src++, dest+=sample_gap)
        {
          val = src->ival;
          val <<= upshift;
          val += 128;
          if (val & ((-1)<<8))
            val = (val<0)?0:(256-(1<<upshift));
          *dest = (kdu_byte) val;
        }
    }
}

/*****************************************************************************/
/* STATIC                    convert_floats_to_words                         */
/*****************************************************************************/

static void
  convert_floats_to_words(kdu_sample32 *src, kdu_byte *dest, int num,
                          int precision, bool is_signed, int sample_bytes)
{
  kdu_int32 val;
  float scale, min, max, fval, offset;

  if (precision < 30)
    scale = (float)(1<<precision);
  else
    scale = ((float)(1<<30)) * ((float)(1<<(precision-30)));
  min = -0.5F;
  max = 0.5F - 1.0F/scale;
  offset = (is_signed)?0.0F:0.5F;
  offset = offset * scale + 0.5F;

  if (sample_bytes == 1)
    for (; num > 0; num--, src++)
      {
        fval = src->fval;
        fval = (fval>min)?fval:min;
        fval = (fval<max)?fval:max;
        val = (kdu_int32) floor(scale*fval + offset);
        *(dest++) = (kdu_byte) val;
      }
  else if (sample_bytes == 2)
    for (; num > 0; num--, src++)
      {
        fval = src->fval;
        fval = (fval>min)?fval:min;
        fval = (fval<max)?fval:max;
        val = (kdu_int32) floor(scale*fval + offset);
        *(dest++) = (kdu_byte)(val>>8);
        *(dest++) = (kdu_byte) val;
      }
  else if (sample_bytes == 3)
    for (; num > 0; num--, src++)
      {
        fval = src->fval;
        fval = (fval>min)?fval:min;
        fval = (fval<max)?fval:max;
        val = (kdu_int32) floor(scale*fval + offset);
        *(dest++) = (kdu_byte)(val>>16);
        *(dest++) = (kdu_byte)(val>>8);
        *(dest++) = (kdu_byte) val;
      }
  else if (sample_bytes == 4)
    for (; num > 0; num--, src++)
      {
        fval = src->fval;
        fval = (fval>min)?fval:min;
        fval = (fval<max)?fval:max;
        val = (kdu_int32) floor(scale*fval + offset);
        *(dest++) = (kdu_byte)(val>>24);
        *(dest++) = (kdu_byte)(val>>16);
        *(dest++) = (kdu_byte)(val>>8);
        *(dest++) = (kdu_byte) val;
      }
  else
    assert(0);
}

/*****************************************************************************/
/* STATIC                   convert_fixpoint_to_words                        */
/*****************************************************************************/

static void
  convert_fixpoint_to_words(kdu_sample16 *src, kdu_byte *dest, int num,
                            int precision, bool is_signed, int sample_bytes)
{
  kdu_int32 val;
  kdu_int32 min, max;
  kdu_int32 downshift = KDU_FIX_POINT-precision;
  if (downshift < 0)
    { kdu_error e; e << "Cannot use 16-bit representation with high "
      "bit-depth data"; }
  kdu_int32 offset = 1<<downshift;
  offset += (is_signed)?0:(1<<(precision+downshift));
  offset >>= 1;
  max = (1<<(KDU_FIX_POINT))>>1;
  min = -max;
  max -= 1<<downshift;

  if (sample_bytes == 1)
    for (; num > 0; num--, src++)
      {
        val = src->ival;
        val = (val < min)?min:val;
        val = (val >= max)?max:val;
        val = (val+offset)>>downshift;
        *(dest++) = (kdu_byte) val;
      }
  else if (sample_bytes == 2)
    for (; num > 0; num--, src++)
      {
        val = src->ival;
        val = (val < min)?min:val;
        val = (val >= max)?max:val;
        val = (val+offset)>>downshift;
        *(dest++) = (kdu_byte)(val>>8);
        *(dest++) = (kdu_byte) val;
      }
  else
    { kdu_error e; e << "Cannot use 16-bit representation with high "
      "bit-depth data"; }
}

/*****************************************************************************/
/* STATIC                     convert_ints_to_words                         */
/*****************************************************************************/

static void
  convert_ints_to_words(kdu_sample32 *src, kdu_byte *dest, int num,
                        int precision, bool is_signed, int sample_bytes)
{
  kdu_int32 val, min, max, offset;

  offset = 1<<(precision-1);
  min = -offset;
  max = (1<<precision)-1-offset;
  offset = (is_signed)?0:offset;

  if (sample_bytes == 1)
    for (; num > 0; num--, src++)
      {
        val = src->ival;
        val = (val>min)?val:min;
        val = (val<max)?val:max;
        val += offset;
        *(dest++) = (kdu_byte) val;
      }
  else if (sample_bytes == 2)
    for (; num > 0; num--, src++)
      {
        val = src->ival;
        val = (val>min)?val:min;
        val = (val<max)?val:max;
        val += offset;
        *(dest++) = (kdu_byte)(val>>8);
        *(dest++) = (kdu_byte) val;
      }
  else if (sample_bytes == 3)
    for (; num > 0; num--, src++)
      {
        val = src->ival;
        val = (val>min)?val:min;
        val = (val<max)?val:max;
        val += offset;
        *(dest++) = (kdu_byte)(val>>16);
        *(dest++) = (kdu_byte)(val>>8);
        *(dest++) = (kdu_byte) val;
      }
  else if (sample_bytes == 4)
    for (; num > 0; num--, src++)
      {
        val = src->ival;
        val = (val>min)?val:min;
        val = (val<max)?val:max;
        val += offset;
        *(dest++) = (kdu_byte)(val>>24);
        *(dest++) = (kdu_byte)(val>>16);
        *(dest++) = (kdu_byte)(val>>8);
        *(dest++) = (kdu_byte) val;
      }
  else
    assert(0);
}

/*****************************************************************************/
/* STATIC                    convert_shorts_to_words                         */
/*****************************************************************************/

static void
  convert_shorts_to_words(kdu_sample16 *src, kdu_byte *dest, int num,
                          int precision, bool is_signed, int sample_bytes)
{
  kdu_int32 val, min, max, offset;

  offset = 1<<(precision-1);
  min = -offset;
  max = (1<<precision)-1-offset;
  offset = (is_signed)?0:offset;

  if (sample_bytes == 1)
    for (; num > 0; num--, src++)
      {
        val = src->ival;
        val = (val>min)?val:min;
        val = (val<max)?val:max;
        val += offset;
        *(dest++) = (kdu_byte) val;
      }
  else if (sample_bytes == 2)
    for (; num > 0; num--, src++)
      {
        val = src->ival;
        val = (val>min)?val:min;
        val = (val<max)?val:max;
        val += offset;
        *(dest++) = (kdu_byte)(val>>8);
        *(dest++) = (kdu_byte) val;
      }
  else
    { kdu_error e; e << "Cannot use 16-bit representation with high "
      "bit-depth data"; }
}


/* ========================================================================= */
/*                                kdu_image_out                              */
/* ========================================================================= */

/*****************************************************************************/
/*                        kdu_image_out::kdu_image_out                       */
/*****************************************************************************/

kdu_image_out::kdu_image_out(char const *fname, kdu_image_dims &dims,
                             int &next_comp_idx, bool &vflip)
{
  char const *suffix;

  out = NULL;
  vflip = false;
  if ((suffix = strrchr(fname,'.')) != NULL)
    {
      if ((strcmp(suffix+1,"pgm") == 0) || (strcmp(suffix+1,"PGM") == 0))
        out = new pgm_out(fname,dims,next_comp_idx);
      else if ((strcmp(suffix+1,"ppm") == 0) || (strcmp(suffix+1,"PPM") == 0))
        out = new ppm_out(fname,dims,next_comp_idx);
      else if ((strcmp(suffix+1,"bmp") == 0) || (strcmp(suffix+1,"BMP") == 0))
        { vflip = true; out = new bmp_out(fname,dims,next_comp_idx); }
      else if ((strcmp(suffix+1,"raw") == 0) || (strcmp(suffix+1,"RAW") == 0))
        out = new raw_out(fname,dims,next_comp_idx);
    }
  if (out == NULL)
    { kdu_error e; e << "Image file, \"" << fname << ", does not have a "
      "recognized suffix.  Valid suffices are currently: "
      "\"bmp\", \"pgm\", \"ppm\" and \"raw\".  "
      "Upper or lower case may be used, but must be used consistently.";
    }
}


/* ========================================================================= */
/*                                  pgm_out                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                              pgm_out::pgm_out                             */
/*****************************************************************************/

pgm_out::pgm_out(char const *fname, kdu_image_dims &dims, int &next_comp_idx)
{
  int is_signed;

  comp_idx = next_comp_idx++;
  if (comp_idx >= dims.get_num_components())
    { kdu_error e; e << "Output image files require more image components "
      "(or mapped colour channels) than are available!"; }
  rows = dims.get_height(comp_idx);
  cols = dims.get_width(comp_idx);
  precision = dims.get_bit_depth(comp_idx);
  is_signed = dims.get_signed(comp_idx);
  if (is_signed)
    { kdu_warning w;
      w << "Signed sample values will be written to the PGM file as unsigned "
           "8-bit quantities, centered about 128.";
    }
  if ((out = fopen(fname,"wb")) == NULL)
    { kdu_error e;
      e << "Unable to open output image file, \"" << fname <<"\"."; }
  fprintf(out,"P5\n%d %d\n255\n",cols,rows);
  
  incomplete_lines = free_lines = NULL;
  num_unwritten_rows = rows;
}

/*****************************************************************************/
/*                              pgm_out::~pgm_out                            */
/*****************************************************************************/

pgm_out::~pgm_out()
{
  if ((num_unwritten_rows > 0) || (incomplete_lines != NULL))
    { kdu_warning w;
      w << "Not all rows of image component "
        << comp_idx << " were completed!";
    }
  image_line_buf *tmp;
  while ((tmp=incomplete_lines) != NULL)
    { incomplete_lines = tmp->next; delete tmp; }
  while ((tmp=free_lines) != NULL)
    { free_lines = tmp->next; delete tmp; }
  fclose(out);
}

/*****************************************************************************/
/*                                pgm_out::put                               */
/*****************************************************************************/

void
  pgm_out::put(int comp_idx, kdu_line_buf &line, int x_tnum)
{
  assert(comp_idx == this->comp_idx);
  image_line_buf *scan, *prev=NULL;
  for (scan=incomplete_lines; scan != NULL; prev=scan, scan=scan->next)
    {
      assert(scan->next_x_tnum >= x_tnum);
      if (scan->next_x_tnum == x_tnum)
        break;
    }
  if (scan == NULL)
    { // Need to open a new line buffer.
      assert(x_tnum == 0); // Must supply samples from left to right.
      if ((scan = free_lines) == NULL)
        scan = new image_line_buf(cols,1);
      free_lines = scan->next;
      if (prev == NULL)
        incomplete_lines = scan;
      else
        prev->next = scan;
      scan->accessed_samples = 0;
      scan->next_x_tnum = 0;
    }
  assert((scan->width-scan->accessed_samples) >= line.get_width());

  if (line.get_buf32() != NULL)
    {
      if (line.is_absolute())
        convert_ints_to_bytes(line.get_buf32(),
                              scan->buf+scan->accessed_samples,
                              line.get_width(),precision);
      else
        convert_floats_to_bytes(line.get_buf32(),
                                scan->buf+scan->accessed_samples,
                                line.get_width(),precision);
    }
  else
    {
      if (line.is_absolute())
        convert_shorts_to_bytes(line.get_buf16(),
                                scan->buf+scan->accessed_samples,
                                line.get_width(),precision);
      else
        convert_fixpoint_to_bytes(line.get_buf16(),
                                  scan->buf+scan->accessed_samples,
                                  line.get_width(),precision);
    }

  scan->next_x_tnum++;
  scan->accessed_samples += line.get_width();
  if (scan->accessed_samples == scan->width)
    { // Write completed line and send it to the free list.
      if (num_unwritten_rows == 0)
        { kdu_error e; e << "Attempting to write too many lines to image "
          "file for component " << comp_idx << "."; }
      if (fwrite(scan->buf,1,(size_t) scan->width,out) != (size_t) scan->width)
        { kdu_error e; e << "Unable to write to image file for component "
          << comp_idx
          << ". File may be write protected, or disk may be full."; }
      num_unwritten_rows--;
      assert(scan == incomplete_lines);
      incomplete_lines = scan->next;
      scan->next = free_lines;
      free_lines = scan;
    }
}


/* ========================================================================= */
/*                                  ppm_out                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                              ppm_out::ppm_out                             */
/*****************************************************************************/

ppm_out::ppm_out(char const *fname, kdu_image_dims &dims, int &next_comp_idx)
{
  bool is_signed;
  int n;

  first_comp_idx = next_comp_idx;
  if ((first_comp_idx+2) >= dims.get_num_components())
    { kdu_error e; e << "Output image files require more image components "
      "(or mapped colour channels) than are available!"; }
  rows = dims.get_height(first_comp_idx);
  cols = dims.get_width(first_comp_idx);
  is_signed = dims.get_signed(first_comp_idx);
  for (n=0; n < 3; n++, next_comp_idx++)
    {
      if ((rows != dims.get_height(next_comp_idx)) ||
          (cols != dims.get_width(next_comp_idx)) ||
          (is_signed != dims.get_signed(next_comp_idx)))
        { kdu_error e; e << "Can only write a PPM file with 3 image "
          "components, all having the same dimensions and the same "
          "signed/unsigned characteristics."; }
      precision[n] = dims.get_bit_depth(next_comp_idx);
    }
  if (is_signed)
    { kdu_warning w;
      w << "Signed sample values will be written to the "
           "PPM file as unsigned 8-bit quantities, centered about 128.";
    }
  if ((out = fopen(fname,"wb")) == NULL)
    { kdu_error e;
      e << "Unable to open output image file, \"" << fname <<"\"."; }
  fprintf(out,"P6\n%d %d\n255\n",cols,rows);

  incomplete_lines = NULL;
  free_lines = NULL;
  num_unwritten_rows = rows;
}

/*****************************************************************************/
/*                              ppm_out::~ppm_out                            */
/*****************************************************************************/

ppm_out::~ppm_out()
{
  if ((num_unwritten_rows > 0) || (incomplete_lines != NULL))
    { kdu_warning w;
      w << "Not all rows of image components "
        << first_comp_idx << " through " << first_comp_idx+2
        << " were completed!";
    }
  image_line_buf *tmp;
  while ((tmp=incomplete_lines) != NULL)
    { incomplete_lines = tmp->next; delete tmp; }
  while ((tmp=free_lines) != NULL)
    { free_lines = tmp->next; delete tmp; }
  fclose(out);
}

/*****************************************************************************/
/*                                ppm_out::put                               */
/*****************************************************************************/

void
  ppm_out::put(int comp_idx, kdu_line_buf &line, int x_tnum)
{
  int idx = comp_idx - this->first_comp_idx;
  assert((idx >= 0) && (idx <= 2));
  x_tnum = x_tnum*3+idx; // Works so long as components written in order.
  image_line_buf *scan, *prev=NULL;
  for (scan=incomplete_lines; scan != NULL; prev=scan, scan=scan->next)
    {
      assert(scan->next_x_tnum >= x_tnum);
      if (scan->next_x_tnum == x_tnum)
        break;
    }
  if (scan == NULL)
    { // Need to open a new line buffer
      assert(x_tnum == 0); // Must consume in very specific order.
      if ((scan = free_lines) == NULL)
        scan = new image_line_buf(cols,3);
      free_lines = scan->next;
      if (prev == NULL)
        incomplete_lines = scan;
      else
        prev->next = scan;
      scan->accessed_samples = 0;
      scan->next_x_tnum = 0;
    }

  assert((scan->width-scan->accessed_samples) >= line.get_width());

  if (line.get_buf32() != NULL)
    {
      if (line.is_absolute())
        convert_ints_to_bytes(line.get_buf32(),
                              scan->buf+3*scan->accessed_samples+idx,
                              line.get_width(),precision[idx],3);
      else
        convert_floats_to_bytes(line.get_buf32(),
                                scan->buf+3*scan->accessed_samples+idx,
                                line.get_width(),precision[idx],3);
    }
  else
    {
      if (line.is_absolute())
        convert_shorts_to_bytes(line.get_buf16(),
                                scan->buf+3*scan->accessed_samples+idx,
                                line.get_width(),precision[idx],3);
      else
        convert_fixpoint_to_bytes(line.get_buf16(),
                                  scan->buf+3*scan->accessed_samples+idx,
                                  line.get_width(),precision[idx],3);
    }

  scan->next_x_tnum++;
  if (idx == 2)
    scan->accessed_samples += line.get_width();
  if (scan->accessed_samples == scan->width)
    { // Write completed line and send it to the free list.
      if (num_unwritten_rows == 0)
        { kdu_error e; e << "Attempting to write too many lines to image "
          "file for components " << first_comp_idx << " through "
          << first_comp_idx+2 << "."; }
      if (fwrite(scan->buf,1,(size_t)(scan->width*3),out) !=
          (size_t)(scan->width*3))
        { kdu_error e; e << "Unable to write to image file for components "
          << first_comp_idx << " through " << first_comp_idx+2
          << ". File may be write protected, or disk may be full."; }
      num_unwritten_rows--;
      assert(scan == incomplete_lines);
      incomplete_lines = scan->next;
      scan->next = free_lines;
      free_lines = scan;
    }
}


/* ========================================================================= */
/*                                  raw_out                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                              raw_out::raw_out                             */
/*****************************************************************************/

raw_out::raw_out(char const *fname, kdu_image_dims &dims, int &next_comp_idx)
{
  comp_idx = next_comp_idx++;
  if (comp_idx >= dims.get_num_components())
    { kdu_error e; e << "Output image files require more image components "
      "(or mapped colour channels) than are available!"; }
  rows = dims.get_height(comp_idx);
  cols = dims.get_width(comp_idx);
  precision = dims.get_bit_depth(comp_idx);
  is_signed = dims.get_signed(comp_idx);
  sample_bytes = (precision+7)>>3;
  incomplete_lines = free_lines = NULL;
  num_unwritten_rows = rows;  
  if ((out = fopen(fname,"wb")) == NULL)
    { kdu_error e;
      e << "Unable to open output image file, \"" << fname <<"\".";}
}

/*****************************************************************************/
/*                              raw_out::~raw_out                            */
/*****************************************************************************/

raw_out::~raw_out()
{
  if ((num_unwritten_rows > 0) || (incomplete_lines != NULL))
    { kdu_warning w;
      w << "Not all rows of image component "
        << comp_idx << " were produced!";
    }
  image_line_buf *tmp;
  while ((tmp=incomplete_lines) != NULL)
    { incomplete_lines = tmp->next; delete tmp; }
  while ((tmp=free_lines) != NULL)
    { free_lines = tmp->next; delete tmp; }
  fclose(out);
}

/*****************************************************************************/
/*                                raw_out::put                               */
/*****************************************************************************/

void
  raw_out::put(int comp_idx, kdu_line_buf &line, int x_tnum)
{
  assert(comp_idx == this->comp_idx);
  image_line_buf *scan, *prev=NULL;
  for (scan=incomplete_lines; scan != NULL; prev=scan, scan=scan->next)
    {
      assert(scan->next_x_tnum >= x_tnum);
      if (scan->next_x_tnum == x_tnum)
        break;
    }
  if (scan == NULL)
    { // Need to open a new line buffer.
      assert(x_tnum == 0); // Must supply samples from left to right.
      if ((scan = free_lines) == NULL)
        scan = new image_line_buf(cols,sample_bytes);
      free_lines = scan->next;
      if (prev == NULL)
        incomplete_lines = scan;
      else
        prev->next = scan;
      scan->accessed_samples = 0;
      scan->next_x_tnum = 0;
    }
  assert((scan->width-scan->accessed_samples) >= line.get_width());

  if (line.get_buf32() != NULL)
    {
      if (line.is_absolute())
        convert_ints_to_words(line.get_buf32(),
                              scan->buf+sample_bytes*scan->accessed_samples,
                              line.get_width(),
                              precision,is_signed,sample_bytes);
      else
        convert_floats_to_words(line.get_buf32(),
                                scan->buf+sample_bytes*scan->accessed_samples,
                                line.get_width(),
                                precision,is_signed,sample_bytes);
    }
  else
    {
      if (line.is_absolute())
        convert_shorts_to_words(line.get_buf16(),
                                scan->buf+sample_bytes*scan->accessed_samples,
                                line.get_width(),
                                precision,is_signed,sample_bytes);
      else
        convert_fixpoint_to_words(line.get_buf16(),
                                 scan->buf+sample_bytes*scan->accessed_samples,
                                 line.get_width(),
                                 precision,is_signed,sample_bytes);
    }

  scan->next_x_tnum++;
  scan->accessed_samples += line.get_width();
  if (scan->accessed_samples == scan->width)
    { // Write completed line and send it to the free list.
      if (num_unwritten_rows == 0)
        { kdu_error e; e << "Attempting to write too many lines to image "
          "file for component " << comp_idx << "."; }
      if (fwrite(scan->buf,1,(size_t)(scan->width*scan->sample_bytes),out) !=
          (size_t)(scan->width*scan->sample_bytes))
        { kdu_error e; e << "Unable to write to image file for component "
          << comp_idx
          << ". File may be write protected, or disk may be full."; }
      num_unwritten_rows--;
      assert(scan == incomplete_lines);
      incomplete_lines = scan->next;
      scan->next = free_lines;
      free_lines = scan;
    }
}


/* ========================================================================= */
/*                                  bmp_out                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                              bmp_out::bmp_out                             */
/*****************************************************************************/

bmp_out::bmp_out(char const *fname, kdu_image_dims &dims, int &next_comp_idx)
{
  bool is_signed;
  int n;

  first_comp_idx = next_comp_idx;
  num_components = dims.get_num_components() - first_comp_idx;
  if (num_components <= 0)
    { kdu_error e; e << "Output image files require more image components "
      "(or mapped colour channels) than are available!"; }
  if (num_components >= 3)
    num_components = 3;
  else
    num_components = 1;
  rows = dims.get_height(first_comp_idx);  
  cols = dims.get_width(first_comp_idx);
  is_signed = dims.get_signed(first_comp_idx);
  for (n=0; n < num_components; n++, next_comp_idx++)
    {
      if ((rows != dims.get_height(next_comp_idx)) ||
          (cols != dims.get_width(next_comp_idx)) ||
          (is_signed != dims.get_signed(next_comp_idx)))
        { assert(n > 0); num_components = 1; break; }
      precision[n] = dims.get_bit_depth(next_comp_idx);
    }
  next_comp_idx = first_comp_idx + num_components;
  if (is_signed)
    { kdu_warning w;
      w << "Signed sample values will be written to the "
           "BMP file as unsigned 8-bit quantities, centered about 128.";
    }

  kdu_byte magic[14];
  bmp_header header;
  int header_bytes = 14+sizeof(header);
  assert(header_bytes == 54);
  if (num_components == 1)
    header_bytes += 1024; // Need colour LUT.
  int line_bytes = num_components * cols;
  alignment_bytes = (4-line_bytes) & 3;
  line_bytes += alignment_bytes;
  int file_bytes = line_bytes*rows + header_bytes;
  magic[0] = 'B'; magic[1] = 'M';
  magic[2] = (kdu_byte) file_bytes;
  magic[3] = (kdu_byte)(file_bytes>>8);
  magic[4] = (kdu_byte)(file_bytes>>16);
  magic[5] = (kdu_byte)(file_bytes>>24);
  magic[6] = magic[7] = magic[8] = magic[9] = 0;
  magic[10] = (kdu_byte) header_bytes;
  magic[11] = (kdu_byte)(header_bytes>>8);
  magic[12] = (kdu_byte)(header_bytes>>16);
  magic[13] = (kdu_byte)(header_bytes>>24);
  header.size = 40;
  header.width = cols;
  header.height = rows;
  header.planes = 1;
  header.bit_count = (num_components==1)?8:24;
  header.compression = 0;
  header.image_size = 0;
  header.xpels_per_metre = header.ypels_per_metre = 0;
  header.num_colours_used = header.num_colours_important = 0;
  to_little_endian((kdu_int32 *) &header,10);
  if ((out = fopen(fname,"wb")) == NULL)
    { kdu_error e; e << "Unable to open output image file, \"" << fname <<"\".";}
  fwrite(magic,1,14,out);
  fwrite(&header,1,40,out);
  if (num_components == 1)
    for (n=0; n < 256; n++)
      { fputc(n,out); fputc(n,out); fputc(n,out); fputc(0,out); }
  incomplete_lines = NULL;
  free_lines = NULL;
  num_unwritten_rows = rows;
}

/*****************************************************************************/
/*                              bmp_out::~bmp_out                            */
/*****************************************************************************/

bmp_out::~bmp_out()
{
  if ((num_unwritten_rows > 0) || (incomplete_lines != NULL))
    { kdu_warning w;
      w << "Not all rows of image components "
        << first_comp_idx << " through "
        << first_comp_idx+num_components-1
        << " were completed!";
    }
  image_line_buf *tmp;
  while ((tmp=incomplete_lines) != NULL)
    { incomplete_lines = tmp->next; delete tmp; }
  while ((tmp=free_lines) != NULL)
    { free_lines = tmp->next; delete tmp; }
  fclose(out);
}

/*****************************************************************************/
/*                                bmp_out::put                               */
/*****************************************************************************/

void
  bmp_out::put(int comp_idx, kdu_line_buf &line, int x_tnum)
{
  int idx = comp_idx - this->first_comp_idx;
  assert((idx >= 0) && (idx < num_components));
  x_tnum = x_tnum*num_components+idx;
  image_line_buf *scan, *prev=NULL;
  for (scan=incomplete_lines; scan != NULL; prev=scan, scan=scan->next)
    {
      assert(scan->next_x_tnum >= x_tnum);
      if (scan->next_x_tnum == x_tnum)
        break;
    }
  if (scan == NULL)
    { // Need to open a new line buffer
      assert(x_tnum == 0); // Must consume in very specific order.
      if ((scan = free_lines) == NULL)
        {
          scan = new image_line_buf(cols+3,num_components);
          for (int k=0; k < alignment_bytes; k++)
            scan->buf[num_components*cols+k] = 0;
        }
      free_lines = scan->next;
      if (prev == NULL)
        incomplete_lines = scan;
      else
        prev->next = scan;
      scan->accessed_samples = 0;
      scan->next_x_tnum = 0;
    }

  assert((cols-scan->accessed_samples) >= line.get_width());
  int comp_offset = (num_components==3)?(2-idx):0;

  if (line.get_buf32() != NULL)
    {
      if (line.is_absolute())
        convert_ints_to_bytes(line.get_buf32(),
                scan->buf+num_components*scan->accessed_samples+comp_offset,
                line.get_width(),precision[idx],num_components);
      else
        convert_floats_to_bytes(line.get_buf32(),
                scan->buf+num_components*scan->accessed_samples+comp_offset,
                line.get_width(),precision[idx],num_components);
    }
  else
    {
      if (line.is_absolute())
        convert_shorts_to_bytes(line.get_buf16(),
                scan->buf+num_components*scan->accessed_samples+comp_offset,
                line.get_width(),precision[idx],num_components);
      else
        convert_fixpoint_to_bytes(line.get_buf16(),
                scan->buf+num_components*scan->accessed_samples+comp_offset,
                line.get_width(),precision[idx],num_components);
    }

  scan->next_x_tnum++;
  if (idx == (num_components-1))
    scan->accessed_samples += line.get_width();
  if (scan->accessed_samples == cols)
    { // Write completed line and send it to the free list.
      if (num_unwritten_rows == 0)
        { kdu_error e; e << "Attempting to write too many lines to image "
          "file for components " << first_comp_idx << " through "
          << first_comp_idx+num_components-1 << "."; }
      if (fwrite(scan->buf,1,(size_t)(cols*num_components+alignment_bytes),
                 out) != (size_t)(cols*num_components+alignment_bytes))
        { kdu_error e; e << "Unable to write to image file for components "
          << first_comp_idx << " through " << first_comp_idx+num_components-1
          << ". File may be write protected, or disk may be full."; }
      num_unwritten_rows--;
      assert(scan == incomplete_lines);
      incomplete_lines = scan->next;
      scan->next = free_lines;
      free_lines = scan;
    }
}
