/*****************************************************************************/
// File: image_in.cpp [scope = APPS/IMAGE-IO]
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
   Implements image file reading for a variety of different file formats:
currently BMP, PGM, PPM and RAW only.  Readily extendible to include other
file formats without affecting the rest of the system.
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

/*****************************************************************************/
/* STATIC                       to_little_endian                             */
/*****************************************************************************/

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
/* INLINE                    eat_white_and_comments                          */
/*****************************************************************************/

static inline void
  eat_white_and_comments(FILE *in)
{
  int ch;
  bool in_comment;

  in_comment = false;
  while ((ch = getc(in)) != EOF)
    if (ch == '#')
      in_comment = true;
    else if (ch == '\n')
      in_comment = false;
    else if ((!in_comment) && (ch != ' ') && (ch != '\t') && (ch != '\r'))
      {
        ungetc(ch,in);
        return;
      }
}

/*****************************************************************************/
/* STATIC                    convert_words_to_floats                         */
/*****************************************************************************/

static void
  convert_words_to_floats(kdu_byte *src, kdu_sample32 *dest, int num,
                          int precision, bool is_signed, int sample_bytes)
{
  float scale;
  if (precision < 30)
    scale = (float)(1<<precision);
  else
    scale = ((float)(1<<30)) * ((float)(1<<(precision-30)));
  scale = 1.0F / scale;
  
  kdu_int32 centre = 1<<(precision-1);
  kdu_int32 offset = (is_signed)?centre:0;
  kdu_int32 mask = ~((-1)<<precision);
  kdu_int32 val;

  if (sample_bytes == 1)
    for (; num > 0; num--, dest++)
      {
        val = *(src++);
        val += offset;
        val &= mask;
        val -= centre;
        dest->fval = ((float) val) * scale;
      }
  else if (sample_bytes == 2)
    for (; num > 0; num--, dest++)
      {
        val = *(src++); val = (val<<8) + *(src++);
        val += offset;
        val &= mask;
        val -= centre;
        dest->fval = ((float) val) * scale;
      }
  else if (sample_bytes == 3)
    for (; num > 0; num--, dest++)
      {
        val = *(src++); val = (val<<8) + *(src++); val = (val<<8) + *(src++);
        val += offset;
        val &= mask;
        val -= centre;
        dest->fval = ((float) val) * scale;
      }
  else if (sample_bytes == 4)
    for (; num > 0; num--, dest++)
      {
        val = *(src++); val = (val<<8) + *(src++);
        val = (val<<8) + *(src++); val = (val<<8) + *(src++);
        val += offset;
        val &= mask;
        val -= centre;
        dest->fval = ((float) val) * scale;
      }
  else
    assert(0);
}

/*****************************************************************************/
/* STATIC                   convert_words_to_fixpoint                        */
/*****************************************************************************/

static void
  convert_words_to_fixpoint(kdu_byte *src, kdu_sample16 *dest, int num,
                            int precision, bool is_signed, int sample_bytes)
{
  kdu_int32 upshift = KDU_FIX_POINT-precision;
  if (upshift < 0)
    { kdu_error e; e << "Cannot use 16-bit representation with high "
      "bit-depth data"; }
  kdu_int32 centre = 1<<(precision-1);
  kdu_int32 offset = (is_signed)?centre:0;
  kdu_int32 mask = ~((-1)<<precision);
  kdu_int32 val;

  if (sample_bytes == 1)
    for (; num > 0; num--, dest++)
      {
        val = *(src++);
        val += offset;
        val &= mask;
        val -= centre;
        dest->ival = (kdu_int16)(val<<upshift);
      }
  else if (sample_bytes == 2)
    for (; num > 0; num--, dest++)
      {
        val = *(src++); val = (val<<8) + *(src++);
        val += offset;
        val &= mask;
        val -= centre;
        dest->ival = (kdu_int16)(val<<upshift);
      }
  else
    { kdu_error e; e << "Cannot use 16-bit representation with high "
      "bit-depth data"; }
}

/*****************************************************************************/
/* STATIC                     convert_words_to_ints                          */
/*****************************************************************************/

static void
  convert_words_to_ints(kdu_byte *src, kdu_sample32 *dest, int num,
                        int precision, bool is_signed, int sample_bytes)
{
  kdu_int32 centre = 1<<(precision-1);
  kdu_int32 offset = (is_signed)?centre:0;
  kdu_int32 mask = ~((-1)<<precision);
  kdu_int32 val;

  if (sample_bytes == 1)
    for (; num > 0; num--, dest++)
      {
        val = *(src++);
        val += offset;
        val &= mask;
        val -= centre;
        dest->ival = val;
      }
  else if (sample_bytes == 2)
    for (; num > 0; num--, dest++)
      {
        val = *(src++); val = (val<<8) + *(src++);
        val += offset;
        val &= mask;
        val -= centre;
        dest->ival = val;
      }
  else if (sample_bytes == 3)
    for (; num > 0; num--, dest++)
      {
        val = *(src++); val = (val<<8) + *(src++); val = (val<<8) + *(src++);
        val += offset;
        val &= mask;
        val -= centre;
        dest->ival = val;
      }
  else if (sample_bytes == 4)
    for (; num > 0; num--, dest++)
      {
        val = *(src++); val = (val<<8) + *(src++);
        val = (val<<8) + *(src++); val = (val<<8) + *(src++);
        val += offset;
        val &= mask;
        val -= centre;
        dest->ival = val;
      }
  else
    assert(0);
}

