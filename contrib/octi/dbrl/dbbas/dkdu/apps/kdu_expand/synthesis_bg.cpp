/*****************************************************************************/
// File: synthesis.cpp [scope = CORESYS/TRANSFORMS]
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
   Implements the reverse DWT (subband/wavelet synthesis).  The implementation
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
#include "synthesis_local.h"

// Set things up for the inclusion of assembler optimized routines
// for specific architectures.  The reason for this is to exploit
// the availability of SIMD type instructions on many modern processors.

#if defined KDU_PENTIUM_MSVC
#  define KDU_SIMD_OPTIMIZATIONS
#  include "msvc_dwt_mmx_local.h" // Header contains all asm commands in-line
static int simd_exists = msvc_dwt_mmx_exists();
#elif defined KDU_PENTIUM_GCC
#  define KDU_SIMD_OPTIMIZATIONS
#  include "gcc_dwt_mmx_local.h" // Header declares functs in separate .s file
static int simd_exists = gcc_dwt_mmx_exists();
#endif // KDU_PENTIUM_GCC

/* ========================================================================= */
/*                               kdu_synthesis                               */
/* ========================================================================= */

/*****************************************************************************/
/*                        kdu_synthesis::kdu_synthesis                       */
/*****************************************************************************/

kdu_bg_synthesis::kdu_bg_synthesis(kdu_resolution resolution, kdu_sample_allocator *allocator,
                   bool use_shorts,bbgm_io& bgm_interface_, float normalization,
                   kde_flow_control* param_flow,kdu_pull_ifc* param_synthesis)
  // In the future, we may create separate, optimized objects for each kernel.
{
  state = new kd_bg_synthesis(resolution,allocator,use_shorts,normalization,
	  param_flow,bgm_interface_,param_synthesis);
}

bgmu_LL_synthesis::bgmu_LL_synthesis(kdu_subband subband, kdu_sample_allocator *allocator,
                   bbgm_io& bgm_interface_,kde_flow_control* param_flow,
				   kdu_pull_ifc* param_synthesis)
  // In the future, we may create separate, optimized objects for each kernel.
{
  state = new bgm_LL_synthesis(subband,allocator,param_flow,bgm_interface_,param_synthesis);
}

/* ========================================================================= */
/*                              kd_bg_synthesis                                 */
/* ========================================================================= */

/*****************************************************************************/
/*                       kd_bg_synthesis::kd_bg_synthesis                          */
/*****************************************************************************/

