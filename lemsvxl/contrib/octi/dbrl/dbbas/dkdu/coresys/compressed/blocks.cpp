/*****************************************************************************/
// File: blocks.cpp [scope = CORESYS/COMPRESSED]
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
   Implements a part of the compressed data management machinery, in
relation to code-blocks.  Includes packet header parsing and construction.
Note that the internal representation employed for code-block state
information has been carefully paired down to a point where it would be
difficult to implement packet operations with substantially less
memory.
******************************************************************************/

#include <string.h>
#include <assert.h>
#include "kdu_elementary.h"
#include "kdu_messaging.h"
#include "kdu_compressed.h"
#include "compressed_local.h"

/* ========================================================================= */
/*                                kdu_block                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                          kdu_block::kdu_block                             */
/*****************************************************************************/

kdu_block::kdu_block()
{
  transpose = vflip = hflip = false;
  pass_lengths = NULL; pass_slopes = NULL; max_passes = 0;
  byte_buffer = NULL;  max_bytes = 0;
  sample_buffer = NULL; max_samples = 0;
  context_buffer = NULL; max_contexts = 0;
  cpu_iterations = 0;
  cpu_time = 0;
  cpu_unique_samples = 0;
  resilient = fussy = false;
}

/*****************************************************************************/
/*                          kdu_block::~kdu_block                            */
/*****************************************************************************/

kdu_block::~kdu_block()
{
  if (pass_lengths != NULL)
    delete[] pass_lengths;
  if (pass_slopes != NULL)
    delete[] pass_slopes;
  if (byte_buffer != NULL)
    delete[] (byte_buffer-1);
  if (sample_buffer != NULL)
    delete[] sample_buffer;
  if (context_buffer != NULL)
    delete[] context_buffer;
}

/*****************************************************************************/
/*                        kdu_block::set_max_passes                          */
/*****************************************************************************/

void
  kdu_block::set_max_passes(int new_passes, bool copy_existing)
{
  if (max_passes >= new_passes)
    return;
  if (max_passes == 0)
    copy_existing = false;
  if (copy_existing)
    {
      int *new_pass_lengths = new int[new_passes];
      kdu_uint16 *new_pass_slopes = new kdu_uint16[new_passes];
      for (int n=0; n < max_passes; n++)
        {
          new_pass_lengths[n] = pass_lengths[n];
          new_pass_slopes[n] = pass_slopes[n];
        }
      delete[] pass_lengths;
      delete[] pass_slopes;
      pass_lengths = new_pass_lengths;
      pass_slopes = new_pass_slopes;
    }
  else
    {
      if (pass_lengths != NULL)
        delete[] pass_lengths;
      if (pass_slopes != NULL)
        delete[] pass_slopes;
      pass_lengths = new int[new_passes];
      pass_slopes = new kdu_uint16[new_passes];
    }
  max_passes = new_passes;
}

/*****************************************************************************/
/*                        kdu_block::set_max_bytes                           */
/*****************************************************************************/

void
  kdu_block::set_max_bytes(int new_bytes, bool copy_existing)
{
  if (max_bytes >= new_bytes)
    return;
  if (max_bytes == 0)
    copy_existing = false;
  if (copy_existing)
    {
      kdu_byte *new_buf = (new kdu_byte[new_bytes+1])+1;
      memcpy(new_buf,byte_buffer,max_bytes);
      delete[] (byte_buffer-1);
      byte_buffer = new_buf;
    }
  else
    {
      if (byte_buffer != NULL)
        delete[] (byte_buffer-1);
      byte_buffer = (new kdu_byte[new_bytes+1])+1;
    }
  max_bytes = new_bytes;
}

/*****************************************************************************/
/*                        kdu_block::set_max_samples                         */
/*****************************************************************************/

void
  kdu_block::set_max_samples(int new_samples)
{
  if (max_samples >= new_samples)
    return;
  if (sample_buffer != NULL)
    delete[] sample_buffer;
  sample_buffer = new kdu_int32[new_samples];
  max_samples = new_samples;
}

/*****************************************************************************/
/*                       kdu_block::set_max_contexts                         */
/*****************************************************************************/

void
  kdu_block::set_max_contexts(int new_contexts)
{
  if (max_contexts >= new_contexts)
    return;
  if (context_buffer != NULL)
    delete[] context_buffer;
  context_buffer = new kdu_int32[new_contexts];
  max_contexts = new_contexts;
}


