/*****************************************************************************/
// File: jp2.cpp [scope = APPS/JP2]
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
   Implements the internal machinery whose external interfaces are defined
in the compressed-io header file, "jp2.h".
******************************************************************************/

#include <assert.h>
#include <string.h>
#include <math.h>
#include "kdu_elementary.h"
#include "kdu_compressed.h"
#include "kdu_messaging.h"
#include "jp2.h"
#include "jp2_local.h"


/* ========================================================================= */
/*                              Box Type Codes                               */
/* ========================================================================= */

static inline kdu_uint32
  string_to_int(char *string)
{
  assert((string[0] != '\0') && (string[1] != '\0') &&
         (string[2] != '\0') && (string[3] != '\0') && (string[4] == '\0'));
  kdu_uint32 result = ((kdu_byte) string[0]);
  result = (result<<8) + ((kdu_byte) string[1]);
  result = (result<<8) + ((kdu_byte) string[2]);
  result = (result<<8) + ((kdu_byte) string[3]);
  return result;
}

static const kdu_uint32 j2_signature_box          = string_to_int("jP  ");
static const kdu_uint32 j2_file_type_box          = string_to_int("ftyp");
static const kdu_uint32 j2_header_box             = string_to_int("jp2h");
static const kdu_uint32 j2_image_header_box       = string_to_int("ihdr");
static const kdu_uint32 j2_bits_per_component_box = string_to_int("bpcc");
static const kdu_uint32 j2_colour_box             = string_to_int("colr");
static const kdu_uint32 j2_palette_box            = string_to_int("pclr");
static const kdu_uint32 j2_component_mapping_box  = string_to_int("cmap");
static const kdu_uint32 j2_channel_definition_box = string_to_int("cdef");
static const kdu_uint32 j2_resolution_box         = string_to_int("res ");
static const kdu_uint32 j2_capture_resolution_box = string_to_int("resc");
static const kdu_uint32 j2_display_resolution_box = string_to_int("resd");
static const kdu_uint32 j2_codestream_box         = string_to_int("jp2c");

static const kdu_uint32 j2_signature = 0x0D0A870A;
static const kdu_uint32 j2_brand     = string_to_int("jp2 ");

/* ========================================================================= */
/*                          ICC Profile Signatures                           */
/* ========================================================================= */

static const kdu_uint32 icc_input_device   = string_to_int("scnr");
static const kdu_uint32 icc_gray_data      = string_to_int("GRAY");
static const kdu_uint32 icc_rgb_data       = string_to_int("RGB ");
static const kdu_uint32 icc_pcs_xyz        = string_to_int("XYZ ");
static const kdu_uint32 icc_file_signature = string_to_int("acsp");

static const kdu_uint32 icc_gray_trc       = string_to_int("kTRC");
static const kdu_uint32 icc_red_trc        = string_to_int("rTRC");
static const kdu_uint32 icc_green_trc      = string_to_int("gTRC");
static const kdu_uint32 icc_blue_trc       = string_to_int("bTRC");
static const kdu_uint32 icc_red_colorant   = string_to_int("rXYZ");
static const kdu_uint32 icc_green_colorant = string_to_int("gXYZ");
static const kdu_uint32 icc_blue_colorant  = string_to_int("bXYZ");
static const kdu_uint32 icc_curve_type     = string_to_int("curv");
static const kdu_uint32 icc_xyz_type       = string_to_int("XYZ ");

/* ========================================================================= */
/*                             Internal Functions                            */
/* ========================================================================= */

/*****************************************************************************/
/* INLINE                           store_big                                */
/*****************************************************************************/

static inline void
  store_big(kdu_uint32 val, kdu_byte * &bp)
{
  bp += 4;
  bp[-1] = (kdu_byte) val; val >>= 8;
  bp[-2] = (kdu_byte) val; val >>= 8;
  bp[-3] = (kdu_byte) val; val >>= 8;
  bp[-4] = (kdu_byte) val;
}

static inline void
  store_big(kdu_uint16 val, kdu_byte * &bp)
{
  bp += 2;
  bp[-1] = (kdu_byte) val; val >>= 8;
  bp[-2] = (kdu_byte) val;
}

static inline void
  store_big(kdu_byte val, kdu_byte * &bp)
{
  bp += 1;
  bp[-1] = (kdu_byte) val;
}


/* ========================================================================= */
/*                                j2_input_box                               */
/* ========================================================================= */

/*****************************************************************************/
/*                          j2_input_box::read_header                        */
/*****************************************************************************/

void
  j2_input_box::read_header()
{
  remaining_bytes = 0xFFFFFFFF; // So initial reads do not fail.
  box_type = 1; // Dummy value until we can read the actual length
  if (!(read(box_bytes) && read(box_type) && (box_type != 0)))
    { box_bytes = remaining_bytes = box_type = 0; return; }
  if (box_bytes == 1)
    { // 64-bit box length
      kdu_uint32 box_bytes_high;
      if (!(read(box_bytes_high) && read(box_bytes)))
        { box_bytes = remaining_bytes = box_type = 0; return; }
      if (box_bytes_high)
        box_bytes = 0; // Can't hold such big lengths. Treat as rubber length.
      else if (box_bytes < 16)
        { kdu_error e;
          e << "Illegal extended box length encountered in JP2 file."; }
      remaining_bytes = box_bytes - 16;
    }
  else if (box_bytes != 0)
    {
      if (box_bytes < 8)
        { kdu_error e;
          e << "Illegal box length field encountered in JP2 file."; }
      remaining_bytes = box_bytes - 8;
    }
}

/*****************************************************************************/
/*                             j2_input_box::close                           */
/*****************************************************************************/

bool
  j2_input_box::close()
{
  if (box_type == 0)
    return true;
  if (box_bytes == 0)
    { // Rubber length box.
      box_bytes = remaining_bytes = box_type = 0;
      if (super_box != NULL)
        return super_box->close();
      else
        {
          char dummy;
          if (fread(&dummy,1,1,file) > 0)
            return false;
          return true;
        }
    }
  else if (remaining_bytes > 0)
    {
      if (super_box != NULL)
        super_box->ignore(remaining_bytes);
      else
        fseek(file,remaining_bytes,SEEK_CUR);
      remaining_bytes = box_bytes = box_type = 0;
      return false;
    }
  remaining_bytes = box_bytes = box_type = 0;
  return true;
}

/*****************************************************************************/
/*                            j2_input_box::ignore                           */
/*****************************************************************************/

int
  j2_input_box::ignore(int num_bytes)
{
  if (box_type == 0)
    return 0;
  if ((box_bytes != 0) && (remaining_bytes < (kdu_uint32) num_bytes))
    num_bytes = remaining_bytes;
  if (super_box != NULL)
    num_bytes = super_box->ignore(num_bytes);
  else
    {
      long int tmp_pos = ftell(file);
      fseek(file,num_bytes,SEEK_CUR);
      num_bytes = (int)(ftell(file) - tmp_pos);
    }
  remaining_bytes -= (kdu_uint32) num_bytes;
  return num_bytes;
}

/*****************************************************************************/
/*                          j2_input_box::read (array)                       */
/*****************************************************************************/

int
  j2_input_box::read(kdu_byte buf[], int num_bytes)
{
  if (box_type == 0)
    { kdu_error e; e << "Attempting to read from a closed JP2 file box."; }
  if ((box_bytes != 0) && (remaining_bytes < (kdu_uint32) num_bytes))
    num_bytes = remaining_bytes;
  if (super_box != NULL)
    num_bytes = super_box->read(buf,num_bytes);
  else
    num_bytes = fread(buf,1,(size_t) num_bytes,file);
  remaining_bytes -= (kdu_uint32) num_bytes;
  return num_bytes;
}

/*****************************************************************************/
/*                          j2_input_box::read (dword)                       */
/*****************************************************************************/

bool
  j2_input_box::read(kdu_uint32 &dword)
{
  kdu_byte buf[4];
  if (read(buf,4) < 4)
    return false;
  dword = buf[0];
  dword = (dword<<8) + buf[1];
  dword = (dword<<8) + buf[2];
  dword = (dword<<8) + buf[3];
  return true;
}

/*****************************************************************************/
/*                          j2_input_box::read (word)                        */
/*****************************************************************************/

bool
  j2_input_box::read(kdu_uint16 &word)
{
  kdu_byte buf[2];
  if (read(buf,2) < 2)
    return false;
  word = buf[0];
  word = (word<<8) + buf[1];
  return true;
}


/* ========================================================================= */
/*                               j2_output_box                               */
/* ========================================================================= */

/*****************************************************************************/
/*                        j2_output_box::write_header                        */
/*****************************************************************************/

void
  j2_output_box::write_header()
{
  if (rubber_length)
    { write((kdu_uint32) 0); write(box_type); }
  else
    {
      kdu_uint32 box_bytes = 8 + (kdu_uint32) buffered_bytes;
      rubber_length = true; // So that the header is output directly.
      write(box_bytes); write(box_type);
      rubber_length = false;
    }
}

/*****************************************************************************/
/*                      j2_output_box::set_rubber_length                     */
/*****************************************************************************/

void
  j2_output_box::set_rubber_length()
{
  assert(box_type != 0);
  if (rubber_length)
    return;
  if (super_box != NULL)
    super_box->set_rubber_length();
  rubber_length = true;
  write_header();
  if (buffer != NULL)
    {
      if (super_box != NULL)
        output_failed = !super_box->write(buffer,buffered_bytes);
      else
        output_failed = (fwrite(buffer,1,(size_t) buffered_bytes,file) !=
                         (size_t) buffered_bytes);
      delete[] buffer;
      buffer_size = buffered_bytes = 0; buffer = NULL;
    }
}

/*****************************************************************************/
/*                            j2_output_box::close                           */
/*****************************************************************************/

bool
  j2_output_box::close()
{
  if (buffer != NULL)
    {
      assert(!rubber_length);
      write_header();
      if (super_box != NULL)
        output_failed = !super_box->write(buffer,buffered_bytes);
      else
        output_failed = (fwrite(buffer,1,(size_t) buffered_bytes,file) !=
                         (size_t) buffered_bytes);
      delete[] buffer;
      buffer_size = buffered_bytes = 0; buffer = NULL;
    }
  box_type = 0;
  return !output_failed;
}

/*****************************************************************************/
/*                        j2_output_box::write (buffer)                      */
/*****************************************************************************/

bool
  j2_output_box::write(kdu_byte buf[], int num_bytes)
{
  if ((box_type == 0) || output_failed)
    return false;
  if (rubber_length)
    { // Flush data directly to the output.
      if (super_box != NULL)
        output_failed = !super_box->write(buf,num_bytes);
      else
        output_failed =
          (fwrite(buf,1,(size_t) num_bytes,file) != (size_t) num_bytes);
      return !output_failed;
    }

  buffered_bytes += num_bytes;
  if (buffered_bytes > buffer_size)
    {
      buffer_size += buffered_bytes + 1024;
      kdu_byte *tmp_buf = new kdu_byte[buffer_size];
      if (buffer != NULL)
        {
          memcpy(tmp_buf,buffer,(size_t)(buffered_bytes-num_bytes));
          delete[] buffer;
        }
      buffer = tmp_buf;
    }
  memcpy(buffer+buffered_bytes-num_bytes,buf,(size_t) num_bytes);
  return true;
}

/*****************************************************************************/
/*                         j2_output_box::write (dword)                      */
/*****************************************************************************/

bool
  j2_output_box::write(kdu_uint32 dword)
{
  kdu_byte buf[4];
  buf[3] = (kdu_byte) dword; dword >>= 8;
  buf[2] = (kdu_byte) dword; dword >>= 8;
  buf[1] = (kdu_byte) dword; dword >>= 8;
  buf[0] = (kdu_byte) dword;
  return write(buf,4);
}

