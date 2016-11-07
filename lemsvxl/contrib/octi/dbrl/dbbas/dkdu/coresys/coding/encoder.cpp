/*****************************************************************************/
// File: encoder.cpp [scope = CORESYS/CODING]
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
   Implements the functionality offered by the "kdu_encoder" object defined
in "kdu_sample_processing.h".  Includes quantization, subband sample buffering
and geometric appearance transformations.
******************************************************************************/

#include <math.h>
#include <string.h>
#include <assert.h>
#include "kdu_messaging.h"
#include "kdu_compressed.h"
#include "kdu_sample_processing.h"
#include "kdu_block_coding.h"
#include "kdu_kernels.h"
#include "kdu_roi_processing.h"

/* ========================================================================= */
/*                      Class and Structure Definitions                      */
/* ========================================================================= */

/*****************************************************************************/
/*                                kd_encoder                                 */
/*****************************************************************************/

class kd_encoder : public kdu_push_ifc_base {
  public: // Member functions
    kd_encoder(kdu_subband band, kdu_sample_allocator *allocator,
               bool use_shorts, float normalization, kdu_roi_node *roi);
  protected: // These functions implement their namesakes in the base class
    virtual ~kd_encoder();
    virtual void push(kdu_line_buf &line, bool allow_exchange);
  private: // Internal implementation
    void encode_row_of_blocks();
      /* Called whenever `push' fills the line buffer. */
  private: // Data
    kdu_block_encoder block_encoder;
    kdu_subband band;
    int K_max; // Maximum magnitude bit-planes, exclucing ROI shift
    int K_max_prime; // Maximum magnitude bit-planes, including ROI shift
    bool reversible;
    float delta; // For irreversible compression only.
    float msb_wmse; // Normalized weighted MSE associated with first mag bit
    float roi_weight; // Multiply `msb_wmse' by this for ROI foreground blocks
    kdu_dims block_indices; // Range of block indices not yet written.
    int subband_rows, subband_cols;
    int nominal_block_height;
    int current_block_height;
    int next_buffered_row; // When reaches `current_block_height' encode blocks
    kdu_sample_allocator *allocator;
    kdu_roi_node *roi_node;
    bool initialized; // True once line buffers allocated in first `push' call
    kdu_sample16 **lines16; // NULL or array of `nominal_block_height' pointers
    kdu_sample32 **lines32; // NULL or array of `nominal_block_height' pointers
    kdu_byte **roi_lines; // NULL or array of `nominal_block_height' pointers
  };
  /* Notes:
     Only one of the `lines16' and `lines32' members may be non-NULL, depending
     on whether the transform will be pushing 16- or 32-bit sample values to
     us.  The relevant array is pre-created during construction and actually
     allocated from the single block of memory associated with the `allocator'
     object during the first `push' call.  Similar considerations apply to
     the `roi_lines' array, which is used only if `roi_source' is non-NULL. */


/* ========================================================================= */
/*                            Internal Functions                             */
/* ========================================================================= */

/*****************************************************************************/
/* STATIC                      find_missing_msbs                             */
/*****************************************************************************/

static int
  find_missing_msbs(kdu_int32 *sp, int num_rows, int num_cols)
{
  kdu_int32 or_val = 0;
  int n;
  for (n=num_rows*num_cols; n > 3; n -= 4)
    { or_val |= *(sp++); or_val |= *(sp++);
      or_val |= *(sp++); or_val |= *(sp++); }
  for (; n > 0; n--)
    or_val |= *(sp++); 
  or_val &= KDU_INT32_MAX;
  if (or_val == 0)
    return 31;
  
  int missing_msbs = 0;
  for (or_val<<=1; or_val >= 0; or_val<<=1)
    missing_msbs++;

  return missing_msbs;
}


/* ========================================================================= */
/*                                kdu_encoder                                */
/* ========================================================================= */