kd_bg_synthesis::kd_bg_synthesis(kdu_resolution resolution, kdu_sample_allocator *allocator,
                 bool use_shorts, float normalization,kde_flow_control* param_flow_,
				 bbgm_io& bgm_interface_,kdu_pull_ifc* param_synthesis):bgm_interface(bgm_interface_)
{
  reversible = resolution.get_reversible();
  this->use_shorts = use_shorts;
  this->param_flow=param_flow_;
  this->isLast=false;
  this->num_comps=param_flow_->num_components;
  int kernel_id = resolution.get_kernel_id();
  kdu_kernels kernels(kernel_id,reversible);
  float low_gain, high_gain;
  float *factors =
    kernels.get_lifting_factors(L_max,low_gain,high_gain);
  int n;

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

  kdu_dims dims,sb_dims;
  kdu_coords min, max;

  // Get output dimensions.

  resolution.get_dims(dims);
  min = dims.pos; max = min + dims.size; max.x--; max.y--;
  y_out_next = min.y;
  y_out_max = max.y;
  x_out_min = min.x;
  x_out_max = max.x;
  
  empty = !dims;
  if (empty)
    return;

  // Get input dimensions.  Note that the following code contains assert
  // statements which may fail if the low- and high-pass filter lengths do not
  // differ by 2 (as reported by `kdu_kernels' and used in `kdu_resolution' to
  // compute the effects of kernel support on regions of interest.

  resolution.access_next().get_dims(dims); // low-pass dimensions.
  low_width = dims.size.x;
  min = dims.pos; max = min + dims.size; max.x--; max.y--;
  min.x+=min.x; min.y+=min.y; max.x+=max.x; max.y+=max.y;
  y_in_next = min.y;
  y_in_max = max.y;
  x_in_min = min.x;
  x_in_max = max.x;

  resolution.access_subband(HH_BAND).get_dims(dims); // high-pass dimensions.
  high_width = dims.size.x;
  min = dims.pos; max = min + dims.size; max.x--; max.y--;
  min.x+=min.x+1; min.y+=min.y+1; max.x+=max.x+1; max.y+=max.y+1;

  assert(min.y <= y_in_next+1);
  if (min.y < y_in_next)
    { y_in_next--; assert(min.y==y_in_next); }
  assert(max.y >= y_in_max-1);
  if (max.y > y_in_max)
    { y_in_max++; assert(max.y==y_in_max); }

  assert(min.x <= x_in_min+1);
  if (min.x < x_in_min)
    { x_in_min--; assert(min.x==x_in_min); }
  assert(max.x >= x_in_max-1);
  if (max.x > x_in_max)
    { x_in_max++; assert(max.x==x_in_max); }

  assert((y_in_next <= y_out_next) && (y_in_max >= y_out_max));
  assert((x_in_min <= x_out_min) && (x_in_max >= x_out_max));
  assert((y_in_next <= y_in_max) && (x_in_min <= x_in_max));
  
  unit_height = (y_in_next==y_in_max);
  unit_width = (x_in_min==x_in_max);

  resolution.access_subband(HH_BAND).get_dims(sb_dims);
  high_height=sb_dims.size.y;
  resolution.access_subband(HL_BAND).get_dims(sb_dims);
  low_height=sb_dims.size.y;

  // Pre-allocate the line buffers.

  augend.pre_create(allocator,low_width,high_width,reversible,use_shorts,true);
  new_state.pre_create(allocator,low_width,high_width,reversible,use_shorts,true);
  lo_line.pre_create(allocator,low_width,reversible,use_shorts);
  hi_line.pre_create(allocator,high_width,reversible,use_shorts);
  for (n=0; n < L_max; n++)
    steps[n].state.pre_create(allocator,low_width,high_width,
                              reversible,use_shorts,true);
  initialized = false; // Finalize creation in the first `push' call.

  // Now determine the normalizing upshift and subband nominal ranges.

  float LL_range, HL_range, LH_range, HH_range;
  LL_range = HL_range = LH_range = HH_range = normalization;
  normalizing_upshift = 0;
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
      while (bibo_max > 0.75*overflow_limit)
        { // Leave some extra headroom to allow for the effects of
          // dequantization artefacts.
          normalizing_upshift++;
          LL_range*=0.5F; LH_range*=0.5F; HL_range*=0.5F; HH_range*=0.5F;
          bibo_max *= 0.5;
        }
    }

  // Finally, create the subband interfaces.
  int ncomps=this->param_flow->num_components;
  hor_high_decoders=new kdu_pull_ifc*[ncomps];
  hor_low_decoders=new kdu_pull_ifc*[ncomps];
  
  param_synth_network=new kdu_pull_ifc[param_flow->num_components];
    
  for(int i=0;i<ncomps;i++)
  {
	kd_synthesis* tm_synthesis=dynamic_cast<kd_synthesis*>(param_synthesis[i].state);
	if (resolution.which()!=1)
		param_synth_network[i]=tm_synthesis->hor_low[0];
	
	hor_high_decoders[i]=new kdu_pull_ifc[2];
	hor_high_decoders[i][0]=tm_synthesis->hor_high[0];
  	hor_high_decoders[i][1]=tm_synthesis->hor_high[1];
  
	hor_low_decoders[i]=new kdu_pull_ifc[2];
	hor_low_decoders[i][1]=tm_synthesis->hor_low[1];
	 if (resolution.which() == 1)
		hor_low_decoders[i][0]=tm_synthesis->hor_low[0];
	}
  assert(resolution.which() > 0);
  this->level=resolution.which();
  if (resolution.which() == 1)
	  this->isLast=true;
  else
    hor_low[0] = kdu_bg_synthesis(resolution.access_next(),
                               allocator,use_shorts,bgm_interface_,LL_range,param_flow,
							   param_synth_network);
  
  }

