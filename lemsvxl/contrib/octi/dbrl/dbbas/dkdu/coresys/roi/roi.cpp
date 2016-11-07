/*****************************************************************************/
// File: roi.cpp [scope = CORESYS/ROI]
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
   Implements the core ROI processing functionality.  This includes the
capability to convert ROI masks at one resolution level into masks for
each of its subbands, in an efficient incremental manner.  It does not
include any methods for supplying original ROI mask geometries, since this
is application dependent.  See the "kdu_compress" example application for
one simple and one very powerful method for specifying ROI geometries.
******************************************************************************/
#include <assert.h>
#include <string.h>
#include "kdu_kernels.h"
#include "roi_local.h"


/* ========================================================================= */
/*                             kdu_roi_level_node                            */
/* ========================================================================= */

/*****************************************************************************/
/*                    kd_roi_level_node::~kd_roi_level_node                  */
/*****************************************************************************/

kd_roi_level_node::~kd_roi_level_node()
{
  assert(!active); // Insist on acquired objects being released first.
  if (row_buffers == NULL)
    return;
  for (int n=0; n < num_row_buffers; n++)
    delete[] row_buffers[n];
  delete[] row_buffers;
}

/*****************************************************************************/
/*                           kd_roi_level_node::pull                         */
/*****************************************************************************/

void
  kd_roi_level_node::pull(kdu_byte buf[], int width)
{
  assert(active && (width == cols) && (remaining_rows > 0));
  while (num_valid_row_buffers == 0)
    owner->advance();
  memcpy(buf,row_buffers[first_valid_row_buffer],(size_t) width);
  num_valid_row_buffers--;
  first_valid_row_buffer++;
  if (first_valid_row_buffer == num_row_buffers)
    first_valid_row_buffer = 0;
  remaining_rows--;
}

/*****************************************************************************/
/*                         kd_roi_level_node::advance                        */
/*****************************************************************************/

kdu_byte *
  kd_roi_level_node::advance()
{
  if (!active)
    {
      available = false; // If not already acquired, client missed its chance.
      return NULL;
    }
  assert(remaining_rows > num_valid_row_buffers);
  if (num_valid_row_buffers == num_row_buffers)
    { // Need to augment buffer.
      int n, r1, r2, num_new_buffers = num_row_buffers + 2;
      kdu_byte **new_buffers = new kdu_byte *[num_new_buffers];
      memset(new_buffers,0,sizeof(kdu_byte *)*(size_t)num_new_buffers);
      r1 = r2 = first_valid_row_buffer;
      for (n=0; n < num_row_buffers; n++, r1++, r2++)
        {
          if (r1 == num_row_buffers)
            r1 = 0;
          if (r2 == num_new_buffers)
            r2 = 0;
          new_buffers[r2] = row_buffers[r1];
        }
      if (row_buffers == NULL)
        delete[] row_buffers;
      row_buffers = new_buffers;
      num_row_buffers = num_new_buffers;
      for (; n < num_new_buffers; n++, r2++)
        {
          if (r2 == num_new_buffers)
            r2 = 0;
          new_buffers[r2] = new kdu_byte[cols];
        }
    }
  int r = num_valid_row_buffers + first_valid_row_buffer;
  if (r >= num_row_buffers)
    r -= num_row_buffers;
  num_valid_row_buffers++;
  return row_buffers[r];
}

/*****************************************************************************/
/*                         kd_roi_level_node::release                        */
/*****************************************************************************/

void
  kd_roi_level_node::release()
{
  available = active = false;
  owner->notify_release(this);
}


/* ========================================================================= */
/*                               kdu_roi_level                               */
/* ========================================================================= */

/*****************************************************************************/
/*                            kdu_roi_level::create                          */
/*****************************************************************************/

void
  kdu_roi_level::create(kdu_resolution res, kdu_roi_node *source)
{
  state = new kd_roi_level();
  try {
    state->init(res,source);
    }
  catch(...) {
      delete state;
      state = NULL;
      throw; // Rethrow the exception to be handled higher up the stack frame.
    }
}

/*****************************************************************************/
/*                            kdu_roi_level::destroy                         */
/*****************************************************************************/

void
  kdu_roi_level::destroy()
{
  if (state != NULL)
    delete state;
  state = NULL;
}

/*****************************************************************************/
/*                         kdu_roi_level::acquire_node                       */
/*****************************************************************************/

kdu_roi_node *
  kdu_roi_level::acquire_node(int band_idx)
{
  assert((band_idx >= 0) && (band_idx < 4));
  state->nodes[band_idx]->acquire();
  return state->nodes[band_idx];
}


/* ========================================================================= */
/*                                kd_roi_level                               */
/* ========================================================================= */

/*****************************************************************************/
/*                         kd_roi_level::~kd_roi_level                       */
/*****************************************************************************/

kd_roi_level::~kd_roi_level()
{
  for (int b=0; b < 4; b++)
    if (nodes[b] != NULL)
      delete nodes[b];
  if (row_buffers != NULL)
    {
      for (int n=0; n < num_row_buffers; n++)
        if (row_buffers[n] != NULL)
          delete[] row_buffers[n];
      delete[] row_buffers;
    }
  if (out_buf != NULL)
    delete[] out_buf;
  if (source != NULL)
    source->release();
}

/*****************************************************************************/
/*                              kd_roi_level::init                           */
/*****************************************************************************/

