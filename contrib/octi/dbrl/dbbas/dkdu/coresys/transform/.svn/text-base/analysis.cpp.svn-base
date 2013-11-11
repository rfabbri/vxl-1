/*****************************************************************************/
// File: analysis.cpp [scope = CORESYS/TRANSFORMS]
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
   Implements the forward DWT (subband/wavelet analysis).  The implementation
uses lifting to reduce memory and processing, while keeping as much of the
implementation as possible common to both the reversible and the irreversible
processing paths.  The implementation is generic to the extent that it
supports any odd-length symmetric wavelet kernels -- although only 3 are
currently accepted by the "kdu_kernels" object.
******************************************************************************/

#include <assert.h>
#include <string.h>
#include <math.h>
#include "kdu_messaging.h"
#include "kdu_compressed.h"
#include "kdu_sample_processing.h"
#include "kdu_kernels.h"
#include "analysis_local.h"

/* ========================================================================= */
/*                               kdu_analysis                                */
/* ========================================================================= */

/*****************************************************************************/
/*                         kdu_analysis::kdu_analysis                        */
/*****************************************************************************/

kdu_analysis::kdu_analysis(kdu_resolution resolution,
                           kdu_sample_allocator *allocator,
                           bool use_shorts, float normalization,
                           kdu_roi_node *roi)
  // In the future, we may create separate, optimized objects for each kernel.
{
	state = new kd_analysis(resolution,allocator,use_shorts,normalization,roi);
}

/* ========================================================================= */
/*                              kd_analysis                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                        kd_analysis::kd_analysis                           */
/*****************************************************************************/

