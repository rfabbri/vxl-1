/*****************************************************************************/
// File: codestream.cpp [scope = CORESYS/COMPRESSED]
// Version: Kakadu, V2.2
// Author: David Taubman
// Last Revised: 20 June, 2001
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
   Implements a part of the compressed data management machinery, including
code-stream I/O, packet sequencing and the top level interfaces associated with
codestream objects.
******************************************************************************/

#include <cmath>
#include <string.h>
#include <limits.h>
#include <iomanip>
#include <assert.h>
#include "kdu_elementary.h"
#include "kdu_utils.h"
#include "kdu_messaging.h"
#include "kdu_kernels.h"
#include "kdu_compressed.h"
#include "compressed_local.h"

using namespace std;

#define KDU_IDENTIFIER "Kakadu-2.2" // Do not disable.  Allows compressed
                                    // code-streams to be identified in case of
                                    // compatibility problems down the road.

/* ========================================================================= */
/*                              Internal Functions                           */
/* ========================================================================= */

/*****************************************************************************/
/* STATIC                        is_power_2                                  */
/*****************************************************************************/

static bool
  is_power_2(int val)
{
  for (; val > 1; val >>= 1)
    if (val & 1)
      return false;
    return (val==1);
}

/* ========================================================================= */
/*                              External Functions                           */
/* ========================================================================= */

/*****************************************************************************/
/* EXTERN                      print_marker_code                             */
/*****************************************************************************/

void
  print_marker_code(kdu_uint16 code, std::ostream &out)
{
  const char *name=NULL;

  if (code == KDU_SOC)
    name = "SOC";
  else if (code == KDU_SOT)
    name = "SOT";
  else if (code == KDU_SOD)
    name = "SOD";
  else if (code == KDU_SOP)
    name = "SOP";
  else if (code == KDU_EPH)
    name = "EPH";
  else if (code == KDU_EOC)
    name = "EOC";
  else if (code == KDU_SIZ)
    name = "SIZ";
  else if (code == KDU_COD)
    name = "COD";
  else if (code == KDU_COC)
    name = "COC";
  else if (code == KDU_QCD)
    name = "QCD";
  else if (code == KDU_QCC)
    name = "QCC";
  else if (code == KDU_RGN)
    name = "RGN";
  else if (code == KDU_POC)
    name = "POC";
  else if (code == KDU_CRG)
    name = "CRG";
  else if (code == KDU_COM)
    name = "COM";
  else if (code == KDU_TLM)
    name = "TLM";
  else if (code == KDU_PLM)
    name = "PLM";
  else if (code == KDU_PLT)
    name = "PLT";
  else if (code == KDU_PPM)
    name = "PPM";
  else if (code == KDU_PPT)
    name = "PPT";

  if (name == NULL)
    {
#ifdef GCC_VERSION_LESS_THAN_3
      long int original_flags;
#else
      ios_base::fmtflags original_flags;
#endif
      original_flags =
        out.flags(ios::showbase|ios::internal|ios::right|ios::hex);
      out.fill('0');
      out << setw(6) << code;
      out.flags(original_flags);
    }
  else
    out << "<" << name << ">";
}


/* ========================================================================= */
/*                                 kd_input                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                    kd_input::process_unexpected_marker                    */
/*****************************************************************************/

void
  kd_input::process_unexpected_marker(kdu_byte last_byte)
{
  assert(throw_markers);
  kdu_uint16 code = 0xFF00; code += last_byte;
  disable_marker_throwing();
  if (!reject_all)
    {
      bool bona_fide = false;
      if ((code == KDU_SOP) || (code == KDU_SOT))
        {
          kdu_byte byte;
          kdu_uint16 length;
          if (!get(byte))
            exhausted = false;
          else
            {
              length = byte;
              if (!get(byte))
                {
                  exhausted = false;
                  putback((kdu_byte) code);
                }
              else
                {
                  length = (length<<8) + byte;
                  if (code == KDU_SOP)
                    bona_fide = (length == 4);
                  else
                    bona_fide = (length == 10);
                  putback(length);
                }
            }
        }
      if (!bona_fide)
        {
          enable_marker_throwing(reject_all);
          have_FF = (last_byte==0xFF);
          return; // Continue processing as though nothing had happened.
        }
    }
  assert(!exhausted);
  putback(code);
  throw code;
}

/*****************************************************************************/
/*                             kd_input::read                                */
/*****************************************************************************/

int
  kd_input::read(kdu_byte *buf, int count)
{
  int xfer_bytes;
  int nbytes = 0;

  if (exhausted)
    return 0;
  while (count > 0)
    {
      if ((xfer_bytes = first_unwritten-first_unread) == 0)
        {
          if (!load_buf())
            break;
          xfer_bytes = first_unwritten-first_unread;
          assert(xfer_bytes > 0);
        }
      xfer_bytes = (xfer_bytes < count)?xfer_bytes:count;
      nbytes += xfer_bytes;
      count -= xfer_bytes;
      if (throw_markers)
        { // Slower loop has to look for marker codes.
          kdu_byte byte;
          while (xfer_bytes--)
            {
              *(buf++) = byte = *(first_unread++);
              if (have_FF && (byte > 0x8F))
                process_unexpected_marker(byte);
              have_FF = (byte==0xFF);
            }
        }
      else
        { // Fastest loop. Probably not beneficial to use `memcpy'.
          while (xfer_bytes--)
            *(buf++) = *(first_unread++);
        }
    }
  return nbytes;
}

/*****************************************************************************/
/*                             kd_input::ignore                              */
/*****************************************************************************/

int
  kd_input::ignore(int count)
{
  int xfer_bytes;
  int nbytes = 0;

  if (exhausted)
    return 0;
  while (count > 0)
    {
      if ((xfer_bytes = first_unwritten-first_unread) == 0)
        {
          if (!load_buf())
            break;
          xfer_bytes = first_unwritten-first_unread;
          assert(xfer_bytes > 0);
        }
      xfer_bytes = (xfer_bytes < count)?xfer_bytes:count;
      nbytes += xfer_bytes;
      count -= xfer_bytes;
      if (throw_markers)
        { // Slower loop has to look for marker codes.
          kdu_byte byte;
          while (xfer_bytes--)
            {
              byte = *(first_unread++);
              if (have_FF && (byte > 0x8F))
                process_unexpected_marker(byte);
              have_FF = (byte==0xFF);
            }
        }
      else
        first_unread += xfer_bytes;
    }
  return nbytes;
}

/* ========================================================================= */
/*                            kd_compressed_input                            */
/* ========================================================================= */

/*****************************************************************************/
/*                       kd_compressed_input::load_buf                       */
/*****************************************************************************/

bool
  kd_compressed_input::load_buf()
{
  if (bytes_available <= 0)
    { exhausted = true; return false; }

  first_unread = buffer + KD_IBUF_PUTBACK;
  int xfer_bytes = KD_IBUF_SIZE - KD_IBUF_PUTBACK;
  if (xfer_bytes > bytes_available)
    xfer_bytes = bytes_available;
  if (xfer_bytes > 0)
    xfer_bytes = source->read(first_unread,xfer_bytes);
  bytes_available -= xfer_bytes;
  first_unwritten = first_unread + xfer_bytes;
  if (xfer_bytes == 0)
    { exhausted = true; return false; }
  return true;
}


/* ========================================================================= */
/*                               kd_pph_input                                */
/* ========================================================================= */

/*****************************************************************************/
/*                        kd_pph_input::~kd_pph_input                        */
/*****************************************************************************/

kd_pph_input::~kd_pph_input()
{
  read_buf = NULL; // Just in case.
  while ((write_buf=first_buf) != NULL)
    {
      first_buf = write_buf->next;
      buf_server->release(write_buf);
    }
}

/*****************************************************************************/
/*                         kd_pph_input::add_bytes                           */
/*****************************************************************************/

void
  kd_pph_input::add_bytes(kdu_byte *data, int num_bytes)
{
  while (num_bytes > 0)
    {
      if (write_buf == NULL)
        {
          write_buf = read_buf = first_buf = buf_server->get();
          write_pos = read_pos = 0;
        }
      else if (write_pos == KD_CODE_BUFFER_LEN)
        {
          write_buf = write_buf->next = buf_server->get();
          write_pos = 0;
        }
      int xfer_bytes = KD_CODE_BUFFER_LEN-write_pos;
      if (xfer_bytes > num_bytes)
        xfer_bytes = num_bytes;
      num_bytes -= xfer_bytes;
      while (xfer_bytes--)
        write_buf->buf[write_pos++] = *(data++);
    }
}

/*****************************************************************************/
/*                          kd_pph_input::load_buf                           */
/*****************************************************************************/

bool
  kd_pph_input::load_buf()
{
  if (read_buf == NULL)
    { exhausted = true; return false; }

  first_unread = first_unwritten = buffer + KD_IBUF_PUTBACK;
  int xfer_bytes = KD_IBUF_SIZE - KD_IBUF_PUTBACK;
  int buf_bytes;
  while (xfer_bytes > 0)
    {
      if (read_pos == KD_CODE_BUFFER_LEN)
        {
          if (read_buf != write_buf)
            {
              read_buf = read_buf->next;
              read_pos = 0;
              assert(read_buf != NULL);
            }
        }
      buf_bytes = (read_buf==write_buf)?write_pos:KD_CODE_BUFFER_LEN;
      buf_bytes -= read_pos;
      assert(buf_bytes >= 0);
      if (buf_bytes == 0)
        break;
      if (buf_bytes > xfer_bytes)
        buf_bytes = xfer_bytes;
      xfer_bytes -= buf_bytes;
      while (buf_bytes--)
        *(first_unwritten++) = read_buf->buf[read_pos++];
    }
  if (first_unread == first_unwritten)
    { exhausted = true; return false; }
  return true;
}

/* ========================================================================= */
/*                                 kd_marker                                 */
/* ========================================================================= */

/*****************************************************************************/
/*                        kd_marker::kd_marker (copy)                        */
/*****************************************************************************/