/*****************************************************************************/
/*                      kd_bg_synthesis::~kd_bg_synthesis                          */
/*****************************************************************************/

kd_bg_synthesis::~kd_bg_synthesis()
{
  hor_low[0].destroy();
  /*hor_low[1].destroy();
  hor_high[0].destroy();
  hor_high[1].destroy();  */   

  for (int i=0;i<this->num_comps;i++){
	  delete hor_high_decoders[i];
	  delete hor_low_decoders[i];  
  }
  delete [] hor_low_decoders;
  delete [] hor_high_decoders; 
  delete [] param_synth_network;
}

/*****************************************************************************/
/*                           kd_bg_synthesis::pull                              */
/*****************************************************************************/

void
  kd_bg_synthesis::pull(kdu_line_buf &line, bool allow_exchange)
{
  if (empty)
    return;

  if (!initialized)
    { // Finish creating all the buffers.
      augend.create(); augend.deactivate();
      new_state.create(); new_state.deactivate();
	  lo_line.create();
	  hi_line.create();
      for (int n=0; n < L_max; n++)
        { steps[n].state.create(); steps[n].state.deactivate(); }
      initialized = true;
    }

  int c, k;
  kd_line_cosets *out = (y_out_next & 1)?(&new_state):(&augend);

  assert(y_out_next <= y_out_max);
  if (unit_height)
    { // No transform performed in this special case.
      horizontal_synthesis(*out);
      if (reversible && (y_out_next & 1))
        { // Need to halve integer sample values.
          if (!use_shorts)
            { // Working with 32-bit data
              kdu_sample32 *dp;
              for (c=0; c < 2; c++)
                for (dp=out->cosets[c].get_buf32(),
                     k=out->cosets[c].get_width(); k--; dp++)
                  dp->ival >>= 1;
            }
          else
            { // Working with 16-bit data
              kdu_sample16 *dp;
              for (c=0; c < 2; c++)
                for (dp=out->cosets[c].get_buf16(),
                     k=out->cosets[c].get_width(); k--; dp++)
                  dp->ival >>= 1;
            }
        }
    }
  else
    {
      while ((!out->is_active()) || (out->lnum != y_out_next))
        { // Perform lifting steps until we produce the desired output row.
          kd_lifting_step *step = steps+(L_max-1);
          if (step->augend_parity == (y_in_next & 1))
            horizontal_synthesis(augend);
          else
            augend.deactivate(); // does nothing if `augend' already inactive
          horizontal_synthesis(new_state);
          for (; step >= steps; step--)
            {
              if (augend.is_active())
                perform_vertical_lifting_step(step);
              kd_line_cosets tmp = step->state;
              step->state = new_state;
              new_state = augend;
              augend = tmp;
            }
        }
    }
  y_out_next++;

  /* Finally, copy newly generated samples into `line', interleaving the
     even and odd sub-sequences and supplying any required upshift. */

  c = x_out_min & 1; // Index of first coset to be interleaved.
  k = (line.get_width()+1)>>1; // May write one extra sample.
  if (!use_shorts)
    { // Working with 32-bit data
      bgm_mix *dp = line.get_bgmBuf();
      bgm_mix *sp1 = out->cosets[c].get_bgmBuf() +
        ((x_out_min+1-c)>>1) - ((x_in_min+1-c)>>1);
      bgm_mix *sp2 = out->cosets[1-c].get_bgmBuf() +
        ((x_out_min+c)>>1) - ((x_in_min+c)>>1);
      if (normalizing_upshift == 0)
        for (; k--; sp1++, sp2++, dp+=2)
          { dp[0] = *sp1; dp[1] = *sp2; }
      else
        {
          float scale = (float)(1<<normalizing_upshift);
          for (; k--; sp1++, sp2++, dp+=2)
            {
              dp[0] = *sp1*scale;
              dp[1] = *sp2*scale;
            }
        }
    }
 
}



/*****************************************************************************/
/*                 kd_bg_synthesis::perform_vertical_lifting_step               */
/*****************************************************************************/