/* ========================================================================= */
/*                                  kd_block                                 */
/* ========================================================================= */

/*****************************************************************************/
/*                             kd_block::cleanup                             */
/*****************************************************************************/

void
  kd_block::cleanup(kd_precinct_band *pband)
{
  kd_codestream *codestream = pband->subband->codestream;
  kd_code_buffer *tmp;

  while ((tmp=first_buf) != NULL)
    {
      first_buf = tmp->next;
      codestream->buf_server->release(tmp);
    }
}

/*****************************************************************************/
/*                       kd_block::parse_packet_header                       */
/*****************************************************************************/

int
  kd_block::parse_packet_header(kd_header_in &head,
                                 kd_buf_server *buf_server,
                                 int layer_idx)
{
  // Parse inclusion information first.

  temp_length = 0;
  if (beta == 0)
    { // Not yet included. Tag tree decoding for `layer_w'.
      assert(layer_wbar == layer_w);
      kd_block *scan, *prev, *next;

      // Walk up to the root node in the tree.
      scan=this; prev=NULL;
      while ((next=scan->up_down) != NULL)
        { scan->up_down=prev; prev=scan; scan=next; }
      scan->up_down = prev;

      // Walk back down the tree, performing the decoding steps.
      kdu_uint16 wbar_min = 0;
      kdu_uint16 threshold = (kdu_uint16)(layer_idx+1);
      prev = NULL;
      while (scan != NULL)
        {
          if (scan->layer_wbar < wbar_min)
            { scan->layer_wbar = wbar_min; scan->layer_w = wbar_min; }
          while ((scan->layer_w == scan->layer_wbar) &&
                 (scan->layer_wbar < threshold))
            {
              scan->layer_wbar++;
              if (head.get_bit() == 0)
                scan->layer_w++;
            }
          wbar_min = scan->layer_w;
          next=scan->up_down; scan->up_down=prev; prev=scan; scan=next;
        }
      if (layer_wbar == layer_w)
        return 0; // Nothing included yet.
      if (layer_w != (kdu_uint16) layer_idx)
        throw KDU_EXCEPTION_ILLEGAL_LAYER;
    }
  else
    { // Already included.
      if (head.get_bit() == 0)
        return 0;
    }

  // If we get here, the code-block does contribute to the current layer.

  bool discard_block = (num_passes == 255);

  if (beta == 0)
    { // First time contribution.  Need to get MSB's and set up buffering.
      while (msbs_w == msbs_wbar)
        { // Run tag tree decoder.
          kd_block *scan, *prev, *next;

          // Walk up to the root note in the tree.
          scan=this; prev=NULL;
          while ((next=scan->up_down) != NULL)
            { scan->up_down=prev; prev=scan; scan=next; }
          scan->up_down = prev;

          // Walk back down the tree, performing the decoding steps.
          kdu_byte wbar_min = 0;
          kdu_byte threshold = msbs_wbar+1;
          prev = NULL;
          while (scan != NULL)
            {
              if (scan->msbs_wbar < wbar_min)
                { scan->msbs_wbar = wbar_min; scan->msbs_w = wbar_min; }
              while ((scan->msbs_w == scan->msbs_wbar) &&
                     (scan->msbs_wbar < threshold))
                {
                  scan->msbs_wbar++;
                  if (head.get_bit() == 0)
                    scan->msbs_w++;
                }
              wbar_min = scan->msbs_w;
              next=scan->up_down; scan->up_down=prev; prev=scan; scan=next;
            }
        }

      num_bytes = 0;
      beta = 3;
      assert(pass_idx == 0);
      if (!discard_block)
        start_buffering(buf_server);
    }

  if (!discard_block)
    { // Record the layer index for this new contribution.
      put_byte((kdu_byte)(layer_idx>>8),buf_server);
      put_byte((kdu_byte) layer_idx,buf_server);
    }

  // Decode number of passes.

  int new_passes = 1;
  new_passes += head.get_bit();
  if (new_passes >= 2)
    {
      new_passes += head.get_bit();
      if (new_passes >= 3)
        {
          new_passes += head.get_bits(2);
          if (new_passes >= 6)
            {
              new_passes += head.get_bits(5);
              if (new_passes >= 37)
                new_passes += head.get_bits(7);
            }
        }
    }

  // Finally, decode the length information.

  while (head.get_bit())
    {
      if (beta == 255)
        throw KDU_EXCEPTION_PRECISION;
      beta++;
    }

  bool bypass_term = ((((int) modes) & Cmodes_BYPASS) != 0);
  bool all_term = ((((int) modes) & Cmodes_RESTART) != 0);
  if (all_term)
    bypass_term = false;

  int segment_passes, segment_bytes;
  int length_bits;
  int idx = pass_idx;
  while (new_passes > 0)
    {
      if (all_term)
        segment_passes = 1;
      else if (bypass_term)
        {
          if (idx < 10)
            segment_passes = 10-idx;
          else if (((idx-10) % 3) == 0)
            segment_passes = 2;
          else
            segment_passes = 1;
          if (segment_passes > new_passes)
            segment_passes = new_passes;
        }
      else
        segment_passes = new_passes;
      for (length_bits=0; (1<<length_bits) <= segment_passes; length_bits++);
      length_bits--;
      length_bits += beta;
      segment_bytes = head.get_bits(length_bits);
      if ((segment_bytes >= (1<<15)) ||
          (segment_bytes >= ((1<<16) - (int) temp_length)))
        throw KDU_EXCEPTION_PRECISION;
      temp_length += segment_bytes;
      idx += segment_passes;
      new_passes -= segment_passes;
      if (new_passes > 0)
        segment_bytes |= (1<<15); // Continuation flag.
      if (!discard_block)
        { // Record the number of bytes and the number of passes in the segment
          put_byte((kdu_byte)(segment_bytes>>8),buf_server);
          put_byte((kdu_byte) segment_bytes,buf_server);
          put_byte((kdu_byte) segment_passes,buf_server);
        }
    }

  pass_idx = (kdu_byte) idx;
  if (!discard_block)
    num_passes = pass_idx;
  return temp_length;
}