/*****************************************************************************/
/* STATIC                   convert_words_to_shorts                          */
/*****************************************************************************/

static void
  convert_words_to_shorts(kdu_byte *src, kdu_sample16 *dest, int num,
                          int precision, bool is_signed, int sample_bytes)
{
  kdu_int32 centre = 1<<(precision-1);
  kdu_int32 offset = (is_signed)?centre:0;
  kdu_int32 mask = ~((-1)<<precision);
  kdu_int32 val;

  if (sample_bytes == 1)
    for (; num > 0; num--, dest++)
      {
        val = *(src++);
        val += offset;
        val &= mask;
        val -= centre;
        dest->ival = (kdu_int16) val;
      }
  else if (sample_bytes == 2)
    for (; num > 0; num--, dest++)
      {
        val = *(src++); val = (val<<8) + *(src++);
        val += offset;
        val &= mask;
        val -= centre;
        dest->ival = (kdu_int16) val;
      }
  else
    { kdu_error e; e << "Cannot use 16-bit representation with high "
      "bit-depth data"; }
}


/* ========================================================================= */
/*                                kdu_image_in                               */
/* ========================================================================= */

/*****************************************************************************/
/*                         kdu_image_in::kdu_image_in                        */
/*****************************************************************************/

kdu_image_in::kdu_image_in(char const *fname, siz_params *siz,
                           int &next_comp_idx, bool &vflip,
                           kdu_rgb8_palette *palette)
{
  char const *suffix;

  in = NULL;
  vflip = false; // Allows derived constructors to ignore the argument.
  if ((suffix = strrchr(fname,'.')) != NULL)
    {
      if ((strcmp(suffix+1,"pbm") == 0) || (strcmp(suffix+1,"PBM") == 0))
        in = new pbm_in(fname,siz,next_comp_idx,palette);
      else if ((strcmp(suffix+1,"pgm") == 0) || (strcmp(suffix+1,"PGM") == 0))
        in = new pgm_in(fname,siz,next_comp_idx);
      else if ((strcmp(suffix+1,"ppm") == 0) || (strcmp(suffix+1,"PPM") == 0))
        in = new ppm_in(fname,siz,next_comp_idx);
      else if ((strcmp(suffix+1,"bmp") == 0) || (strcmp(suffix+1,"BMP") == 0))
        in = new bmp_in(fname,siz,next_comp_idx,vflip,palette);
      else if ((strcmp(suffix+1,"raw") == 0) || (strcmp(suffix+1,"RAW") == 0))
        in = new raw_in(fname,siz,next_comp_idx);
    }
  if (in == NULL)
    { kdu_error e; e << "Image file, \"" << fname << ", does not have a "
      "recognized suffix.  Valid suffices are currently: "
      "\"bmp\", \"pgm\", \"ppm\" and \"raw\".  "
      "Upper or lower case may be used, but must be used consistently.";
    }
}

/* ========================================================================= */
/*                                   pbm_in                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                               pbm_in::pbm_in                              */
/*****************************************************************************/

pbm_in::pbm_in(char const *fname, siz_params *siz, int &next_comp_idx,
               kdu_rgb8_palette *palette)
{
  char magic[3];

  if ((in = fopen(fname,"rb")) == NULL)
    { kdu_error e;
      e << "Unable to open input image file, \"" << fname <<"\".";}
  magic[0] = magic[1] = magic[2] = '\0';
  fread(magic,1,2,in);
  if (strcmp(magic,"P4") != 0)
    { kdu_error e; e << "PBM image file must start with the magic string, "
      "\"P4\"!"; }
  bool failed = false;
  eat_white_and_comments(in);
  if (fscanf(in,"%d",&cols) != 1)
    failed = true;
  eat_white_and_comments(in);
  if (fscanf(in,"%d",&rows) != 1)
    failed = true;
  if (failed)
    {kdu_error e; e << "Image file \"" << fname << "\" does not appear to "
     "have a valid PBM header."; }
  int ch;
  while ((ch = fgetc(in)) != EOF)
    if (ch == '\n')
      break;
  comp_idx = next_comp_idx++;
  siz->set(Sdims,comp_idx,0,rows);
  siz->set(Sdims,comp_idx,1,cols);
  siz->set(Ssigned,comp_idx,0,false);
  siz->set(Sprecision,comp_idx,0,1);
  if ((palette != NULL) && !palette->exists())
    {
      palette->input_bits = 1;
      palette->output_bits = 8;
      palette->source_component = comp_idx;
      palette->blue[0] = palette->green[0] = palette->red[0] = 0;
      palette->blue[1] = palette->green[1] = palette->red[1] = 255;
      // Note that we will be flipping the bits so that a 0 really does
      // represent black, rather than white -- this is more efficient for
      // coding facsimile type images where the background is white.
    }
  incomplete_lines = free_lines = NULL;
  num_unread_rows = rows;
}