kd_analysis::kd_analysis(kdu_resolution resolution,
                         kdu_sample_allocator *allocator,
                         bool use_shorts, float normalization,
                         kdu_roi_node *roi)
{
  reversible = resolution.get_reversible();
  this->use_shorts = use_shorts;
  int kernel_id = resolution.get_kernel_id();
  kdu_kernels kernels(kernel_id,reversible);
  float low_gain, high_gain;
  float *factors =
    kernels.get_lifting_factors(L_max,low_gain,high_gain);
  int n;

  if ((roi != NULL) && !resolution.propagate_roi())
    {
      roi->release();
      roi = NULL;
    }
  if (roi != NULL)
    roi_level.create(resolution,roi);

  assert(L_max <= 4); // We have statically sized the array to improve locality
  for (n=0; n < L_max; n++)
    {
      steps[n].augend_parity = (n+1) & 1; // Step 0 updates odd locations
      steps[n].lambda = factors[n];
      if (kernels.get_lifting_downshift(n,steps[n].downshift))
        { // Reversible case
          steps[n].i_lambda = (kdu_int32)
            floor(0.5 + steps[n].lambda*(1<<steps[n].downshift));
        }
      else
        { // Irreversible case
          steps[n].i_lambda = steps[n].downshift = 0;
          kdu_int32 fix_lambda = (kdu_int32) floor(0.5 + factors[n]*(1<<16));
          steps[n].fixpoint.fix_lambda = fix_lambda;
          steps[n].fixpoint.i_lambda = 0;
          while (fix_lambda >= (1<<15))
            { steps[n].fixpoint.i_lambda++; fix_lambda -= (1<<16); }
          while (fix_lambda < -(1<<15))
            { steps[n].fixpoint.i_lambda--; fix_lambda += (1<<16); }
          steps[n].fixpoint.remainder = (kdu_int16) fix_lambda;
          steps[n].fixpoint.pre_offset = (kdu_int16)
            floor(0.5 + ((double)(1<<15)) / ((double) fix_lambda));
        }
    }

  kdu_dims dims;
  kdu_coords min, max;

  // Get output dimensions.

  resolution.get_dims(dims);
  min = dims.pos; max = min + dims.size; max.x--; max.y--;
  y_next = min.y;
  y_max = max.y;
  x_min = min.x;
  x_max = max.x;
  
  empty = !dims;
  unit_height = (y_next==y_max);
  unit_width = (x_min==x_max);

  low_width = ((max.x+2)>>1) - ((min.x+1)>>1);
  high_width = ((max.x+1)>>1) - (min.x>>1);

  output_rows_remaining = y_max+1-y_next;

  if (empty)
    return;

  // Pre-allocate the line buffers.

  augend.pre_create(allocator,low_width,high_width,reversible,use_shorts);
  new_state.pre_create(allocator,low_width,high_width,reversible,use_shorts);
  for (n=0; n < L_max; n++)
    steps[n].state.pre_create(allocator,low_width,high_width,
                              reversible,use_shorts);
  initialized = false; // Finalize creation in the first `push' call.

  // Now determine the normalizing downshift and subband nominal ranges.

  float LL_range, HL_range, LH_range, HH_range;
  LL_range = HL_range = LH_range = HH_range = normalization;
  normalizing_downshift = 0;
  if (!reversible)
    {
      int lev_idx = resolution.get_dwt_level(); assert(lev_idx > 0);
      double bibo_low, bibo_high, bibo_prev;
      kernels.get_bibo_gains(lev_idx-1,bibo_prev,bibo_high);
      double *bibo_steps = kernels.get_bibo_gains(lev_idx,bibo_low,bibo_high);
      double bibo_max = 0.0;

      // Find BIBO and nominal ranges for the vertical analysis transform.
      if (unit_height)
        bibo_max = normalization;
      else
        {
          LL_range /= low_gain;  HL_range /= low_gain;
          LH_range /= high_gain; HH_range /= high_gain;
          bibo_prev *= normalization; // BIBO horizontal range at stage input
          for (n=0; n < L_max; n++)
            if ((bibo_prev * bibo_steps[n]) > bibo_max)
              bibo_max = bibo_prev * bibo_steps[n];
        }
      // Find BIBO gains for horizontal analysis
      if (!unit_width)
        {
          LL_range /= low_gain;  LH_range /= low_gain;
          HL_range /= high_gain; HH_range /= high_gain;
          bibo_prev = bibo_low / low_gain; // If bounded by vertical low band
          if ((bibo_high / high_gain) > bibo_prev)
            bibo_prev = bibo_high / high_gain; // Bounded by vertical high band
          bibo_prev *= normalization; // BIBO vertical range at horiz. input
          for (n=0; n < L_max; n++)
            if ((bibo_prev * bibo_steps[n]) > bibo_max)
              bibo_max = bibo_prev * bibo_steps[n];
        }
      double overflow_limit = 1.0 * (double)(1<<(16-KDU_FIX_POINT));
          // This is the largest numeric range which can be represented in
          // our signed 16-bit fixed-point representation without overflow.
      while (bibo_max > 0.95*overflow_limit)
        { // Leave a little extra headroom to allow for approximations in
          // the numerical BIBO gain calculations.
          normalizing_downshift++;
          LL_range*=0.5F; LH_range*=0.5F; HL_range*=0.5F; HH_range*=0.5F;
          bibo_max *= 0.5;
        }
    }

  // Finally, create the subband interfaces.

  kdu_roi_node *LL_node=NULL, *HL_node=NULL, *LH_node=NULL, *HH_node=NULL;
  if (roi != NULL)
    {
      LL_node = roi_level.acquire_node(LL_BAND);
      HL_node = roi_level.acquire_node(HL_BAND);
      LH_node = roi_level.acquire_node(LH_BAND);
      HH_node = roi_level.acquire_node(HH_BAND);
    }

  assert(resolution.which() > 0);
  if (resolution.which() == 1)
    hor_low[0] = kdu_encoder(resolution.access_next().access_subband(LL_BAND),
                             allocator,use_shorts,LL_range,LL_node);
  else
    hor_low[0] = kdu_analysis(resolution.access_next(),
                              allocator,use_shorts,LL_range,LL_node);
  hor_high[0] = kdu_encoder(resolution.access_subband(HL_BAND),
                            allocator,use_shorts,HL_range,HL_node);
  hor_low[1] = kdu_encoder(resolution.access_subband(LH_BAND),
                           allocator,use_shorts,LH_range,LH_node);
  hor_high[1] = kdu_encoder(resolution.access_subband(HH_BAND),
                            allocator,use_shorts,HH_range,HH_node);
}

/*****************************************************************************/
/*                       kd_analysis::~kd_analysis                           */
/*****************************************************************************/