/*****************************************************************************/
/*                         kd_block::read_body_bytes                         */
/*****************************************************************************/

void
  kd_block::read_body_bytes(kd_input *source, kd_buf_server *buf_server)
{
  int body_bytes = this->temp_length; this->temp_length = 0;
  int xfer_bytes;

  if (body_bytes == 0)
    return;

  if (num_passes == 255)
    { // Read and discard the code-bytes.
      assert(first_buf == NULL);
      kd_code_buffer *tmp_buf = buf_server->get();
      while ((body_bytes > 0) && !source->failed())
        {
          xfer_bytes = KD_CODE_BUFFER_LEN;
          xfer_bytes = (body_bytes < xfer_bytes)?body_bytes:xfer_bytes;
          source->read(tmp_buf->buf,xfer_bytes);
          body_bytes -= xfer_bytes;
        }
      buf_server->release(tmp_buf);
      return;
    }

  while (body_bytes > 0)
    {
      xfer_bytes = KD_CODE_BUFFER_LEN - buf_pos;
      if (xfer_bytes == 0)
        {
          current_buf = current_buf->next = buf_server->get();
          buf_pos = 0;
          xfer_bytes = KD_CODE_BUFFER_LEN;
        }
      xfer_bytes = (body_bytes < xfer_bytes)?body_bytes:xfer_bytes;
      xfer_bytes = source->read(current_buf->buf+buf_pos,xfer_bytes);
      if (xfer_bytes == 0)
        break;
      body_bytes -= xfer_bytes;
      buf_pos += xfer_bytes;
      this->num_bytes += xfer_bytes;
    }
}

/*****************************************************************************/
/*                          kd_block::retrieve_data                          */
/*****************************************************************************/