/*****************************************************************************/
/*                               pbm_in::~pbm_in                             */
/*****************************************************************************/

pbm_in::~pbm_in()
{
  if ((num_unread_rows > 0) || (incomplete_lines != NULL))
    { kdu_warning w;
      w << "Not all rows of image component "
        << comp_idx << " were consumed!";
    }
  image_line_buf *tmp;
  while ((tmp=incomplete_lines) != NULL)
    { incomplete_lines = tmp->next; delete tmp; }
  while ((tmp=free_lines) != NULL)
    { free_lines = tmp->next; delete tmp; }
  fclose(in);
}

/*****************************************************************************/
/*                                 pbm_in::get                               */
/*****************************************************************************/

bool
  pbm_in::get(int comp_idx, kdu_line_buf &line, int x_tnum)
{
  assert(comp_idx == this->comp_idx);
  image_line_buf *scan, *prev=NULL;
  kdu_byte *sp;
  int n;

  for (scan=incomplete_lines; scan != NULL; prev=scan, scan=scan->next)
    {
      assert(scan->next_x_tnum >= x_tnum);
      if (scan->next_x_tnum == x_tnum)
        break;
    }
  if (scan == NULL)
    { // Need to read a new image line.
      assert(x_tnum == 0); // Must consume line from left to right.
      if (num_unread_rows == 0)
        return false;
      if ((scan = free_lines) == NULL)
        scan = new image_line_buf(cols+7,1);
      free_lines = scan->next;
      if (prev == NULL)
        incomplete_lines = scan;
      else
        prev->next = scan;
      n = (cols+7) >> 3;
      if (fread(scan->buf,1,(size_t) n,in) != (size_t) n)
        { kdu_error e; e << "Image file for component " << comp_idx
          << " terminated prematurely!"; }
      // Expand the packed representation into whole bytes, flipping and
      // storing each binary digit in the LSB of a single byte.  The reason
      // for flipping is that PBM files represent white using a 0 and
      // black using a 1, but the more natural and also more efficient
      // representation for coding unsigned data with 1-bit precision in
      // JPEG2000 is the opposite.
      sp = scan->buf + n;
      kdu_byte val, *dp = scan->buf + (n<<3);
      for (; n > 0; n--)
        {
          val = *(--sp); val = ~val;
          *(--dp) = (val&1); val >>= 1;    *(--dp) = (val&1); val >>= 1;
          *(--dp) = (val&1); val >>= 1;    *(--dp) = (val&1); val >>= 1;
          *(--dp) = (val&1); val >>= 1;    *(--dp) = (val&1); val >>= 1;
          *(--dp) = (val&1); val >>= 1;    *(--dp) = (val&1);
        }
      scan->accessed_samples = 0;
      scan->next_x_tnum = 0;
      num_unread_rows--;
    }
  assert((cols-scan->accessed_samples) >= line.get_width());

  sp = scan->buf+scan->accessed_samples;
  n = line.get_width();

  if (line.get_buf32() != NULL)
    {
      kdu_sample32 *dp = line.get_buf32();
      if (line.is_absolute())
        { // 32-bit absolute integers
          for (; n > 0; n--, sp++, dp++)
            dp->ival = ((kdu_int32)(*sp)) - 1;
        }
      else
        { // true 32-bit floats
          for (; n > 0; n--, sp++, dp++)
            dp->fval = (((float)(*sp)) / 2.0F) - 0.5F;
        }
    }
  else
    {
      kdu_sample16 *dp = line.get_buf16();
      if (line.is_absolute())
        { // 16-bit absolute integers
          for (; n > 0; n--, sp++, dp++)
            dp->ival = ((kdu_int16)(*sp)) - 1;
        }
      else
        { // 16-bit normalized representation.
          for (; n > 0; n--, sp++, dp++)
            dp->ival = (((kdu_int16)(*sp)) - 1) << (KDU_FIX_POINT-1);
        }
    }

  scan->next_x_tnum++;
  scan->accessed_samples += line.get_width();
  if (scan->accessed_samples == cols)
    { // Send empty line to free list.
      assert(scan == incomplete_lines);
      incomplete_lines = scan->next;
      scan->next = free_lines;
      free_lines = scan;
    }

  return true;
}


/* ========================================================================= */
/*                                   pgm_in                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                               pgm_in::pgm_in                              */
/*****************************************************************************/

pgm_in::pgm_in(char const *fname, siz_params *siz, int &next_comp_idx)
{
  char magic[3];
  int max_val; // We don't actually use this.

  if ((in = fopen(fname,"rb")) == NULL)
    { kdu_error e;
      e << "Unable to open input image file, \"" << fname <<"\"."; }
  magic[0] = magic[1] = magic[2] = '\0';
  fread(magic,1,2,in);
  if (strcmp(magic,"P5") != 0)
    { kdu_error e; e << "PGM image file must start with the magic string, "
      "\"P5\"!"; }
  bool failed = false;
  eat_white_and_comments(in);
  if (fscanf(in,"%d",&cols) != 1)
    failed = true;
  eat_white_and_comments(in);
  if (fscanf(in,"%d",&rows) != 1)
    failed = true;
  eat_white_and_comments(in);
  if (fscanf(in,"%d",&max_val) != 1)
    failed = true;
  if (failed)
    {kdu_error e; e << "Image file \"" << fname << "\" does not appear to "
     "have a valid PGM header."; }
  int ch;
  while ((ch = fgetc(in)) != EOF)
    if ((ch == '\n') || (ch == ' '))
      break;
  comp_idx = next_comp_idx++;
  siz->set(Sdims,comp_idx,0,rows);
  siz->set(Sdims,comp_idx,1,cols);
  siz->set(Ssigned,comp_idx,0,false);
  siz->set(Sprecision,comp_idx,0,8);
  incomplete_lines = free_lines = NULL;
  num_unread_rows = rows;
}

