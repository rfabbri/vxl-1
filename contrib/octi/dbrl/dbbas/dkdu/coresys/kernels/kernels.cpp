/*****************************************************************************/
// File: kernels.cpp [scope = CORESYS/DWT-KERNELS]
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
   Implements the services defined by "kdu_kernels.h"
******************************************************************************/

#include <assert.h>
#include <math.h>
#include "kdu_elementary.h"
#include "kdu_messaging.h"
#include "kdu_params.h"
#include "kdu_kernels.h"


/* ========================================================================= */
/*                                kdu_kernels                                */
/* ========================================================================= */

/*****************************************************************************/
/*                          kdu_kernels::kdu_kernels                         */
/*****************************************************************************/

kdu_kernels::kdu_kernels(int kernel_id, bool reversible)
{
  this->kernel_id = kernel_id;
  this->downshifts = NULL;
  if (kernel_id == Ckernels_W5X3)
    {
      num_steps = 2;
      lifting_factors = new float[num_steps];
      lifting_factors[0] = -0.5F;
      lifting_factors[1] = 0.25F;
      if (reversible)
        {
          downshifts = new int[num_steps];
          downshifts[0] = 1;
          downshifts[1] = 2;
        }
    }
  else if (kernel_id == Ckernels_W9X7)
    {
      num_steps = 4;
      lifting_factors = new float[num_steps];
      if (reversible)
        { kdu_error e; e << "The W9X7 kernel may not be used for reversible "
          "compression!"; }
      lifting_factors[0] = (float) -1.586134342;
      lifting_factors[1] = (float) -0.052980118;
      lifting_factors[2] = (float) 0.882911075;
      lifting_factors[3] = (float)  0.443506852;
    }
  else
    { kdu_error e; e << "Illegal DWT kernel ID used to construct a "
      "`kdu_kernels' object."; }

  // Now let's derive all the remaining quantities.

  low_analysis_L   = num_steps;   // These lengths may be pessimistic
  high_analysis_L  = num_steps-1; // if one or more of the lifting factors
  low_synthesis_L  = num_steps-1; // is equal to 0.
  high_synthesis_L = num_steps;
  
  low_analysis_taps = (new float[2*low_analysis_L+1])+low_analysis_L;
  high_analysis_taps = (new float[2*high_analysis_L+1])+high_analysis_L;
  low_synthesis_taps = (new float[2*low_synthesis_L+1])+low_synthesis_L;
  high_synthesis_taps = (new float[2*high_synthesis_L+1])+high_synthesis_L;

  // Initialize the vector expansion buffers.

  max_expansion_levels = 4;
  work_L = num_steps+1; // Allow for placing the input impulse at n=1 or n=0.
  for (int d=1; d < max_expansion_levels; d++)
    work_L = work_L*2 + num_steps;
  work1 = (new float[2*work_L+1]) + work_L;
  work2 = (new float[2*work_L+1]) + work_L;
  bibo_step_gains = new double[num_steps];

  // Deduce synthesis impulse responses, without scaling factors.

  int n, k;

  for (n=0; n <= work_L; n++)
    work1[n] = work1[-n] = 0.0F;
  work1[0]=1.0F; // Simulate an impulse in the low-pass subband.
  for (k=num_steps-1; k >= 0; k--)
    {
      if (k&1)
        n = -(num_steps & (~1)); // Smallest even integer >= -num_steps.
      else
        n = ((-num_steps) & (~1)) + 1; // Smallest odd integer >= -num_steps.
      for (; n <= num_steps; n+=2)
        work1[n] -= lifting_factors[k]*(work1[n-1]+work1[n+1]);
    }
  for (n=0; n <= low_synthesis_L; n++)
    low_synthesis_taps[n] = low_synthesis_taps[-n] = work1[n];

  for (n=0; n <= work_L; n++)
    work1[n] = work1[-n] = 0.0F;
  work1[1]=1.0F; // Simulate an impulse in the high-pass subband.
  for (k=num_steps-1; k >= 0; k--)
    {
      if (k&1)
        n = -(num_steps & (~1)); // Smallest even integer >= -num_steps.
      else
        n = ((-num_steps) & (~1)) + 1; // Smallest odd integer >= -num_steps.
      for (; n <= (num_steps+1); n+=2)
        work1[n] -= lifting_factors[k]*(work1[n-1]+work1[n+1]);
    }
  for (n=0; n <= high_synthesis_L; n++)
    high_synthesis_taps[n] = high_synthesis_taps[-n] = work1[n+1];

  // Deduce analysis kernels from synthesis kernels.

  float sign_flip;
  for (sign_flip=1.0F, n=0; n <= low_analysis_L; n++, sign_flip=-sign_flip)
    low_analysis_taps[n] = low_analysis_taps[-n] =
      sign_flip*high_synthesis_taps[n];
  for (sign_flip=1.0F, n=0; n <= high_analysis_L; n++, sign_flip=-sign_flip)
    high_analysis_taps[n] = high_analysis_taps[-n] =
      sign_flip*low_synthesis_taps[n];

  // Deduce scaling factors and normalize filter taps.

  if (reversible)
    { low_scale = high_scale = 1.0F; return; }

  float gain;

  gain=low_analysis_taps[0];
  for (n=1; n <= low_analysis_L; n++)
    gain += 2*low_analysis_taps[n];
  low_scale = 1.0F / gain;
  for (n=-low_analysis_L; n <= low_analysis_L; n++)
    low_analysis_taps[n] *= low_scale;
  for (n=-low_synthesis_L; n <= low_synthesis_L; n++)
    low_synthesis_taps[n] *= gain;

  gain = high_analysis_taps[0];
  for (sign_flip=-1.0F, n=1; n <= high_analysis_L; n++, sign_flip=-sign_flip)
    gain += 2*sign_flip*high_analysis_taps[n];
  high_scale = 1.0F / gain;
  for (n=-high_analysis_L; n <= high_analysis_L; n++)
    high_analysis_taps[n] *= high_scale;
  for (n=-high_synthesis_L; n <= high_synthesis_L; n++)
    high_synthesis_taps[n] *= gain;
}