void
  kd_block::retrieve_data(kdu_block *block, int max_layers)
{
  assert(num_passes < 255); // 255 has the special meaning: "discard block"
  block->num_passes = 0; // We will increment this as we read them in.
  block->missing_msbs = msbs_w;
  if (beta == 0)
    return;

  if ((num_bytes+2) > block->max_bytes)
    block->set_max_bytes(((int) num_bytes)+4096,false); // A generous allocation
  if (num_passes > block->max_passes)
    block->set_max_passes(((int) num_passes)+32,false); // A generous allocation

  int remaining_bytes = num_bytes;
  kdu_byte *bp = block->byte_buffer;
  current_buf = first_buf;
  buf_pos = 0;
  for (pass_idx=0; pass_idx < num_passes; )
    {
      // Get the layer index for the next contribution.
      int layer_idx = get_byte();
      layer_idx = (layer_idx<<8) + get_byte();
      if (layer_idx >= max_layers)
        break;

      int new_passes, new_bytes;
      kdu_byte idx = pass_idx;
      bool more_headers=true;

      assert(block->num_passes == (int) pass_idx);
      while (more_headers)
        {
          new_bytes = get_byte();
          new_bytes = (new_bytes << 8) + get_byte();
          new_passes = get_byte();
          more_headers = false;
          if (new_bytes & (1<<15))
            {
              more_headers = true;
              new_bytes &= ~(1<<15);
            }
          for (; new_passes > 0; new_passes--, new_bytes=0, idx++)
            {
              block->pass_lengths[idx] = new_bytes;
              block->pass_slopes[idx] = 0;
            }
          assert(idx <= num_passes);
        }
      assert(idx > 0);
      block->pass_slopes[idx-1] = ((1<<16)-1) - layer_idx;

      for (; pass_idx < idx; pass_idx++)
        {
          new_bytes = block->pass_lengths[pass_idx];
          if (new_bytes > remaining_bytes)
            return;
          block->num_passes = pass_idx+1;
          remaining_bytes -= new_bytes;
          while (new_bytes > 0)
            {
              int xfer_bytes = KD_CODE_BUFFER_LEN - buf_pos;
              if (xfer_bytes == 0)
                {
                  current_buf = current_buf->next;
                  buf_pos = 0;
                  assert(current_buf != NULL);
                  xfer_bytes = KD_CODE_BUFFER_LEN;
                }
              xfer_bytes = (xfer_bytes < new_bytes)?xfer_bytes:new_bytes;
              new_bytes -= xfer_bytes;
              while (xfer_bytes--)
                *(bp++) = current_buf->buf[buf_pos++];
            }
        }
    }
}

/*****************************************************************************/
/*                           kd_block::store_data                            */
/*****************************************************************************/

void
  kd_block::store_data(kdu_block *block, kd_buf_server *buf_server)
{
  assert(block->modes == (int) modes);
  assert(block->missing_msbs < 255);
  assert(block->num_passes <= 255);
  assert(first_buf == NULL);

  // Now copy the coding pass summary information.
  int n, val, total_bytes;

  msbs_w = (kdu_byte) block->missing_msbs;
  start_buffering(buf_server);
  num_passes = (kdu_byte) block->num_passes;
  total_bytes = 0;
  for (n=0; n < block->num_passes; n++)
    {
      val = block->pass_slopes[n];
      assert(val >= 0);
      put_byte((kdu_byte)(val>>8),buf_server);
      put_byte((kdu_byte) val,buf_server);
      val = block->pass_lengths[n];
      assert((val >= 0) && (val < (1<<16)));
      total_bytes += val;
      put_byte((kdu_byte)(val>>8),buf_server);
      put_byte((kdu_byte) val,buf_server);
    }
  assert(total_bytes <= block->max_bytes);

  // Finally, copy the code-bytes.
  kdu_byte *bp = block->byte_buffer;
  while (total_bytes > 0)
    {
      int xfer_bytes = KD_CODE_BUFFER_LEN - buf_pos;
      if (xfer_bytes == 0)
        {
          current_buf = current_buf->next = buf_server->get();
          buf_pos = 0;
          xfer_bytes = KD_CODE_BUFFER_LEN;
        }
      xfer_bytes = (xfer_bytes < total_bytes)?xfer_bytes:total_bytes;
      total_bytes -= xfer_bytes;
      while (xfer_bytes--)
        current_buf->buf[buf_pos++] = *(bp++);
    }
  current_buf = first_buf;
  buf_pos = 0;
}

/*****************************************************************************/
/*                           kd_block::trim_data                             */
/*****************************************************************************/