kd_analysis::~kd_analysis()
{
  hor_low[0].destroy();
  hor_low[1].destroy();
  hor_high[0].destroy();
  hor_high[1].destroy();
  if (roi_level.exists())
    roi_level.destroy(); // Important to do this last, giving descendants a
                         // chance to call the `release' function on their
                         // `roi_node' interfaces.
}

/*****************************************************************************/
/*                            kd_analysis::push                              */
/*****************************************************************************/

void
  kd_analysis::push(kdu_line_buf &line, bool allow_exchange)
{
  assert(y_next <= y_max);
  assert(reversible == line.is_absolute());
  if (empty)
    {
      y_next++;
      output_rows_remaining--;
      return;
    }
  int k, c;

  if (!initialized)
    { // Finish creating all the buffers.
      augend.create(); augend.deactivate();
      new_state.create(); new_state.deactivate();
      for (k=0; k < L_max; k++)
        { steps[k].state.create(); steps[k].state.deactivate(); }
      initialized = true;
    }

  // Determine the appropriate input line.

  kd_line_cosets *in = (y_next & 1)?(&augend):(&new_state);
  if (!in->is_active())
    in->activate();
  in->lnum = y_next++;

  // Copy the samples from `line', de-interleaving even and odd cosets

  assert(line.get_width() == (low_width+high_width));
  c = x_min & 1; // Index of first coset to be de-interleaved.
  k = (line.get_width()+1)>>1; // May move one extra sample.
  if (!use_shorts)
    { // Working with 32-bit data
      kdu_sample32 *sp = line.get_buf32();
      kdu_sample32 *dp1 = in->cosets[c].get_buf32();
      kdu_sample32 *dp2 = in->cosets[1-c].get_buf32();
      if (normalizing_downshift == 0)
        for (; k--; sp+=2, dp1++, dp2++)
          { *dp1 = sp[0]; *dp2 = sp[1]; }
      else
        {
          float scale = 1.0F / (float)(1<<normalizing_downshift);
          for (; k--; sp+=2, dp1++, dp2++)
            {
              dp1->fval = sp[0].fval * scale;
              dp2->fval = sp[1].fval * scale;
            }
        }
    }
  else
    { // Working with 16-bit data
      kdu_sample16 *sp = line.get_buf16();
      kdu_sample16 *dp1 = in->cosets[c].get_buf16();
      kdu_sample16 *dp2 = in->cosets[1-c].get_buf16();
      if (normalizing_downshift == 0)
        for (; k--; sp+=2, dp1++, dp2++)
          { *dp1 = sp[0]; *dp2 = sp[1]; }
      else
        {
          kdu_int16 offset = (kdu_int16)((1<<normalizing_downshift)>>1);
          for (; k--; sp+=2, dp1++, dp2++)
            {
              dp1->ival = (sp[0].ival+offset) >> normalizing_downshift;
              dp2->ival = (sp[1].ival+offset) >> normalizing_downshift;
            }
        }
    }

  // Perform the transformation

  if (unit_height)
    { // No transform performed in this special case.
      if (reversible && (in->lnum & 1))
        { // Need to double the integer sample values.
          if (!use_shorts)
            { // Working with 32-bit data
              kdu_sample32 *dp;
              for (c=0; c < 2; c++)
                for (dp=in->cosets[c].get_buf32(),
                     k=in->cosets[c].get_width(); k--; dp++)
                  dp->ival <<= 1;
            }
          else
            { // Working with 16-bit data
              kdu_sample16 *dp;
              for (c=0; c < 2; c++)
                for (dp=in->cosets[c].get_buf16(),
                     k=in->cosets[c].get_width(); k--; dp++)
                  dp->ival <<= 1;
            }
        }
      horizontal_analysis(*in);
    }
  else
    { // Need to perform the vertical transform.
      int n;
      kd_line_cosets tmp;
      kd_lifting_step *step;

      if (in == &augend)
        {
          if (y_next <= y_max)
            return; // Still waiting for the row after augend
          new_state.deactivate(); // We have received the last line.
        }
      do { // Loop runs multiple times only to flush the pipe after last line
          if (in == NULL)
            { // There is no more input; must be flushing the pipe.
              augend.deactivate();
              new_state.deactivate();
            }
          in = NULL; // Just use it to flag the flushing condition if we loop

          // Run the vertical analysis network
          for (n=0; n < L_max; n++)
            {
              step = steps + n;
              if (augend.is_active())
                perform_vertical_lifting_step(step);
              tmp = step->state;
              step->state = new_state;
              new_state = augend;
              augend = tmp;
            }

          // Push newly generated subband lines down the pipe.
          if (new_state.is_active())
            horizontal_analysis(new_state);
          if (augend.is_active())
            horizontal_analysis(augend);
        } while((y_next > y_max) && (output_rows_remaining > 0));
    }
}