kd_marker::kd_marker(const kd_marker &orig)
{
  source = NULL;
  code = orig.code;
  max_length = length = orig.length;
  if (max_length == 0)
    buf = NULL;
  else
    buf = new kdu_byte[max_length];
  memcpy(buf,orig.buf,(size_t) length);
}

/*****************************************************************************/
/*                              kd_marker::read                              */
/*****************************************************************************/

bool
  kd_marker::read(bool exclude_stuff_bytes, bool skip_to_marker)
{
  assert(source != NULL);
  kdu_byte byte = 0;
  bool valid_code;

  source->disable_marker_throwing();
  do {
      if ((byte != 0xFF) && !source->get(byte))
        { code = 0; length = 0; return false; }
      if (skip_to_marker)
        while (byte != 0xFF)
          if (!source->get(byte))
            { code = 0; length = 0; return false; }
      if (byte != 0xFF)
        {
          source->putback(byte);
          code = 0; length = 0; return false;
        }
      if (!source->get(byte))
        { code = 0; length = 0; return false; }
      code = 0xFF00 + byte;
      valid_code = true;
      if (exclude_stuff_bytes)
        valid_code = (byte > 0x8F);
      if ((code == KDU_SOP) || (code == KDU_SOT))
        { // Want to be really sure that this is not a corrupt marker code.
          assert(valid_code);
          if (!source->get(byte))
            { code = 0; length = 0; return false; }
          length = byte; length <<= 8;
          if (!source->get(byte))
            { code = 0; length = 0; return false; }
          length += byte;
          if ((code == KDU_SOP) && (length != 4))
            {
              valid_code = false;
              source->putback((kdu_uint16) length);
              kdu_warning w; w << "Skipping over corrupt SOP marker code!";
            }
          else if ((code == KDU_SOT) && (length != 10))
            {
              valid_code = false;
              source->putback((kdu_uint16) length);
              kdu_warning w;  w << "Skipping over corrupt SOT marker code!";
            }
          byte = (kdu_byte) code;
        }
      else if (code == KDU_EOC)
        { /* If the input source is about to end, we will simply skip over
             the EOC marker so that true EOC termination can be treated in
             the same way as premature termination of the code-stream.
             Otherwise, the EOC marker would appear to have been generated
             by some type of code-stream corruption.  In this case, we will
             treat it as an invalid marker code. */
          if (source->get(byte))
            {
              valid_code = false;
              source->putback(byte);
              byte = (kdu_byte) code;
              kdu_warning w; w << "Disregarding non-terminal EOC marker.";
            }
          else
            { length = 0; code = 0; return false; }
        }
    } while (skip_to_marker && (!valid_code));

  if (!valid_code)
    {
      source->putback(code);
      code = 0; length = 0;
      return false;
    }

  // Now we are committed to processing the marker, returning false only if
  // the source becomes exhausted.

  if ((code == KDU_SOC) || (code == KDU_SOD) ||
      (code == KDU_EOC) || (code == KDU_EPH))
    return true; // Delimiter found. There is no marker segment.
  if ((code != KDU_SOT) && (code != KDU_SOP) && (code != KDU_SIZ) &&
      (code != KDU_COD) && (code != KDU_COC) &&
      (code != KDU_QCD) && (code != KDU_QCC) &&
      (code != KDU_RGN) && (code != KDU_POC) &&
      (code != KDU_CRG) && (code != KDU_COM) &&
      (code != KDU_TLM) && (code != KDU_PLM) && (code != KDU_PLT) &&
      (code != KDU_PPM) && (code != KDU_PPT))
    {
      kdu_warning w; w << "Unrecognized/unimplemented marker code, ";
      print_current_code(w);  w << ", found in code-stream.";
    }
  if ((code != KDU_SOP) && (code != KDU_SOT))
    { // Otherwise, we already have the length.
      if (!source->get(byte))
        { code = 0; return false; }
      length = byte; length <<= 8;
      if (!source->get(byte))
        { code = 0; length = 0; return false; }
      length += byte;
    }
  length -= 2;
  if (length < 0)
    { code = 0; length = 0; return false; }
  if (length > max_length)
    {
      max_length = 2*length; // Don't want to have to re-allocate too often
      delete[] buf;
      buf = new kdu_byte[max_length];
    }
  if (source->read(buf,length) < length)
    {
      code = 0; length = 0;
      return false;
    }
  return true;
}


/* ========================================================================= */
/*                                kd_pp_markers                              */
/* ========================================================================= */

/*****************************************************************************/
/*                        kd_pp_markers::~kd_pp_markers                      */
/*****************************************************************************/

kd_pp_markers::~kd_pp_markers()
{
  kd_pp_marker_list *tmp;
  while ((tmp=list) != NULL)
    {
      list = tmp->next;
      delete tmp;
    }
}

/*****************************************************************************/
/*                          kd_pp_markers::add_markers                       */
/*****************************************************************************/

void
  kd_pp_markers::add_marker(kd_marker &copy_source)
{
  if (copy_source.get_length() < 1)
    { kdu_error e;
      e << "PPM/PPT marker segments must be at least 3 bytes long!"; }
  kd_pp_marker_list *elt = new kd_pp_marker_list(copy_source);
  kdu_byte *data = elt->get_bytes();
  elt->znum = data[0];
  elt->bytes_read = 1;
  if (elt->get_code() == KDU_PPM)
    {
      assert((list == NULL) || is_ppm);
      is_ppm = true;
    }
  else
    {
      assert(elt->get_code() == KDU_PPT);
      assert((list == NULL) || !is_ppm);
      is_ppm = false;
    }

  kd_pp_marker_list *scan, *prev;

  for (prev=NULL, scan=list; scan != NULL; prev=scan, scan=scan->next)
    if (scan->znum > elt->znum)
      break;
  elt->next = scan;
  if (prev == NULL)
    list = elt;
  else
    {
      prev->next = elt;
      if (prev->znum == elt->znum)
        { kdu_error e; e << "Found multiple PPM/PPT markers with identical "
          "Zppt/Zppm indices within the same header scope (main or tile-part "
          "header)!"; }
    }
}

/*****************************************************************************/
/*                        kd_pp_markers::transfer_tpart                      */
/*****************************************************************************/

void
  kd_pp_markers::transfer_tpart(kd_pph_input *pph_input)
{
  int xfer_bytes = INT_MAX;

  if (is_ppm)
    {
      while ((list != NULL) && (list->bytes_read == list->get_length()))
        advance_list();
      if (list == NULL)
        { kdu_error e; e << "Insufficient packet header data in PPM marker "
          "segments!"; }
      if ((list->get_length()-list->bytes_read) < 4)
        { kdu_error e; e << "Encountered malformed PPM marker: 4-byte Nppm "
          "values may not straddle multiple PPM marker segments.  Problem "
          "is most likely due to a previously incorrect Nppm value."; }
      kdu_byte *data = list->get_bytes();
      xfer_bytes = data[list->bytes_read++];
      xfer_bytes = (xfer_bytes << 8) + data[list->bytes_read++];
      xfer_bytes = (xfer_bytes << 8) + data[list->bytes_read++];
      xfer_bytes = (xfer_bytes << 8) + data[list->bytes_read++];
    }
  while ((list != NULL) && (xfer_bytes > 0))
    {
      int elt_bytes = list->get_length()-list->bytes_read;
      if (elt_bytes > xfer_bytes)
        elt_bytes = xfer_bytes;
      pph_input->add_bytes(list->get_bytes()+list->bytes_read,elt_bytes);
      xfer_bytes -= elt_bytes;
      list->bytes_read += elt_bytes;
      if (list->bytes_read == list->get_length())
        advance_list();
    }
  if (is_ppm && (xfer_bytes > 0))
    { kdu_error e; e << "Insufficient packet header data in PPM marker "
      "segments, or else Nppm values must be incorrect!"; }
}

/*****************************************************************************/
/*                         kd_pp_markers::ignore_tpart                       */
/*****************************************************************************/

void
  kd_pp_markers::ignore_tpart()
{
  int xfer_bytes = INT_MAX;

  if (is_ppm)
    {
      kdu_byte byte;

      int len_bytes = 0;
      while (len_bytes < 4)
        { // Need to read 4 bytes of length information.
          if (list == NULL)
            { kdu_error e; e << "Insufficient packet header data in PPM "
              "marker segments!"; }
          if (list->bytes_read == list->get_length())
            {
              advance_list();
              continue;
            }
          byte = (list->get_bytes())[list->bytes_read++];
          xfer_bytes = (xfer_bytes << 8) + byte;
          len_bytes++;
        }
    }
  while ((list != NULL) && (xfer_bytes > 0))
    {
      int elt_bytes = list->get_length()-list->bytes_read;
      if (elt_bytes > xfer_bytes)
        elt_bytes = xfer_bytes;
      xfer_bytes -= elt_bytes;
      list->bytes_read += elt_bytes;
      if (list->bytes_read == list->get_length())
        advance_list();
    }
  if (is_ppm && (xfer_bytes > 0))
    { kdu_error e; e << "Insufficient packet header data in PPM marker "
      "segments, or else Nppm values must be incorrect!"; }
}

/*****************************************************************************/
/*                         kd_pp_markers::advance_list                       */
/*****************************************************************************/

void
  kd_pp_markers::advance_list()
{
  assert((list != NULL) && (list->bytes_read == list->get_length()));
  kd_pp_marker_list *tmp = list;
  list = tmp->next;
  delete tmp;
}


/* ========================================================================= */
/*                               kd_buf_server                               */
/* ========================================================================= */

/*****************************************************************************/
/*                       kd_buf_server::~kd_buf_server                       */
/*****************************************************************************/

kd_buf_server::~kd_buf_server()
{
  assert(num_users == 0);
  if (num_allocated_buffers != 0)
    { kdu_warning w;  w << "The compressed data buffer server is being "
      "destroyed before all allocated buffers have been returned.  The "
      "problem is most likely connected with a bug in the code-block "
      "destruction code."; }
  kd_code_alloc *tmp;
  while ((tmp=alloc) != NULL)
    {
      alloc = tmp->next;
      delete tmp;
    }
}