bool
  kd_block::trim_data(kdu_uint16 slope_threshold, kd_buf_server *buf_server)
{
  if (num_passes == 0)
    return false;

  // Find the number of passes we can keep.
  int n;
  int potential_body_bytes = 0;
  int max_body_bytes = 0;
  int max_passes = 0;
  kd_code_buffer *save_current_buf = current_buf;
  kdu_byte save_buf_pos = buf_pos;

  current_buf = first_buf;
  buf_pos = 0;
  for (n=0; n < num_passes; n++)
    {
      kdu_uint16 slope = get_byte();
      slope = (slope<<8) + get_byte();
      if ((slope != 0) && (slope <= slope_threshold))
        break; // Need to discard this one.
      kdu_uint16 length = get_byte();
      length = (length<<8) + get_byte();
      potential_body_bytes += length;
      if (slope != 0)
        {
          max_body_bytes = potential_body_bytes;
          max_passes = n+1;
        }
    }

  // Restore the counters.
  current_buf = save_current_buf;
  buf_pos = save_buf_pos;

  // Now go back through the data setting additional pass slopes to 0.

     /* Notice that we do not change `num_passes' for that would prevent
        us from later locating the body bytes. */

  if (n == num_passes)
    return false;
  kd_code_buffer *buf_ptr = first_buf;
  int pos = max_passes << 2;
  while (pos > KD_CODE_BUFFER_LEN)
    { buf_ptr = buf_ptr->next; pos -= KD_CODE_BUFFER_LEN; }
  n = (num_passes-max_passes)<<2; // Number of bytes to set to 0
  while (n--)
    {
      if (pos == KD_CODE_BUFFER_LEN)
        { buf_ptr = buf_ptr->next; pos = 0; }
      buf_ptr->buf[pos++] = 0;
    }

  // Now remove the unwanted body bytes.

  buf_ptr = first_buf;
  pos = (num_passes<<2) + max_body_bytes;
  while (pos > KD_CODE_BUFFER_LEN)
    { buf_ptr = buf_ptr->next; pos -= KD_CODE_BUFFER_LEN; }
  kd_code_buffer *tmp;
  while ((tmp=buf_ptr->next) != NULL)
    {
      buf_ptr->next = tmp->next;
      buf_server->release(tmp);
    }
  return true;
}

/*****************************************************************************/
/*                          kd_block::start_packet                           */
/*****************************************************************************/

int
  kd_block::start_packet(int layer_idx, kdu_uint16 slope_threshold)
{
  if (layer_idx == 0)
    {
      pass_idx = 0;
      current_buf = first_buf;
      buf_pos = 0;
      layer_w = 0xFFFF;
      if (num_passes == 0)
        msbs_w = 0xFF; /* We will never include this block.  Make sure its
                          `msbs_w' value does not damage the efficiency with
                          with the neighbouring blocks are represented. */

      // Reflect `msbs_w' value into higher nodes in the tag tree.
      for (kd_block *scan=this->up_down; scan != NULL; scan=scan->up_down)
        if (scan->msbs_w > msbs_w)
          scan->msbs_w = msbs_w;
        else
          break; // No point in going further up the tree.
    }

  // Find number of new passes.

  pending_new_passes = 0;
  temp_length = 0;
  if (pass_idx == num_passes)
    {
      layer_w = 0xFFFF; // `save_beta' may have been reset by `save_output_tree
      return 0;
    }

  kd_code_buffer *save_current_buf = current_buf;
  kdu_byte save_buf_pos = buf_pos;
  int test_length = 0;
  int test_passes = 0;
  for (int n=(num_passes-pass_idx); n > 0; n--)
    {
      kdu_uint16 slope = get_byte();
      slope = (slope<<8) + get_byte();
      if ((slope != 0) && (slope <= slope_threshold))
        break;
      kdu_uint16 length = get_byte();
      length = (length<<8) + get_byte();
      test_passes++;
      test_length += length;
      if (slope != 0)
        {
          pending_new_passes = test_passes;
          assert(test_length < (1<<16));
          temp_length = (kdu_uint16) test_length;
        }
    }
  current_buf = save_current_buf;
  buf_pos = save_buf_pos;

  // See if we need to update tag tree nodes.

  if (pass_idx == 0)
    {
      if (pending_new_passes)
        {
          layer_w  = (kdu_uint16) layer_idx;
          for (kd_block *scan=up_down; scan != NULL; scan=scan->up_down)
            if (scan->layer_w > layer_w)
              scan->layer_w = layer_w;
            else
              break; // No point in going further up the tree.
        }
      else
        layer_w = 0xFFFF;
    }
  return temp_length;
}

/*****************************************************************************/
/*                       kd_block::write_packet_header                       */
/*****************************************************************************/