void
  kd_roi_level::init(kdu_resolution res, kdu_roi_node *source)
{
  this->source = source;
  res.get_dims(dims);
  for (int b=0; b < 4; b++)
    {
      kdu_dims node_dims;
      if (b == LL_BAND)
        res.access_next().get_dims(node_dims);
      else
        res.access_subband(b).get_dims(node_dims);
      nodes[b] = new kd_roi_level_node(this,node_dims.size);
      node_released[b] = false;
    }
  num_nodes_released = 0;

  // Determine the synthesis kernel extents.
    { 
      kdu_kernels kernels(res.get_kernel_id(),res.get_reversible());
      kernels.get_impulse_response(KDU_SYNTHESIS_LOW,extent[0]);
      kernels.get_impulse_response(KDU_SYNTHESIS_HIGH,extent[1]);
    }

  // Create the buffer.
  num_row_buffers = 1+2*((extent[0] > extent[1])?extent[0]:extent[1]);
  row_buffers = new kdu_byte *[num_row_buffers];
  int n;
  for (n=0; n < num_row_buffers; n++)
    row_buffers[n] = NULL; // In case memory allocation fails later.
  for (n=0; n < num_row_buffers; n++)
    row_buffers[n] = new kdu_byte[dims.size.x];
  out_buf = new kdu_byte[dims.size.x];

  // Initialize state variables
  first_buffer_idx = 0;
  num_valid_rows = 0;
  first_valid_row_loc = dims.pos.y;
  next_row_loc = dims.pos.y;
}

/*****************************************************************************/
/*                            kd_roi_level::advance                          */
/*****************************************************************************/

void
  kd_roi_level::advance()
  /* Note: the processing performed here maps a region of interest into
     each of the four subbands produced by a single stage of 2D DWT
     processing.  For more information on the algorithmic details, the
     reader is referred to Section 10.6.4 in the book by Taubman
     and Marcellin. */
{
  assert(source != NULL);
  kdu_coords lim = dims.pos + dims.size;
  assert(next_row_loc < lim.y);
  int r, min_row, max_row;
  min_row = next_row_loc - extent[next_row_loc & 1];
  max_row = next_row_loc + extent[next_row_loc & 1];
  if (min_row < dims.pos.y)
    min_row = dims.pos.y;
  if (max_row >= lim.y)
    max_row = lim.y-1;
  while (max_row >= (first_valid_row_loc+num_valid_rows))
    { // Load a new row.
      r = first_buffer_idx+num_valid_rows;
      if (r >= num_row_buffers)
        r -= num_row_buffers;
      source->pull(row_buffers[r],dims.size.x);
      if (num_valid_rows == num_row_buffers)
        {
          first_buffer_idx++; first_valid_row_loc++;
          if (first_buffer_idx == num_row_buffers)
            first_buffer_idx = 0;
        }
      else
        num_valid_rows++;
    }

  // Now for the vertical processing
  int c;
  kdu_byte *sp, *dp;

  r = min_row - first_valid_row_loc + first_buffer_idx;
  assert(r >= first_buffer_idx);
  if (r >= num_row_buffers)
    r -= num_row_buffers;
  memcpy(out_buf,row_buffers[r],(size_t)(dims.size.x));
  for (min_row++, r++; min_row <= max_row; min_row++, r++)
    {
      if (r == num_row_buffers)
        r = 0;
      for (sp=row_buffers[r], dp=out_buf, c=dims.size.x; c > 0; c--)
        *(dp++) |= *(sp++);
    }

  // Finally, time for the horizontal processing

  for (int band=0; band < 2; band++)
    {
      int node_idx = band+2*(next_row_loc&1);
      if (node_released[node_idx])
        continue;
      kd_roi_level_node *node = nodes[node_idx];
      dp = node->advance();
      if (dp == NULL)
        continue; // Node released but `notify_release' not called; never mind.
      int ext = extent[band];
      int left = (dims.pos.x + band) & 1; // Samples to left of current loc
      int right = dims.size.x-left-1; // Samples to right of current loc
      int width = 1 + (right>>1);
      sp = out_buf+left;
      kdu_byte val;
      for (; (width > 0) && (left < ext);
           width--, left+=2, right-=2, sp+=2, dp++)
        { // Initial samples treated specially to deal with boundaries.
          for (val=0, c=-left; (c <= ext) && (c <= right); c++)
            val |= sp[c];
          *dp = val;
        }
      for (; (width > 0) && (right >= ext);
           width--, left+=2, right-=2, sp+=2, dp++)
        {
          for (val=0, c=-ext; c <= ext; c++)
            val |= sp[c];
          *dp = val;
        }
      for (; width > 0; width--, left+=2, right-=2, sp+=2, dp++)
        {
          for (val=0, c=-ext; c <= right; c++)
            val |= sp[c];
          *dp = val;
        }
    }

  next_row_loc++;
  if (num_nodes_released == 4)
    {
      source->release();
      source = NULL;
    }
}

/*****************************************************************************/
/*                        kd_roi_level::notify_release                       */
/*****************************************************************************/

void
  kd_roi_level::notify_release(kd_roi_level_node *caller)
{
  int n;

  for (n=0; n < 4; n++)
    if (nodes[n] == caller)
      break;
  assert((n < 4) && !node_released[n]);
  node_released[n] = true;
  num_nodes_released++;
  if (num_nodes_released == 4)
    {
      source->release();
      source = NULL;
    }
}