/*****************************************************************************/
/*                         j2_output_box::write (word)                       */
/*****************************************************************************/

bool
  j2_output_box::write(kdu_uint16 word)
{
  kdu_byte buf[2];
  buf[1] = (kdu_byte) word; word >>= 8;
  buf[0] = (kdu_byte) word;
  return write(buf,2);
}


/* ========================================================================= */
/*                                j2_dimensions                              */
/* ========================================================================= */

/*****************************************************************************/
/*                        j2_dimensions::init (output)                       */
/*****************************************************************************/

void
  j2_dimensions::init(kdu_coords size, int num_components, bool unknown_space)
{
  if (this->num_components != 0)
    { kdu_error e; e << "JP2 dimensions may be initialized only once!"; }
  assert(num_components > 0);
  this->size = size;
  this->num_components = num_components;
  this->colour_space_unknown = unknown_space;
  ipr_box_available = false;
  bit_depths = new int[num_components];
  for (int c=0; c < num_components; c++)
    bit_depths[c] = 0; // Uninitialized state
}

/*****************************************************************************/
/*                         j2_dimensions::init (input)                       */
/*****************************************************************************/

void
  j2_dimensions::init(j2_input_box *ihdr)
{
  if (this->num_components != 0)
    { kdu_error e;
      e << "JP2 file contains multiple image header (ihdr) boxes!"; }
  assert(ihdr->get_box_type() == j2_image_header_box);
  kdu_uint32 height, width;
  kdu_uint16 nc;
  kdu_byte bpc, c_type, unk, ipr;
  if (!(ihdr->read(height) && ihdr->read(width) && ihdr->read(nc) &&
        ihdr->read(bpc) && ihdr->read(c_type) &&
        ihdr->read(unk) && ihdr->read(ipr)))
    { kdu_error e; e << "Malformed image header box (ihdr) found in JP2 "
      "file.  Not all fields were present."; }
  if (!ihdr->close())
    { kdu_error e; e << "Malformed image header box (ihdr) found in JP2 "
      "file.  The box appears to be too long."; }
  if ((nc < 1) || (nc > 16384) || (c_type != 7) ||
      (unk != (unk & 1)) || (ipr != (ipr & 1)) ||
      ((bpc != 0xFF) && ((bpc & 0x7F) > 37)))
    { kdu_error e; e << "Malformed image header box (ihdr) found in JP2 "
      "file.  The box contains fields which do not conform to their legal "
      "range."; }
  if ((height & 0x80000000) || (width & 0x80000000))
    { kdu_error e; e << "Sorry: Cannot process JP2 files whose image header "
      "box contains height and width values larger than 2^{31}-1."; }
  size.y = (int) height;
  size.x = (int) width;
  num_components = (int) nc;
  colour_space_unknown = (unk != 0);
  ipr_box_available = (ipr != 0);
  bit_depths = new int[num_components];
  for (int c=0; c < num_components; c++)
    if (bpc == 0xFF)
      bit_depths[c] = 0;
    else
      bit_depths[c] = (bpc & 0x80)?(-((bpc & 0x7F)+1)):(bpc+1);
}

/*****************************************************************************/
/*                       j2_dimensions::process_bpcc_box                     */
/*****************************************************************************/

void
  j2_dimensions::process_bpcc_box(j2_input_box *bpcc)
{
  kdu_byte bpc;

  for (int c=0; c < num_components; c++)
    if (bit_depths[c])
      { kdu_error e; e << "Encountered an illegal bits per component (bpcc) "
        "box in a JP2 header box.  The bpcc box is legal only if "
        "the image header box does not specify bit-depth information."; }
    else if (!bpcc->read(bpc))
      { kdu_error e; e << "Malformed bits per component (bpcc) box found in "
        "JP2 file.  The box contains insufficient bit-depth specifiers."; }
    else if ((bpc & 0x7F) > 37)
      { kdu_error e; e << "Malformed bits per component (bpcc) box found in "
        "JP2 file.  The box contains an illegal bit-depth specifier.  Bit "
        "depths may not exceed 38 bits per sample."; }
    else
      bit_depths[c] = (bpc & 0x80)?(-((bpc & 0x7F)+1)):(bpc+1);
  if (!bpcc->close())
    { kdu_error e; e << "Malformed bits per component (bpcc) box found in JP2 "
      "file.  The box appears to be too long."; }
}

/*****************************************************************************/
/*                           j2_dimensions::finalize                         */
/*****************************************************************************/

void
  j2_dimensions::finalize()
{
  int c;
  for (c=0; c < num_components; c++)
    if ((bit_depths[c] == 0) || (bit_depths[c] > 38) || (bit_depths[c] < -38))
      break;
  if ((num_components < 1) || (c < num_components) ||
      (num_components > 16384))
    { kdu_error e; e << "Incomplete or invalid dimensional information "
      "provided for the JP2 file image header box."; }
}

/*****************************************************************************/
/*                          j2_dimensions::save_boxes                        */
/*****************************************************************************/

void
  j2_dimensions::save_boxes(j2_output_box *super_box)
{
  finalize();
  int c;
  kdu_byte bpc = 0;
  for (c=1; c < num_components; c++)
    if (bit_depths[c] != bit_depths[0])
      bpc = 0xFF;
  if (bpc == 0)
    bpc = (kdu_byte)
      ((bit_depths[0]>0)?(bit_depths[0]-1):(0x80 | (-bit_depths[0]-1)));

  j2_output_box ihdr;
  ihdr.open(super_box,j2_image_header_box);
  ihdr.write((kdu_uint32) size.y);
  ihdr.write((kdu_uint32) size.x);
  ihdr.write((kdu_uint16) num_components);
  ihdr.write(bpc);
  ihdr.write((kdu_byte) 7);
  ihdr.write((kdu_byte)((colour_space_unknown)?1:0));
  ihdr.write((kdu_byte)((ipr_box_available)?1:0));
  ihdr.close();
  if (bpc != 0xFF)
    return;

  j2_output_box bpcc;
  bpcc.open(super_box,j2_bits_per_component_box);
  for (c=0; c < num_components; c++)
    {
      bpc = (kdu_byte)
        ((bit_depths[c]>0)?(bit_depths[c]-1):(0x80 | (-bit_depths[c]-1)));
      bpcc.write(bpc);
    }
  bpcc.close();
}


/* ========================================================================= */
/*                               jp2_dimensions                              */
/* ========================================================================= */

/*****************************************************************************/
/*                            jp2_dimensions::init                           */
/*****************************************************************************/

void
  jp2_dimensions::init(kdu_coords size, int num_components, bool unknown_space)
{
  assert(state != NULL);
  state->init(size,num_components,unknown_space);
}

/*****************************************************************************/
/*                          jp2_dimensions::init (siz)                       */
/*****************************************************************************/

void
  jp2_dimensions::init(siz_params *siz, bool unknown_space)
{
  kdu_coords size, origin;
  int num_components;

  if (!(siz->get(Ssize,0,0,size.y) && siz->get(Ssize,0,1,size.x) &&
        siz->get(Sorigin,0,0,origin.y) && siz->get(Sorigin,0,1,origin.x) &&
        siz->get(Scomponents,0,0,num_components)))
    { kdu_error e; e << "Attempting to initialize a `jp2_dimensions' object "
      "using an incomplete `siz_params' object."; }
  size -= origin;
  init(size,num_components,unknown_space);
  for (int c=0; c < num_components; c++)
    {
      bool is_signed;
      int bit_depth;
      if (!(siz->get(Ssigned,c,0,is_signed) &&
            siz->get(Sprecision,c,0,bit_depth)))
        { kdu_error e; e << "Attempting to initialize a `jp2_dimensions' "
          "object using an incomplete `siz_params' object."; }
      set_precision(c,bit_depth,is_signed);
    }
}

/*****************************************************************************/
/*                        jp2_dimensions::set_precision                      */
/*****************************************************************************/

void
  jp2_dimensions::set_precision(int component_idx, int bit_depth,
                                bool is_signed)
{
  assert((state != NULL) && (component_idx >= 0) &&
         (component_idx < state->num_components));
  state->bit_depths[component_idx] = (is_signed)?-bit_depth:bit_depth;
}

/*****************************************************************************/
/*                          jp2_dimensions::get_size                         */
/*****************************************************************************/

kdu_coords
  jp2_dimensions::get_size()
{
  assert(state != NULL);
  return state->size;
}

/*****************************************************************************/
/*                     jp2_dimensions::get_num_components                    */
/*****************************************************************************/

int
  jp2_dimensions::get_num_components()
{
  assert(state != NULL);
  return state->num_components;
}

/*****************************************************************************/
/*                     jp2_dimensions::colour_space_known                    */
/*****************************************************************************/

bool
  jp2_dimensions::colour_space_known()
{
  assert(state != NULL);
  return state->colour_space_unknown;
}

/*****************************************************************************/
/*                        jp2_dimensions::get_bit_depth                      */
/*****************************************************************************/

int
  jp2_dimensions::get_bit_depth(int component_idx)
{
  assert((state != NULL) && (component_idx >= 0) &&
         (component_idx < state->num_components));
  int depth = state->bit_depths[component_idx];
  return (depth < 0)?-depth:depth;
}

/*****************************************************************************/
/*                         jp2_dimensions::get_signed                        */
/*****************************************************************************/

bool
  jp2_dimensions::get_signed(int component_idx)
{
  assert((state != NULL) && (component_idx >= 0) &&
         (component_idx < state->num_components));
  return (state->bit_depths[component_idx] < 0);
}


/* ========================================================================= */
/*                                  j2_palette                               */
/* ========================================================================= */

/*****************************************************************************/
/*                          j2_palette::init (output)                        */
/*****************************************************************************/

void
  j2_palette::init(int num_components, int num_entries)
{
  if (this->num_components != 0)
    { kdu_error e;
      e << "JP2 palette information may be initialized only once!"; }
  assert((num_components > 0) && (num_components < 256));
  this->num_components = num_components;
  this->num_entries = num_entries;
  bit_depths = new int[num_components];
  luts = new kdu_int32 *[num_components];
  for (int c=0; c < num_components; c++)
    {
      bit_depths[c] = 0;
      luts[c] = new kdu_int32[num_entries];
    }
}

/*****************************************************************************/
/*                          j2_palette::init (input)                         */
/*****************************************************************************/