void
  kd_block::write_packet_header(kd_header_out &head, int layer_idx,
                                 bool simulate)
{
  // Process inclusion information first

  bool included = (pending_new_passes > 0);
  if (pass_idx == 0)
    { // First time inclusion.
      assert((included && (layer_w < 0xFFFF)) ||
             ((!included) && (layer_w == 0xFFFF)));
      kdu_byte save_new_passes=pending_new_passes; // Shares layer_wbar storage
      layer_wbar = (kdu_uint16) layer_idx;
      kd_block *scan, *prev, *next;

      // Walk up to the root node in the tree.
      scan=this; prev=NULL;
      while ((next=scan->up_down) != NULL)
        { scan->up_down=prev; prev=scan; scan=next; }
      scan->up_down = prev;

      // Walk back down the tree, performing the encoding steps.

      kdu_uint16 wbar_min = 0;
      kdu_uint16 threshold = layer_wbar+1;
      prev = NULL;
      while (scan != NULL)
        {
          if (scan->layer_wbar < wbar_min)
            scan->layer_wbar = wbar_min;
          while ((scan->layer_w >= scan->layer_wbar) &&
                 (scan->layer_wbar < threshold))
            {
              scan->layer_wbar++;
              head.put_bit((scan->layer_w >= scan->layer_wbar)?0:1);
            }
          wbar_min =
            (scan->layer_w<scan->layer_wbar)?scan->layer_w:scan->layer_wbar;
          next=scan->up_down; scan->up_down=prev; prev=scan; scan=next;
        }
      pending_new_passes = save_new_passes; // Finished using layer_wbar now
    }
  else
    { // Has been included at least once before
      head.put_bit((included)?1:0);
    }
  if (!included)
    return;

  // If we get here, the code-block does contribute to the current layer.

  if (pass_idx == 0)
    { // First time contribution.  Need to get MSB's.
      for (msbs_wbar=0; msbs_wbar <= msbs_w; )
        { // Run tag tree decoder.
          kd_block *scan, *prev, *next;

          // Walk up to the root note in the tree.
          scan=this; prev=NULL;
          while ((next=scan->up_down) != NULL)
            { scan->up_down=prev; prev=scan; scan=next; }
          scan->up_down = prev;

          // Walk back down the tree, performing the decoding steps.
          kdu_byte wbar_min = 0;
          kdu_byte threshold = msbs_wbar+1;
          prev = NULL;
          while (scan != NULL)
            {
              if (scan->msbs_wbar < wbar_min)
                scan->msbs_wbar = wbar_min;
              while ((scan->msbs_w >= scan->msbs_wbar) &&
                     (scan->msbs_wbar < threshold))
                {
                  scan->msbs_wbar++;
                  head.put_bit((scan->msbs_w >= scan->msbs_wbar)?0:1);
                }
              wbar_min =
                (scan->msbs_w<scan->msbs_wbar)?scan->msbs_w:scan->msbs_wbar;
              next=scan->up_down; scan->up_down=prev; prev=scan; scan=next;
            }
        }
      beta = 3;
    }

  // Encode number of passes.

  int val = pending_new_passes - 1;
  int delta = (val > 1)?1:val;
  val -= delta;
  head.put_bit(delta);
  if (delta)
    { // new_passes > 1
      delta = (val > 1)?1:val;
      val -= delta;
      head.put_bit(delta);
      if (delta)
        { // new_passes > 2
          delta = (val > 3)?3:val;
          val -= delta;
          head.put_bits(delta,2);
          if (delta == 3)
            {
              delta = (val > 31)?31:val;
              val -= delta;
              head.put_bits(delta,5);
              if (delta == 31)
                {
                  delta = (val > 127)?127:val;
                  val -= delta;
                  head.put_bits(delta,7);
                }
            }
        }
    }
  assert(val == 0);

  // Finally, encode the length information.

  int segment_passes, segment_bytes, total_bytes, length_bits, new_passes, idx;
  bool bypass_term = ((((int) modes) & Cmodes_BYPASS) != 0);
  bool all_term = ((((int) modes) & Cmodes_RESTART) != 0);
  if (all_term)
    bypass_term = false;

  // Save buffer status
  kd_code_buffer *save_current_buf = current_buf;
  kdu_byte save_buf_pos = buf_pos;

  // Walk through the coding passes a first time to determine beta.
  for (total_bytes=0, idx=pass_idx, new_passes=pending_new_passes;
       new_passes > 0; new_passes-=segment_passes,
       total_bytes+=segment_bytes, idx+=segment_passes)
    {
      if (all_term)
        segment_passes = 1;
      else if (bypass_term)
        {
          if (idx < 10)
            segment_passes = 10-idx;
          else if (((idx-10) % 3) == 0)
            segment_passes = 2;
          else
            segment_passes = 1;
          if (segment_passes > new_passes)
            segment_passes = new_passes;
        }
      else
        segment_passes = new_passes;
      for (length_bits=0; (1<<length_bits) <= segment_passes; length_bits++);
      length_bits--;
      length_bits += beta;
      for (segment_bytes=0, val=segment_passes; val > 0; val--)
        {
          get_byte(); get_byte();
          delta = get_byte(); delta = (delta<<8) + get_byte();
          segment_bytes += delta;
        }
      while (segment_bytes >= (1<<length_bits))
        {
          head.put_bit(1);
          length_bits++;
          beta++;
        }
    }
  assert(total_bytes == (int) temp_length);
  head.put_bit(0); // End of the `beta' signalling comma code.

  // Restore buffer status for second time through
  current_buf = save_current_buf;
  buf_pos = save_buf_pos;

  // Walk through the coding passes a second time to encode the segment lengths
  for (total_bytes=0, idx=pass_idx, new_passes=pending_new_passes;
       new_passes > 0; new_passes-=segment_passes,
       total_bytes+=segment_bytes, idx+=segment_passes)
    {
      if (all_term)
        segment_passes = 1;
      else if (bypass_term)
        {
          if (idx < 10)
            segment_passes = 10-idx;
          else if (((idx-10) % 3) == 0)
            segment_passes = 2;
          else
            segment_passes = 1;
          if (segment_passes > new_passes)
            segment_passes = new_passes;
        }
      else
        segment_passes = new_passes;
      for (length_bits=0; (1<<length_bits) <= segment_passes; length_bits++);
      length_bits--;
      length_bits += beta;
      for (segment_bytes=0, val=segment_passes; val > 0; val--)
        {
          get_byte(); get_byte();
          delta = get_byte(); delta = (delta<<8) + get_byte();
          segment_bytes += delta;
        }
      assert(segment_bytes < (1<<length_bits));
      head.put_bits(segment_bytes,length_bits);
    }
  assert(total_bytes == (int) temp_length);

  if (simulate)
    { // Restore buffer status so we can try different thresholds for this layer
      current_buf = save_current_buf;
      buf_pos = save_buf_pos;
    }
  else
    { // Commit state changes for this layer
      if (pass_idx == 0)
        body_bytes_offset = ((kdu_uint16) num_passes) << 2;
      pass_idx += pending_new_passes;
      pending_new_passes = 0;
    }
}