/*****************************************************************************/
/*                          kdu_encoder::kdu_encoder                         */
/*****************************************************************************/

kdu_encoder::kdu_encoder(kdu_subband band, kdu_sample_allocator *allocator,
                         bool use_shorts, float normalization,
                         kdu_roi_node *roi)
  // In the future, we may create separate, optimized objects for each kernel.
{
  state = new kd_encoder(band,allocator,use_shorts,normalization,roi);
}

/* ========================================================================= */
/*                               kd_encoder                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                         kd_encoder::kd_encoder                            */
/*****************************************************************************/

kd_encoder::kd_encoder(kdu_subband band, kdu_sample_allocator *allocator,
                       bool use_shorts, float normalization,
                       kdu_roi_node *roi)
{
  this->band = band;
  
  K_max = band.get_K_max();
  K_max_prime = band.get_K_max_prime();
  reversible = band.get_reversible();
  delta = band.get_delta() * normalization;
  msb_wmse = band.get_msb_wmse();
  roi_weight = 1.0F;
  bool have_roi_weight = band.get_roi_weight(roi_weight);

  kdu_dims dims;
  band.get_dims(dims);
  kdu_coords nominal_block_size, first_block_size;
  band.get_block_size(nominal_block_size,first_block_size);
  band.get_valid_blocks(block_indices);

  subband_cols = dims.size.x;
  subband_rows = dims.size.y;
  nominal_block_height = nominal_block_size.y;
  current_block_height = first_block_size.y;
  next_buffered_row = 0;

  roi_node = roi;
  initialized = false;
  lines16 = NULL;
  lines32 = NULL;
  roi_lines = NULL;
  this->allocator = NULL;
  if (!dims)
    { subband_rows = 0; return; }
  this->allocator = allocator;
  allocator->pre_alloc(use_shorts,0,subband_cols,nominal_block_height);
  if (use_shorts)
    lines16 = new kdu_sample16 *[nominal_block_height];
  else
    lines32 = new kdu_sample32 *[nominal_block_height];
  if (roi_node != NULL)
    {
      if ((K_max_prime == K_max) && !have_roi_weight)
        {
          roi_node->release();
          roi_node = NULL;
          return;
        }
      allocator->pre_alloc(true,0,(subband_cols+1)>>1,nominal_block_height);
      roi_lines = new kdu_byte *[nominal_block_height];
    }
}

/*****************************************************************************/
/*                        kd_encoder::~kd_encoder                            */
/*****************************************************************************/

kd_encoder::~kd_encoder()
{
  if (lines16 != NULL)
    delete[] lines16;
  else if (lines32 != NULL)
    delete[] lines32;
  if (roi_lines != NULL)
    delete[] roi_lines;
  if (roi_node != NULL)
    roi_node->release();
  if (subband_rows != 0)
    { kdu_error e; e << "Terminating before block coding complete.  Expected "
      << subband_rows << " additional subband rows from the DWT analysis "
      "engine!"; }
}

/*****************************************************************************/
/*                             kd_encoder::push                              */
/*****************************************************************************/

void
  kd_encoder::push(kdu_line_buf &line, bool allow_exchange)
{
  if (line.get_width() == 0)
    return;
  assert(subband_rows > 0);
  assert(next_buffered_row < current_block_height);

  if (!initialized)
    { // Allocate all lines -- they will be aligned and contiguous in memory.
      int n;
      if (lines16 != NULL)
        for (n=0; n < nominal_block_height; n++)
          lines16[n] = allocator->alloc16(0,subband_cols);
      else
        for (n=0; n < nominal_block_height; n++)
          lines32[n] = allocator->alloc32(0,subband_cols);
      if (roi_lines != NULL)
        for (n=0; n < nominal_block_height; n++)
          roi_lines[n] = (kdu_byte *)
            allocator->alloc16(0,(subband_cols+1)>>1);
      initialized = true;
    }

  // Transfer data

  assert(line.get_width() == subband_cols);
  if (lines32 != NULL)
    memcpy(lines32[next_buffered_row],line.get_buf32(),
           (size_t)(subband_cols<<2));
  else
    memcpy(lines16[next_buffered_row],line.get_buf16(),
           (size_t)(subband_cols<<1));
  if (roi_node != NULL)
    roi_node->pull(roi_lines[next_buffered_row],subband_cols);

  // Update the buffer state, flushing if necessary.

  subband_rows--;
  next_buffered_row++;
  if (next_buffered_row == current_block_height)
    encode_row_of_blocks();
}