void
  j2_palette::init(j2_input_box *pclr)
{
  if (this->num_components != 0)
    { kdu_error e;
      e << "JP2 file contains multiple palette (pclr) boxes!"; }
  assert(pclr->get_box_type() == j2_palette_box);
  kdu_uint16 ne;
  kdu_byte npc;
  if (!(pclr->read(ne) && pclr->read(npc) &&
        (ne >= 1) && (ne <= 1024) && (npc >= 1)))
    { kdu_error e; e << "Malformed palette (pclr) box found in JP2 file.  "
      "Insufficient or illegal fields encountered."; }
  num_components = npc;
  num_entries = ne;

  int c;
  bit_depths = new int[num_components];
  for (c=0; c < num_components; c++)
    {
      kdu_byte bpc;

      if (!pclr->read(bpc))
        { kdu_error e; e << "Malformed palette (pclr) box found in "
          "JP2 file.  The box contains insufficient bit-depth specifiers."; }
      else if ((bpc & 0x7F) > 37)
        { kdu_error e; e << "Malformed palette (pclr) box found in "
          "JP2 file.  The box contains an illegal bit-depth specifier.  Bit "
          "depths may not exceed 38 bits per sample."; }
      else
        bit_depths[c] = (bpc & 0x80)?(-((bpc & 0x7F)+1)):(bpc+1);
    }

  luts = new kdu_int32 *[num_components];
  for (c=0; c < num_components; c++)
    luts[c] = new kdu_int32[num_entries];
  for (c=0; c < num_components; c++)
    {
      int bits = (bit_depths[c] < 0)?(-bit_depths[c]):bit_depths[c];
      int entry_bytes = (bits+7)>>3;
      int downshift = bits-32;
      downshift = (downshift < 0)?0:downshift;
      int upshift = 32+downshift-bits;
      kdu_int32 val, offset = (bit_depths[c]<0)?0:KDU_INT32_MIN;
      kdu_byte val_buf[5]; assert((entry_bytes <= 5) && (entry_bytes > 0));
      for (int n=0; n < num_entries; n++)
        {
          if (pclr->read(val_buf,entry_bytes) != entry_bytes)
            { kdu_error e; e << "Malformed palette (pclr) box found in JP2 "
              "file.  The box contains insufficient palette entries."; }

          val = val_buf[0];
          if (entry_bytes > 1)
            {
              val = (val<<8) + val_buf[1];
              if (entry_bytes > 2)
                {
                  val = (val<<8) + val_buf[2];
                  if (entry_bytes > 3)
                    {
                      val = (val<<8) + val_buf[3];
                      if (entry_bytes > 4)
                        {
                          val <<= (8-downshift);
                          val += (val_buf[4] >> downshift);
                        }
                    }
                }
            }
          val <<= upshift;
          val += offset;
          (luts[c])[n] = val;
        }
      if (downshift)
        bit_depths[c] = (bit_depths[c]<0)?-32:32;
    }

  if (!pclr->close())
    { kdu_error e; e << "Malformed palette (pclr) box found in JP2 file.  "
      "Box appears to be too long."; }
}


/*****************************************************************************/
/*                             j2_palette::finalize                          */
/*****************************************************************************/

void
  j2_palette::finalize()
{
  if (num_components == 0)
    return;
  int c;
  for (c=0; c < num_components; c++)
    if ((bit_depths[c] == 0) || (bit_depths[c] > 32) || (bit_depths[c] < -32))
      break;
  if ((num_components < 1) || (c < num_components) ||
      (num_components > 255) || (num_entries < 1) || (num_entries > 1024))
    { kdu_error e; e << "Incomplete or invalid information "
      "provided for the JP2 file palette box."; }
}

/*****************************************************************************/
/*                            j2_palette::save_box                           */
/*****************************************************************************/

void
  j2_palette::save_box(j2_output_box *super_box)
{
  if (num_components == 0)
    return;
  finalize();
  j2_output_box pclr;
  pclr.open(super_box,j2_palette_box);
  pclr.write((kdu_uint16) num_entries);
  pclr.write((kdu_byte) num_components);

  int c;
  kdu_byte bpc;
  for (c=0; c < num_components; c++)
    {
      bpc = (kdu_byte)
        ((bit_depths[c]>0)?(bit_depths[c]-1):(0x80 | (-bit_depths[c]-1)));
      pclr.write(bpc);
    }

  for (c=0; c < num_components; c++)
    {
      int bits = (bit_depths[c] < 0)?(-bit_depths[c]):bit_depths[c];
      int entry_bytes = (bits+7)>>3;
      int downshift = 32 - bits; assert(downshift >= 0);
      kdu_int32 offset = (bit_depths[c]<0)?0:KDU_INT32_MIN;
      kdu_uint32 val;
      kdu_byte val_buf[4]; assert((entry_bytes > 0) && (entry_bytes <= 4));
      for (int n=0; n < num_entries; n++)
        {
          val = (kdu_uint32)((luts[c])[n] - offset);
          val >>= downshift; // Most significant bits hold 0's
          val_buf[entry_bytes-1] = (kdu_byte) val;
          if (entry_bytes > 1)
            {
              val >>= 8; val_buf[entry_bytes-2] = (kdu_byte) val;
              if (entry_bytes > 2)
                {
                  val >>= 8; val_buf[entry_bytes-3] = (kdu_byte) val;
                  if (entry_bytes > 3)
                    {
                      val >>= 8; val_buf[entry_bytes-4] = (kdu_byte) val;
                    }
                }
            }
          pclr.write(val_buf,entry_bytes);
        }
    }
  pclr.close();
}

/* ========================================================================= */
/*                                  jp2_palette                              */
/* ========================================================================= */

/*****************************************************************************/
/*                               jp2_palette::init                           */
/*****************************************************************************/

void
  jp2_palette::init(int num_components, int num_entries)
{
  assert(state != NULL);
  state->init(num_components,num_entries);
}

/*****************************************************************************/
/*                              jp2_palette::set_lut                         */
/*****************************************************************************/

void
  jp2_palette::set_lut(int comp_idx, kdu_int32 *lut, int bit_depth,
                       bool is_signed)
{
  assert((state != NULL) &&
         (comp_idx >= 0) && (comp_idx < state->num_components) &&
         (bit_depth <= 32) && (bit_depth >= 1));
  state->bit_depths[comp_idx] = (is_signed)?(-bit_depth):bit_depth;
  int upshift = 32-bit_depth;
  kdu_int32 offset = (is_signed)?0:KDU_INT32_MIN;
  kdu_int32 *dst = state->luts[comp_idx];
  for (int n=0; n < state->num_entries; n++)
    dst[n] = (lut[n] << upshift) + offset;
}

/*****************************************************************************/
/*                         jp2_palette::get_num_entries                      */
/*****************************************************************************/

int
  jp2_palette::get_num_entries()
{
  assert(state != NULL);
  return state->num_entries;
}

/*****************************************************************************/
/*                        jp2_palette::get_num_components                    */
/*****************************************************************************/

int
  jp2_palette::get_num_components()
{
  assert(state != NULL);
  return state->num_components;
}

/*****************************************************************************/
/*                          jp2_palette::get_bit_depth                       */
/*****************************************************************************/

int
  jp2_palette::get_bit_depth(int comp_idx)
{
  assert((state != NULL) &&
         (comp_idx >= 0) && (comp_idx < state->num_components));
  int depth = state->bit_depths[comp_idx];
  return (depth<0)?-depth:depth;
}

/*****************************************************************************/
/*                           jp2_palette::get_signed                         */
/*****************************************************************************/

bool
  jp2_palette::get_signed(int comp_idx)
{
  assert((state != NULL) &&
         (comp_idx >= 0) && (comp_idx < state->num_components));
  return (state->bit_depths[comp_idx] < 0);
}

/*****************************************************************************/
/*                        jp2_palette::get_lut (float)                       */
/*****************************************************************************/

void
  jp2_palette::get_lut(int comp_idx, float lut[])
{
  assert((state != NULL) &&
         (comp_idx >= 0) && (comp_idx < state->num_components));
  kdu_int32 *src = state->luts[comp_idx];
  float scale = 1.0F / (((float)(1<<16)) * ((float)(1<<16)));
  for (int n=0; n < state->num_entries; n++)
    lut[n] = ((float) src[n]) * scale;
}

/*****************************************************************************/
/*                      jp2_palette::get_lut (fixed point)                   */
/*****************************************************************************/

void
  jp2_palette::get_lut(int comp_idx, kdu_sample16 lut[])
{
  assert((state != NULL) &&
         (comp_idx >= 0) && (comp_idx < state->num_components));
  kdu_int32 *src = state->luts[comp_idx];
  kdu_int32 downshift = 32 - KDU_FIX_POINT;
  kdu_int32 offset = (1<<downshift)>>1;
  for (int n=0; n < state->num_entries; n++)
    lut[n].ival = (kdu_int16)((src[n]+offset)>>downshift);
}


/* ========================================================================= */
/*                                j2_channels                                */
/* ========================================================================= */

/*****************************************************************************/
/*                          j2_channels::j2_channels                         */
/*****************************************************************************/

j2_channels::j2_channels()
{
  int c;

  num_colours = num_components = num_palette_components = 0;
  num_cmap_channels = 0;
  cmap_channels = NULL;
  for (c=0; c < 3; c++)
    { // Set up default mapping in case no colour definition box.
      channel_functions[c].source_component = c;
      channel_functions[c].palette_component = -1;
    }
  for (; c < 9; c++)
    {
      channel_functions[c].source_component = -1;
      channel_functions[c].palette_component = -1;
    }
}

/*****************************************************************************/
/*                       j2_channels::process_cdef_box                       */
/*****************************************************************************/

void
  j2_channels::process_cdef_box(j2_input_box *cdef)
{
  assert(cdef->get_box_type() == j2_channel_definition_box);
  kdu_uint16 num_descriptions, n;

  if (!(cdef->read(num_descriptions) && (num_descriptions > 0)))
    { kdu_error e; e << "Malformed channel definition (cdef) box found in JP2 "
      "file.  Missing or invalid fields."; }
  for (n=0; n < num_descriptions; n++)
    {
      kdu_uint16 channel_idx, typ, assoc;
      if (!(cdef->read(channel_idx) && cdef->read(typ) && cdef->read(assoc) &&
            ((typ < 3) || (typ == (kdu_uint16) 0xFFFF))))
        { kdu_error e; e << "Malformed channel definition (cdef) box found "
          "in JP2 file.  Missing or invalid channel association information.";}
      if ((assoc > 3) || (typ > 2))
        continue; // Channel has no defined function in JP2
      if (assoc > 0)
        channel_functions[assoc-1+3*typ].source_component = channel_idx;
      else
        channel_functions[3*typ].source_component =
          channel_functions[3*typ+1].source_component =
            channel_functions[3*typ+2].source_component = channel_idx;
    }

  if (!cdef->close())
    { kdu_error e; e << "Malformed channel definition (cdef) box found in JP2 "
      "file.  The box appears to be too long."; }
}

/*****************************************************************************/
/*                       j2_channels::process_cmap_box                       */
/*****************************************************************************/

void
  j2_channels::process_cmap_box(j2_input_box *cmap)
{
  assert(cmap->get_box_type() == j2_component_mapping_box);
  if ((cmap_channels != NULL) || num_cmap_channels)
    { kdu_error e; e << "Multiple instances of the component mapping (cmap) "
      "box encountered in JP2 file!"; }
  int box_bytes = cmap->get_remaining_bytes();
  if ((box_bytes & 3) || (box_bytes == 0))
    { kdu_error e; e << "Malformed component mapping (cmap) box encountered "
      "in JP2 file.  The body of any such box must contain exactly four "
      "bytes for each channel and there must be at least one channel."; }
  num_cmap_channels = box_bytes >> 2;
  if (num_cmap_channels < 1)
    { kdu_error e; e << "Malformed component mapping (cmap) box encountered "
      "in JP2 file.  The body of the box does not appear to contain any "
      "channel mappings."; }
  cmap_channels = new j2_channel[num_cmap_channels];
  for (int n=0; n < num_cmap_channels; n++)
    {
      kdu_uint16 cmp;
      kdu_byte mtyp, pcol;
      if (!(cmap->read(cmp) && cmap->read(mtyp) && cmap->read(pcol) &&
            (mtyp < 2)))
        { kdu_error e; e << "Malformed component mapping (cmap) box "
          "encountered in JP2 file.  Invalid or truncated mapping specs."; }
      cmap_channels[n].source_component = cmp;
      cmap_channels[n].palette_component = (mtyp)?((int) pcol):-1;
    }

  cmap->close();
}

/*****************************************************************************/
/*                           j2_channels::finalize                           */
/*****************************************************************************/