/*****************************************************************************/
/*                             kd_buf_server::get                            */
/*****************************************************************************/

kd_code_buffer *
  kd_buf_server::get()
{
  if (free == NULL)
    {
      assert(num_allocated_buffers == total_buffers);
      kd_code_alloc *new_alloc = new kd_code_alloc;
      new_alloc->next = alloc;
      alloc = new_alloc;
      int n;
      for (n=0; n < (KD_CODE_ALLOC_NUM-1); n++)
        alloc->bufs[n].next = &(alloc->bufs[n+1]);
      alloc->bufs[n].next = free;
      free = &(alloc->bufs[0]);
      total_buffers += KD_CODE_ALLOC_NUM;
    }
  kd_code_buffer *result = free;
  free = result->next;
  result->next = NULL;
  num_allocated_buffers++;
  if (num_allocated_buffers > peak_allocated_buffers)
    peak_allocated_buffers++;
  return result;
}

/*****************************************************************************/
/*                           kd_buf_server::release                          */
/*****************************************************************************/

void
  kd_buf_server::release(kd_code_buffer *buf)
{
  assert(num_allocated_buffers > 0);
  buf->next = free;
  free = buf;
  num_allocated_buffers--;
}


/* ========================================================================= */
/*                          kd_packet_sequencer                              */
/* ========================================================================= */

/*****************************************************************************/
/*               kd_packet_sequencer::kd_packet_sequencer                    */
/*****************************************************************************/

kd_packet_sequencer::kd_packet_sequencer(kd_tile *tile)
{
  int c, r;
  kd_tile_comp *tc;
  kd_resolution *res;

  assert(tile->initialized);
  this->tile = tile;
  max_dwt_levels = 0;
  common_grids = true;
  for (c=0; c < tile->num_components; c++)
    {
      tc = tile->comps + c;
      if (tc->dwt_levels > max_dwt_levels)
        max_dwt_levels = tc->dwt_levels;
      if (!(is_power_2(tc->sub_sampling.x) && is_power_2(tc->sub_sampling.y)))
        common_grids = false;
      for (r=0; r <= tc->dwt_levels; r++)
        {
          int inc;

          res = tc->resolutions + r;
          inc = res->precinct_partition.size.x;
          inc <<= (tc->dwt_levels-r);
          inc *= tc->sub_sampling.x;
          if ((r == 0) || (inc < tc->grid_inc.x))
            tc->grid_inc.x = inc;

          inc = res->precinct_partition.size.y;
          inc <<= (tc->dwt_levels-r);
          inc *= tc->sub_sampling.y;
          if ((r == 0) || (inc < tc->grid_inc.y))
            tc->grid_inc.y = inc;
        }
      tc->grid_min = tile->dims.pos - tile->coding_origin;
      tc->grid_min.x = tc->grid_inc.x *
        floor_ratio(tc->grid_min.x,tc->grid_inc.x);
      tc->grid_min.y = tc->grid_inc.y *
        floor_ratio(tc->grid_min.y,tc->grid_inc.y);
      tc->grid_min += tile->coding_origin;
    }
  grid_lim = tile->dims.pos + tile->dims.size;
  poc = NULL;
  next_poc_record = 0;
  next_progression();
}

/*****************************************************************************/
/*                    kd_packet_sequencer::save_state                        */
/*****************************************************************************/

void
  kd_packet_sequencer::save_state()
{
  for (int c=0; c < tile->num_components; c++)
    {
      kd_tile_comp *tc = tile->comps + c;
      for (int r=0; r <= tc->dwt_levels; r++)
        {
          kd_resolution *res = tc->resolutions + r;
          int num_precincts = res->precinct_indices.area();
          for (int n=0; n < num_precincts; n++)
            {
              kd_precinct *precinct = res->precinct_refs[n];
              if ((precinct != NULL) && (precinct != KD_EXPIRED_PRECINCT))
                precinct->saved_next_layer_idx = precinct->next_layer_idx;
            }
        }
    }
  tile->saved_num_transferred_packets = tile->num_transferred_packets;
  saved_poc = poc;
}

/*****************************************************************************/
/*                  kd_packet_sequencer::restore_state                       */
/*****************************************************************************/

void
  kd_packet_sequencer::restore_state()
{
  for (int c=0; c < tile->num_components; c++)
    {
      kd_tile_comp *tc = tile->comps + c;
      for (int r=0; r <= tc->dwt_levels; r++)
        {
          kd_resolution *res = tc->resolutions + r;
          int num_precincts = res->precinct_indices.area();
          for (int n=0; n < num_precincts; n++)
            {
              kd_precinct *precinct = res->precinct_refs[n];
              if ((precinct != NULL) && (precinct != KD_EXPIRED_PRECINCT))
                precinct->next_layer_idx = precinct->saved_next_layer_idx;
            }
        }
    }
  tile->num_transferred_packets = tile->saved_num_transferred_packets;
  poc = saved_poc;
  next_poc_record = 0;
  next_progression();
}

/*****************************************************************************/
/*                 kd_packet_sequencer::next_progression                     */
/*****************************************************************************/

bool
  kd_packet_sequencer::next_progression()
{
  if (poc == NULL) // Must be initial call.
    {
      poc = tile->codestream->siz->access_cluster(POC_params);
      assert(poc != NULL);
      poc = poc->access_relation(tile->tnum,-1);
      assert(poc != NULL);
      if (!poc->get(Porder,0,0,res_min))
        poc = NULL;
    }
  if (poc == NULL)
    { // Get information from COD marker.
      kdu_params *cod =
        tile->codestream->siz->access_cluster(COD_params);
      cod = cod->access_relation(tile->tnum,-1);
      if (!cod->get(Corder,0,0,order))
        assert(0);
      comp_min = res_min = 0;
      layer_lim = tile->num_layers;
      comp_lim = tile->num_components;
      res_lim = max_dwt_levels+1;
    }
  else
    { // Get information from POC marker.
      if (!poc->get(Porder,next_poc_record,0,res_min))
        { // Need to access a new POC instance.
          int inst_idx = poc->get_instance();
          inst_idx++;
          kdu_params *tmp_poc = poc->access_relation(tile->tnum,-1,inst_idx);
          if ((tmp_poc == NULL) ||
              !tmp_poc->get(Porder,0,0,res_min))
            {
              if (tile->codestream->in == NULL)
                { kdu_error e; e << "Supplied progression order attributes "
                  "for tile " << tile->tnum << " are insuffient to cover "
                  "all packets for the tile!"; }
              return false;
            }
          if (inst_idx >= tile->next_tpart)
            return false; // Need to generate a new tile-part first.
          poc = tmp_poc;
          next_poc_record = 0;
        }
      poc->get(Porder,next_poc_record,1,comp_min);
      poc->get(Porder,next_poc_record,2,layer_lim);
      poc->get(Porder,next_poc_record,3,res_lim);
      poc->get(Porder,next_poc_record,4,comp_lim);
      poc->get(Porder,next_poc_record,5,order);
      next_poc_record++;
    }
  if (layer_lim > tile->num_layers)
    layer_lim = tile->num_layers;
  if (comp_lim > tile->num_components)
    comp_lim = tile->num_components;
  if (res_lim > max_dwt_levels)
    res_lim = max_dwt_levels+1;
  layer_idx=0;
  comp_idx = comp_min;
  res_idx = res_min;
  pos.x=pos.y=0;

  bool spatial = false;
  if ((order == Corder_PCRL) || (order == Corder_RPCL))
    {
      spatial = true;
      if (!common_grids)
        { kdu_error e; e << "Attempting to use a spatially progressive "
          "packet sequence where position order dominates component order. "
          "This is illegal when the component sub-sampling factors are not "
          "exact powers of 2!"; }
      for (int c=0; c < tile->num_components; c++)
        {
          kd_tile_comp *tc = tile->comps + c;
          if ((c == 0) || (tc->grid_inc.x < grid_inc.x))
            {
              grid_inc.x = tc->grid_inc.x;
              grid_min.x = tc->grid_min.x;
            }
          if ((c == 0) || (tc->grid_inc.y < grid_inc.y))
            {
              grid_inc.y = tc->grid_inc.y;
              grid_min.y = tc->grid_min.y;
            }
        }
      grid_loc = grid_min;
    }
  else if ((order == Corder_CPRL) && (comp_idx < comp_lim))
    {
      spatial = true;
      grid_min = tile->comps[comp_idx].grid_min;
      grid_inc = tile->comps[comp_idx].grid_inc;
      grid_loc = grid_min;
    }

  if (spatial)
    { // Need to reset the precinct position indices in each resolution.
      for (int c=0; c < tile->num_components; c++)
        {
          kd_tile_comp *tc = tile->comps + c;
          for (int r=0; r <= tc->dwt_levels; r++)
            {
              kd_resolution *res = tc->resolutions + r;
              res->current_sequencer_pos.x = res->current_sequencer_pos.y = 0;
            }
        }
    }
  return true;
}

/*****************************************************************************/
/*                  kd_packet_sequencer::next_in_sequence                    */
/*****************************************************************************/

kd_precinct *
  kd_packet_sequencer::next_in_sequence()
{
  kd_precinct *result = NULL;

  if (tile->num_transferred_packets == tile->total_packets)
    return NULL;
  do {
      if (order == Corder_LRCP)
        result = next_in_lrcp();
      else if (order == Corder_RLCP)
        result = next_in_rlcp();
      else if (order == Corder_RPCL)
        result = next_in_rpcl();
      else if (order == Corder_PCRL)
        result = next_in_pcrl();
      else if (order == Corder_CPRL)
        result = next_in_cprl();
      else
        assert(0);
    } while ((result == NULL) && next_progression());
  return result;
}

/*****************************************************************************/
/*                    kd_packet_sequencer::next_in_lrcp                      */
/*****************************************************************************/