/*****************************************************************************/
/*                               pgm_in::~pgm_in                             */
/*****************************************************************************/

pgm_in::~pgm_in()
{
  if ((num_unread_rows > 0) || (incomplete_lines != NULL))
    { kdu_warning w;
      w << "Not all rows of image component "
        << comp_idx << " were consumed!";
    }
  image_line_buf *tmp;
  while ((tmp=incomplete_lines) != NULL)
    { incomplete_lines = tmp->next; delete tmp; }
  while ((tmp=free_lines) != NULL)
    { free_lines = tmp->next; delete tmp; }
  fclose(in);
}

/*****************************************************************************/
/*                                 pgm_in::get                               */
/*****************************************************************************/

bool
  pgm_in::get(int comp_idx, kdu_line_buf &line, int x_tnum)
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
    { // Need to read a new image line.
      assert(x_tnum == 0); // Must consume line from left to right.
      if (num_unread_rows == 0)
        return false;
      if ((scan = free_lines) == NULL)
        scan = new image_line_buf(cols,1);
      free_lines = scan->next;
      if (prev == NULL)
        incomplete_lines = scan;
      else
        prev->next = scan;
      if (fread(scan->buf,1,(size_t) scan->width,in) != (size_t) scan->width)
        { kdu_error e; e << "Image file for component " << comp_idx
          << " terminated prematurely!"; }
      scan->accessed_samples = 0;
      scan->next_x_tnum = 0;
      num_unread_rows--;
    }
  assert((scan->width-scan->accessed_samples) >= line.get_width());

  kdu_byte *sp = scan->buf+scan->accessed_samples;
  int n=line.get_width();

  if (line.get_buf32() != NULL)
    {
      kdu_sample32 *dp = line.get_buf32();
      if (line.is_absolute())
        { // 32-bit absolute integers
          for (; n > 0; n--, sp++, dp++)
            dp->ival = ((kdu_int32)(*sp)) - 128;
        }
      else
        { // true 32-bit floats
          for (; n > 0; n--, sp++, dp++)
            dp->fval = (((float)(*sp)) / 256.0F) - 0.5F;
        }
    }
  else
    {
      kdu_sample16 *dp = line.get_buf16();
      if (line.is_absolute())
        { // 16-bit absolute integers
          for (; n > 0; n--, sp++, dp++)
            dp->ival = ((kdu_int16)(*sp)) - 128;
        }
      else
        { // 16-bit normalized representation.
          for (; n > 0; n--, sp++, dp++)
            dp->ival = (((kdu_int16)(*sp)) - 128) << (KDU_FIX_POINT-8);
        }
    }

  scan->next_x_tnum++;
  scan->accessed_samples += line.get_width();
  if (scan->accessed_samples == scan->width)
    { // Send empty line to free list.
      assert(scan == incomplete_lines);
      incomplete_lines = scan->next;
      scan->next = free_lines;
      free_lines = scan;
    }

  return true;
}


/* ========================================================================= */
/*                                   ppm_in                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                               ppm_in::ppm_in                              */
/*****************************************************************************/

ppm_in::ppm_in(char const *fname, siz_params *siz, int &next_comp_idx)
{
  char magic[3];
  int max_val; // We don't actually use this.
  int n;

  if ((in = fopen(fname,"rb")) == NULL)
    { kdu_error e;
      e << "Unable to open input image file, \"" << fname <<"\"."; }
  magic[0] = magic[1] = magic[2] = '\0';
  fread(magic,1,2,in);
  if (strcmp(magic,"P6") != 0)
    { kdu_error e; e << "PPM image file must start with the magic string, "
      "\"P6\"!"; }
  bool failed = false;
  eat_white_and_comments(in);
  if (fscanf(in,"%d",&cols) != 1)
    failed = true;
  eat_white_and_comments(in);
  if (fscanf(in,"%d",&rows) != 1)
    failed = true;
  eat_white_and_comments(in);
  if (fscanf(in,"%d",&max_val) != 1)
    failed = true;
  if (failed)
    {kdu_error e; e << "Image file \"" << fname << "\" does not appear to "
     "have a valid PPM header."; }
  int ch;
  while ((ch = fgetc(in)) != EOF)
    if ((ch == '\n') || (ch == ' '))
      break;
  first_comp_idx = next_comp_idx;
  for (n=0; n < 3; n++)
    {
      next_comp_idx++;
      siz->set(Sdims,first_comp_idx+n,0,rows);
      siz->set(Sdims,first_comp_idx+n,1,cols);
      siz->set(Ssigned,first_comp_idx+n,0,false);
      siz->set(Sprecision,first_comp_idx+n,0,8);
    }
  incomplete_lines = NULL;
  free_lines = NULL;
  num_unread_rows = rows;
}