/*****************************************************************************/
/*                     kd_encoder::encode_row_of_blocks                      */
/*****************************************************************************/

void
  kd_encoder::encode_row_of_blocks()
{
  assert(next_buffered_row == current_block_height);
  assert((current_block_height > 0) && (block_indices.size.y > 0));

  int offset=0;
  kdu_coords idx = block_indices.pos;
  int blocks_remaining = block_indices.size.x;
  kdu_coords xfer_size;
  kdu_block *block;
  kdu_uint16 estimated_slope_threshold =
    band.get_conservative_slope_threshold();

  for (; blocks_remaining > 0; blocks_remaining--,
       idx.x++, offset+=xfer_size.x)
    {
      // Open the block and make sure we have enough sample buffer storage.
      block = band.open_block(idx);
      int num_stripes = (block->size.y+3) >> 2;
      int num_samples = (num_stripes<<2) * block->size.x;
      assert(num_samples > 0);
      if (block->max_samples < num_samples)
        block->set_max_samples((num_samples>4096)?num_samples:4096);
      
      /* Now quantize and transfer samples to the block, observing any
         required geometric transformations. */
      xfer_size = block->size;
      assert((xfer_size.x == block->region.size.x) &&
             (xfer_size.y == block->region.size.y) &&
             (0 == block->region.pos.x) && (0 == block->region.pos.y));
      if (block->transpose)
        xfer_size.transpose();
      assert(xfer_size.y == current_block_height);
      assert((xfer_size.x+offset) <= subband_cols);

      int m, n;
      kdu_int32 *dp, *dpp = block->sample_buffer;
      int row_gap = block->size.x;
      int m_start = 0, m_inc = 1, n_start=offset, n_inc = 1;
      if (block->vflip)
        { m_start += xfer_size.y-1; m_inc = -1; }
      if (block->hflip)
        { n_start += xfer_size.x-1; n_inc = -1; }

      // First transfer the sample data
      if (lines32 != NULL)
        { // Working with 32-bit data types.
          kdu_sample32 *sp, **spp = lines32+m_start;
          if (reversible)
            { // Source data is 32-bit absolute integers.
              kdu_int32 val;
              kdu_int32 upshift = 31-K_max;
              if (upshift < 0)
                { kdu_error e; e << "Insufficient implementation "
                  "precision available for true reversible compression!"; }
              if (!block->transpose)
                for (m=xfer_size.y; m--; spp+=m_inc, dpp+=row_gap)
                  for (sp=(*spp)+n_start, dp=dpp,
                       n=xfer_size.x; n--; dp++, sp+=n_inc)
                    {
                      val = sp->ival;
                      if (val < 0)
                        *dp = ((-val)<<upshift) | KDU_INT32_MIN;
                      else
                        *dp = val<<upshift;
                    }
              else
                for (m=xfer_size.y; m--; spp+=m_inc, dpp++)
                  for (sp=(*spp)+n_start, dp=dpp,
                       n=xfer_size.x; n--; dp+=row_gap, sp+=n_inc)
                    {
                      val = sp->ival;
                      if (val < 0)
                        *dp = ((-val)<<upshift) | KDU_INT32_MIN;
                      else
                        *dp = val<<upshift;
                    }
            }
          else
            { // Source data is true floating point values.
              float val;
              float scale = (1.0F / delta);
              if (K_max <= 31)
                scale *= (float)(1<<(31-K_max));
              else
                scale /= (float)(1<<(K_max-31)); // Can't decode all planes
              if (!block->transpose)
                for (m=xfer_size.y; m--; spp+=m_inc, dpp+=row_gap)
                  for (sp=(*spp)+n_start, dp=dpp,
                       n=xfer_size.x; n--; dp++, sp+=n_inc)
                    {
                      val = scale * sp->fval;
                      if (val < 0.0F)
                        *dp =  ((kdu_int32)(-val)) | KDU_INT32_MIN;
                      else
                        *dp = static_cast<kdu_int32>(val);
                    }
              else
                for (m=xfer_size.y; m--; spp+=m_inc, dpp++)
                  for (sp=(*spp)+n_start, dp=dpp,
                       n=xfer_size.x; n--; dp+=row_gap, sp+=n_inc)
                    {
                      val = scale * sp->fval;
                      if (val < 0.0F)
                        *dp = ((kdu_int32)(-val)) | KDU_INT32_MIN;
                      else
                        *dp = (kdu_int32) val;
                    }
            }
        }
      else
        { // Working with 16-bit source data.
          kdu_sample16 *sp, **spp=lines16+m_start;
          if (reversible)
            { // Source data is 16-bit absolute integers.
              kdu_int32 val;
              kdu_int32 upshift = 31-K_max;
              assert(upshift >= 0); // Otherwise should not have chosen 16 bits
              if (!block->transpose)
                for (m=xfer_size.y; m--; spp+=m_inc, dpp+=row_gap)
                  for (sp=(*spp)+n_start, dp=dpp,
                       n=xfer_size.x; n--; dp++, sp+=n_inc)
                    {
                      val = sp->ival;
                      if (val < 0)
                        *dp = ((-val)<<upshift) | KDU_INT32_MIN;
                      else
                        *dp = val<<upshift;
                    }
              else
                for (m=xfer_size.y; m--; spp+=m_inc, dpp++)
                  for (sp=(*spp)+n_start, dp=dpp,
                       n=xfer_size.x; n--; dp+=row_gap, sp+=n_inc)
                    {
                      val = sp->ival;
                      if (val < 0)
                        *dp = ((-val)<<upshift) | KDU_INT32_MIN;
                      else
                        *dp = val<<upshift;
                    }
            }
          else
            { // Source data is 16-bit fixed point integers.
              float fscale = 1.0F / (delta * (float)(1<<KDU_FIX_POINT));
              if (K_max <= 31)
                fscale *= (float)(1<<(31-K_max));
              else
                fscale /= (float)(1<<(K_max-31));
              kdu_int32 val, scale = (kdu_int32)(fscale+0.5F);
              if (!block->transpose)
                for (m=xfer_size.y; m--; spp+=m_inc, dpp+=row_gap)
                  for (sp=(*spp)+n_start, dp=dpp,
                       n=xfer_size.x; n--; dp++, sp+=n_inc)
                    {
                      val = sp->ival; val *= scale;
                      if (val < 0.0F)
                        *dp = ((kdu_int32)(-val)) | KDU_INT32_MIN;
                      else
                        *dp = (kdu_int32) val;
                    }
              else
                for (m=xfer_size.y; m--; spp+=m_inc, dpp++)
                  for (sp=(*spp)+n_start, dp=dpp,
                       n=xfer_size.x; n--; dp+=row_gap, sp+=n_inc)
                    {
                      val = sp->ival; val *= scale;
                      if (val < 0.0F)
                        *dp = ((kdu_int32)(-val)) | KDU_INT32_MIN;
                      else
                        *dp = (kdu_int32) val;
                    }
            }
        }

      // Now check to see if any ROI up-shift has been specified.  If so,
      // we need to zero out sufficient LSB's to ensure that the foreground
      // and background regions do not get confused.
      if (K_max_prime > K_max)
        {
          dpp = block->sample_buffer;
          kdu_int32 mask = ((kdu_int32)(-1)) << (31-K_max);
          if ((K_max_prime - K_max) < K_max)
            { kdu_error e; e << "You have selected too small a value for "
              "the ROI up-shift parameter.  The up-shift should be "
              "at least as large as the largest number of magnitude "
              "bit-planes in any subband; otherwise, the foreground and "
              "background regions might not be properly distinguished by "
              "the decompressor."; }
          if (!block->transpose)
            for (m=xfer_size.y; m--; dpp+=row_gap)
              for (dp=dpp, n=xfer_size.x; n--; dp++)
                *dp &= mask;
          else
            for (m=xfer_size.x; m--; dpp+=row_gap)
              for (dp=dpp, n=xfer_size.y; n--; dp++)
                *dp &= mask;
        }

      // Now transfer any ROI information which may be available.
      bool have_background = false; // If no background, code less bit-planes.
      bool have_foreground = false; // If no foreground, no extra MSE weighting
      if ((roi_lines != NULL) && (K_max_prime != K_max))
        {
          dpp = block->sample_buffer;
          kdu_byte *sp, **spp=roi_lines+m_start;
          kdu_int32 val;
          kdu_int32 downshift = K_max_prime - K_max;
          assert(downshift >= K_max);
          if (!block->transpose)
            {
              for (m=xfer_size.y; m--; spp+=m_inc, dpp+=row_gap)
                for (sp=(*spp)+n_start, dp=dpp,
                     n=xfer_size.x; n--; dp++, sp+=n_inc)
                  if (*sp == 0)
                    { // Adjust background samples down.
                      have_background = true;
                      val = *dp;
                      *dp = (val & KDU_INT32_MIN)
                          | ((val & KDU_INT32_MAX) >> downshift);
                    }
                  else
                    have_foreground = true;
            }
          else
            {
              for (m=xfer_size.y; m--; spp+=m_inc, dpp++)
                for (sp=(*spp)+n_start, dp=dpp,
                     n=xfer_size.x; n--; dp+=row_gap, sp+=n_inc)
                  if (*sp == 0)
                    { // Adjust background samples down.
                      have_background = true;
                      val = *dp;
                      *dp = (val & KDU_INT32_MIN)
                          | ((val & KDU_INT32_MAX) >> downshift);
                    }
                  else
                    have_foreground = true;
            }
        }
      else if (roi_lines != NULL)
        {
          kdu_byte *sp, **spp=roi_lines+m_start;
          for (m=xfer_size.y; m--; spp+=m_inc)
            for (sp=(*spp)+n_start, n=xfer_size.x; n--; sp+=n_inc)
              if (*sp != 0)
                { have_foreground = true; m=0; break; }
        }
      else
        have_foreground = true;

      // Finally, we can encode the block.
      int K = (have_background)?K_max_prime:K_max;
      K = (K>31)?31:K;
      block->missing_msbs =
        find_missing_msbs(block->sample_buffer,block->size.y,block->size.x);
      if (block->missing_msbs >= K)
        {
          block->missing_msbs = K;
          block->num_passes = 0;
#ifdef _DEBUG
		  vcl_cout<<"num passes are 0 in "<<this->band.which()
			  <<" with size "<<this->subband_cols<<vcl_endl;
#endif
        }
      else
        {
          K -= block->missing_msbs;
          block->num_passes = 3*K-2;
        }
      double block_msb_wmse = (have_foreground)?(msb_wmse*roi_weight):msb_wmse;
      block_encoder.encode(block,reversible,block_msb_wmse,
                           estimated_slope_threshold);
      band.close_block(block);
    }

  // Update the buffer state to reflect what we have just done.

  block_indices.pos.y++;
  block_indices.size.y--;

  next_buffered_row = 0;
  current_block_height = nominal_block_height;
  if (current_block_height > subband_rows)
    current_block_height = subband_rows;
}