/*****************************************************************************/
/*                         kd_block::write_body_bytes                        */
/*****************************************************************************/

void
  kd_block::write_body_bytes(kdu_output *dest)
{
  if (temp_length == 0)
    return;

  int pos = body_bytes_offset;
  kd_code_buffer *scan = first_buf;
  while (pos >= KD_CODE_BUFFER_LEN)
    {
      pos -= KD_CODE_BUFFER_LEN;
      scan = scan->next;
      assert(scan != NULL);
    }
  int new_bytes = temp_length;
  assert((new_bytes+(int) body_bytes_offset) < (2<<16));
  body_bytes_offset += temp_length;
  temp_length = 0;
  while (new_bytes > 0)
    {
      int xfer_bytes = KD_CODE_BUFFER_LEN-pos;
      assert((xfer_bytes > 0) && (scan != NULL));
      if (xfer_bytes > new_bytes)
        xfer_bytes = new_bytes;
      new_bytes -= xfer_bytes;
      dest->write(scan->buf+pos,xfer_bytes);
      scan = scan->next;
      pos = 0;
    }
  pending_new_passes = 0;
}

/*****************************************************************************/
/* STATIC MEMBER              kd_block::build_tree                           */
/*****************************************************************************/

kd_block *
  kd_block::build_tree(kdu_coords size, int *return_num_nodes)
{
  int level_nodes, total_nodes, level_idx, num_levels;
  kdu_coords level_size;

  total_nodes = level_nodes = size.x*size.y;
  assert(total_nodes >= 0);
  level_size = size;
  num_levels = 1;
  while (level_nodes > 1)
    {
      level_size.x = (level_size.x+1)>>1;
      level_size.y = (level_size.y+1)>>1;
      level_nodes = level_size.x*level_size.y;
      total_nodes += level_nodes;
      num_levels++;
    }
  if (return_num_nodes != NULL)
    *return_num_nodes = total_nodes;
  if (total_nodes == 0)
    return NULL;

  kd_block *blocks = new kd_block[total_nodes];
  memset(blocks,0,(size_t)(sizeof(kd_block)*total_nodes));
  kd_block *node, *next;
  kdu_coords next_size;
  for (node=blocks, level_size=size, level_idx=0;
       level_idx < num_levels;
       level_idx++, level_size=next_size)
    {
      next_size.x = (level_size.x+1)>>1;
      next_size.y = (level_size.y+1)>>1;
      level_nodes = level_size.x*level_size.y;
      next = node+level_nodes;

      for (int y=0; y < level_size.y; y++)
        for (int x=0; x < level_size.x; x++, node++)
          {
            node->up_down = next + (y>>1)*next_size.x + (x>>1);
            if (level_idx == (num_levels-1))
              {
                assert((x==0) && (y==0));
                node->up_down = NULL;
              }
          }
    }
  assert(node == (blocks+total_nodes));
  return blocks;
}