/*****************************************************************************/
/*                               ppm_in::~ppm_in                             */
/*****************************************************************************/

ppm_in::~ppm_in()
{
  if ((num_unread_rows > 0) || (incomplete_lines != NULL))
    { kdu_warning w;
      w << "Not all rows of image components "
        << first_comp_idx << " through " << first_comp_idx+2
        << " were consumed!";
    }
  image_line_buf *tmp;
  while ((tmp=incomplete_lines) != NULL)
    { incomplete_lines = tmp->next; delete tmp; }
  while ((tmp=free_lines) != NULL)
    { free_lines = tmp->next; delete tmp; }
  fclose(in);
}

/*****************************************************************************/
/*                                 ppm_in::get                               */
/*****************************************************************************/

bool
  ppm_in::get(int comp_idx, kdu_line_buf &line, int x_tnum)
{
  int idx = comp_idx - this->first_comp_idx;
  assert((idx >= 0) && (idx <= 2));
  x_tnum = x_tnum*3+idx; // Works so long as components read in order.
  image_line_buf *scan, *prev=NULL;
  for (scan=incomplete_lines; scan != NULL; prev=scan, scan=scan->next)
    {
      assert(scan->next_x_tnum >= x_tnum);
      if (scan->next_x_tnum == x_tnum)
        break;
    }
  if (scan == NULL)
    { // Need to read a new image line.
      assert(x_tnum == 0); // Must consume in very specific order.
      if (num_unread_rows == 0)
        return false;
      if ((scan = free_lines) == NULL)
        scan = new image_line_buf(cols,3);
      free_lines = scan->next;
      if (prev == NULL)
        incomplete_lines = scan;
      else
        prev->next = scan;
      if (fread(scan->buf,1,(size_t)(scan->width*3),in) !=
          (size_t)(scan->width*3))
        { kdu_error e; e << "Image file for components " << first_comp_idx
          << " through " << first_comp_idx+2 << " terminated prematurely!"; }
      num_unread_rows--;
      scan->accessed_samples = 0;
      scan->next_x_tnum = 0;
    }

  assert((scan->width-scan->accessed_samples) >= line.get_width());

  kdu_byte *sp = scan->buf+3*scan->accessed_samples+idx;
  int n=line.get_width();
  if (line.get_buf32() != NULL)
    {
      kdu_sample32 *dp = line.get_buf32();
      if (line.is_absolute())
        { // 32-bit absolute integers
          for (; n > 0; n--, sp+=3, dp++)
            dp->ival = ((kdu_int32)(*sp)) - 128;
        }
      else
        { // true 32-bit floats
          for (; n > 0; n--, sp+=3, dp++)
            dp->fval = (((float)(*sp)) / 256.0F) - 0.5F;
        }
    }
  else
    {
      kdu_sample16 *dp = line.get_buf16();
      if (line.is_absolute())
        { // 16-bit absolute integers
          for (; n > 0; n--, sp+=3, dp++)
            dp->ival = ((kdu_int16)(*sp)) - 128;
        }
      else
        { // 16-bit normalized representation.
          for (; n > 0; n--, sp+=3, dp++)
            dp->ival = (((kdu_int16)(*sp)) - 128) << (KDU_FIX_POINT-8);
        }
    }

  scan->next_x_tnum++;
  if (idx == 2)
    scan->accessed_samples += line.get_width();
  if (scan->accessed_samples == scan->width)
    { // Send empty line to free list.
      assert(scan == incomplete_lines);
      incomplete_lines = scan->next;
      scan->next = free_lines;
      free_lines = scan;
    }

  return true;
}


/* ========================================================================= */
/*                                   raw_in                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                               raw_in::raw_in                              */
/*****************************************************************************/

raw_in::raw_in(char const *fname, siz_params *siz, int &next_comp_idx)
{
  if ((in = fopen(fname,"rb")) == NULL)
    { kdu_error e;
      e << "Unable to open input image file, \"" << fname <<"\".";}
  comp_idx = next_comp_idx++;
  if (!(siz->get(Sdims,comp_idx,0,rows) &&
        siz->get(Sdims,comp_idx,1,cols) &&
        siz->get(Sprecision,comp_idx,0,precision) &&
        siz->get(Ssigned,comp_idx,0,is_signed)))
    { kdu_error e;
      e << "To use the raw image input file format, you must explicitly "
           "supply image component dimensions, image sample bit-depth and "
           "signed/unsigned information, using the \"Sdims\", \"Sprecision\" "
           "and \"Ssigned\" arguments."; }
  if (precision > 32)
    { kdu_error e; e << "Current implementation does not support "
      "image sample bit-depths in excess of 32 bits!"; }
  num_unread_rows = rows;
  sample_bytes = (precision+7)>>3;
  incomplete_lines = free_lines = NULL;
}

/*****************************************************************************/
/*                               raw_in::~raw_in                             */
/*****************************************************************************/