/*****************************************************************************/
/*                          kdu_kernels::~kdu_kernels                        */
/*****************************************************************************/

kdu_kernels::~kdu_kernels()
{
  if (downshifts != NULL)
    delete[] downshifts;
  delete[] lifting_factors;
  delete[] (low_analysis_taps-low_analysis_L);
  delete[] (high_analysis_taps-high_analysis_L);
  delete[] (low_synthesis_taps-low_synthesis_L);
  delete[] (high_synthesis_taps-high_synthesis_L);
  delete[] (work1-work_L);
  delete[] (work2-work_L);
  delete[] bibo_step_gains;
}

/*****************************************************************************/
/*                      kdu_kernels::get_lifting_factors                     */
/*****************************************************************************/

float *
  kdu_kernels::get_lifting_factors(int &num_steps,
                                   float &low_scale, float &high_scale)
{
  num_steps = this->num_steps;
  low_scale = this->low_scale;
  high_scale = this->high_scale;
  return lifting_factors;
}

/*****************************************************************************/
/*                     kdu_kernels::get_impulse_response                     */
/*****************************************************************************/

float *
  kdu_kernels::get_impulse_response(kdu_kernel_type which, int &half_length)
{
  switch (which) {
    case KDU_ANALYSIS_LOW:
      half_length = low_analysis_L;
      return low_analysis_taps;
    case KDU_ANALYSIS_HIGH:
      half_length = high_analysis_L;
      return high_analysis_taps;
    case KDU_SYNTHESIS_LOW:
      half_length = low_synthesis_L;
      return low_synthesis_taps;
    case KDU_SYNTHESIS_HIGH:
      half_length = high_synthesis_L;
      return high_synthesis_taps;
    default:
      assert(0);
    }
  return NULL;
}

/*****************************************************************************/
/*                        kdu_kernels::get_energy_gain                       */
/*****************************************************************************/