kd_precinct *
  kd_packet_sequencer::next_in_lrcp()
{
  for (; layer_idx < layer_lim; layer_idx++, res_idx=res_min)
    for (; res_idx < res_lim; res_idx++, comp_idx=comp_min)
      for (; comp_idx < comp_lim; comp_idx++, pos.y=0)
        {
          kd_tile_comp *tc = tile->comps + comp_idx;
          if (res_idx > tc->dwt_levels)
            continue; // Advance to next component.
          kd_resolution *res = tc->resolutions + res_idx;
          for (; pos.y < res->precinct_indices.size.y; pos.y++, pos.x=0)
            for (; pos.x < res->precinct_indices.size.x; pos.x++)
              {
                kd_precinct **refs = res->precinct_refs +
                  pos.x + pos.y*res->precinct_indices.size.x;
                if (*refs == NULL)
                  new kd_precinct(res,pos);
                if (*refs == KD_EXPIRED_PRECINCT)
                  continue; // All layers already sequenced.
                assert((*refs)->next_layer_idx >= layer_idx);
                if ((*refs)->next_layer_idx == layer_idx)
                  return *refs;
              }
        }
  return NULL;
}

/*****************************************************************************/
/*                    kd_packet_sequencer::next_in_rlcp                      */
/*****************************************************************************/

kd_precinct *
  kd_packet_sequencer::next_in_rlcp()
{
  for (; res_idx < res_lim; res_idx++, layer_idx=0)
    for (; layer_idx < layer_lim; layer_idx++, comp_idx=comp_min)
      for (; comp_idx < comp_lim; comp_idx++, pos.y=0)
        {
          kd_tile_comp *tc = tile->comps + comp_idx;
          if (res_idx > tc->dwt_levels)
            continue; // Advance to next component.
          kd_resolution *res = tc->resolutions + res_idx;
          for (; pos.y < res->precinct_indices.size.y;
               pos.y++, pos.x=0)
            for (; pos.x < res->precinct_indices.size.x; pos.x++)
              {
                kd_precinct **refs = res->precinct_refs +
                  pos.x + pos.y*res->precinct_indices.size.x;
                if (*refs == NULL)
                  new kd_precinct(res,pos);
                if (*refs == KD_EXPIRED_PRECINCT)
                  continue; // All layers already sequenced.
                assert((*refs)->next_layer_idx >= layer_idx);
                if ((*refs)->next_layer_idx == layer_idx)
                  return *refs;
              }
        }
  return NULL;
}

/*****************************************************************************/
/*                    kd_packet_sequencer::next_in_rpcl                      */
/*****************************************************************************/

kd_precinct *
  kd_packet_sequencer::next_in_rpcl()
{
  if (layer_lim <= 0)
    return NULL;
  for (; res_idx < res_lim; res_idx++, grid_loc.y=grid_min.y)
    for (; grid_loc.y < grid_lim.y;
         grid_loc.y += grid_inc.y, grid_loc.x=grid_min.x)
      for (; grid_loc.x < grid_lim.x;
           grid_loc.x += grid_inc.x, comp_idx=comp_min)
        for (; comp_idx < comp_lim; comp_idx++)
          {
            kd_tile_comp *tc = tile->comps + comp_idx;
            if (res_idx > tc->dwt_levels)
              continue; // Advance to next component.
            kd_resolution *res = tc->resolutions + res_idx;
            pos = res->current_sequencer_pos;
            if ((pos.x >= res->precinct_indices.size.x) ||
                (pos.y >= res->precinct_indices.size.y))
              continue; // No precincts left in this resolution.
            kd_precinct **refs = res->precinct_refs +
              pos.x + pos.y*res->precinct_indices.size.x;
            if ((*refs == KD_EXPIRED_PRECINCT) ||
                ((*refs != NULL) && ((*refs)->next_layer_idx >= layer_lim)))
              { // Cannot sequence this one any further.
                pos.x++;
                if (pos.x >= res->precinct_indices.size.x)
                  { pos.x = 0; pos.y++; }
                res->current_sequencer_pos = pos;
                continue; // Move sequencing loops ahead.
              }

            int gpos;
            gpos = res->precinct_indices.pos.y + pos.y;
            gpos *= res->precinct_partition.size.y;
            gpos <<= (tc->dwt_levels-res_idx);
            gpos *= tc->sub_sampling.y;
            gpos += tile->coding_origin.y;
            if ((gpos >= grid_min.y) && (gpos != grid_loc.y))
              continue;
            gpos = res->precinct_indices.pos.x + pos.x;
            gpos *= res->precinct_partition.size.x;
            gpos <<= (tc->dwt_levels-res_idx);
            gpos *= tc->sub_sampling.x;
            gpos += tile->coding_origin.x;
            if ((gpos >= grid_min.x) && (gpos != grid_loc.x))
              continue;
            if (*refs == NULL)
              new kd_precinct(res,pos);
            assert((*refs)->next_layer_idx < layer_lim);
            return *refs;
          }
  return NULL;
}

/*****************************************************************************/
/*                    kd_packet_sequencer::next_in_pcrl                      */
/*****************************************************************************/

kd_precinct *
  kd_packet_sequencer::next_in_pcrl()
{
  if (layer_lim <= 0)
    return NULL;

  for (; grid_loc.y < grid_lim.y;
       grid_loc.y += grid_inc.y, grid_loc.x=grid_min.x)
    for (; grid_loc.x < grid_lim.x;
         grid_loc.x += grid_inc.x, comp_idx=comp_min)
      for (; comp_idx < comp_lim; comp_idx++, res_idx=res_min)
        for (; res_idx < res_lim; res_idx++)
          {
            kd_tile_comp *tc = tile->comps + comp_idx;
            if (res_idx > tc->dwt_levels)
              break; // Advance to next component.
            kd_resolution *res = tc->resolutions + res_idx;
            pos = res->current_sequencer_pos;
            if ((pos.x >= res->precinct_indices.size.x) ||
                (pos.y >= res->precinct_indices.size.y))
              continue; // No precincts left in this resolution.
            kd_precinct **refs = res->precinct_refs +
              pos.x + pos.y*res->precinct_indices.size.x;
            if ((*refs == KD_EXPIRED_PRECINCT) ||
                ((*refs != NULL) && ((*refs)->next_layer_idx >= layer_lim)))
              { // Cannot sequence this one any further.
                pos.x++;
                if (pos.x >= res->precinct_indices.size.x)
                  { pos.x = 0; pos.y++; }
                res->current_sequencer_pos = pos;
                continue; // Move sequencing loops ahead.
              }

            int gpos;
            gpos = res->precinct_indices.pos.y + pos.y;
            gpos *= res->precinct_partition.size.y;
            gpos <<= (tc->dwt_levels-res_idx);
            gpos *= tc->sub_sampling.y;
            gpos += tile->coding_origin.y;
            if ((gpos >= grid_min.y) && (gpos != grid_loc.y))
              continue;
            gpos = res->precinct_indices.pos.x + pos.x;
            gpos *= res->precinct_partition.size.x;
            gpos <<= (tc->dwt_levels-res_idx);
            gpos *= tc->sub_sampling.x;
            gpos += tile->coding_origin.x;
            if ((gpos >= grid_min.x) && (gpos != grid_loc.x))
              continue;
            if (*refs == NULL)
              new kd_precinct(res,pos);
            assert((*refs)->next_layer_idx < layer_lim);
            return *refs;
          }
  return NULL;
}

/*****************************************************************************/
/*                    kd_packet_sequencer::next_in_cprl                      */
/*****************************************************************************/

kd_precinct *
  kd_packet_sequencer::next_in_cprl()
{
  if (layer_lim <= 0)
    return NULL;
  while (comp_idx < comp_lim)
    {
      kd_tile_comp *tc = tile->comps + comp_idx;

      for (; grid_loc.y < grid_lim.y;
           grid_loc.y += grid_inc.y, grid_loc.x=grid_min.x)
        for (; grid_loc.x < grid_lim.x;
             grid_loc.x += grid_inc.x, res_idx=res_min)
          for (; res_idx < res_lim; res_idx++)
            {
              if (res_idx > tc->dwt_levels)
                break; // Advance to next position.
              kd_resolution *res = tc->resolutions + res_idx;
              pos = res->current_sequencer_pos;
              if ((pos.x >= res->precinct_indices.size.x) ||
                  (pos.y >= res->precinct_indices.size.y))
                continue; // No precincts left in this resolution.
              kd_precinct **refs = res->precinct_refs +
                pos.x + pos.y*res->precinct_indices.size.x;
              if ((*refs == KD_EXPIRED_PRECINCT) ||
                  ((*refs != NULL) && ((*refs)->next_layer_idx >= layer_lim)))
                { // Cannot sequence this one any further.
                  pos.x++;
                  if (pos.x >= res->precinct_indices.size.x)
                    { pos.x = 0; pos.y++; }
                  res->current_sequencer_pos = pos;
                  continue; // Move sequencing loops ahead.
                }

              int gpos;
              gpos = res->precinct_indices.pos.y + pos.y;
              gpos *= res->precinct_partition.size.y;
              gpos <<= (tc->dwt_levels-res_idx);
              gpos *= tc->sub_sampling.y;
              gpos += tile->coding_origin.y;
              if ((gpos >= grid_min.y) && (gpos != grid_loc.y))
                continue;
              gpos = res->precinct_indices.pos.x + pos.x;
              gpos *= res->precinct_partition.size.x;
              gpos <<= (tc->dwt_levels-res_idx);
              gpos *= tc->sub_sampling.x;
              gpos += tile->coding_origin.x;
              if ((gpos >= grid_min.x) && (gpos != grid_loc.x))
                continue;
              if (*refs == NULL)
                new kd_precinct(res,pos);
              assert((*refs)->next_layer_idx < layer_lim);
              return *refs;
            }

      // Advance component index.

      comp_idx++;
      if (comp_idx < comp_lim)
        { // Install spatial progression parameters for new tile-component.
          tc = tile->comps + comp_idx;
          grid_min = tc->grid_min;
          grid_inc = tc->grid_inc;
          grid_loc = grid_min;
        }
    }
  return NULL;
}