raw_in::~raw_in()
{
  if ((num_unread_rows > 0) || (incomplete_lines != NULL))
    { kdu_warning w;
      w << "Not all rows of image component "
        << comp_idx << " were consumed!";
    }
  image_line_buf *tmp;
  while ((tmp=incomplete_lines) != NULL)
    { incomplete_lines = tmp->next; delete tmp; }
  while ((tmp=free_lines) != NULL)
    { free_lines = tmp->next; delete tmp; }
  fclose(in);
}

/*****************************************************************************/
/*                                 raw_in::get                               */
/*****************************************************************************/

bool
  raw_in::get(int comp_idx, kdu_line_buf &line, int x_tnum)
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
    { // Need to read a new image line.
      assert(x_tnum == 0); // Must consume line from left to right.
      if (num_unread_rows == 0)
        return false;
      if ((scan = free_lines) == NULL)
        scan = new image_line_buf(cols,sample_bytes);
      free_lines = scan->next;
      if (prev == NULL)
        incomplete_lines = scan;
      else
        prev->next = scan;
      if (fread(scan->buf,1,(size_t)(scan->width*scan->sample_bytes),in) !=
          (size_t)(scan->width*scan->sample_bytes))
        { kdu_error e; e << "Image file for component " << comp_idx
          << " terminated prematurely!"; }
      num_unread_rows--;
      scan->accessed_samples = 0;
      scan->next_x_tnum = 0;
    }
  assert((scan->width-scan->accessed_samples) >= line.get_width());

  if (line.get_buf32() != NULL)
    {
      if (line.is_absolute())
        convert_words_to_ints(scan->buf+sample_bytes*scan->accessed_samples,
                              line.get_buf32(),line.get_width(),
                              precision,is_signed,sample_bytes);
      else
        convert_words_to_floats(scan->buf+sample_bytes*scan->accessed_samples,
                                line.get_buf32(),line.get_width(),
                                precision,is_signed,sample_bytes);
    }
  else
    {
      if (line.is_absolute())
        convert_words_to_shorts(scan->buf+sample_bytes*scan->accessed_samples,
                                line.get_buf16(),line.get_width(),
                                precision,is_signed,sample_bytes);
      else
        convert_words_to_fixpoint(scan->buf+sample_bytes*scan->accessed_samples,
                                  line.get_buf16(),line.get_width(),
                                  precision,is_signed,sample_bytes);
    }
  scan->next_x_tnum++;
  scan->accessed_samples += line.get_width();
  if (scan->accessed_samples == scan->width)
    { // Send empty line to free list.
      assert(scan == incomplete_lines);
      incomplete_lines = scan->next;
      scan->next = free_lines;
      free_lines = scan;
    }

  return true;
}


/* ========================================================================= */
/*                                   bmp_in                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                               bmp_in::bmp_in                              */
/*****************************************************************************/

bmp_in::bmp_in(char const *fname, siz_params *siz, int &next_comp_idx,
               bool &vflip, kdu_rgb8_palette *palette)
{
  int n;

  if ((in = fopen(fname,"rb")) == NULL)
    { kdu_error e;
      e << "Unable to open input image file, \"" << fname <<"\"."; }

  kdu_byte magic[14];
  bmp_header header;
  fread(magic,1,14,in);
  if ((magic[0] != 'B') || (magic[1] != 'M') || (fread(&header,1,40,in) != 40))
    { kdu_error e; e << "BMP image file must start with the magic string, "
      "\"BM\", and continue with a header whose total size is at least 54 "
      "bytes."; }
  from_little_endian((kdu_int32 *) &header,10);
  cols = header.width;
  rows = header.height;
  if (rows < 0)
    rows = -rows;
  else
    vflip = true;
  bytes = nibbles = bits = expand_palette = false;
  int precision = 8;
  if (header.bit_count == 24)
    num_components = 3;
  else if (header.bit_count == 8)
    { num_components = 1; bytes = true; }
  else if (header.bit_count == 4)
    { num_components = 1; nibbles = true; precision = 4; }
  else if (header.bit_count == 1)
    { num_components = 1; bits = true; precision = 1; }
  else
    { kdu_error e;
      e << "We currently support only 1-, 4-, 8- and 24-bit BMP files."; }
  int header_size = 54 + ((num_components==1)?(4<<precision):0);

  int offset = magic[13];
  offset <<= 8; offset += magic[12];
  offset <<= 8; offset += magic[11];
  offset <<= 8; offset += magic[10];
  if (offset < header_size)
    { kdu_error e; e << "Invalid sample data offset field specified in BMP "
      "file header!"; }
  if (num_components == 1)
    {
      fread(map,1,4<<precision,in);
      if (bytes)
        {
          for (n=0; n < 256; n++)
            if ((map[4*n] != n) || (map[4*n+1] != n) || (map[4*n+2] != n))
              break;
          if (n == 256)
            bytes = false; // No need to use palette
        }
    }
  if (bytes || nibbles || bits)
    {
      if ((palette == NULL) || palette->exists())
        {
          expand_palette = true; // Need to expand the palette here.
          precision = 8;
          for (n=0; n < 256; n++)
            if ((map[4*n] != map[4*n+1]) || (map[4*n] != map[4*n+2]))
              break; // Not a monochrome source.
          num_components = (n==256)?1:3;
        }
      else
        { // Set up the colour palette.
          palette->input_bits = precision;
          palette->output_bits = 8;
          palette->source_component = next_comp_idx;
          for (n=0; n < (1<<precision); n++)
            {
              palette->blue[n] = map[4*n+0];
              palette->green[n] = map[4*n+1];
              palette->red[n] = map[4*n+2];
            }
          for (n=0; n < (1<<precision); n++)
            map[n] = n; // Set identity permutation for now.
          if (nibbles || bytes)
            palette->rearrange(map); // Try to find a better permutation.
        }
    }
  if (offset > header_size)
    fseek(in,offset-header_size,SEEK_CUR);

  first_comp_idx = next_comp_idx;
  for (n=0; n < num_components; n++)
    {
      next_comp_idx++;
      siz->set(Sdims,first_comp_idx+n,0,rows);
      siz->set(Sdims,first_comp_idx+n,1,cols);
      siz->set(Ssigned,first_comp_idx+n,0,false);
      siz->set(Sprecision,first_comp_idx+n,0,precision);
    }
  incomplete_lines = NULL;
  free_lines = NULL;
  num_unread_rows = rows;
  if (bytes)
    line_bytes = cols;
  else if (nibbles)
    line_bytes = (cols+1)>>1;
  else if (bits)
    line_bytes = (cols+7)>>3;
  else
    line_bytes = cols*num_components;
  line_bytes += (-line_bytes) & 3; // Pad to a multiple of 4 bytes.
}