double
  kdu_kernels::get_energy_gain(kdu_kernel_type which, int level_idx)
{
  if (level_idx == 0)
    return (which==KDU_SYNTHESIS_LOW)?1.0:0.0;
  int extra_levels = level_idx - max_expansion_levels;
  if (extra_levels < 0)
    extra_levels = 0; 
  else
    level_idx -= extra_levels;

  int L, n, k;

  if (which == KDU_SYNTHESIS_LOW)
    {
      L = low_synthesis_L;
      for (n=-L; n <= L; n++)
        work1[n] = low_synthesis_taps[n];
    }
  else if (which == KDU_SYNTHESIS_HIGH)
    {
      L = high_synthesis_L;
      for (n=-L; n <= L; n++)
        work1[n] = high_synthesis_taps[n];
    }
  else
    assert(0); // Function only computes synthesis energy gains.
  for (level_idx--; level_idx > 0; level_idx--)
    {
      float *tbuf=work1; work1=work2; work2=tbuf;
      int new_L = 2*L + low_synthesis_L;
      assert(new_L <= work_L);
      for (n=-new_L; n <= new_L; n++)
        work1[n] = 0.0F;
      for (n=-L; n <= L; n++)
        for (k=-low_synthesis_L; k <= low_synthesis_L; k++)
          work1[2*n+k] += work2[n]*low_synthesis_taps[k];
      L = new_L;
    }

  double val, energy = 0.0;
  for (n=-L; n <= L; n++)
    {
      val = work1[n];
      energy += val*val;
    }
  while (extra_levels--)
    energy *= 2.0;

  return energy;
}

/*****************************************************************************/
/*                         kdu_kernels::get_bibo_gains                       */
/*****************************************************************************/

double *
  kdu_kernels::get_bibo_gains(int level_idx,
                              double &low_gain, double &high_gain)
{
  if (level_idx == 0)
    {
      low_gain = 1.0;
      high_gain = 0.0;
      return NULL;
    }

  if (level_idx > max_expansion_levels)
    level_idx = max_expansion_levels;
  float *work_low=work1, *work_high=work2;

  // In the seqel, `work_low' will hold the analysis kernel used to compute
  // the even sub-sequence entry at location 0, while `work_high' will hold the
  // analysis kernels used to compute the odd sub-sequence entry at location
  // 1.  The lifting procedure is followed to alternately update these
  // analysis kernels.

  int k, lev, low_L, high_L, gap;

  // Initialize analysis vectors and gains for a 1 level lazy wavelet
  for (k=-work_L; k <= work_L; k++)
    work_low[k] = work_high[k] = 0.0F;
  work_low[0] = 1.0F;
  low_L = high_L = 0;
  low_gain = high_gain = 1.0;

  for (gap=1, lev=1; lev <= level_idx; lev++, gap<<=1)
    { // Work through the levels
      /* Copy the low analysis vector from the last level to the high analysis
         vector for the current level. */
      for (k=0; k <= low_L; k++)
        work_high[k] = work_high[-k] = work_low[k];
      for (; k <= high_L; k++)
        work_high[k] = work_high[-k] = 0.0F;
      high_L = low_L;
      high_gain = low_gain;

      for (int step=0; step < num_steps; step+=2)
        { // Work through the lifting steps in this level
          float factor;
          // Start by updating the odd sub-sequence analysis kernel
          factor = lifting_factors[step];
          assert((low_L+gap) <= work_L);
          for (k=-low_L; k <= low_L; k++)
            {
              work_high[k-gap] += work_low[k]*factor;
              work_high[k+gap] += work_low[k]*factor;
            }
          high_L = ((low_L+gap) > high_L)?(low_L+gap):high_L;
          for (high_gain=0.0, k=-high_L; k <= high_L; k++)
            high_gain += fabs(work_high[k]);
          bibo_step_gains[step] = high_gain;
          
          // Now update the even sub-sequence analysis kernel
          if ((step+1) < num_steps)
            {
              factor = lifting_factors[step+1];
              assert((high_L+gap) <= work_L);
              for (k=-high_L; k <= high_L; k++)
                {
                  work_low[k-gap] += work_high[k]*factor;
                  work_low[k+gap] += work_high[k]*factor;
                }
              low_L = ((high_L+gap) > low_L)?(high_L+gap):low_L;
              for (low_gain=0.0, k=-low_L; k <= low_L; k++)
                low_gain += fabs(work_low[k]);
              bibo_step_gains[step+1] = low_gain;
            }
        }

      // Now incorporate the subband scaling factors
      for (k=-high_L; k <= high_L; k++)
        work_high[k] *= high_scale;
      high_gain *= high_scale;
      for (k=-low_L; k <= low_L; k++)
        work_low[k] *= low_scale;
      low_gain *= low_scale;
    }

  return bibo_step_gains;
}