void
  kd_bg_synthesis::perform_vertical_lifting_step(kd_lifting_step *step)
{
  assert(step->state.is_active() || new_state.is_active());
  assert((!step->state) || (step->state.lnum==(augend.lnum-1)));
  assert((!new_state) || (new_state.lnum==(augend.lnum+1)));
  for (int c=0; c < 2; c++) // Walk through the two horizontal cosets
    if (!use_shorts)
      { // Processing 32-bit samples.
        bgm_mix *sp1 = step->state.cosets[c].get_bgmBuf();
        bgm_mix *sp2 = new_state.cosets[c].get_bgmBuf();
        if (sp1 == NULL) sp1 = sp2;
        if (sp2 == NULL) sp2 = sp1;
        bgm_mix *dp = augend.cosets[c].get_bgmBuf();
        int k = augend.cosets[c].get_width();
        if (!reversible)
          {
            float lambda = step->lambda;
            for (; k--; sp1++, sp2++, dp++)
              *dp -= (*sp1+*sp2)*lambda;
          }
        /*else
          {
            kdu_int32 downshift = step->downshift;
            kdu_int32 offset = (1<<downshift)>>1;
            kdu_int32 i_lambda = step->i_lambda;
            if (i_lambda == 1)
              for (; k--; sp1++, sp2++, dp++)
                dp->ival -= (offset+sp1->ival+sp2->ival)>>downshift;
            else if (i_lambda == -1)
              for (; k--; sp1++, sp2++, dp++)
                dp->ival -= (offset-sp1->ival-sp2->ival)>>downshift;
            else
              for (; k--; sp1++, sp2++, dp++)
                dp->ival -= (offset+i_lambda*(sp1->ival+sp2->ival))>>downshift;
          }*/
      }
    
      
}

/*****************************************************************************/
/*                   kd_bg_synthesis::horizontal_synthesis                      */
/*****************************************************************************/