/*****************************************************************************/
/*                               bmp_in::~bmp_in                             */
/*****************************************************************************/

bmp_in::~bmp_in()
{
  if ((num_unread_rows > 0) || (incomplete_lines != NULL))
    { kdu_warning w;
      w << "Not all rows of image components "
        << first_comp_idx << " through "
        << first_comp_idx+num_components-1
        << " were consumed!";
    }
  image_line_buf *tmp;
  while ((tmp=incomplete_lines) != NULL)
    { incomplete_lines = tmp->next; delete tmp; }
  while ((tmp=free_lines) != NULL)
    { free_lines = tmp->next; delete tmp; }
  fclose(in);
}

/*****************************************************************************/
/*                                 bmp_in::get                               */
/*****************************************************************************/

bool
  bmp_in::get(int comp_idx, kdu_line_buf &line, int x_tnum)
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
    { // Need to read a new image line.
      assert(x_tnum == 0); // Must consume in very specific order.
      if (num_unread_rows == 0)
        return false;
      if ((scan = free_lines) == NULL)
        scan = new image_line_buf(cols+7,num_components);
                          // Big enough for padding and expanding bits to bytes
      free_lines = scan->next;
      if (prev == NULL)
        incomplete_lines = scan;
      else
        prev->next = scan;
      if (fread(scan->buf,1,(size_t) line_bytes,in) != (size_t) line_bytes)
        { kdu_error e; e << "Image file for components " << first_comp_idx
          << " through " << first_comp_idx+num_components-1
          << " terminated prematurely!"; }
      num_unread_rows--;
      scan->accessed_samples = 0;
      scan->next_x_tnum = 0;
      if (bytes)
        map_palette_index_bytes(scan->buf,line.is_absolute());
      else if (nibbles)
        map_palette_index_nibbles(scan->buf,line.is_absolute());
      else if (bits)
        map_palette_index_bits(scan->buf,line.is_absolute());
    }
  assert((cols-scan->accessed_samples) >= line.get_width());

  int comp_offset = (num_components==3)?(2-idx):0;
  kdu_byte *sp = scan->buf+num_components*scan->accessed_samples + comp_offset;
  int n=line.get_width();

  if (line.get_buf32() != NULL)
    {
      kdu_sample32 *dp = line.get_buf32();
      if (line.is_absolute())
        { // 32-bit absolute integers
          kdu_int32 offset = 128;
          if ((num_components == 1) && nibbles) offset = 8;
          if ((num_components == 1) && bits) offset = 1;
          for (; n > 0; n--, sp+=num_components, dp++)
            dp->ival = ((kdu_int32)(*sp)) - offset;
        }
      else
        { // true 32-bit floats
          for (; n > 0; n--, sp+=num_components, dp++)
            dp->fval = (((float)(*sp)) / 256.0F) - 0.5F;
        }
    }
  else
    {
      kdu_sample16 *dp = line.get_buf16();
      if (line.is_absolute())
        { // 16-bit absolute integers
          kdu_int16 offset = 128;
          if ((num_components == 1) && nibbles) offset = 8;
          if ((num_components == 1) && bits) offset = 1;
          for (; n > 0; n--, sp+=num_components, dp++)
            dp->ival = ((kdu_int16)(*sp)) - offset;
        }
      else
        { // 16-bit normalized representation.
          for (; n > 0; n--, sp+=num_components, dp++)
            dp->ival = (((kdu_int16)(*sp)) - 128) << (KDU_FIX_POINT-8);
        }
    }

  scan->next_x_tnum++;
  if (idx == (num_components-1))
    scan->accessed_samples += line.get_width();
  if (scan->accessed_samples == cols)
    { // Send empty line to free list.
      assert(scan == incomplete_lines);
      incomplete_lines = scan->next;
      scan->next = free_lines;
      free_lines = scan;
    }

  return true;
}