/*****************************************************************************/
/*               kd_analysis::perform_vertical_lifting_step                  */
/*****************************************************************************/

void
  kd_analysis::perform_vertical_lifting_step(kd_lifting_step *step)
{
  assert(step->state.is_active() || new_state.is_active());
  assert((!step->state) || (step->state.lnum==(augend.lnum-1)));
  assert((!new_state) || (new_state.lnum==(augend.lnum+1)));
  for (int c=0; c < 2; c++) // Walk through the two horizontal cosets
    if (!use_shorts)
      { // Processing 32-bit samples.
        kdu_sample32 *sp1 = step->state.cosets[c].get_buf32();
        kdu_sample32 *sp2 = new_state.cosets[c].get_buf32();
        if (sp1 == NULL) sp1 = sp2;
        if (sp2 == NULL) sp2 = sp1;
        kdu_sample32 *dp = augend.cosets[c].get_buf32();
        int k = augend.cosets[c].get_width();
        if (!reversible)
          {
            float lambda = step->lambda;
            for (; k--; sp1++, sp2++, dp++)
              dp->fval += lambda*(sp1->fval+sp2->fval);
          }
        else
          {
            kdu_int32 downshift = step->downshift;
            kdu_int32 offset = (1<<downshift)>>1;
            kdu_int32 i_lambda = step->i_lambda;
            if (i_lambda == 1)
              for (; k--; sp1++, sp2++, dp++)
                dp->ival += (offset+sp1->ival+sp2->ival)>>downshift;
            else if (i_lambda == -1)
              for (; k--; sp1++, sp2++, dp++)
                dp->ival += (offset-sp1->ival-sp2->ival)>>downshift;
            else
              for (; k--; sp1++, sp2++, dp++)
                dp->ival += (offset+i_lambda*(sp1->ival+sp2->ival))>>downshift;
          }
      }
    else
      { // Processing 16-bit samples.
        kdu_sample16 *sp1 = step->state.cosets[c].get_buf16();
        kdu_sample16 *sp2 = new_state.cosets[c].get_buf16();
        if (sp1 == NULL) sp1 = sp2;
        if (sp2 == NULL) sp2 = sp1;
        kdu_sample16 *dp = augend.cosets[c].get_buf16();
        int k = augend.cosets[c].get_width();
        if (!reversible)
          {
            kdu_int32 val, lambda = step->fixpoint.fix_lambda;
            for (; k--; sp1++, sp2++, dp++)
              {
                val = sp1->ival + sp2->ival; val *= lambda;
                dp->ival += (kdu_int16)((val+(1<<15))>>16);
              }
          }
        else
          {
            kdu_int32 downshift = step->downshift;
            kdu_int32 offset = (1<<downshift)>>1;
            kdu_int32 val, i_lambda = step->i_lambda;
            if (i_lambda == 1)
              for (; k--; sp1++, sp2++, dp++)
                { val = offset+sp1->ival+sp2->ival;
                  dp->ival += (kdu_int16)(val>>downshift); }
            else if (i_lambda == -1)
              for (; k--; sp1++, sp2++, dp++)
                { val = offset-sp1->ival-sp2->ival;
                  dp->ival += (kdu_int16)(val>>downshift); }
            else
              for (; k--; sp1++, sp2++, dp++)
                { val = sp1->ival+sp2->ival;
                  val = offset + i_lambda*val;
                  dp->ival += (kdu_int16)(val>>downshift); }
          }
      }
}

/*****************************************************************************/
/*                    kd_analysis::horizontal_analysis                       */
/*****************************************************************************/