void
  j2_channels::finalize(int num_colours, int num_components,
                        int num_palette_components)
{
  int c, n;

  if (this->num_components != 0)
    { // already finalized.
      assert((num_colours == this->num_colours) &&
             (num_components == this->num_components) &&
             (num_palette_components == this->num_palette_components));
      return;
    }
  assert((num_colours == 1) || (num_colours == 3));
  if (num_colours == 1)
    for (c=0; c < 9; c+=3) // Eliminate unused colours
      channel_functions[c+1].source_component =
        channel_functions[c+2].source_component = -1;
  if (this->num_colours == 0)
    { // This is either a default output object or an input object.  Need
      // to merge information collected from cdef and/or cmap boxes.
      this->num_colours = num_colours;
      this->num_components = num_components;
      this->num_palette_components = num_palette_components;
      if (cmap_channels != NULL)
        { // Transfer information to the `channel_functions' array.
          for (c=0; c < 9; c++)
            if ((n=channel_functions[c].source_component) >= 0)
              {
                if (n >= num_cmap_channels)
                  { kdu_error e; e << "The JP2 file's colour definition "
                    "(cdef) box references a channel which is not defined "
                    "within the colour mapping (cmap) box."; }
                channel_functions[c] = cmap_channels[n];
              }
          delete[] cmap_channels;
          cmap_channels = NULL;
          num_cmap_channels = 0;
        }
    }
  else
    { // This is an output object.
      if (this->num_colours != num_colours)
        { kdu_error e; e << "Attempting to use a `jp2_channels' object "
          "whose number of colours differs from that specified by the "
          "`jp2_colour' object."; }
      this->num_components = num_components;
      this->num_palette_components = num_palette_components;

      // Check requirement that no image component be used both as
      // input to a palette LUT and also directly.
      for (c=0; c < 9; c++)
        if (channel_functions[c].palette_component >= 0)
          for (n=0; n < 9; n++)
            if ((channel_functions[n].source_component ==
                 channel_functions[c].source_component) &&
                (channel_functions[n].palette_component < 0))
              { kdu_error e; e << "It is illegal to use any image "
                "component directly as well as through a colour palette "
                "mapping."; }
      // Check requirement that no image component or component/palette
      // combination be used for two different types of reproduction
      // function.
      for (c=0; c < 6; c++)
        {
          for (n=(c<3)?3:6; n < 9; n++)
            if ((channel_functions[c].source_component >= 0) &&
                (channel_functions[c].source_component ==
                 channel_functions[n].source_component) &&
                (channel_functions[c].palette_component ==
                 channel_functions[n].palette_component))
              { kdu_error e; e << "It is illegal to specify the same "
                "source component/palette LUT combination for use with "
                "different types of colour reproduction functions, where "
                "the three types are colour, opacity and pre-multiplied "
                "opacity."; }
        }
    }

  // Finally, perform some general checks concerning the range of image
  // component and palette component indices and make sure that the
  // minimal set of colour functions are specified.

  for (c=0; c < num_colours; c++)
    if (channel_functions[c].source_component < 0)
      { kdu_error e; e << "Insufficient channel association information for "
        "a valid JP2 file.  Every colour channel must be associated with an "
        "image component in some way or another."; }
  for (c=0; c < 9; c++)
    if (channel_functions[c].source_component >= num_components)
      { kdu_error e; e << "Attempting to describe a colour channel in terms "
        "of a non-existent image component!"; }
    else if (channel_functions[c].palette_component >= num_palette_components)
      { kdu_error e; e << "Attempting to describe a colour channel in terms "
        "of a non-existent palette component (lookup table)."; }
}

/*****************************************************************************/
/*                           j2_channels::save_boxes                          */
/*****************************************************************************/