void
  kd_bg_synthesis::horizontal_synthesis(kd_line_cosets &line)
{
  line.lnum = y_in_next;
  if (y_in_next > y_in_max)
    { // No more lines available. Signal this by returning with empty buffer.
      line.deactivate();
      return;
    }

  if (!line)
    {
      line.activate();
      line.lnum = y_in_next;
    }
  kdu_dims hi_dims,lo_dims;
  int parity=line.lnum &1;
  if (!(parity)&& (!this->isLast))
      hor_low[parity].pull(line.cosets[0]);
  
  hi_dims.size.x=line.cosets[1].get_width();
  hi_dims.size.y=high_height;
  
  lo_dims.size.x=line.cosets[0].get_width();
  lo_dims.size.y=low_height;

  //this->bgm_interface.init_bgm_line(line.cosets[0].get_bgmBuf(),low_width);
  //this->bgm_interface.init_bgm_line(line.cosets[1].get_bgmBuf(),high_width);
  for (int i=0;i<param_flow->num_components;i++)
	{
										  
		if (parity)
		{
			hor_high_decoders[i][parity].pull(this->hi_line); //HH
			hor_low_decoders[i][parity].pull(this->lo_line); //LH

			/*if ((i==0)||(i==7)||(i==14)){
				this->bgm_interface.putWeightFromModel(
			line.cosets[1].get_bgmBuf(),HH_BAND,lo_dims,hi_dims,this->level,i/7);
					this->bgm_interface.putWeightFromModel(
			line.cosets[0].get_bgmBuf(),LH_BAND,lo_dims,hi_dims,this->level,i/7);
			}else{ */
			
				this->bgm_interface.convert_rv_to_param(this->hi_line,
				line.cosets[1].get_bgmBuf(),i,false);
				this->bgm_interface.convert_rv_to_param(this->lo_line,
				line.cosets[0].get_bgmBuf(),i,false);	
			
			//takes care of HH and LH
		}else{
			hor_high_decoders[i][parity].pull(this->hi_line);
			
			if (this->isLast)
				hor_low_decoders[i][parity].pull(this->lo_line);
			
										
			this->bgm_interface.convert_rv_to_param(this->hi_line,
			line.cosets[1].get_bgmBuf(),i,false); //HL
			
			if(this->isLast){
			this->bgm_interface.convert_rv_to_param(this->lo_line,
				line.cosets[0].get_bgmBuf(),i,false);	//LL	
				
				}
			}
		
			
    }

  this->bgm_interface.clean_bgm_line(line.cosets[0].get_bgmBuf(),low_width);
  this->bgm_interface.clean_bgm_line(line.cosets[1].get_bgmBuf(),high_width);
   y_in_next++;

  if (unit_width)
    { // Special processing for this case.		//needs further editing
      assert((low_width+high_width)==1);
      if (reversible && (x_in_min & 1))
        {
          if (line.cosets[1].get_buf32() != NULL)
            line.cosets[1].get_buf32()->ival >>= 1;
          else
            line.cosets[1].get_buf16()->ival >>= 1;
        }
      return;
    }

  // Perform lifting steps.

  kd_lifting_step *step = steps + L_max-1;
  for (; step >= steps; step--)
    {
      int c = step->augend_parity; // Coset associated with augend.
      int k = line.cosets[c].get_width();
      int k_src = line.cosets[1-c].get_width();
      int extend_left = ((x_in_min & 1) == c)?1:0;
      if (!use_shorts)
        { // Processing 32-bit samples
          bgm_mix *sp=line.cosets[1-c].get_bgmBuf();
          sp[k_src] = sp[k_src-1]; // Achieves symmetric extension as required
          sp[-1] = sp[0]; sp -= extend_left;
          bgm_mix *dp=line.cosets[c].get_bgmBuf();
          if (!reversible)
            {
              float lambda = step->lambda; 
			  bgm_mix val,last_in = *(sp++);
              while (k--)
                {
                  val = last_in; last_in = *(sp++); val += last_in;
                  *(dp++) -= val*lambda;
                }
            }
         /* else
            {
              kdu_int32 downshift = step->downshift;
              kdu_int32 offset = (1<<downshift)>>1;
              kdu_int32 val, i_lambda = step->i_lambda, last_in = (sp++)->ival;
              if (i_lambda == 1)
                while (k--)
                  {
                    val = last_in; last_in = (sp++)->ival; val += last_in;
                    (dp++)->ival -= (offset+val)>>downshift;
                  }
              else if (i_lambda == -1)
                while (k--)
                  {
                    val = last_in; last_in = (sp++)->ival; val += last_in;
                    (dp++)->ival -= (offset-val)>>downshift;
                  }
              else
                while (k--)
                  {
                    val = last_in; last_in = (sp++)->ival; val += last_in;
                    (dp++)->ival -= (offset+i_lambda*val)>>downshift;
                  }
            }*/
        }
      
    }
}

bgm_LL_synthesis::bgm_LL_synthesis(kdu_subband LL, kdu_sample_allocator *allocator,
                kde_flow_control* param_flow_,
				 bbgm_io& bgm_interface_,kdu_pull_ifc* param_synthesis):bgm_interface(bgm_interface_)
{
  
  this->param_flow=param_flow_;
  this->isLast=true;
  this->initialized=false;
  kdu_dims sb_dims;
  LL.get_dims(sb_dims);
  low_width=sb_dims.size.x;
  lo_line.pre_create(allocator,low_width,false,false);
 
  int ncomps=this->param_flow->num_components;
  hor_low_decoders=new kdu_pull_ifc[ncomps];

  for(int i=0;i<ncomps;i++)
  {
	hor_low_decoders[i]=param_synthesis[i];
  }
}
void
  bgm_LL_synthesis::pull(kdu_line_buf &line, bool allow_exchange)
{
	
	if (!initialized)
    { // Finish creating all the buffers.
      lo_line.create();
	  initialized = true;
    }
	
	for (int i=0;i<param_flow->num_components;i++)
	{
	  hor_low_decoders[i].pull(this->lo_line);
	  this->bgm_interface.convert_rv_to_param(this->lo_line,
	  	line.get_bgmBuf(),i,false);	//LL	
	
    }

  this->bgm_interface.clean_bgm_line(line.get_bgmBuf(),low_width);
 
}
bgm_LL_synthesis::~bgm_LL_synthesis(){
	   delete [] hor_low_decoders;

}