/*****************************************************************************/
/* STATIC MEMBER          kd_block::reset_output_tree                        */
/*****************************************************************************/

void
  kd_block::reset_output_tree(kd_block *node, kdu_coords size)
{
  bool leaf_node = true;
  int x, y;

  if ((size.x == 0) || (size.y == 0))
    return;
  do {
      if (leaf_node)
        {
          x = size.x; y = size.y;
          node += x*y;
        }
      else
        {
          for (y=0; y < size.y; y++)
            for (x=0; x < size.x; x++, node++)
              {
                node->msbs_wbar = 0;
                node->layer_wbar = 0;
                node->msbs_w = 0xFF;
                node->layer_w = 0xFFFF;
              }
        }
      size.y = (size.y+1)>>1;
      size.x = (size.x+1)>>1;
      leaf_node = false;
    } while ((x > 1) || (y > 1));
}

/*****************************************************************************/
/* STATIC MEMBER           kd_block::save_output_tree                        */
/*****************************************************************************/

void
  kd_block::save_output_tree(kd_block *node, kdu_coords size)
{
  bool leaf_node = true;
  int x, y;

  if ((size.x == 0) || (size.y == 0))
    return;
  do {
      for (y=0; y < size.y; y++)
        for (x=0; x < size.x; x++, node++)
          {
            if (leaf_node)
              { // Commit the pending new coding passes.
                int buf_pos = node->buf_pos;
                buf_pos += ((int) node->pending_new_passes)<<2;
                while (buf_pos > KD_CODE_BUFFER_LEN)
                  {
                    node->current_buf = node->current_buf->next;
                    assert(node->current_buf != NULL);
                    buf_pos -= KD_CODE_BUFFER_LEN;
                  }
                node->buf_pos = (kdu_byte) buf_pos;
                node->pass_idx += node->pending_new_passes;
                node->pending_new_passes = 0;
                node->save_beta = node->beta;
              }
            else
              {
                node->save_layer_w = node->layer_w;
                node->save_layer_wbar = node->layer_wbar;
                node->save_msbs_wbar = node->msbs_wbar;
              }
          }
      size.y = (size.y+1)>>1;
      size.x = (size.x+1)>>1;
      leaf_node = false;
    } while ((x > 1) || (y > 1));
}

/*****************************************************************************/
/* STATIC MEMBER          kd_block::restore_output_tree                      */
/*****************************************************************************/

void
  kd_block::restore_output_tree(kd_block *node, kdu_coords size)
{
  bool leaf_node = true;
  int x, y;

  if ((size.x == 0) || (size.y == 0))
    return;
  do {
      if (leaf_node)
        {
          for (y=0; y < size.y; y++)
            for (x=0; x < size.x; x++, node++)
              node->beta = node->save_beta;
        }
      else
        { // Only restore non-leaf node state.
          for (y=0; y < size.y; y++)
            for (x=0; x < size.x; x++, node++)
              {
                node->layer_w = node->save_layer_w;
                node->layer_wbar = node->save_layer_wbar;
                node->msbs_wbar = node->save_msbs_wbar;
              }
        }
      size.y = (size.y+1)>>1;
      size.x = (size.x+1)>>1;
      leaf_node = false;
    } while ((x > 1) || (y > 1));
}
