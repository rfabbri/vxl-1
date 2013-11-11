/*****************************************************************************/
// File: decoder.cpp [scope = CORESYS/CODING]
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
   Implements the functionality offered by the "kdu_decoder" object defined
in "kdu_sample_processing.h".  Includes ROI adjustments, dequantization,
subband sample buffering and geometric appearance transformations.
******************************************************************************/

#include <string.h>
#include <assert.h>
#include "kdu_messaging.h"
#include "kdu_compressed.h"
#include "kdu_sample_processing.h"
#include "kdu_block_coding.h"
#include "kdu_kernels.h"

/* ========================================================================= */
/*                   Local Class and Structure Definitions                   */
/* ========================================================================= */

/*****************************************************************************/
/*                                kd_decoder                                 */
/*****************************************************************************/

class kd_decoder : public kdu_pull_ifc_base {
  public: // Member functions
    kd_decoder(kdu_subband band, kdu_sample_allocator *allocator,
               bool use_shorts, float normalization);
  protected: // These functions implement their namesakes in the base class
    virtual ~kd_decoder();
    virtual void pull(kdu_line_buf &line, bool allow_exchange);
  private: // Internal implementation
    void decode_row_of_blocks();
      /* Called whenever `pull' encounters an empty line buffer. */
    void adjust_roi_background(kdu_block *block);
      /* Shifts up background samples after a block has been decoded. */
  private: // Data
    kdu_block_decoder block_decoder;
    kdu_subband band;
    int K_max; // Maximum magnitude bit-planes, excluding ROI upshift
    int K_max_prime; // Maximum magnitude bit-planes, including ROI upshift.
    bool reversible;
    float delta; // For irreversible compression only.
    kdu_dims block_indices; // Range of block indices not yet read.
    int subband_rows, subband_cols;
    int nominal_block_height;
    int current_block_height;
    int next_buffered_row; // If 0, the current row of blocks needs to be read
    kdu_sample_allocator *allocator;
    bool initialized; // True once line buffers allocated in first push call
    kdu_sample16 **lines16; // NULL or array of `nominal_block_height' pointers
    kdu_sample32 **lines32; // NULL or array of `nominal_block_height' pointers
  };
  /* Notes:
     Only one of the `lines16' and `lines32' members may be non-NULL, depending
     on whether the transform will be pulling 16- or 32-bit sample values to
     us.  The relevant array is pre-created during construction and actually
     allocated from the single block of memory associated with the `allocator'
     object during the first `pull' call. */


/* ========================================================================= */
/*                                kdu_decoder                                */
/* ========================================================================= */

/*****************************************************************************/
/*                          kdu_decoder::kdu_decoder                         */
/*****************************************************************************/

kdu_decoder::kdu_decoder(kdu_subband band, kdu_sample_allocator *allocator,
                         bool use_shorts, float normalization)
  // In the future, we may create separate, optimized objects for each kernel.
{
  state = new kd_decoder(band,allocator,use_shorts,normalization);
}

/* ========================================================================= */
/*                               kd_decoder                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                         kd_decoder::kd_decoder                            */
/*****************************************************************************/

kd_decoder::kd_decoder(kdu_subband band, kdu_sample_allocator *allocator,
                       bool use_shorts, float normalization)
{
  this->band = band;
  
  K_max = band.get_K_max();
  K_max_prime = band.get_K_max_prime();
  assert(K_max_prime >= K_max);
  reversible = band.get_reversible();
  delta = band.get_delta() * normalization;

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
  initialized = false;
  lines16 = NULL;
  lines32 = NULL;
  this->allocator = NULL;
  if (!dims)
    { subband_rows = 0; return; }
  this->allocator = allocator;
  allocator->pre_alloc(use_shorts,0,subband_cols+3,nominal_block_height);
         // Note the extra 3 samples, for hassle free quad-word transfers
  if (use_shorts)
    lines16 = new kdu_sample16 *[nominal_block_height];
  else
    lines32 = new kdu_sample32 *[nominal_block_height];
}

/*****************************************************************************/
/*                          kd_decoder::~kd_decoder                          */
/*****************************************************************************/

kd_decoder::~kd_decoder()
{
  if (lines16 != NULL)
    delete[] lines16;
  else if (lines32 != NULL)
    delete[] lines32;
}

/*****************************************************************************/
/*                              kd_decoder::pull                             */
/*****************************************************************************/