/* ========================================================================= */
/*                               kd_codestream                               */
/* ========================================================================= */

/*****************************************************************************/
/*                        kd_codestream::construct_common                    */
/*****************************************************************************/

void
 kd_codestream::construct_common()
{
  // Get summary parameters from SIZ object.

  siz->finalize();
  if (!(siz->get(Scomponents,0,0,num_components) &&
        siz->get(Ssize,0,0,canvas.size.y) && // Subtract y_pos later
        siz->get(Ssize,0,1,canvas.size.x) && // Subtract x_pos later
        siz->get(Sorigin,0,0,canvas.pos.y) &&
        siz->get(Sorigin,0,1,canvas.pos.x) &&
        siz->get(Stiles,0,0,tile_partition.size.y) &&
        siz->get(Stiles,0,1,tile_partition.size.x) &&
        siz->get(Stile_origin,0,0,tile_partition.pos.y) &&
        siz->get(Stile_origin,0,1,tile_partition.pos.x)))
    assert(0);
  canvas.size.y -= canvas.pos.y;
  canvas.size.x -= canvas.pos.x;
  if ((canvas.size.y <= 0) || (canvas.size.x <= 0) ||
      (tile_partition.pos.x > canvas.pos.x) ||
      (tile_partition.pos.y > canvas.pos.y) ||
      ((tile_partition.pos.x+tile_partition.size.x) <= canvas.pos.x) ||
      ((tile_partition.pos.y+tile_partition.size.y) <= canvas.pos.y))
    { kdu_error e; e << "Illegal canvas coordinates: the first tile is "
      "required to have a non-empty intersection with the image on the "
      "high resolution grid."; }

  int n;

  sub_sampling = new kdu_coords[num_components];
  crg_x = crg_y = NULL;
  precision = new int[num_components];
  is_signed = new bool[num_components];
  for (n=0; n < num_components; n++)
    {
      if (!siz->get(Sprecision,n,0,precision[n]))
        { kdu_error e; e << "No information available concerning component "
          "sample bit-depths (i.e., sample precision)."; }
      if (!siz->get(Ssigned,n,0,is_signed[n]))
        { kdu_error e; e << "No information available regarding whether "
          "components are signed or unsigned."; }
      if (!(siz->get(Ssampling,n,0,sub_sampling[n].y) &&
            siz->get(Ssampling,n,1,sub_sampling[n].x)))
        { kdu_error e; e << "No information available concerning component "
          "sub-sampling factors."; }
    }
  num_tiles.y = ceil_ratio(canvas.size.y+canvas.pos.y-tile_partition.pos.y,
                           tile_partition.size.y);
  num_tiles.x = ceil_ratio(canvas.size.x+canvas.pos.x-tile_partition.pos.x,
                           tile_partition.size.x);
  if ((num_tiles.x*num_tiles.y) > 65535)
    { kdu_error e; e << "Maximum number of allowable tiles is 65535 for any "
      "JPEG2000 code-stream.  You have managed to exceed this number!!"; }

  tile_refs = new kd_tile *[num_tiles.y*num_tiles.x];
  for (n=0; n < num_tiles.y*num_tiles.x; n++)
    tile_refs[n] = NULL;

  // Build the parameter structure.

  kdu_params *element;
  for (n=-1; n < num_tiles.y*num_tiles.x; n++)
    {
      for (int c=-1; c < num_components; c++)
        {
          element = new cod_params; element->link(siz,n,c);
          element = new qcd_params; element->link(siz,n,c);
          element = new rgn_params; element->link(siz,n,c);
        }
      element = new poc_params; element->link(siz,n,-1);
    }
  element = new crg_params; element->link(siz,-1,-1);

  // Now set up some common services, which we are sure to need.

  buf_server = new kd_buf_server;
  buf_server->attach();
  shared_block = new kdu_block();

  // Now some final initialization steps.

  if (in != NULL)
    {
      do {
          if (!marker->read())
            { kdu_error e;
              e << "Code-stream must contain at least one SOT marker!";}
          if (marker->get_code() == KDU_PPM)
            {
              if (ppm_markers == NULL)
                ppm_markers = new kd_pp_markers();
              ppm_markers->add_marker(*marker);
            }
          else
            siz->translate_marker_segment(marker->get_code(),
                                          marker->get_length(),
                                          marker->get_bytes(),-1,0);
        } while (marker->get_code() != KDU_SOT);
    }

  region = canvas;
  first_apparent_component = 0;
  num_apparent_components = num_components;
  discard_levels = 0;
  max_apparent_layers = 0xFFFF;
  max_tile_layers = 1;
  tiles_accessed = false;
  stats = NULL;
  start_time = clock();
}

/*****************************************************************************/
/*                        kd_codestream::~kd_codestream                      */
/*****************************************************************************/

kd_codestream::~kd_codestream()
{
  if (ppm_markers != NULL)
    delete ppm_markers;
  if (marker != NULL)
    delete marker;
  if (in != NULL)
    { delete in; in = NULL; }
  if (out != NULL)
    { delete out; out = NULL; }
  if (shared_block != NULL)
    delete shared_block;
  if (sub_sampling != NULL)
    delete[] sub_sampling;
  if (crg_x != NULL)
    delete[] crg_x;
  if (crg_y != NULL)
    delete[] crg_y;
  if (precision != NULL)
    delete[] precision;
  if (is_signed != NULL)
    delete[] is_signed;
  if (tile_refs != NULL)
    {
      for (int n=0; n < num_tiles.x*num_tiles.y; n++)
        if ((tile_refs[n] != NULL) && (tile_refs[n] != KD_EXPIRED_TILE))
          delete tile_refs[n]; // Invokes tile destructor.
      delete[] tile_refs;
    }
  if (siz != NULL)
    delete siz;
  if (buf_server != NULL)
    {
      buf_server->detach();
      if (buf_server->can_destroy())
        delete buf_server;
    }
  if (stats != NULL)
    delete stats;
}

/*****************************************************************************/
/*                         kd_codestream::create_tile                        */
/*****************************************************************************/

kd_tile *

  kd_codestream::create_tile(int tnum)
{
  kd_tile *tp = tile_refs[tnum];
  assert(tp == NULL);

  tp = tile_refs[tnum] = new kd_tile(this,tnum);
  if ((in != NULL) && (!persistent) && !(tp->dims & region))
    delete tp;
  else
    tp->initialize();
  return tp;
}

/*****************************************************************************/
/*                   kd_codestream::trim_compressed_data                     */
/*****************************************************************************/

void
  kd_codestream::trim_compressed_data()
{
  if (stats == NULL)
    return;
  kdu_uint16 threshold = stats->get_conservative_slope_threshold(true);
  if (threshold <= 1)
    return;

  for (int t=0; t < num_tiles.x*num_tiles.y; t++)
    {
      kd_tile *tile = tile_refs[t];
      assert(tile != KD_EXPIRED_TILE);
      if (tile == NULL)
        continue;
      for (int c=0; c < tile->num_components; c++)
        {
          kd_tile_comp *tc = tile->comps+c;
          for (int r=0; r <= tc->dwt_levels; r++)
            {
              kd_resolution *res = tc->resolutions + r;
              for (int p=0; p < res->precinct_indices.area(); p++)
                {
                  kd_precinct *precinct = res->precinct_refs[p];
                  assert(precinct != KD_EXPIRED_PRECINCT);
                  if (precinct == NULL)
                    continue;
                  for (int b=res->min_band; b <= res->max_band; b++)
                    {
                      kd_precinct_band *pb = precinct->bands + b;
                      for (int n=0; n < pb->block_indices.area(); n++)
                        {
                          kd_block *block = pb->blocks + n;
                          block->trim_data(threshold,buf_server);
                        }
                    }
                }
            }
        }
    }
}

/*****************************************************************************/
/*                    kd_codestream::get_min_header_cost                     */
/*****************************************************************************/

void
  kd_codestream::get_min_header_cost(int &fixed_cost, int &per_layer_cost)
{
  per_layer_cost = 0;
  fixed_cost = 2 + siz->generate_marker_segments(NULL,-1,0);
#ifdef KDU_IDENTIFIER
  fixed_cost += strlen(KDU_IDENTIFIER)+6;
#endif
  for (int tnum=0; tnum < num_tiles.x*num_tiles.y; tnum++)
    {
      kd_tile *tile = tile_refs[tnum];
      assert((tile != NULL) && (tile != KD_EXPIRED_TILE));

      // Calculate cost of all tile-part headers for this tile.
      int new_bytes = 0;
      int try_tpart = 0;
      do {
          new_bytes = siz->generate_marker_segments(NULL,tnum,try_tpart);
          if ((new_bytes > 0) || (try_tpart == 0))
            {
              new_bytes += 12; // Cost of SOT marker segment
              new_bytes += 2; // Cost of SOD marker
            }
          fixed_cost += new_bytes;
          try_tpart++;
        } while (new_bytes != 0);
     
      // Calculate min packet header cost
      per_layer_cost += tile->total_precincts;
      if (tile->use_eph)
        per_layer_cost += tile->total_precincts*2;
      if (tile->use_sop)
        per_layer_cost += tile->total_precincts*6;
    }
}

/*****************************************************************************/
/*                       kd_codestream::simulate_output                      */
/*****************************************************************************/