void
  kd_analysis::horizontal_analysis(kd_line_cosets &line)
{
  assert(output_rows_remaining > 0);
  assert((low_width == line.cosets[0].get_width()) &&
         (high_width == line.cosets[1].get_width()));
  output_rows_remaining--;

  if (unit_width)
    { // Special processing for this case.			   hi
      assert((low_width+high_width)==1);
      if (reversible && (x_min & 1))
        {
          if (!use_shorts)	 
            line.cosets[1].get_buf32()->ival <<= 1;
          else
            line.cosets[1].get_buf16()->ival <<= 1;
        }
      if (low_width)
        hor_low[line.lnum & 1].push(line.cosets[0]);
      else
        hor_high[line.lnum & 1].push(line.cosets[1]);
      return;
    }
  
  // Perform lifting steps.

  for (int n=0; n < L_max; n++)
    {
      kd_lifting_step *step = steps + n;
      int c = step->augend_parity; // Coset associated with augend.
      int k = line.cosets[c].get_width();
      int k_src = line.cosets[1-c].get_width();
      int extend_left = ((x_min & 1) == c)?1:0;
      if (!use_shorts)
        { // Processing 32-bit samples
          kdu_sample32 *sp=line.cosets[1-c].get_buf32();
          sp[k_src] = sp[k_src-1]; // Achieves symmetric extension as required
          sp[-1] = sp[0]; sp -= extend_left;
          kdu_sample32 *dp=line.cosets[c].get_buf32();
          if (!reversible)
            {
              float val, lambda = step->lambda, last_in = (sp++)->fval;
              while (k--)
                {
                  val = last_in; last_in = (sp++)->fval; val += last_in;
                  (dp++)->fval += lambda*val;
                }
            }
          else
            {
              kdu_int32 downshift = step->downshift;
              kdu_int32 offset = (1<<downshift)>>1;
              kdu_int32 val, i_lambda = step->i_lambda, last_in = (sp++)->ival;

              if (i_lambda == 1)
                while (k--)
                  {
                    val = last_in; last_in = (sp++)->ival; val += last_in;
                    (dp++)->ival += (offset+val)>>downshift;
                  }
              else if (i_lambda == -1)
                while (k--)
                  {
                    val = last_in; last_in = (sp++)->ival; val += last_in;
                    (dp++)->ival += (offset-val)>>downshift;
                  }
              else
                while (k--)
                  {
                    val = last_in; last_in = (sp++)->ival; val += last_in;
                    (dp++)->ival += (offset+i_lambda*val)>>downshift;
                  }
            }
        }
      else
        { // Processing 16-bit samples
          kdu_sample16 *sp=line.cosets[1-c].get_buf16();
          sp[k_src] = sp[k_src-1]; // Achieves symmetric extension as required
          sp[-1] = sp[0]; sp -= extend_left;
          kdu_sample16 *dp=line.cosets[c].get_buf16();
          if (!reversible)
            {
              kdu_int32 val, last_in = (sp++)->ival;
              kdu_int32 lambda = step->fixpoint.fix_lambda;
              while (k--)
                {
                  val = last_in; last_in = (sp++)->ival; val += last_in;
                  (dp++)->ival += (kdu_int16)((val*lambda + (1<<15))>>16);
                }
            }
          else
            {
              kdu_int32 downshift = step->downshift;
              kdu_int32 offset = (1<<downshift)>>1;
              kdu_int32 val, i_lambda = step->i_lambda, last_in=(sp++)->ival;

              if (i_lambda == 1)
                while (k--)
                  {
                    val = last_in; last_in = (sp++)->ival; val += last_in;
                    (dp++)->ival += (kdu_int16)((offset+val)>>downshift);
                  }
              else if (i_lambda == -1)
                while (k--)
                  {
                    val = last_in; last_in = (sp++)->ival; val += last_in;
                    (dp++)->ival += (kdu_int16)((offset-val)>>downshift);
                  }
              else
                while (k--)
                  {
                    val = last_in; last_in = (sp++)->ival; val += last_in;
                    (dp++)->ival += (kdu_int16)
                      ((offset+i_lambda*val)>>downshift);
                  }
            }
        }
    }

  // Push subband lines out.

  hor_low[line.lnum & 1].push(line.cosets[0]);
  hor_high[line.lnum & 1].push(line.cosets[1]);
}