void
  kd_decoder::pull(kdu_line_buf &line, bool allow_exchange)
{
  if (line.get_width() == 0)
    return;

  if (!initialized)
    { // Allocate all lines -- they will be aligned and contiguous in memory.
      int n;
      if (lines16 != NULL)
        for (n=0; n < nominal_block_height; n++)
          lines16[n] = allocator->alloc16(0,subband_cols);
      else
        for (n=0; n < nominal_block_height; n++)
          lines32[n] = allocator->alloc32(0,subband_cols);
      initialized = true;
    }

  if ((next_buffered_row == 0) || (next_buffered_row == current_block_height))
    decode_row_of_blocks();

  // Transfer data

  assert(line.get_width() == subband_cols);
  if (lines32 != NULL)
    memcpy(line.get_buf32(),lines32[next_buffered_row],
           (size_t)(subband_cols<<2));
  else
    memcpy(line.get_buf16(),lines16[next_buffered_row],
           (size_t)(subband_cols<<1));

  // Finally, update the line buffer management state.

  next_buffered_row++;
  subband_rows--;
}

/*****************************************************************************/
/*                      kd_decoder::decode_row_of_blocks                     */
/*****************************************************************************/

void
  kd_decoder::decode_row_of_blocks()
{
  if (next_buffered_row == current_block_height)
    {
      next_buffered_row = 0;
      current_block_height = nominal_block_height;
      if (current_block_height > subband_rows)
        current_block_height = subband_rows;
    }
  assert((next_buffered_row == 0) && (current_block_height > 0) &&
         (block_indices.size.y > 0));

  int offset=0;
  kdu_coords idx = block_indices.pos;
  int blocks_remaining = block_indices.size.x;
  kdu_coords xfer_size;
  kdu_block *block;

  for (; blocks_remaining > 0; blocks_remaining--,
       idx.x++, offset+=xfer_size.x)
    {
      block = band.open_block(idx);
      block_decoder.decode(block);
      xfer_size = block->region.size;
      if (block->transpose)
        xfer_size.transpose();
      assert(xfer_size.y == current_block_height);
      assert((xfer_size.x+offset) <= subband_cols);
      int m, n;
      if (block->num_passes == 0)
        { /* Fill block region with 0's.  Note the unrolled loops and the
             fact that there is no need to worry if the code zeros out as
             many as 3 samples beyond the end of each block line, since the
             buffers were allocated with a bit of extra space at the end. */
          if (lines32 != NULL)
            { // 32-bit samples
              kdu_sample32 *dp;
              for (m=0; m < xfer_size.y; m++)
                if (reversible)
                  for (dp=lines32[m]+offset, n=xfer_size.x; n>0; n-=4, dp+=4)
                    { dp[0].ival=0; dp[1].ival=0; dp[2].ival=0; dp[3].ival=0;}
                else
                  for (dp=lines32[m]+offset, n=xfer_size.x; n>0; n-=4, dp+=4)
                    { dp[0].fval = 0.0F; dp[1].fval = 0.0F;
                      dp[2].fval = 0.0F; dp[3].fval = 0.0F; }
            }
          else
            { // 16-bit samples
              kdu_sample16 *dp;
              for (m=0; m < xfer_size.y; m++)
                for (dp=lines16[m]+offset, n=xfer_size.x; n>0; n-=4, dp+=4)
                  { dp[0].ival=0; dp[1].ival=0; dp[2].ival=0; dp[3].ival=0;}
            }
        }
      else
        { // Need to dequantize and/or convert quantization indices.
          if (K_max_prime > K_max)
            adjust_roi_background(block);
          kdu_int32 *sp, *spp = block->sample_buffer;
          int row_gap = block->size.x;
          spp += block->region.pos.y * row_gap + block->region.pos.x;
          int m_start = 0, m_inc = 1, n_start=offset, n_inc = 1;
          if (block->vflip)
            { m_start += xfer_size.y-1; m_inc = -1; }
          if (block->hflip)
            { n_start += xfer_size.x-1; n_inc = -1; }
          if (lines32 != NULL)
            { // Need to generate 32-bit dequantized values.
              kdu_sample32 *dp, **dpp = lines32+m_start;        
              if (reversible)
                { // Output is 32-bit absolute integers.
                  kdu_int32 val;
                  kdu_int32 downshift = 31-K_max;
                  if (downshift < 0)
                    { kdu_error e; e << "Insufficient implementation "
                      "precision available for true reversible processing!"; }
                  if (!block->transpose)
                    for (m=xfer_size.y; m--; dpp+=m_inc, spp+=row_gap)
                      for (dp=(*dpp)+n_start, sp=spp,
                           n=xfer_size.x; n--; sp++, dp+=n_inc)
                        {
                          val = *sp;
                          if (val < 0)
                            dp->ival = -((val & KDU_INT32_MAX) >> downshift);
                          else
                            dp->ival = val >> downshift;
                        }
                  else
                    for (m=xfer_size.y; m--; dpp+=m_inc, spp++)
                      for (dp=(*dpp)+n_start, sp=spp,
                           n=xfer_size.x; n--; sp+=row_gap, dp+=n_inc)
                        {
                          val = *sp;
                          if (val < 0)
                            dp->ival = -((val & KDU_INT32_MAX) >> downshift);
                          else
                            dp->ival = val >> downshift;
                        }
                }
              else
                { // Output is true floating point values.
                  kdu_int32 val;
                  float scale = delta;
                  if (K_max <= 31)
                    scale /= (float)(1<<(31-K_max));
                  else
                    scale *= (float)(1<<(K_max-31)); // Can't decode all planes
                  if (!block->transpose)
                    for (m=xfer_size.y; m--; dpp+=m_inc, spp+=row_gap)
                      for (dp=(*dpp)+n_start, sp=spp,
                           n=xfer_size.x; n--; sp++, dp+=n_inc)
                        {
                          val = *sp;
                          if (val < 0)
                            val = -(val & KDU_INT32_MAX);
                          dp->fval = scale * val;
                        }
                  else
                    for (m=xfer_size.y; m--; dpp+=m_inc, spp++)
                      for (dp=(*dpp)+n_start, sp=spp,
                           n=xfer_size.x; n--; sp+=row_gap, dp+=n_inc)
                        {
                          val = *sp;
                          if (val < 0)
                            val = -(val & KDU_INT32_MAX);
                          dp->fval = scale * val;
                        }
                }
            }
          else
            { // Need to produce 16-bit dequantized values.
              kdu_sample16 *dp, **dpp = lines16+m_start;        
              if (reversible)
                { // Output is 16-bit absolute integers.
                  kdu_int32 val;
                  kdu_int32 downshift = 31-K_max;
                  assert(downshift >= 0); // Otherwise should be using 32 bits.
                  if (!block->transpose)
                    for (m=xfer_size.y; m--; dpp+=m_inc, spp+=row_gap)
                      for (dp=(*dpp)+n_start, sp=spp,
                           n=xfer_size.x; n--; sp++, dp+=n_inc)
                        {
                          val = *sp;
                          if (val < 0)
                            dp->ival = (kdu_int16)
                              -((val & KDU_INT32_MAX) >> downshift);
                          else
                            dp->ival = (kdu_int16)(val >> downshift);
                        }
                  else
                    for (m=xfer_size.y; m--; dpp+=m_inc, spp++)
                      for (dp=(*dpp)+n_start, sp=spp,
                           n=xfer_size.x; n--; sp+=row_gap, dp+=n_inc)
                        {
                          val = *sp;
                          if (val < 0)
                            dp->ival = (kdu_int16)
                              -((val & KDU_INT32_MAX) >> downshift);
                          else
                            dp->ival = (kdu_int16)(val >> downshift);
                        }
                }
              else
                { // Output is 16-bit fixed point values.
                  float fscale = delta * (float)(1<<KDU_FIX_POINT);
                  if (K_max <= 31)
                    fscale /= (float)(1<<(31-K_max));
                  else
                    fscale *= (float)(1<<(K_max-31));
                  fscale *= (float)(1<<16) * (float)(1<<16);
                  kdu_int32 val, scale = ((kdu_int32)(fscale+0.5F));
                  if (!block->transpose)
                    for (m=xfer_size.y; m--; dpp+=m_inc, spp+=row_gap)
                      for (dp=(*dpp)+n_start, sp=spp,
                           n=xfer_size.x; n--; sp++, dp+=n_inc)
                        {
                          val = *sp;
                          if (val < 0)
                            val = -(val & KDU_INT32_MAX);
                          val = (val+(1<<15))>>16; val *= scale;
                          dp->ival = (kdu_int16)((val+(1<<15))>>16);
                        }
                  else
                    for (m=xfer_size.y; m--; dpp+=m_inc, spp++)
                      for (dp=(*dpp)+n_start, sp=spp,
                           n=xfer_size.x; n--; sp+=row_gap, dp+=n_inc)
                        {
                          val = *sp;
                          if (val < 0)
                            val = -(val & KDU_INT32_MAX);
                          val = (val+(1<<15))>>16; val *= scale;
                          dp->ival = (kdu_int16)((val+(1<<15))>>16);
                        }
                }
            }
        }
      band.close_block(block);
    }

  block_indices.pos.y++;
  block_indices.size.y--;
}

/*****************************************************************************/
/*                     kd_decoder::adjust_roi_background                     */
/*****************************************************************************/

void
  kd_decoder::adjust_roi_background(kdu_block *block)
{
  kdu_int32 upshift = K_max_prime - K_max;
  kdu_int32 mask = (-1)<<(31-K_max); mask &= KDU_INT32_MAX;
  kdu_int32 *sp = block->sample_buffer;
  kdu_int32 val;
  int num_samples = ((block->size.y+3)>>2) * (block->size.x<<2);

  for (int n=num_samples; n--; sp++)
    if ((((val=*sp) & mask) == 0) && (val != 0))
      {
        if (val < 0)
          *sp = (val << upshift) | KDU_INT32_MIN;
        else
          *sp <<= upshift;
      }
}