int
  kd_codestream::simulate_output(int &header_bytes, int layer_idx,
                                  kdu_uint16 slope_threshold,
                                  bool finalize_layer, bool last_layer,
                                  int max_bytes, int *sloppy_bytes)
{
  header_bytes = 0;

  if (layer_idx == 0)
    {
      header_bytes = 2 + siz->generate_marker_segments(NULL,-1,0);
#ifdef KDU_IDENTIFIER
      header_bytes += strlen(KDU_IDENTIFIER)+6;
#endif
    }

  int total_bytes = header_bytes;

  for (int n=0; n < num_tiles.x*num_tiles.y; n++)
    {
      if (total_bytes > max_bytes)
        {
          assert(!finalize_layer);
          return total_bytes;
        }
      int tile_header_bytes;
      assert((tile_refs[n] != NULL) && (tile_refs[n] != KD_EXPIRED_TILE));
      total_bytes +=
        tile_refs[n]->simulate_output(tile_header_bytes,layer_idx,
                                      slope_threshold,finalize_layer,
                                      last_layer,max_bytes-total_bytes,
                                      sloppy_bytes);
      header_bytes += tile_header_bytes;
    }
  return total_bytes;
}

/*****************************************************************************/
/*                          kd_codestream::pcrd_opt                          */
/*****************************************************************************/

void
  kd_codestream::pcrd_opt(int layer_bytes[], kdu_uint16 slope_thresholds[],
                          int num_layers)
{
  int rough_upper_bound = 0;
  int fixed_header_cost, per_layer_header_cost;
  get_min_header_cost(fixed_header_cost,per_layer_header_cost);
  int new_header_bytes, new_layer_bytes;
  int last_layer_bytes, last_target_bytes, layer_idx;
  
  if (layer_bytes[num_layers-1] == 0)
    { // Find rough upper bound for the purpose of spacing intermediate layers
      rough_upper_bound =
        simulate_output(new_header_bytes,0,0,false,false);
    }

  // Now work through the layers.
  int threshold;
  int min_threshold = 0; // Inclusive lower bound.
  int lim_threshold = (1<<16); // Exclusive upper bound.
  last_layer_bytes = 0;
  last_target_bytes = 0;
  for (layer_idx=0; layer_idx < num_layers; layer_idx++)
    {
      bool last_layer = (layer_idx == (num_layers-1));
      bool auto_targeting = (layer_bytes[layer_idx] == 0);
      if (auto_targeting)
        { // Have to make up a suitable number of target bytes
          if (last_layer)
            {
              layer_bytes[layer_idx] = INT_MAX;
              auto_targeting = false;
            }
          else
            {
              int next_idx, next_target_bytes;
              for (next_idx=layer_idx+1; next_idx < num_layers; next_idx++)
                if ((next_target_bytes = layer_bytes[next_idx]) > 0)
                  break;
              if (next_target_bytes == 0)
                { // Can only happen if final layer has no assigned rate target
                  next_idx = num_layers-1;
                  next_target_bytes = rough_upper_bound;
                }
              if (layer_idx > 0)
                { // Logarithmically interpolate known rate targets, after
                  // discounting the minimum header cost.
                  int start_bytes = last_target_bytes
                    - fixed_header_cost - layer_idx*per_layer_header_cost;
                  int end_bytes = next_target_bytes
                    - fixed_header_cost - (next_idx+1)*per_layer_header_cost;
                  if (start_bytes < 1)
                    start_bytes = 1;
                  if (end_bytes < start_bytes)
                    end_bytes = start_bytes;
                  double y_start = log((double)start_bytes);
                  double y_end = log((double)end_bytes);
                  double x_start = layer_idx-1;
                  double x_end = next_idx;
                  double x = layer_idx;
                  double y = y_start +
                    (x-x_start)/(x_end-x_start)*(y_end-y_start);
                  layer_bytes[layer_idx] = ((int) exp(y))
                    + fixed_header_cost + (layer_idx+1)*per_layer_header_cost;
                }
              else
                { // Need to settle on a good starting rate target.
                  int end_bytes = next_target_bytes
                    - fixed_header_cost - (next_idx+1)*per_layer_header_cost;
                  if (end_bytes < 1)
                    end_bytes = 1;
                  int start_bytes = (int) (end_bytes / sqrt((double)( 1<<next_idx)));
                  layer_bytes[0] = start_bytes
                    + fixed_header_cost + per_layer_header_cost;
                }
            }
        }

      // First use a simple bisection search for the slope threshold
      if (lim_threshold < 1)
        lim_threshold = 1;
      int target_bytes =
        layer_bytes[layer_idx] - last_layer_bytes - 2; // 2 is for an EOC
      int best_threshold = lim_threshold-1;
      min_threshold = 0;
      do {
          threshold = (min_threshold+lim_threshold)>>1;
          assert(threshold < (1<<16));
          new_layer_bytes =
            simulate_output(new_header_bytes,layer_idx,(kdu_uint16) threshold,
                            false,last_layer,target_bytes);
          if (new_layer_bytes > target_bytes)
            min_threshold = threshold+1;
          else if (new_layer_bytes <= target_bytes)
            {
              best_threshold = threshold;
              if (new_layer_bytes == target_bytes)
                break; // Found target exactly
              lim_threshold = threshold;
            }
        } while (lim_threshold > min_threshold);

      // Finalize the allocation for this layer
      threshold = best_threshold;
      new_layer_bytes =  // Don't finalize yet
        simulate_output(new_header_bytes,layer_idx,(kdu_uint16) threshold,
                        false,last_layer);
      int remaining_bytes = target_bytes - new_layer_bytes;
      if ((remaining_bytes < 0) && !auto_targeting)
        { kdu_warning w; w << "Unable to achieve the rate target "
          "for quality layer " << layer_idx << ".  Try requesting a smaller "
          "number of quality layers.\n";
        }
      if (last_layer && (remaining_bytes > 0) && (threshold > 0))
        {
          threshold--;
          new_layer_bytes = // Finalize with sloppy fill in.
            simulate_output(new_header_bytes,layer_idx,(kdu_uint16) threshold,
                            true,true,target_bytes,&remaining_bytes);
        }
      else
        new_layer_bytes = // Finalize without sloppy fill in.
          simulate_output(new_header_bytes,layer_idx,(kdu_uint16) threshold,
                          true,last_layer);
      slope_thresholds[layer_idx] = (kdu_uint16) threshold;
      lim_threshold = threshold+1; // We are allowed to repeat the threshold.
      last_layer_bytes += new_layer_bytes;
      last_target_bytes = layer_bytes[layer_idx];
      layer_bytes[layer_idx] = last_layer_bytes+2; // Put EOC back.
    }
}

/*****************************************************************************/
/*                  kd_codestream::generate_codestream                       */
/*****************************************************************************/

void
  kd_codestream::generate_codestream(int max_layers,
                                     kdu_uint16 slope_thresholds[])
{
  assert(out != NULL);

  // First write the main header.

  out->put(KDU_SOC);
  siz->generate_marker_segments(out,-1,0);
#ifdef KDU_IDENTIFIER
  int length = strlen(KDU_IDENTIFIER);
  out->put(KDU_COM);
  out->put((kdu_uint16)(length+4));
  out->put((kdu_uint16) 1); // Latin values for COM body.
  out->write((kdu_byte *) KDU_IDENTIFIER,length);
#endif

  // Now cycle through the tiles, interleaving their tile-parts.

  bool done;
  do {
      done = true;
      for (int n=0; n < num_tiles.x*num_tiles.y; n++)
        {
          assert(tile_refs[n] != KD_EXPIRED_TILE);
          if (tile_refs[n] == NULL)
            { kdu_error e; e << "You may not currently flush compressed "
              "code-stream data without completing the compression of all "
              "tiles."; }
          int tpart_bytes =
            tile_refs[n]->generate_tile_part(max_layers,slope_thresholds);
          if (tpart_bytes > 0)
            done = false;
        }
    } while (!done);

  // Finish up with the EOC marker.
  out->put(KDU_EOC);
}


/* ========================================================================= */
/*                              kdu_codestream                               */
/* ========================================================================= */

/*****************************************************************************/
/*                       kdu_codestream::create (input)                      */
/*****************************************************************************/

void
  kdu_codestream::create(kdu_compressed_source *source)
{
  assert(state == NULL);
  state = new kd_codestream;
  state->in = new kd_compressed_input(source);
  state->marker = new kd_marker(state->in);
  if ((!state->marker->read()) ||
      (state->marker->get_code() != KDU_SOC))
    { kdu_error e; e << "Code-stream must start with an SOC marker!"; }
  state->siz = new siz_params;
  if (!(state->marker->read() &&
        state->siz->translate_marker_segment(state->marker->get_code(),
                                             state->marker->get_length(),
                                             state->marker->get_bytes(),-1,0)))
    { kdu_error e; e << "Code-stream must contain a valid SIZ marker segment, "
      "immediately after the SOC marker!"; }
  state->construct_common();
}

/*****************************************************************************/
/*                          kdu_codestream::create (output)                  */
/*****************************************************************************/

void
  kdu_codestream::create(siz_params *siz_in,
                         kdu_compressed_target *target)
{
  assert(state == NULL);
  state = new kd_codestream;
  state->out = new kd_compressed_output(target);
  state->siz = new siz_params;
  state->siz->copy_from(siz_in,-1,-1);
  state->construct_common();
}

/*****************************************************************************/
/*                           kdu_codestream::destroy                         */
/*****************************************************************************/

void
  kdu_codestream::destroy()
{
  assert(state != NULL);
  delete state;
  state = NULL;
}

/*****************************************************************************/
/*                       kdu_codestream::share_buffering                     */
/*****************************************************************************/

void
  kdu_codestream::share_buffering(kdu_codestream existing)
{
  assert(!state->tiles_accessed);
  state->buf_server->detach();
  delete state->buf_server;
  state->buf_server = existing.state->buf_server;
  state->buf_server->attach();
}

/*****************************************************************************/
/*                       kdu_codestream::set_persistent                      */
/*****************************************************************************/

void
  kdu_codestream::set_persistent()
{
  assert(!state->tiles_accessed);
  state->persistent = true;
}

/*****************************************************************************/
/*                          kdu_codestream::access_siz                       */
/*****************************************************************************/

siz_params *
  kdu_codestream::access_siz()
{
  return state->siz;
}

/*****************************************************************************/
/*                      kdu_codestream::set_textualization                   */
/*****************************************************************************/