void
  j2_channels::save_boxes(j2_output_box *super_box)
{
  assert((num_colours > 0) && (num_components > 0)); // ensure `finalized'.
  int c, n;

  // First, determine the number of unmapped image components.  For some
  // reason, JP2 seems to require that information for these also appear
  // in the channel definition and/or component mapping boxes.
  int num_unmapped_components = 0;
  for (n=0; n < num_components; n++)
    {
      for (c=0; c < 9; c++)
        if (channel_functions[c].source_component == n)
          break;
      if (c == 9)
        num_unmapped_components++;
    }

  // Now see what boxes need to be written at all.
  bool need_boxes = false;
  for (c=0; c < num_colours; c++)
    if (channel_functions[c].source_component != c)
      need_boxes = true;
  for (; c < 9; c++)
    if (channel_functions[c].source_component >= 0)
      need_boxes = true;
  if (num_unmapped_components > 0)
    need_boxes = true;
  if (num_palette_components > 0)
    need_boxes = true;
  if (!need_boxes)
    return;

  bool need_cmap_box =
    (num_palette_components > 0); // cmap used if and only palette box appears
  int channel_indices[9] = {-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int num_functional_cmap_channels = 0;
  if (need_cmap_box)
    { // Assign a unique channel index to each colour function
      for (c=0; c < 9; c++)
        if (channel_functions[c].source_component >= 0)
          {
            for (n=0; n < c; n++)
              if (channel_functions[c] == channel_functions[n])
                break;
            if (n < c)
              channel_indices[c] = channel_indices[n];
            else
              channel_indices[c] = num_functional_cmap_channels++;
          }
    }

  // Start with the channel definition box.
  j2_output_box cdef;
  cdef.open(super_box,j2_channel_definition_box);
  int num_descriptions = 0;
  for (c=0; c < 9; c++)
    if (channel_functions[c].source_component >= 0)
      num_descriptions++;
  num_descriptions += num_unmapped_components;
  cdef.write((kdu_uint16) num_descriptions);
  for (c=0; c < 9; c++)
    if ((n = channel_functions[c].source_component) >= 0)
      {
        if (need_cmap_box)
          n = channel_indices[c];
        kdu_uint16 typ = (kdu_uint16)(c / 3);
        kdu_uint16 assoc = (kdu_uint16)((c % 3) + 1);
        cdef.write((kdu_uint16) n);
        cdef.write(typ);
        cdef.write(assoc);
      }
  if (need_cmap_box)
    { // We will create extra channels at the end for unmapped components.
      for (n=0; n < num_unmapped_components; n++)
        {
          cdef.write((kdu_uint16)(n+num_functional_cmap_channels));
          cdef.write((kdu_uint16) 0xFFFF);
          cdef.write((kdu_uint16) 0xFFFF);
        }
    }
  else
    { // Identify unmapped components directly.
      for (n=0; n < num_components; n++)
        {
          for (c=0; c < 9; c++)
            if (channel_functions[c].source_component == n)
              break;
          if (c == 9)
            {
              cdef.write((kdu_uint16) n);
              cdef.write((kdu_uint16) 0xFFFF);
              cdef.write((kdu_uint16) 0xFFFF);
              num_unmapped_components--; // Check them off as we go.
            }
        }
      assert(num_unmapped_components == 0);
    }
  cdef.close();
  if (!need_cmap_box)
    return;

  // Finish with the component mapping box, if any.
  j2_output_box cmap;
  cmap.open(super_box,j2_component_mapping_box);
  for (n=0; n < num_functional_cmap_channels; n++)
    {
      for (c=0; c < 9; c++)
        if (channel_indices[c] == n)
          break;
      assert(c < 9);
      cmap.write((kdu_uint16)(channel_functions[c].source_component));
      if (channel_functions[c].palette_component < 0)
        cmap.write((kdu_uint16) 0);
      else
        {
          cmap.write((kdu_byte) 1);
          cmap.write((kdu_byte)(channel_functions[c].palette_component));
        }
    }
  // Now create extra channels for any unmapped components.
  for (n=0; n < num_components; n++)
    {
      for (c=0; c < 9; c++)
        if (channel_functions[c].source_component == n)
          break;
      if (c == 9)
        {
          cmap.write((kdu_uint16) n);
          cmap.write((kdu_uint16) 0); // Unmapped components always direct
          num_unmapped_components--;
        }
    }
  assert(num_unmapped_components == 0);
  
  cmap.close();
}


/* ========================================================================= */
/*                                jp2_channels                               */
/* ========================================================================= */

/*****************************************************************************/
/*                             jp2_channels::init                            */
/*****************************************************************************/

void
  jp2_channels::init(int num_colours)
{
  assert(state != NULL);
  assert((num_colours == 1) || (num_colours == 3));
  state->num_colours = num_colours;
  for (int c=num_colours; c < 9; c++)
    state->channel_functions[c].source_component = -1;
}

/*****************************************************************************/
/*                      jp2_channels::set_colour_mapping                     */
/*****************************************************************************/

void
  jp2_channels::set_colour_mapping(int colour_idx, int codestream_component,
                                   int palette_component)
{
  assert((state != NULL) &&
         (colour_idx >= 0) && (colour_idx < state->num_colours));
  if (palette_component < 0)
    palette_component = -1; // Standardize to allow comparisons in `finalize'
  state->channel_functions[colour_idx].source_component = codestream_component;
  state->channel_functions[colour_idx].palette_component = palette_component;
}

/*****************************************************************************/
/*                     jp2_channels::set_opacity_mapping                     */
/*****************************************************************************/

void
  jp2_channels::set_opacity_mapping(int colour_idx, int codestream_component,
                                    int palette_component)
{
  assert((state != NULL) &&
         (colour_idx >= 0) && (colour_idx < state->num_colours));
  if (palette_component < 0)
    palette_component = -1; // Standardize to allow comparisons in `finalize'
  colour_idx += 3;
  state->channel_functions[colour_idx].source_component = codestream_component;
  state->channel_functions[colour_idx].palette_component = palette_component;
}

/*****************************************************************************/
/*                     jp2_channels::set_premult_mapping                     */
/*****************************************************************************/

void
  jp2_channels::set_premult_mapping(int colour_idx, int codestream_component,
                                    int palette_component)
{
  assert((state != NULL) &&
         (colour_idx >= 0) && (colour_idx < state->num_colours));
  if (palette_component < 0)
    palette_component = -1; // Standardize to allow comparisons in `finalize'
  colour_idx += 6;
  state->channel_functions[colour_idx].source_component = codestream_component;
  state->channel_functions[colour_idx].palette_component = palette_component;
}

/*****************************************************************************/
/*                       jp2_channels::get_num_colours                       */
/*****************************************************************************/

int
  jp2_channels::get_num_colours()
{
  assert(state != NULL);
  return state->num_colours;
}

/*****************************************************************************/
/*                      jp2_channels::get_colour_mapping                     */
/*****************************************************************************/

bool
  jp2_channels::get_colour_mapping(int colour_idx, int &codestream_component,
                                   int &palette_component)
{
  assert((state != NULL) &&
         (colour_idx >= 0) && (colour_idx < state->num_colours));
  if (state->channel_functions[colour_idx].source_component < 0)
    return false;
  codestream_component = state->channel_functions[colour_idx].source_component;
  palette_component = state->channel_functions[colour_idx].palette_component;
  return true;
}

/*****************************************************************************/
/*                     jp2_channels::get_opacity_mapping                     */
/*****************************************************************************/

bool
  jp2_channels::get_opacity_mapping(int colour_idx, int &codestream_component,
                                    int &palette_component)
{
  assert((state != NULL) &&
         (colour_idx >= 0) && (colour_idx < state->num_colours));
  colour_idx += 3;
  if (state->channel_functions[colour_idx].source_component < 0)
    return false;
  codestream_component = state->channel_functions[colour_idx].source_component;
  palette_component = state->channel_functions[colour_idx].palette_component;
  return true;
}

/*****************************************************************************/
/*                     jp2_channels::get_premult_mapping                     */
/*****************************************************************************/

bool
  jp2_channels::get_premult_mapping(int colour_idx, int &codestream_component,
                                    int &palette_component)
{
  assert((state != NULL) &&
         (colour_idx >= 0) && (colour_idx < state->num_colours));
  colour_idx += 6;
  if (state->channel_functions[colour_idx].source_component < 0)
    return false;
  codestream_component = state->channel_functions[colour_idx].source_component;
  palette_component = state->channel_functions[colour_idx].palette_component;
  return true;
}


/* ========================================================================= */
/*                              j2_icc_profile                               */
/* ========================================================================= */

/*****************************************************************************/
/*                     j2_icc_profile::init (profile buf)                    */
/*****************************************************************************/

void
  j2_icc_profile::init(kdu_byte *profile_buf, bool donate_buffer)
{
  kdu_uint32 val32;
  
  buffer = profile_buf;
  num_buffer_bytes = 4; read(val32,0); num_buffer_bytes = (int) val32;
  if (!donate_buffer)
    buffer = NULL; // Just in case we get destroyed thru an exception catcher
  if (num_buffer_bytes < 132)
    { kdu_error e; e << "Embedded ICC profile for JP2 file does not have a "
      "complete header."; }

  // Copy the buffer.
  if (!donate_buffer)
    {
      buffer = new kdu_byte[num_buffer_bytes];
      memcpy(buffer,profile_buf,(size_t) num_buffer_bytes);
    }

  // Check the profile and locate the relevant tags, recording their locations
  // and lengths in the relevant member arrays.
  read(val32,12); // Get profile/device class signature.
  if (val32 != icc_input_device)
    { kdu_error e; e << "Embedded ICC profile for JP2 file must contain "
      "an input device profile (as opposed to a display profile, output "
      "profile, link profile, etc.)."; }
  read(val32,16);
  if (val32 == icc_gray_data)
    num_colours = 1;
  else
    num_colours = 3;
  read(val32,128);
  num_tags = (int) val32;
  int t;
  for (t=0; t < 3; t++)
    trc_offsets[t] = colorant_offsets[t] = 0;
  for (t=0; t < num_tags; t++)
    {
      kdu_uint32 signature, offset, length;
      if (!(read(signature,12*t+132) && read(offset,12*t+136) &&
            read(length,12*t+140)))
        { kdu_error e; e << "Embedded ICC profile for JP2 file appears to "
          "have been truncated!"; }
      if (signature == icc_gray_trc)
        trc_offsets[0] = get_curve_data_offset(offset,length);
      else if (signature == icc_red_trc)
        trc_offsets[0] = get_curve_data_offset(offset,length);
      else if (signature == icc_green_trc)
        trc_offsets[1] = get_curve_data_offset(offset,length);
      else if (signature == icc_blue_trc)
        trc_offsets[2] = get_curve_data_offset(offset,length);
      else if (signature == icc_red_colorant)
        colorant_offsets[0] = get_xyz_data_offset(offset,length);
      else if (signature == icc_green_colorant)
        colorant_offsets[1] = get_xyz_data_offset(offset,length);
      else if (signature == icc_blue_colorant)
        colorant_offsets[2] = get_xyz_data_offset(offset,length);
    }
  for (t=0; t < num_colours; t++)
    if (trc_offsets[t] == 0)
      { kdu_error e; e << "Embedded ICC profile for JP2 file does not contain "
        "a complete set of tone reproduction curves!"; }
  if (num_colours == 3)
    for (t=0; t < 3; t++)
      if (colorant_offsets[t] == 0)
        { kdu_error e; e << "Embedded ICC profile for JP2 file does not "
          "contain a complete set of primary colorant specifications."; }
}



/*****************************************************************************/
/*                           j2_icc_profile::get_lut                         */
/*****************************************************************************/

void
  j2_icc_profile::get_lut(int channel_idx, float lut[], int index_bits)
{
  kdu_uint32 val32;
  kdu_uint16 val16;
  assert((channel_idx >= 0) && (channel_idx < num_colours));
  int offset = trc_offsets[channel_idx]; assert(offset > 128);
  int p, num_points; read(val32,offset); num_points = val32;
  offset += 4; // Get offset to first data point.
  int n, lut_entries = 1<<index_bits;

  if (num_points == 0)
    { // Curve is straight line from 0 to 1.
      float delta = 1.0F / ((float)(lut_entries-1));
      for (n=0; n < lut_entries; n++)
        lut[n] = n*delta;
    }
  else if (num_points == 1)
    { // Curve is a pure power law.
      read(val16,offset); offset += 2;
      float exponent = ((float) val16) / 256.0F;
      float delta = 1.0F / ((float)(lut_entries-1));
      for (n=0; n < lut_entries; n++)
        lut[n] = (float) pow(n*delta,exponent);
    }
  else
    {
      float lut_delta = ((float)(num_points-1)) / ((float)(lut_entries-1));
            // Holds the separation between lut entries, relative to that
            // between curve data points.
      float lut_pos = 0.0F;
      read(val16,offset); offset += 2;
      float last_val = ((float) val16) / ((float)((1<<16)-1));
      read(val16,offset); offset += 2;
      float next_val = ((float) val16) / ((float)((1<<16)-1));
      for (p=1, n=0; n < lut_entries; n++, lut_pos += lut_delta)
        {
          while (lut_pos > 1.0F)
            { // Need to advance points.
              last_val = next_val;
              lut_pos -= 1.0F;
              p++;
              if (p < num_points)
                {
                  read(val16,offset); offset += 2;
                  next_val = ((float) val16) / ((float)((1<<16)-1));
                }
            }
          lut[n] = next_val*lut_pos + last_val*(1.0F-lut_pos);
        }
    }
  assert ((offset-trc_offsets[channel_idx]) == (2*num_points+4));
}

/*****************************************************************************/
/*                          j2_icc_profile::get_matrix                       */
/*****************************************************************************/

void
  j2_icc_profile::get_matrix(float matrix3x3[])
{
  assert(num_colours == 3);
  for (int c=0; c < 3; c++)
    {
      int offset = colorant_offsets[c]; assert(offset > 128);
      for (int t=0; t < 3; t++)
        {
          kdu_uint32 uval; read(uval,offset); offset += 4;
          kdu_int32 sval = (kdu_int32) uval;
          matrix3x3[c+3*t] = ((float) sval) / ((float)(1<<16));
        }
    }
}

/*****************************************************************************/
/*                     j2_icc_profile::get_curve_data_offset                 */
/*****************************************************************************/

int
  j2_icc_profile::get_curve_data_offset(int tag_offset, int tag_length)
{
  if ((tag_length+tag_offset) > num_buffer_bytes)
    { kdu_error e; e << "Illegal tag offset or length value supplied in "
      "the JP2 embedded icc profile."; }
  kdu_uint32 val32; read(val32,tag_offset);
  if ((val32 != icc_curve_type) || (tag_length < 12))
    { kdu_error e; e << "Did not find a valid `curv' data type "
      "in the embedded ICC profile's tone reproduction curve tag."; }
  read(val32,tag_offset+4); read(val32,tag_offset+8);
  int num_points = (int) val32;
  if (tag_length != ((2*num_points)+12))
    { kdu_error e; e << "The `curv' data type used to represent an embedded "
      "ICC profile's tone reproduction curve appears to have been truncated.";}
  return tag_offset+8;
}

/*****************************************************************************/
/*                     j2_icc_profile::get_xyz_data_offset                   */
/*****************************************************************************/

int
  j2_icc_profile::get_xyz_data_offset(int tag_offset, int tag_length)
{
  if ((tag_length+tag_offset) > num_buffer_bytes)
    { kdu_error e; e << "Illegal tag offset or length value supplied in "
      "JP2 embedded icc profile."; }
  kdu_uint32 val32; read(val32,tag_offset);
  if ((val32 != icc_xyz_type) || (tag_length < 20))
    { kdu_error e; e << "Did not find a valid `XYZ ' data type "
      "in the embedded ICC profile's colorant description tag."; }
  return tag_offset + 8;
}

/* ========================================================================= */
/*                                 j2_colour                                 */
/* ========================================================================= */

/*****************************************************************************/
/*                           j2_colour::init (space)                         */
/*****************************************************************************/

void
  j2_colour::init(jp2_colour_space space)
{
  if ((num_colours != 0) || (icc_profile != NULL))
    { kdu_error e;
      e << "JP2 colour information may be initialized only once!"; }
  assert((space != JP2_iccLUM_SPACE) && (space != JP2_iccRGB_SPACE));
  this->space = space;
  this->icc_profile = NULL;
  this->num_colours = (space == JP2_sLUM_SPACE)?1:3;
}

/*****************************************************************************/
/*                            j2_colour::init (icc)                          */
/*****************************************************************************/

void
  j2_colour::init(j2_icc_profile *profile)
{
  if ((num_colours != 0) || (icc_profile != NULL))
    { kdu_error e;
      e << "JP2 colour information may be initialized only once!"; }
  assert(profile != NULL);
  icc_profile = profile;
  num_colours = profile->get_num_colours();
  space = (num_colours==1)?JP2_iccLUM_SPACE:JP2_iccRGB_SPACE;
}

/*****************************************************************************/
/*                            j2_colour::init (box)                          */
/*****************************************************************************/

void
  j2_colour::init(j2_input_box *colr)
{
  if ((num_colours != 0) || (icc_profile != NULL))
    { // Ignore all but the first colour box.
      colr->close();
      return;
    }
  assert(colr->get_box_type() == j2_colour_box);
  kdu_byte meth, prec, approx;

  if (!(colr->read(meth) && colr->read(prec) && colr->read(approx) &&
        (meth >= 1) && (meth <= 2)))
    { kdu_error e; e << "Malformed colour specification (colr) box found in "
      "JP2 file.  Insufficient or illegal fields found in box."; }
  if (meth == 1)
    { // Enumerated colour space method
      kdu_uint32 enum_cs;
      if (!(colr->read(enum_cs) &&
            ((enum_cs == 16) || (enum_cs == 17) || (enum_cs == 22))))
        { kdu_error e; e << "Illegal enumerated colour space found in JP2 "
          "colour specification (colr) box.  Allowed values are 16, 17 and "
          "22, corresponding to sRGB, sLUM and sYCC, respectively."; }
      if (enum_cs == 16)
        { space = JP2_sRGB_SPACE; num_colours = 3; }
      else if (enum_cs == 17)
        { space = JP2_sLUM_SPACE; num_colours = 1; }
      else
        { space = JP2_sYCC_SPACE; num_colours = 3; }
    }
  else
    { // Embedded ICC profile method
      int profile_bytes = colr->get_remaining_bytes();
      kdu_byte *buf = new kdu_byte[profile_bytes];
      if (colr->read(buf,profile_bytes) != profile_bytes)
        { delete[] buf;
          kdu_error e; e << "JP2 file terminated unexpectedly inside the "
          "colour specification (colr) box."; }
      icc_profile = new j2_icc_profile;
      icc_profile->init(buf,true); // We have donated the buffer.
      num_colours = icc_profile->get_num_colours();
      space = (num_colours==1)?JP2_iccLUM_SPACE:JP2_iccRGB_SPACE;
    }
  if (!colr->close())
    { kdu_error e; e << "Malformed colour specification (colr) box found in "
      "JP2 file.  The box appears to be too large."; }
}

/*****************************************************************************/
/*                            j2_colour::finalize                            */
/*****************************************************************************/

void
  j2_colour::finalize()
{
  if (num_colours == 0)
    { kdu_error e; e << "No JP2 file colour information available!"; }
}

/*****************************************************************************/
/*                            j2_colour::save_box                            */
/*****************************************************************************/

void
  j2_colour::save_box(j2_output_box *super_box)
{
  finalize();
  j2_output_box colr;
  colr.open(super_box,j2_colour_box);
  if (icc_profile == NULL)
    { // Enumerated colour space.
      colr.write((kdu_byte) 1); // Method = 1
      colr.write((kdu_uint16) 0); // Unused fields.
      if (space == JP2_sLUM_SPACE)
        colr.write((kdu_uint32) 17);
      else if (space == JP2_sRGB_SPACE)
        colr.write((kdu_uint32) 16);
      else if (space == JP2_sYCC_SPACE)
        colr.write((kdu_uint32) 22);
    }
  else
    { // Need embedded ICC profile.
      colr.write((kdu_byte) 2); // Method = 2
      colr.write((kdu_uint16) 0); // Unused fields.
      int buf_bytes = 0;
      kdu_byte *buf = icc_profile->get_profile_buf(&buf_bytes);
      colr.write(buf,buf_bytes);
    }
  colr.close();
}

/* ========================================================================= */
/*                                jp2_colour                                 */
/* ========================================================================= */

/*****************************************************************************/
/*                          jp2_colour::init (space)                         */
/*****************************************************************************/

void
  jp2_colour::init(jp2_colour_space space)
{
  assert(state != NULL);
  state->init(space);
}

/*****************************************************************************/
/*                         jp2_colour::init (profile)                        */
/*****************************************************************************/

void
  jp2_colour::init(kdu_byte *profile_buf)
{
  assert(state != NULL);
  j2_icc_profile profile;
  profile.init(profile_buf); // Safe construction will cleanup resources if
                             // exception is thrown.
  j2_icc_profile *heap_profile = new j2_icc_profile;
  heap_profile->init(profile.get_profile_buf());
  state->init(heap_profile);
}

/*****************************************************************************/
/*                      jp2_colour::init (monochrome ICC)                    */
/*****************************************************************************/

void
  jp2_colour::init(float gamma, float beta, int num_points)
{
  assert(state != NULL);
  if (gamma == 1.0F)
    num_points = 0; // Straight line.
  if (beta == 0.0F)
    num_points = 1; // The pure exponent.
  else
    if (gamma < 1.0F)
      { kdu_error e; e << "Currently can only construct profiles having "
        "gamma values greater than or equal to 1.0."; }

  int body_offset = 128 + 4 + 12*1;
  int trc_length = 12+2*num_points;
  int num_bytes = body_offset + trc_length;
  kdu_byte *buf = new kdu_byte[num_bytes];
  kdu_byte *bp = buf;

  // Write header
  store_big((kdu_uint32) num_bytes,bp); // Profile length
  store_big((kdu_uint32) 0,bp); // CMM signature field 0.
  store_big((kdu_uint32) 0x02200000,bp); // Profile version 2.2.0
  store_big(icc_input_device,bp); // Profile class
  store_big(icc_gray_data,bp); // type of colour space
  store_big(icc_pcs_xyz,bp); // PCS signature
  store_big((kdu_uint16) 2001,bp); // Year number
  store_big((kdu_uint16) 1,bp); // Month number
  store_big((kdu_uint16) 1,bp); // Day number
  store_big((kdu_uint16) 0,bp); // Hour number
  store_big((kdu_uint16) 0,bp); // Minute number
  store_big((kdu_uint16) 0,bp); // Second number
  store_big(icc_file_signature,bp); // Standard file signature
  store_big((kdu_uint32) 0,bp); // Primary platform signature
  store_big((kdu_uint32) 0xC00000,bp); // Profile flags
  store_big((kdu_uint32) 0,bp); // Device manufacturer
  store_big((kdu_uint32) 0,bp); // Device model
  store_big((kdu_uint32) 0x80000000,bp); // Transparency (not reflective)
  store_big((kdu_uint32) 0,bp); // Reserved
  store_big((kdu_uint32) 0x00010000,bp); // Intent (relative colorimetry)
  store_big((kdu_uint32)(0.9642*(1<<16)+0.5),bp); // D50 whitepoint, X
  store_big((kdu_uint32)(1.0000*(1<<16)+0.5),bp); // D50 whitepoint, Y
  store_big((kdu_uint32)(0.8249*(1<<16)+0.5),bp); // D50 whitepoint, Z
  store_big((kdu_uint32) 0,bp); // Profile creator signature
  for (int f=0; f < 44; f++)
    *(bp++) = 0;
  assert((bp-buf) == 128);

  // Write tag table
  store_big((kdu_uint32) 1,bp); // Only one tag
  store_big(icc_gray_trc,bp);
  store_big((kdu_uint32) body_offset,bp); // Offset to gray TRC curve
  store_big((kdu_uint32) trc_length,bp); // Size of gray TRC curve

  // Write gray TRC curve
  store_big(icc_curve_type,bp);
  store_big((kdu_uint32) 0,bp);
  store_big((kdu_uint32) num_points,bp);
  if (num_points == 1)
    store_big((kdu_uint16)(gamma*256+0.5),bp);
  else
    {
      gamma = 1.0F / gamma; // Constructing inverse map back to linear space.
      assert(gamma < 1.0F);
      float x, y;
      float breakpoint = beta*gamma/(1.0F-gamma);
      float gradient = (float)
        pow(breakpoint/(gamma*(1.0+beta)),1.0/gamma) / breakpoint;
      for (int n=0; n < num_points; n++)
        {
          x = ((float) n) / ((float)(num_points-1));
          if (x < breakpoint)
            y = x*gradient;
          else
            y = (float) pow((x+beta)/(1.0+beta),1.0/gamma);
          store_big((kdu_uint16)(y*((1<<16)-1)),bp);
        }
    }
  assert((bp-buf) == num_bytes);

  // Construct and install the profile.
  j2_icc_profile *profile = new j2_icc_profile;
  profile->init(buf,true);
  state->init(profile);
}

/*****************************************************************************/
/*                          jp2_colour::init (RGB ICC)                       */
/*****************************************************************************/

void
  jp2_colour::init(float matrix3x3[],float gamma, float beta, int num_points)
{
  if (gamma == 1.0F)
    num_points = 0; // Straight line.
  if (beta == 0.0F)
    num_points = 1; // The pure exponent.
  else
    if (gamma < 1.0F)
      { kdu_error e; e << "Currently can only construct profiles having "
        "gamma values greater than or equal to 1.0."; }

  int body_offset = 128 + 4 + 12*6;
  int trc_length = 12+2*num_points;
  int xyz_length = 20;
  int num_bytes = body_offset + trc_length + xyz_length*3;
  kdu_byte *buf = new kdu_byte[num_bytes];
  kdu_byte *bp = buf;
  // Write header
  store_big((kdu_uint32) num_bytes,bp); // Profile length
  store_big((kdu_uint32) 0,bp); // CMM signature field 0.
  store_big((kdu_uint32) 0x02200000,bp); // Profile version 2.2.0
  store_big(icc_input_device,bp); // Profile class
  store_big(icc_rgb_data,bp); // type of colour space
  store_big(icc_pcs_xyz,bp); // PCS signature
  store_big((kdu_uint16) 2001,bp); // Year number
  store_big((kdu_uint16) 1,bp); // Month number
  store_big((kdu_uint16) 1,bp); // Day number
  store_big((kdu_uint16) 0,bp); // Hour number
  store_big((kdu_uint16) 0,bp); // Minute number
  store_big((kdu_uint16) 0,bp); // Second number
  store_big(icc_file_signature,bp); // Standard file signature
  store_big((kdu_uint32) 0,bp); // Primary platform signature
  store_big((kdu_uint32) 0xC00000,bp); // Profile flags
  store_big((kdu_uint32) 0,bp); // Device manufacturer
  store_big((kdu_uint32) 0,bp); // Device model
  store_big((kdu_uint32) 0x80000000,bp); // Transparency (not reflective)
  store_big((kdu_uint32) 0,bp); // Reserved
  store_big((kdu_uint32) 0x00010000,bp); // Intent (relative colorimetry)
  store_big((kdu_uint32)(0.9642*(1<<16)+0.5),bp); // D50 whitepoint, X
  store_big((kdu_uint32)(1.0000*(1<<16)+0.5),bp); // D50 whitepoint, Y
  store_big((kdu_uint32)(0.8249*(1<<16)+0.5),bp); // D50 whitepoint, Z
  store_big((kdu_uint32) 0,bp); // Profile creator signature
  for (int f=0; f < 44; f++)
    *(bp++) = 0;
  assert((bp-buf) == 128);

  // Write tag table
  store_big((kdu_uint32) 6,bp); // Six tags
  store_big(icc_red_trc,bp);
  store_big((kdu_uint32) body_offset,bp); // Offset to the single TRC curve
  store_big((kdu_uint32) trc_length,bp);
  store_big(icc_green_trc,bp);
  store_big((kdu_uint32) body_offset,bp); // Offset to the single TRC curve
  store_big((kdu_uint32) trc_length,bp);
  store_big(icc_blue_trc,bp);
  store_big((kdu_uint32) body_offset,bp); // Offset to the single TRC curve
  store_big((kdu_uint32) trc_length,bp);
  store_big(icc_red_colorant,bp);
  store_big((kdu_uint32) body_offset+trc_length,bp);
  store_big((kdu_uint32) xyz_length,bp);
  store_big(icc_green_colorant,bp);
  store_big((kdu_uint32) body_offset+trc_length+xyz_length,bp);
  store_big((kdu_uint32) xyz_length,bp);
  store_big(icc_blue_colorant,bp);
  store_big((kdu_uint32) body_offset+trc_length+xyz_length*2,bp);
  store_big((kdu_uint32) xyz_length,bp);

  // Write the single TRC curve
  store_big(icc_curve_type,bp);
  store_big((kdu_uint32) 0,bp);
  store_big((kdu_uint32) num_points,bp);
  if (num_points == 1)
    store_big((kdu_uint16)(gamma*256+0.5),bp);
  else
    {
      gamma = 1.0F / gamma; // Constructing inverse map back to linear space.
      assert (gamma < 1.0F);
      float x, y;
      float breakpoint = beta*gamma/(1.0F-gamma);
      float gradient = (float)
        pow(breakpoint/(gamma*(1.0+beta)),1.0/gamma) / breakpoint;
      for (int n=0; n < num_points; n++)
        {
          x = ((float) n) / ((float)(num_points-1));
          if (x < breakpoint)
            y = x*gradient;
          else
            y = (float) pow((x+beta)/(1.0+beta),1.0/gamma);
          store_big((kdu_uint16)(y*((1<<16)-1)),bp);
        }
    }

  // Write the colorant XYZ values
  for (int c=0; c < 3; c++)
    {
      store_big(icc_xyz_type,bp);
      store_big((kdu_uint32) 0,bp);
      for (int t=0; t < 3; t++)
        {
          kdu_int32 sval = (kdu_int32)(matrix3x3[c+t*3]*(1<<16)+0.5);
          store_big((kdu_uint32) sval,bp);
        }
    }
  assert((bp-buf) == num_bytes);

  assert((bp-buf) == num_bytes);

  // Construct and install the profile.
  j2_icc_profile *profile = new j2_icc_profile;
  profile->init(buf,true);
  state->init(profile);
}

/*****************************************************************************/
/*                         jp2_colour::get_num_colours                       */
/*****************************************************************************/

int
  jp2_colour::get_num_colours()
{
  assert(state != NULL);
  return state->num_colours;
}

/*****************************************************************************/
/*                            jp2_colour::get_space                          */
/*****************************************************************************/

jp2_colour_space
  jp2_colour::get_space()
{
  assert(state != NULL);
  return state->space;
}

/*****************************************************************************/
/*                         jp2_colour::get_icc_profile                       */
/*****************************************************************************/

kdu_byte *
  jp2_colour::get_icc_profile(int *num_bytes)
{
  assert(state != NULL);
  if (state->icc_profile == NULL)
    return NULL;
  return state->icc_profile->get_profile_buf(num_bytes);
}

/*****************************************************************************/
/*                           jp2_colour::get_icc_lut                         */
/*****************************************************************************/

bool
  jp2_colour::get_icc_lut(int channel_idx,float lut[], int index_bits)
{
  assert(state != NULL);
  if (state->icc_profile == NULL)
    return false;
  state->icc_profile->get_lut(channel_idx,lut,index_bits);
  return true;
}

/*****************************************************************************/
/*                         jp2_colour::get_icc_matrix                        */
/*****************************************************************************/

bool
  jp2_colour::get_icc_matrix(float matrix3x3[])
{
  assert(state != NULL);
  if (state->icc_profile == NULL)
    return false;
  if (state->num_colours != 3)
    return false;
  state->icc_profile->get_matrix(matrix3x3);
  return true;
}

/*****************************************************************************/
/*                      jp2_colour::convert_icc_to_slum                      */
/*****************************************************************************/

void
  jp2_colour::convert_icc_to_slum(kdu_line_buf &line, int width)
{
  assert((state != NULL) &&
         (state->icc_profile != NULL) && (state->space == JP2_iccLUM_SPACE));
  if (width < 0)
    width = line.get_width();
  if (state->srgb_curve == NULL)
    { // Make the conversion lookup table, having 10 index bits.
      state->srgb_curve = new kdu_sample16[1024]; // Use a 10-bit LUT index
      float tmp_buf[1024]; state->icc_profile->get_lut(0,tmp_buf,10);
      // Convert linear data to sLUM data by applying sRGB gamma function
      double beta = 0.055, gamma = 2.4;
      double epsilon = pow(beta/((1.0+beta)*(1.0-1.0/gamma)),gamma);
      double g = beta/(epsilon*(gamma-1.0));
      for (int n=0; n < 1024; n++)
        {
          double x = tmp_buf[n];
          if (x <= epsilon)
            x *= g;
          else
            x = (1.0+beta)*pow(x,1.0/gamma) - beta;
          x -= 0.5; // Make it a signed quantity.
          state->srgb_curve[n].ival = (kdu_int16)
            floor(x*(double)(1<<KDU_FIX_POINT));
        }
    }

  // Now we can process the data
  kdu_sample16 *sp = line.get_buf16();
  kdu_sample16 *lut = state->srgb_curve;
  assert((sp != NULL) && !line.is_absolute());
  kdu_int16 idx;
  for (; width > 0; width--, sp++)
    {
      idx = sp->ival;
      idx += (kdu_int16)(1<<(KDU_FIX_POINT-1));
      idx >>= (KDU_FIX_POINT-10);
      if (idx & ~((kdu_int16) 1023))
        idx = (idx<0)?0:1023;
      *sp = lut[idx];
    }
}

/*****************************************************************************/
/*                      jp2_colour::convert_icc_to_srgb                      */
/*****************************************************************************/

void
  jp2_colour::convert_icc_to_srgb(kdu_line_buf &red, kdu_line_buf &green,
                                  kdu_line_buf &blue, int width)
{
  assert((state != NULL) &&
         (state->icc_profile != NULL) && (state->space == JP2_iccRGB_SPACE));
  if (width < 0)
    width = red.get_width();
  assert((width <= red.get_width()) && (width <= green.get_width()) &&
         (width <= blue.get_width()));
  if (state->icc_curves[0] == NULL)
    { // Need to construct the conversion lookup tables and matrix.
      float tmp_buf[1024];
      int n, c;

      // First build the source lookup tables to make channel data linear
      for (c=0; c < 3; c++)
        {
          assert(state->icc_curves[c] == NULL);
          state->icc_curves[c] = new kdu_int16[1024];
          state->icc_profile->get_lut(c,tmp_buf,10);
          for (n=0; n < 1024; n++)
            (state->icc_curves[c])[n] = (kdu_int16)
              floor(tmp_buf[n]*0x7FFF + 0.5);
        }

      // Now fill in the 3x3 matrix entries
      float mat_in[9]; state->icc_profile->get_matrix(mat_in);
      float mul[9] = { 3.1337F,-1.6173F,-0.4907F,
                      -0.9785F, 1.9162F, 0.0334F,
                       0.0720F,-0.2290F, 1.4056F};
      float matrix[9];
      for (c=0; c < 3; c++)
        for (n=0; n < 3; n++)
          matrix[c+3*n] = mul[0+3*n]*mat_in[c+0]
                        + mul[1+3*n]*mat_in[c+3]
                        + mul[2+3*n]*mat_in[c+6];
      double scale = ((double)(1<<12)) * ((double)(1<<16)) / ((double) 0x7FFF);
      for (n=0; n < 9; n++)
        state->icc_matrix[n] = (kdu_int32) floor(matrix[n]*scale + 0.5);
      
      // Finally, fill in the 12-bit `srgb_curve' lookup table
      assert(state->srgb_curve == NULL);
      state->srgb_curve = new kdu_sample16[4096];
      double beta = 0.055, gamma = 2.4;
      double epsilon = pow(beta/((1.0+beta)*(1.0-1.0/gamma)),gamma);
      double g = beta/(epsilon*(gamma-1.0));
      for (n=0; n < 4096; n++)
        {
          double x = ((double) n) / 4095.0;
          if (x <= epsilon)
            x *= g;
          else
            x = (1.0+beta)*pow(x,1.0/gamma) - beta;
          x -= 0.5; // Make it a signed quantity.
          state->srgb_curve[n].ival = (kdu_int16)
            floor(x*(double)(1<<KDU_FIX_POINT));
        }
    }

  // Now we are ready to process the data.

  kdu_sample16 *sp1 = red.get_buf16();
  kdu_sample16 *sp2 = green.get_buf16();
  kdu_sample16 *sp3 = blue.get_buf16();
  kdu_int16 *lut1 = state->icc_curves[0];
  kdu_int16 *lut2 = state->icc_curves[1];
  kdu_int16 *lut3 = state->icc_curves[2];
  kdu_int32 val1, val2, val3, out, *mat = state->icc_matrix;
  kdu_sample16 *sgamma = state->srgb_curve;
  assert((sp1 != NULL) && (sp2 != NULL) && (sp3 != NULL) &&
         (lut1 != NULL) && (lut2 != NULL) && (lut3 != NULL) &&
         (sgamma != NULL));
  for (; width > 0; width--, sp1++, sp2++, sp3++)
    {
      val1 = (sp1->ival+(kdu_int16)(1<<(KDU_FIX_POINT-1)))>>(KDU_FIX_POINT-10);
      if (val1 & ~1023)
        val1 = (val1<0)?0:1023;
      val1 = lut1[val1];
      val2 = (sp2->ival+(kdu_int16)(1<<(KDU_FIX_POINT-1)))>>(KDU_FIX_POINT-10);
      if (val2 & ~1023)
        val2 = (val2<0)?0:1023;
      val2 = lut1[val2];
      val3 = (sp3->ival+(kdu_int16)(1<<(KDU_FIX_POINT-1)))>>(KDU_FIX_POINT-10);
      if (val3 & ~1023)
        val3 = (val3<0)?0:1023;
      val3 = lut3[val3];
      out = (val1*mat[0] + val2*mat[1] + val3*mat[2] + (1<<15)) >> 16;
      if (out & ~4095)
        out = (out<0)?0:4095;
      *sp1 = sgamma[out];
      out = (val1*mat[3] + val2*mat[4] + val3*mat[5] + (1<<15)) >> 16;
      if (out & ~4095)
        out = (out<0)?0:4095;
      *sp2 = sgamma[out];
      out = (val1*mat[6] + val2*mat[7] + val3*mat[8] + (1<<15)) >> 16;
      if (out & ~4095)
        out = (out<0)?0:4095;
      *sp3 = sgamma[out];
    }
}


/* ========================================================================= */
/*                                j2_resolution                              */
/* ========================================================================= */

/*****************************************************************************/
/*                             j2_resolution::init                           */
/*****************************************************************************/

void
  j2_resolution::init(float aspect_ratio)
{
  if (display_ratio > 0.0F)
    { kdu_error e;
      e << "JP2 resolution information may be initialized only once!"; }
  display_ratio = capture_ratio = aspect_ratio;
  display_res = capture_res = 0.0F;
}

/*****************************************************************************/
/*                             j2_resolution::init                           */
/*****************************************************************************/

void
  j2_resolution::init(j2_input_box *res)
{
  if (display_ratio > 0.0F)
    { kdu_error e; e << "JP2 file contains multiple instances of the "
      "resolution (res) box!"; }
  j2_input_box sub;
  while (sub.open(res).exists())
    {
      if ((sub.get_box_type() != j2_capture_resolution_box) &&
          (sub.get_box_type() != j2_display_resolution_box))
        sub.close();
      else
        parse_sub_box(&sub);
    }
  if ((capture_res <= 0.0F) && (display_res <= 0.0F))
    { kdu_error e; e << "The JP2 resolution box must contain at least one "
      "of the capture or display resolution sub-boxes."; }
  if (!res->close())
    { kdu_error e; e << "Malformed resolution box "
      "found in JP2 file.  Box appears to be too long."; }
}

/*****************************************************************************/
/*                        j2_resolution::parse_sub_box                       */
/*****************************************************************************/

void
  j2_resolution::parse_sub_box(j2_input_box *box)
{
  kdu_uint16 v_num, v_den, h_num, h_den;
  kdu_byte v_exp, h_exp;

  if (!(box->read(v_num) && box->read(v_den) &&
        box->read(h_num) && box->read(h_den) &&
        box->read(v_exp) && box->read(h_exp) &&
        v_den && h_den && v_num && h_num))
    { kdu_error e; e << "Malformed capture or display resolution sub-box "
      "found in JP2 file.  Insufficient or illegal data fields."; }

  double v_res, h_res;
  v_res = ((double) v_num) / ((double) v_den);
  while (v_exp & 0x80)
    { v_res *= 0.1F; v_exp++; }
  while (v_exp != 0)
    { v_res *= 10.0F; v_exp--; }
  h_res = ((double) h_num) / ((double) h_den);
  while (h_exp & 0x80)
    { h_res *= 0.1F; h_exp++; }
  while (h_exp != 0)
    { h_res *= 10.0F; h_exp--; }

  if (box->get_box_type() == j2_capture_resolution_box)
    {
      capture_ratio = (float)(h_res / v_res);
      if (display_res <= 0.0F)
        display_ratio = capture_ratio;
      capture_res = (float) v_res;
    }
  else if (box->get_box_type() == j2_display_resolution_box)
    {
      display_ratio = (float)(h_res / v_res);
      if (capture_res <= 0.0F)
        capture_ratio = display_ratio;
      display_res = (float) v_res;
    }
  else
    assert(0);

  if (!box->close())
    { kdu_error e; e << "Malformed capture or display resolution sub-box "
      "found in JP2 file.  Box appears to be too long."; }
}

/*****************************************************************************/
/*                          j2_resolution::finalize                          */
/*****************************************************************************/

void
  j2_resolution::finalize()
{
  if (display_ratio <= 0.0F)
    display_ratio = 1.0F;
  if (capture_ratio <= 0.0F)
    capture_ratio = 1.0F;
}

/*****************************************************************************/
/*                          j2_resolution::save_box                          */
/*****************************************************************************/

void
  j2_resolution::save_box(j2_output_box *super_box)
{
  bool save_display_ratio = (fabs(display_ratio-1.0) > 0.01F);
  bool save_capture_ratio = (fabs(capture_ratio-1.0) > 0.01F);
  bool save_display_res = (display_res > 0.0F);
  bool save_capture_res = (capture_res > 0.0F);
  if (!(save_display_ratio || save_display_res ||
        save_capture_ratio || save_capture_res))
    return;

  j2_output_box res;
  res.open(super_box,j2_resolution_box);
  if (save_display_ratio || save_display_res)
    {
      float v_res = (display_res<=0.0F)?1.0F:display_res;
      save_sub_box(&res,j2_display_resolution_box,v_res,
                   v_res*display_ratio);
      if (fabs(capture_ratio/display_ratio-1.0) < 0.01F)
        save_capture_ratio = false;
    }
  if (save_capture_ratio || save_capture_res)
    {
      float v_res = (capture_res<=0.0F)?1.0F:capture_res;
      save_sub_box(&res,j2_capture_resolution_box,v_res,
                   v_res*capture_ratio);
    }
  res.close();
}

/*****************************************************************************/
/*                        j2_resolution::save_sub_box                          */
/*****************************************************************************/

void
  j2_resolution::save_sub_box(j2_output_box *super_box, kdu_uint32 box_type,
                              double v_res, double h_res)
{
  int v_num, v_den, h_num, h_den;
  int v_exp, h_exp;

  for (v_exp=0; (v_res < 1.0) && (v_exp > -128); v_exp--, v_res*=10.0);
  for (; (v_res > 1.0) && (v_exp < 127); v_exp++, v_res*=0.1);
  v_den = 1<<15;
  v_num = (int)(v_den * v_res + 0.5);

  for (h_exp=0; (h_res < 1.0) && (h_exp > -128); h_exp--, h_res*=10.0);
  for (; (h_res > 1.0) && (h_exp < 127); h_exp++, h_res*=0.1);
  h_den = 1<<15;
  h_num = (int)(h_den * h_res + 0.5);

  if ((h_num <= 0) || (h_num >= (1<<16)) || (v_num <= 0) || (v_num >= (1<<16)))
    { kdu_error e; e << "Unable to save resolution information having "
      "illegal or ridiculously small or large values!"; }

  j2_output_box box;
  box.open(super_box,box_type);
  box.write((kdu_uint16) v_num);
  box.write((kdu_uint16) v_den);
  box.write((kdu_uint16) h_num);
  box.write((kdu_uint16) h_den);
  box.write((kdu_byte) v_exp);
  box.write((kdu_byte) h_exp);
  box.close();
}


/* ========================================================================= */
/*                               jp2_resolution                              */
/* ========================================================================= */

/*****************************************************************************/
/*                            jp2_resolution::init                           */
/*****************************************************************************/

void
  jp2_resolution::init(float aspect_ratio)
{
  assert(state != NULL);
  state->init(aspect_ratio);
}

/*****************************************************************************/
/*            jp2_resolution::set_different_capture_aspect_ratio             */
/*****************************************************************************/

void
  jp2_resolution::set_different_capture_aspect_ratio(float aspect_ratio)
{
  assert((state != NULL) && (state->display_ratio > 0.0F));
  state->capture_ratio = aspect_ratio;
}

/*****************************************************************************/
/*                        jp2_resolution::set_resolution                     */
/*****************************************************************************/

void
  jp2_resolution::set_resolution(float resolution, bool for_display)
{
  assert((state != NULL) && (state->display_ratio > 0.0F));
  if (for_display)
    state->display_res = resolution;
  else
    state->capture_res = resolution;
}

/*****************************************************************************/
/*                       jp2_resolution::get_aspect_ratio                    */
/*****************************************************************************/

float
  jp2_resolution::get_aspect_ratio(bool for_display)
{
  assert(state != NULL);
  return (for_display)?(state->display_ratio):(state->capture_ratio);
}

/*****************************************************************************/
/*                        jp2_resolution::get_resolution                     */
/*****************************************************************************/

float
  jp2_resolution::get_resolution(bool for_display)
{
  assert(state != NULL);
  return (for_display)?(state->display_res):(state->capture_res);
}


/* ========================================================================= */
/*                                 j2_source                                 */
/* ========================================================================= */

/*****************************************************************************/
/*                              j2_source::init                              */
/*****************************************************************************/

void
  j2_source::init(char *fname)
{
  file = fopen(fname,"rb");
  if (file == NULL)
    { kdu_error e; e << "Unable to open JP2 file, \"" << fname << "\"."; }
  kdu_uint32 signature;
  if ((!box.open(file)) || (box.get_box_type() != j2_signature_box) ||
      (!box.read(signature)) || (signature != j2_signature) || !box.close())
    { kdu_error e; e << "JP2 file \"" << fname << "\" does not commence "
      "with a valid signature box."; }

  // Loop through boxes until we encounter the code-stream box.
  bool compatible_file_type = false;
  do {
      if (!box.open(file))
        { kdu_error e;
          e << "JP2 file contains no contiguous code-stream box."; }
      if (box.get_box_type() == j2_file_type_box)
        {
          kdu_uint32 brand, minor_version, compat;
          box.read(brand); box.read(minor_version);
          while (box.read(compat))
            if (compat == j2_brand)
              compatible_file_type = true;
          box.close();
          if (!compatible_file_type)
            { kdu_error e; e << "Input file \"" << fname << "\" contains a "
              "file type box whose compatibility list does not include JP2.";}
        }
      else if (box.get_box_type() == j2_header_box)
        {
          if (!compatible_file_type)
            { kdu_error e; e << "Input file \"" << fname
              << "\" does not appear to be compatible with the JP2 format. "
              "A compatible file type box must be encountered prior to the "
              "header box."; }
          j2_input_box sub_box;
          while (sub_box.open(&box).exists())
            {
              if (sub_box.get_box_type() == j2_image_header_box)
                dimensions.init(&sub_box);
              else if (sub_box.get_box_type() == j2_bits_per_component_box)
                dimensions.process_bpcc_box(&sub_box);
              else if (sub_box.get_box_type() == j2_colour_box)
                colour.init(&sub_box);
              else if (sub_box.get_box_type() == j2_palette_box)
                palette.init(&sub_box);
              else if (sub_box.get_box_type() == j2_channel_definition_box)
                channels.process_cdef_box(&sub_box);
              else if (sub_box.get_box_type() == j2_component_mapping_box)
                channels.process_cmap_box(&sub_box);
              else if (sub_box.get_box_type() == j2_resolution_box)
                resolution.init(&sub_box);
              else
                sub_box.close(); // Skip over unknown boxes.
            }
          dimensions.finalize();
          colour.finalize();
          palette.finalize();
          resolution.finalize();
          jp2_dimensions dims(&dimensions);
          jp2_colour clr(&colour);
          jp2_palette plt(&palette);
          channels.finalize(clr.get_num_colours(),
                            dims.get_num_components(),
                            plt.get_num_components());
          if (!box.close())
            { kdu_error e; e << "The JP2 file's image header box contains "
              "data which does not belong to any sub-box."; }
        }
      else if (box.get_box_type() != j2_codestream_box)
        box.close(); // Skip over all other boxes.
    } while (box.get_box_type() != j2_codestream_box);
}


/* ========================================================================= */
/*                                 jp2_source                                */
/* ========================================================================= */

/*****************************************************************************/
/*                              jp2_source::open                             */
/*****************************************************************************/

void
  jp2_source::open(char *fname)
{
  assert(state == NULL);
  state = new j2_source;
  state->init(fname);
}

/*****************************************************************************/
/*                              jp2_source::close                            */
/*****************************************************************************/

void
  jp2_source::close()
{
  if (state == NULL)
    return;
  if (state->file != NULL)
    { // Although these operations will be performed automatically by the
      // default destructor for `state', they may be performed in the wrong
      // order.
      state->box.close();
      fclose(state->file);
      state->file = NULL;
    }
  delete state;
  state = NULL;
}

/*****************************************************************************/
/*                       jp2_source::access_dimensions                       */
/*****************************************************************************/

jp2_dimensions
  jp2_source::access_dimensions()
{
  assert(state != NULL);
  return jp2_dimensions(&(state->dimensions));
}

/*****************************************************************************/
/*                         jp2_source::access_colour                         */
/*****************************************************************************/

jp2_colour
  jp2_source::access_colour()
{
  assert(state != NULL);
  return jp2_colour(&(state->colour));
}

/*****************************************************************************/
/*                        jp2_source::access_palette                         */
/*****************************************************************************/

jp2_palette
  jp2_source::access_palette()
{
  assert(state != NULL);
  return jp2_palette(&(state->palette));
}

/*****************************************************************************/
/*                        jp2_source::access_channels                        */
/*****************************************************************************/

jp2_channels
  jp2_source::access_channels()
{
  assert(state != NULL);
  return jp2_channels(&(state->channels));
}

/*****************************************************************************/
/*                       jp2_source::access_resolution                       */
/*****************************************************************************/

jp2_resolution
  jp2_source::access_resolution()
{
  assert(state != NULL);
  return jp2_resolution(&(state->resolution));
}

/*****************************************************************************/
/*                              jp2_source::read                             */
/*****************************************************************************/

int
  jp2_source::read(kdu_byte *buf, int num_bytes)
{
  assert(state != NULL);
  return state->box.read(buf,num_bytes);
}

/* ========================================================================= */
/*                                  j2_target                                */
/* ========================================================================= */

/*****************************************************************************/
/*                              j2_target::init                              */
/*****************************************************************************/

void
  j2_target::init()
{
  dimensions.finalize();
  colour.finalize();
  palette.finalize();
  resolution.finalize();
  jp2_dimensions dims(&dimensions);
  jp2_colour clr(&colour);
  jp2_palette plt(&palette);
  channels.finalize(clr.get_num_colours(),
                    dims.get_num_components(),
                    plt.get_num_components());
  assert(file != NULL);
  initialized = true;

  box.open(file,j2_signature_box);
  box.write(j2_signature);
  box.close();

  box.open(file,j2_file_type_box);
  box.write(j2_brand);
  box.write((kdu_uint32) 0);
  box.write(j2_brand);
  box.close();

  box.open(file,j2_header_box);
  {
    dimensions.save_boxes(&box);
    colour.save_box(&box);
    palette.save_box(&box);
    channels.save_boxes(&box);
    resolution.save_box(&box);
  }
  box.close();

  box.open(file,j2_codestream_box,true);
}


/* ========================================================================= */
/*                                 jp2_target                                */
/* ========================================================================= */

/*****************************************************************************/
/*                              jp2_target::open                             */
/*****************************************************************************/

void
  jp2_target::open(char *fname)
{
  assert(state == NULL);
  state = new j2_target;
  state->file = fopen(fname,"wb");
  if (state->file == NULL)
    { kdu_error e; e << "Unable to open output file, \"" << fname << "\"."; }
}

/*****************************************************************************/
/*                              jp2_target::close                            */
/*****************************************************************************/

void
  jp2_target::close()
{
  if (state == NULL)
    return;
  if (state->file != NULL)
    {
      state->box.close();
      fclose(state->file);
      state->file = NULL;
    }
  delete state;
  state = NULL;
}

/*****************************************************************************/
/*                       jp2_target::access_dimensions                       */
/*****************************************************************************/

jp2_dimensions
  jp2_target::access_dimensions()
{
  assert(state != NULL);
  return jp2_dimensions(&(state->dimensions));
}

/*****************************************************************************/
/*                         jp2_target::access_colour                         */
/*****************************************************************************/

jp2_colour
  jp2_target::access_colour()
{
  assert(state != NULL);
  return jp2_colour(&(state->colour));
}

/*****************************************************************************/
/*                        jp2_target::access_palette                         */
/*****************************************************************************/

jp2_palette
  jp2_target::access_palette()
{
  assert(state != NULL);
  return jp2_palette(&(state->palette));
}

/*****************************************************************************/
/*                        jp2_target::access_channels                        */
/*****************************************************************************/

jp2_channels
  jp2_target::access_channels()
{
  assert(state != NULL);
  return jp2_channels(&(state->channels));
}

/*****************************************************************************/
/*                       jp2_target::access_resolution                       */
/*****************************************************************************/

jp2_resolution
  jp2_target::access_resolution()
{
  assert(state != NULL);
  return jp2_resolution(&(state->resolution));
}

/*****************************************************************************/
/*                              jp2_target::write                            */
/*****************************************************************************/

bool
  jp2_target::write(kdu_byte *buf, int num_bytes)
{
  assert(state != NULL);
  if (!state->initialized)
    state->init();
  return state->box.write(buf,num_bytes);
}