/*****************************************************************************/
/*                       bmp_in::map_palette_index_bytes                     */
/*****************************************************************************/

void
  bmp_in::map_palette_index_bytes(kdu_byte *buf, bool absolute)
{
  int n = cols;

  if (num_components == 3)
    { // Expand single component through palette
      assert(expand_palette);
      kdu_byte *sp = buf + n;
      kdu_byte *dp = buf + (3*n);
      kdu_byte *mp;
      for (; n > 0; n--)
        {
          mp = map + (((int) *(--sp))<<2);
          *(--dp) = mp[2]; *(--dp) = mp[1]; *(--dp) = mp[0];
        }
    }
  else if (expand_palette)
    {
      assert(num_components == 1);
      kdu_byte *sp = buf + n;
      kdu_byte *dp = buf + n;
      for (; n > 0; n--)
        *(--dp) = map[((int) *(--sp))<<2];
    }
  else
    { // Apply optimized permutation map to the palette indices
      assert(num_components == 1);
      for (; n > 0; n--, buf++)
        *buf = map[*buf];
    }
}

/*****************************************************************************/
/*                      bmp_in::map_palette_index_nibbles                    */
/*****************************************************************************/

void
  bmp_in::map_palette_index_nibbles(kdu_byte *buf, bool absolute)
{
  int n = (cols+1)>>1;

  if (num_components == 3)
    { // Expand single component through palette
      assert(expand_palette);
      kdu_byte *sp = buf + n;
      kdu_byte *dp = buf + (6*n);
      kdu_byte *mp;
      kdu_uint32 val;
      for (; n > 0; n--)
        {
          val = *(--sp);
          mp = map + ((val & 0x0F)<<2);
          *(--dp) = mp[2]; *(--dp) = mp[1]; *(--dp) = mp[0];
          val >>= 4;
          mp = map + ((val & 0x0F)<<2);
          *(--dp) = mp[2]; *(--dp) = mp[1]; *(--dp) = mp[0];
        }
    }
  else if (expand_palette)
    {
      assert(num_components == 1);
      kdu_byte *sp = buf + n;
      kdu_byte *dp = buf + (2*n);
      kdu_uint32 val;
      for (; n > 0; n--)
        {
          val = *(--sp);
          *(--dp) = map[(val & 0x0F) << 2];
          val >>= 4;
          *(--dp) = map[(val & 0x0F) << 2];
        }
    }
  else
    { // Apply optimized permutation map to the palette indices
      assert(num_components == 1);
      kdu_byte *sp = buf + n;
      kdu_byte *dp = buf + (2*n);
      kdu_byte val;
      if (absolute)
        { // Map nibbles and store in least significant 4 bits of byte
          for (; n > 0; n--)
            {
              val = *(--sp);
              *(--dp) = map[val & 0x0F];
              val >>= 4;
              *(--dp) = map[val & 0x0F];
            }
        }
      else
        { // Map nibbles and store in most significant 4 bits of byte
          for (; n > 0; n--)
            {
              val = *(--sp);
              *(--dp) = map[val & 0x0F]<<4;
              val >>= 4;
              *(--dp) = map[val & 0x0F]<<4;
            }
        }
    }
}

/*****************************************************************************/
/*                       bmp_in::map_palette_index_bits                      */
/*****************************************************************************/

void
  bmp_in::map_palette_index_bits(kdu_byte *buf, bool absolute)
{
  int b, n = (cols+7)>>3;

  if (num_components == 3)
    { // Expand single component through palette
      assert(expand_palette);
      kdu_byte *sp = buf + n;
      kdu_byte *dp = buf + (24*n);
      kdu_byte *mp;
      kdu_uint32 val;
      for (; n > 0; n--)
        {
          val = *(--sp);
          for (b=8; b > 0; b--, val>>=1)
            {
              mp = map + ((val & 1)<<2);
              *(--dp) = mp[2]; *(--dp) = mp[1]; *(--dp) = mp[0];
            }
        }
    }
  else if (expand_palette)
    {
      assert(num_components == 1);
      kdu_byte *sp = buf + n;
      kdu_byte *dp = buf + (8*n);
      kdu_uint32 val;
      for (; n > 0; n--)
        {
          val = *(--sp);
          for (b=8; b > 0; b--, val>>=1)
            *(--dp) = map[(val & 1)<<2];
        }
    }
  else
    { // Apply optimized permutation map to the palette indices
      assert(num_components == 1);
      kdu_byte *sp = buf + n;
      kdu_byte *dp = buf + (8*n);
      kdu_byte val;
      if (absolute)
        { // Store bits in LSB's of bytes
          for (; n > 0; n--)
            {
              val = *(--sp);
              for (b=8; b > 0; b--, val>>=1)
                *(--dp) = val&1;
            }
        }
      else
        { // Store bits in MSB's of bytes
          for (; n > 0; n--)
            {
              val = *(--sp);
              for (b=8; b > 0; b--, val>>=1)
                *(--dp) = (val&1)<<7;
            }
        }
    }
}