void
  kdu_codestream::set_textualization(ostream *stream)
{
  assert(!state->tiles_accessed);
  if (stream != NULL)
    {
      state->siz->textualize_attributes(*stream,-1,-1);
      stream->flush();
    }
  state->textualize_out = stream;
}

/*****************************************************************************/
/*                        kdu_codestream::set_max_bytes                      */
/*****************************************************************************/

void
  kdu_codestream::set_max_bytes(int max_bytes)
{
  assert(!state->tiles_accessed);

  if (state->in != NULL)
    {
      state->in->set_max_bytes(max_bytes);
      if (state->in->failed())
        { kdu_error e; e << "Attempting to impose too small a limit on the "
          "number of code-stream bytes. " << max_bytes << " bytes is "
          "insufficient to accomodate even the main header!"; }
    }
  else
    {
      if (state->stats != NULL)
        { kdu_error e; e << "\"kdu_codestream::set_max_bytes\" may not be "
          "called multiple times."; }
      int total_samples = 0;
      for (int c=0; c < state->num_components; c++)
        {
          kdu_dims comp_dims; get_dims(c,comp_dims);
          total_samples += comp_dims.area();
        }
      state->stats = new kd_compressed_stats(total_samples,max_bytes);
    }
}

/*****************************************************************************/
/*                   kdu_codestream::set_min_slope_threshold                 */
/*****************************************************************************/

void
  kdu_codestream::set_min_slope_threshold(kdu_uint16 threshold)
{
  state->min_slope_threshold = threshold;
}

/*****************************************************************************/
/*                        kdu_codestream::set_resilient                      */
/*****************************************************************************/

void
  kdu_codestream::set_resilient(bool expect_ubiquitous_sops)
{
  state->resilient = true;
  state->expect_ubiquitous_sops = expect_ubiquitous_sops;
  state->fussy = false;
  state->shared_block->resilient = state->resilient;
  state->shared_block->fussy = state->fussy;
}

/*****************************************************************************/
/*                          kdu_codestream::set_fussy                        */
/*****************************************************************************/

void
  kdu_codestream::set_fussy()
{
  state->resilient = false;
  state->fussy = true;
  state->shared_block->resilient = state->resilient;
  state->shared_block->fussy = state->fussy;
}

/*****************************************************************************/
/*                          kdu_codestream::set_fast                         */
/*****************************************************************************/

void
  kdu_codestream::set_fast()
{
  state->resilient = false;
  state->fussy = false;
  state->shared_block->resilient = state->resilient;
  state->shared_block->fussy = state->fussy;
}

/*****************************************************************************/
/*                kdu_codestream::apply_input_restrictions                   */
/*****************************************************************************/

void
  kdu_codestream::apply_input_restrictions(int first_component,
                                           int max_components,
                                           int discard_levels,
                                           int max_layers,
                                           kdu_dims *region_of_interest)
{
  if (state->in == NULL)
    { kdu_error e; e << "Input restrictions may be applied only to codestream "
      "objects opened for input!"; }
  if (state->tiles_accessed)
    {
      if (state->num_open_tiles != 0)
        { kdu_error e; e << "You may apply restrictions to the resolution "
          "or number of image components only after closing all open tiles."; }
      if (state->tiles_accessed && !state->persistent)
        { kdu_error e; e << "You may not apply restrictions to the resolution "
          "or number of image components after the first tile access, unless "
          "the codestream object is set up to be persistent."; }
    }
  if ((first_component < 0) || (first_component >= state->num_components))
    { kdu_error e; e << "The range of apparent image components supplied to "
      "`kdu_codestream::apply_input_restrictions' is empty or illegal!"; }
  state->first_apparent_component = first_component;
  if (max_components <= 0)
    max_components = state->num_components;
  max_components += first_component;
  if (max_components > state->num_components)
    state->num_apparent_components = state->num_components-first_component;
  else
    state->num_apparent_components = max_components-first_component;
  state->discard_levels = discard_levels;
  if (max_layers <= 0)
    max_layers = 0xFFFF;
  state->max_apparent_layers = max_layers;
  state->region = state->canvas;
  if (region_of_interest != NULL)
    state->region &= *region_of_interest;
}

/*****************************************************************************/
/*                    kdu_codestream::change_appearance                      */
/*****************************************************************************/

void
  kdu_codestream::change_appearance(bool transpose, bool vflip, bool hflip)
{
  if (state->tiles_accessed)
    {
      if (state->num_open_tiles != 0)
        { kdu_error e; e << "You may change the apparent geometry of the "
          "code-stream only after closing all open tiles."; }
      if (state->tiles_accessed && !state->persistent)
        { kdu_error e; e << "You may not change the apparent geometry of the "
          "code-stream after the first tile access, unless "
          "the codestream object is set up to be persistent."; }
    }
  state->transpose = transpose;
  state->vflip = vflip;
  state->hflip = hflip;
  state->shared_block->transpose = transpose;
  state->shared_block->vflip = vflip;
  state->shared_block->hflip = hflip;
}

/*****************************************************************************/
/*                       kdu_codestream::get_tile_dims                       */
/*****************************************************************************/

void
  kdu_codestream::get_tile_dims(kdu_coords tile_idx,
                                int comp_idx, kdu_dims &dims)
{
  state->from_apparent(tile_idx);
  assert((tile_idx.x >= 0) && (tile_idx.x < state->num_tiles.x) &&
         (tile_idx.y >= 0) && (tile_idx.y < state->num_tiles.y));
  dims = state->tile_partition;
  dims.pos.x += tile_idx.x * dims.size.x;
  dims.pos.y += tile_idx.y * dims.size.y;
  dims &= state->canvas;
  kdu_coords min = dims.pos;
  kdu_coords lim = min + dims.size;
  kdu_coords subs = kdu_coords(1,1);
  if (comp_idx >= 0)
    {
      assert(comp_idx < state->num_apparent_components);
      comp_idx += state->first_apparent_component;
      subs = state->sub_sampling[comp_idx];
    }
  subs.x <<= state->discard_levels;
  subs.y <<= state->discard_levels;
  min.x = ceil_ratio(min.x,subs.x);
  min.y = ceil_ratio(min.y,subs.y);
  lim.x = ceil_ratio(lim.x,subs.x);
  lim.y = ceil_ratio(lim.y,subs.y);
  dims.pos = min;
  dims.size = lim-min;
  state->to_apparent(dims);
}

/*****************************************************************************/
/*                    kdu_codestream::get_num_components                     */
/*****************************************************************************/

int
  kdu_codestream::get_num_components()
{
  return state->num_apparent_components;
}

/*****************************************************************************/
/*                      kdu_codestream::get_bit_depth                        */
/*****************************************************************************/

int
  kdu_codestream::get_bit_depth(int comp_idx)
{
  assert((comp_idx >= 0) && (comp_idx < state->num_apparent_components));
  comp_idx += state->first_apparent_component;
  return state->precision[comp_idx];
}

/*****************************************************************************/
/*                       kdu_codestream::get_signed                          */
/*****************************************************************************/

bool
  kdu_codestream::get_signed(int comp_idx)
{
  assert((comp_idx >= 0) && (comp_idx < state->num_apparent_components));
  comp_idx += state->first_apparent_component;
  return state->is_signed[comp_idx];
}

/*****************************************************************************/
/*                      kdu_codestream::get_subsampling                      */
/*****************************************************************************/

void
  kdu_codestream::get_subsampling(int comp_idx, kdu_coords &subs)
{
  assert((comp_idx >= 0) && (comp_idx < state->num_apparent_components));
  comp_idx += state->first_apparent_component;
  subs = state->sub_sampling[comp_idx];
  subs.x <<= state->discard_levels;
  subs.y <<= state->discard_levels;
  if (state->transpose)
    subs.transpose();
}

/*****************************************************************************/
/*                      kdu_codestream::get_registration                     */
/*****************************************************************************/

void
  kdu_codestream::get_registration(int comp_idx, kdu_coords scale,
                                   kdu_coords &reg)
{
  assert((comp_idx >= 0) && (comp_idx < state->num_apparent_components));
  comp_idx += state->first_apparent_component;
  if (state->crg_x == NULL)
    {
      assert(state->crg_y == NULL);
      state->crg_x = new float[state->num_components];
      state->crg_y = new float[state->num_components];
      kdu_params *crg = state->siz->access_cluster(CRG_params);
      for (int n=0; n < state->num_components; n++)
        if ((crg == NULL) || (!crg->get(CRGoffset,n,0,state->crg_y[n])) ||
            (!crg->get(CRGoffset,n,1,state->crg_x[n])))
          state->crg_x[n] = state->crg_y[n] = 0.0F;
    }
  if (state->transpose)
    scale.transpose();
  reg.x = (int) floor(state->crg_x[comp_idx]*scale.x+0.5);
  reg.y = (int) floor(state->crg_y[comp_idx]*scale.y+0.5);
  state->to_apparent(reg);
}

/*****************************************************************************/
/*                           kdu_codestream::get_dims                        */
/*****************************************************************************/
    
void
  kdu_codestream::get_dims(int comp_idx, kdu_dims &dims)
{
  if (comp_idx < 0)
    dims = state->region;
  else
    {
      assert(comp_idx < state->num_apparent_components);
      comp_idx += state->first_apparent_component;

      kdu_coords min = state->region.pos;
      kdu_coords lim = min+state->region.size;
      int y_fact = state->sub_sampling[comp_idx].y << state->discard_levels;
      int x_fact = state->sub_sampling[comp_idx].x << state->discard_levels;

      min.x = ceil_ratio(min.x,x_fact);
      lim.x = ceil_ratio(lim.x,x_fact);
      min.y = ceil_ratio(min.y,y_fact);
      lim.y = ceil_ratio(lim.y,y_fact);
      dims.pos = min;
      dims.size = lim-min;
    }
  state->to_apparent(dims);
}

/*****************************************************************************/
/*                      kdu_codestream::get_max_tile_layers                  */
/*****************************************************************************/

int
  kdu_codestream::get_max_tile_layers()
{
  return state->max_tile_layers;
}

/*****************************************************************************/
/*                          kdu_codestream::map_region                       */
/*****************************************************************************/

void
  kdu_codestream::map_region(int comp_idx, kdu_dims comp_region,
                             kdu_dims &canvas_region)
{
  assert((comp_idx >= 0) && (comp_idx < state->num_apparent_components));
  comp_idx += state->first_apparent_component;

  state->from_apparent(comp_region);
  kdu_coords min = comp_region.pos;
  kdu_coords lim = min + comp_region.size;
  min.x *= state->sub_sampling[comp_idx].x << state->discard_levels;
  min.y *= state->sub_sampling[comp_idx].y << state->discard_levels;
  lim.x *= state->sub_sampling[comp_idx].x << state->discard_levels;
  lim.y *= state->sub_sampling[comp_idx].y << state->discard_levels;
  canvas_region.pos = min;
  canvas_region.size = lim-min;
}

/*****************************************************************************/
/*                       kdu_codestream::get_valid_tiles                     */
/*****************************************************************************/

void
  kdu_codestream::get_valid_tiles(kdu_dims &indices)
{
  kdu_coords min = state->region.pos - state->tile_partition.pos;
  kdu_coords lim = min + state->region.size;

  indices.pos.x = floor_ratio(min.x,state->tile_partition.size.x);
  indices.size.x =
    ceil_ratio(lim.x,state->tile_partition.size.x)-indices.pos.x;
  if (lim.x <= min.x)
    indices.size.x = 0;
  indices.pos.y = floor_ratio(min.y,state->tile_partition.size.y);
  indices.size.y =
    ceil_ratio(lim.y,state->tile_partition.size.y) - indices.pos.y;
  if (lim.y <= min.y)
    indices.size.y = 0;
  state->to_apparent(indices);
}

/*****************************************************************************/
/*                          kdu_codestream::find_tile                        */
/*****************************************************************************/

bool
  kdu_codestream::find_tile(int comp_idx, kdu_coords loc, kdu_coords &tile_idx)
{
  assert((comp_idx >= 0) && (comp_idx < state->num_apparent_components));
  comp_idx += state->first_apparent_component;

  state->from_apparent(loc);
  loc.x *= state->sub_sampling[comp_idx].x << state->discard_levels;
  loc.y *= state->sub_sampling[comp_idx].y << state->discard_levels;
  loc -= state->region.pos;
  if ((loc.x < 0) || (loc.y < 0) ||
      (loc.x >= state->region.size.x) || (loc.y >= state->region.size.y))
    return false;
  loc += state->region.pos;
  loc -= state->tile_partition.pos;
  tile_idx.x = floor_ratio(loc.x,state->tile_partition.size.x);
  tile_idx.y = floor_ratio(loc.y,state->tile_partition.size.y);
  state->to_apparent(tile_idx);
  return true;
}

/*****************************************************************************/
/*                         kdu_codestream::open_tile                         */
/*****************************************************************************/

kdu_tile
  kdu_codestream::open_tile(kdu_coords tile_idx)
{
  state->tiles_accessed = true;

  state->from_apparent(tile_idx);
  assert((tile_idx.x >= 0) && (tile_idx.x < state->num_tiles.x) &&
         (tile_idx.y >= 0) && (tile_idx.y < state->num_tiles.y));
  int tnum = tile_idx.x + tile_idx.y * state->num_tiles.x;
  kd_tile *tp = state->tile_refs[tnum];

  if (tp == NULL)
    tp = state->create_tile(tnum);
  if ((tp == KD_EXPIRED_TILE) || tp->closed)
    { kdu_error e; e << "Attempting to access a tile which has already been "
      "discarded or closed!"; }
  tp->open();
  return kdu_tile(tp);
}

/*****************************************************************************/
/*                          kdu_codestream::flush                            */
/*****************************************************************************/

void
  kdu_codestream::flush(int layer_bytes[], int num_layer_specs,
                        kdu_uint16 *thresholds)
{
  assert(num_layer_specs > 0);
  if (thresholds != NULL)
    {
      if (thresholds[0] != 0)
        { // Use the supplied thresholds to generate layers.
          int n, header_bytes, total_bytes = 0;
          for (n=0; n < num_layer_specs; n++)
            {
              bool last_layer = (n == (num_layer_specs-1));
              kdu_uint16 thresh = thresholds[n];
              total_bytes +=
                state->simulate_output(header_bytes,n,thresh,true,last_layer);
              layer_bytes[n] = total_bytes;
            }
        }
      else
        state->pcrd_opt(layer_bytes,thresholds,num_layer_specs);
      state->generate_codestream(num_layer_specs,thresholds);
    }
  else
    {
      kdu_uint16 *slope_thresholds = new kdu_uint16[num_layer_specs];
      state->pcrd_opt(layer_bytes,slope_thresholds,num_layer_specs);
      state->generate_codestream(num_layer_specs,slope_thresholds);
      delete[] slope_thresholds;
    }
}

/*****************************************************************************/
/*                         kdu_codestream::trans_out                         */
/*****************************************************************************/

int
  kdu_codestream::trans_out(int max_bytes)
{
  int num_layers=0;
  for (int t=0; t < state->num_tiles.x*state->num_tiles.y; t++)
    {
      kd_tile *tile = state->tile_refs[t];
      assert(tile != KD_EXPIRED_TILE);
      if ((tile != NULL) && (tile->num_layers > num_layers))
        num_layers = tile->num_layers;
    }
  kdu_uint16 *slope_thresholds = new kdu_uint16[num_layers];
  int n;
  for (n=0; n < num_layers; n++)
    slope_thresholds[n] = 0xFFFF-n-1;

  // Run simulations until we find a suitable number of non-empty layers
  max_bytes -= 2; // Allow room for the EOC marker.
  int total_bytes, header_bytes;
  bool last_layer;
  do {
      total_bytes = 0;
      for (n=0; n < num_layers; n++)
        {
          last_layer = (n == (num_layers-1));
          kdu_uint16 threshold = slope_thresholds[n];
          if (last_layer)
            threshold = 0xFFFF; // This way we will just count the header cost
          total_bytes +=
            state->simulate_output(header_bytes,n,threshold,true,last_layer);
          if (total_bytes >= max_bytes)
            {
              if (last_layer)
                {
                  num_layers = n; // Could not even afford the header cost.
                  last_layer = false; // Make sure we iterate.
                }
              else
                num_layers = n+1; // Try running current layer as the last one.
            }
        }
      if (num_layers == 0)
        { kdu_error e; e << "You have set the byte limit too low.  "
          "All compressed data would have to be discarded!"; }
    } while (!last_layer);

  // Now go back through all the layers, finalizing the allocation.
  total_bytes = 0;
  for (n=0; n < num_layers; n++)
    {
      last_layer = (n == (num_layers-1));
      kdu_uint16 threshold = slope_thresholds[n];
      if (!last_layer)
        {
          total_bytes +=
            state->simulate_output(header_bytes,n,threshold,true,false);
          assert(total_bytes < max_bytes);
        }
      else
        { // Last layer is a sloppy one
          int trial_bytes = total_bytes +
            state->simulate_output(header_bytes,n,threshold+1,false,true);
          assert(trial_bytes <= max_bytes); // Header cost only.
          int sloppy_bytes = max_bytes - trial_bytes;
          total_bytes +=
            state->simulate_output(header_bytes,n,threshold,true,true,
                                   max_bytes-total_bytes,&sloppy_bytes);
          assert(total_bytes <= max_bytes);
        }
    }

  // Finally, generate the code-stream.
  state->generate_codestream(num_layers,slope_thresholds);
  delete[] slope_thresholds;
  return num_layers;
}

/*****************************************************************************/
/*                      kdu_codestream::get_total_bytes                      */
/*****************************************************************************/

int
  kdu_codestream::get_total_bytes()
{
  if (state->in != NULL)
    return state->in->get_bytes_read();
  else if (state->out != NULL)
    return state->out->get_bytes_written();
  else
    return 0;
}

/*****************************************************************************/
/*                      kdu_codestream::get_num_tparts                       */
/*****************************************************************************/

int
  kdu_codestream::get_num_tparts()
{
  return state->num_tparts_used;
}

/*****************************************************************************/
/*                   kdu_codestream::collect_timing_stats                    */
/*****************************************************************************/

void
  kdu_codestream::collect_timing_stats(int num_coder_iterations)
{
  if (num_coder_iterations < 0)
    num_coder_iterations = 0;
  state->shared_block->initialize_timing(num_coder_iterations);
  state->start_time = clock();
}

/*****************************************************************************/
/*                     kdu_codestream::get_timing_stats                      */
/*****************************************************************************/

double
  kdu_codestream::get_timing_stats(int *num_samples, bool coder_only)
{
  double system_time = clock() - state->start_time;
  system_time /= CLOCKS_PER_SEC;
  int system_samples = 0;
  for (int c=0; c < state->num_apparent_components; c++)
    {
      kdu_dims region; get_dims(c,region);
      system_samples += region.area();
    }

  int coder_samples;
  double wasted_time;
  double coder_time =
    state->shared_block->get_timing_stats(coder_samples,wasted_time);
  system_time -= wasted_time;

  if (coder_only)
    {
      if (num_samples != NULL)
        *num_samples = coder_samples;
      return coder_time;
    }
  if (num_samples != NULL)
    *num_samples = system_samples;
  return system_time;
}

/*****************************************************************************/
/*                kdu_codestream::get_compressed_data_memory                 */
/*****************************************************************************/

int
  kdu_codestream::get_compressed_data_memory()
{
  return state->buf_server->get_peak_bytes();
}

/*****************************************************************************/
/*                kdu_codestream::get_compressed_state_memory                */
/*****************************************************************************/

int
  kdu_codestream::get_compressed_state_memory()
{
  return state->peak_var_structure_bytes;
}
