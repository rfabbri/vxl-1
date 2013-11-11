/*****************************************************************************/
// File: synthesis_local.h [scope = CORESYS/TRANSFORMS]
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
   Local definitions used by "synthesis.cpp".  These are not to be included
from any other scope.
******************************************************************************/

#ifndef SYNTHESIS_LOCAL_H
#define SYNTHESIS_LOCAL_H

#include <assert.h>
#include "kdu_compressed.h"
#include "kdu_sample_processing.h"
#include "transform_local.h"
#include "expand_local.h"

// Defined here

class kd_synthesis;
class kdu_synthesis;
class bgmu_LL_synthesis;
/*****************************************************************************/
/*                               kd_synthesis                                */
/*****************************************************************************/

class kd_synthesis : public kdu_pull_ifc_base {
  public: // Member functions
    kd_synthesis(kdu_resolution resolution, kdu_sample_allocator *allocator,
                 bool use_shorts, float normalization);
  protected: // These functions implement their namesakes in the base class
    virtual ~kd_synthesis();
    virtual void pull(kdu_line_buf &line, bool allow_exchange);
  protected: // Internal implementation functions
    void perform_vertical_lifting_step(kd_lifting_step *step);
      /* This function implements a single vertical lifting step, using the
         three lines identified by `step->state', `augend' and `new_state'.
         For an explanation of the algorithm and the interpretation of these
         three quantities, see the notes below. */
    void horizontal_synthesis(kd_line_cosets &line);
      /* This function is responsible for loading subband samples and
         horizontally synthesizing a single line.  The relevant line and
         hence the relevant subbands are determined by the value of the
         state variable, `y_in_next', which the function increments before
         returning.  If `y_in_next' >= `y_in_lim', the function calls
         `line.destroy()'.  This has the dual effect of freeing storage
         as soon as possible and also signalling the fact that the symmetric
         extension mechanism should be used to obtain the line.  Otherwise,
         if `line' is already empty, it is allocated storage inside this
         function.  Upon completion, the line's `lnum' field is updated to
         reflect the value of `y_in_next' on entry. */
  protected: // Data
    friend class kd_bg_synthesis;
	friend class bgm_LL_synthesis;
	kdu_pull_ifc hor_low[2];  // Interfaces for acquiring LL and LH samples
    kdu_pull_ifc hor_high[2]; // Interfaces for acquiring HL and HH samples
    int L_max; // Max filter length is 2L_max+1, but also `L_max' = num steps
    bool reversible;
    bool use_shorts; // All `pull' calls will request 16-bit samples
    bool initialized; // True after the first `pull' call.
    int normalizing_upshift; // Amount to post-shift irreversible outputs.
    kd_lifting_step steps[4]; // Array of L_max lifting steps in analysis order
    kd_line_cosets augend; // Augend for the next vertical synthesis step
    kd_line_cosets new_state; // Will become the next vertical step's state.
    int y_out_next, y_out_max;
    int y_in_next, y_in_max;
    bool unit_height; // Sequences of length 1 must be processed differently.
    int x_out_min, x_out_max;
    int x_in_min, x_in_max;
    int low_width, high_width; // Width of horizontal low- and high-pass bands
    bool unit_width;
    bool empty; // True only if the resolution has no area.
  };
  /* Notes:
     This object is responsible for performing synthesis operations for a
     single 2D DWT stage.  Horizontal synthesis is performed first, followed
     by vertical synthesis.  The implementation is incremental and aims to
     minimize memory consumption.  Synthesized lines are requested one by
     one via the `pull' function.  This function invokes the vertical synthesis
     logic, which in turn may need to recover subband lines from one of
     the two `hor_low' interfaces and one of the two `hor_high' interfaces,
     performing horizontal synthesis.
        Vertical synthesis is implemented in a remarkably simple and efficient
     manner.  The implementation is based around the lifting state machine
     depicted in Figure 6.14 of the book by Taubman and Marcellin.  This
     implementation structure may be used to realize any symmetric
     DWT kernel having filters with odd lengths differing by 2 (it is, in fact,
     a canonical factorization for the class of two channel FIR PR subband
     transforms, having linear phase filters with least dissimilar odd
     lengths -- low- and high-pass filter lengths differ by exactly 2).
     For a thorough explanation of the lifting state machine and its
     implications for boundary extension, consult Sections 6.4.4 and 6.5.3
     in the book by Taubman and Marcellin.
        The vertical synthesis implementation assigns a single state (line
     buffer) to each lifting step, which represents the earlier of the two
     inputs to the lifting step's two tap, symmetric filter.  Each lifting
     step may be described in terms of 3 line buffers, which we call `state',
     `new_state' and `augend'.  `new_state' holds the later of the two inputs
     to the lifting step's filter and `augend' holds the samples which will
     be updated by the lifting step.  To perform the lifting step and reshuffle
     line buffers in preparation for the next synthesis step, the following
     simple agorithm suffices:
           augend <- augend - lambda*(state+new_state)
           tmp <- state; state <- new_state; new_state <- augend; augend <- tmp
     Once all lifting steps have been performed, `augend' and `new_state' hold
     two newly synthesized rows.
        Symmetric extension at boundaries is remarkably simple within this
     framework.  It may be shown that symmetric extension  of the original
     signals is always equivalent to zero-order hold within the
     lifting network; this is true only because the lifting network involves
     two-tap symmetric lifting filters.  This allows us to manage boundaries
     in the following way: if `augend' is missing (i.e. `augend.is_active'
     returns false) at some point (due to its notional line number being beyond
     the boudaries of the current tile-component resolution), the lifting
     step produces no result, meaning that `augend' is empty on output
     (however, buffer shuffling is performed in the usual way); if either
     `new_state' or `state' is missing, the one line buffer is used for both
     inputs to the two-tap filter.  It should not be possible for `augend' to
     be non-empty while both `state' and `new_state' are empty. This simple
     policy causes emptiness to propagate through the lifting network, which
     effectively realizes the boundary extension policy.
        `y_out_next' holds the coordinate (true canvas coordinates) of the
     next output row to be requested via the `pull' function.  The last valid
     output row has coordinate `y_out_max'.
        `y_in_next' holds the coordinate of the next vertically low- or
     high-pass row to be supplied to the vertical synthesis engine.  The value
     is even if the next row is a vertically low-pass row; otherwise, it is
     odd.  The last valid input row has coordinate `y_in_max'.  The reason
     for maintaining separate ranges for the input and output line numbers is
     to accommodate a user-specified region of interest -- the problem does
     not arise during DWT analysis, where the entire image must be compressed.
     When the user has specified a region of interest, the range of input
     rows is generally a superset of the range of output rows, to acommodate
     the spatial spreading effects of subband synthesis.
        The `x_in_*' and `x_out_*' member variables have a similar
     interpretation to their vertical cousins.
        To minimize memory fragmentation, all line buffers used by this
     object are allocated together from adjacent locations in a single memory
     block provided by the `kdu_sample_allocator' object.  The memory block
     is sized during construction and actual allocation is completed in the
     first `pull' call, which is identified by the `initialized' flag being
     false. */

class kd_bg_synthesis : public kdu_pull_ifc_base {
  public: // Member functions
    kd_bg_synthesis(kdu_resolution resolution, kdu_sample_allocator *allocator,
                 bool use_shorts, float normalization,kde_flow_control* param_flow,
				 bbgm_io& bgm_interface_,kdu_pull_ifc* param_synthesis);
  protected: // These functions implement their namesakes in the base class
    virtual ~kd_bg_synthesis();
    virtual void pull(kdu_line_buf &line, bool allow_exchange);
  protected: // Internal implementation functions
    void perform_vertical_lifting_step(kd_lifting_step *step);
      /* This function implements a single vertical lifting step, using the
         three lines identified by `step->state', `augend' and `new_state'.
         For an explanation of the algorithm and the interpretation of these
         three quantities, see the notes below. */
    void horizontal_synthesis(kd_line_cosets &line);
      /* This function is responsible for loading subband samples and
         horizontally synthesizing a single line.  The relevant line and
         hence the relevant subbands are determined by the value of the
         state variable, `y_in_next', which the function increments before
         returning.  If `y_in_next' >= `y_in_lim', the function calls
         `line.destroy()'.  This has the dual effect of freeing storage
         as soon as possible and also signalling the fact that the symmetric
         extension mechanism should be used to obtain the line.  Otherwise,
         if `line' is already empty, it is allocated storage inside this
         function.  Upon completion, the line's `lnum' field is updated to
         reflect the value of `y_in_next' on entry. */
  protected: // Data
    kdu_pull_ifc hor_low[2];  // Interfaces for acquiring LL and LH samples
    kdu_pull_ifc hor_high[2]; // Interfaces for acquiring HL and HH samples
    int L_max,level; // Max filter length is 2L_max+1, but also `L_max' = num steps
	kdu_pull_ifc **hor_high_decoders, **hor_low_decoders;
	kdu_pull_ifc* param_synth_network;
	kdu_line_buf lo_line,hi_line;
	bbgm_io& bgm_interface;
    bool reversible,isLast;
    bool use_shorts; // All `pull' calls will request 16-bit samples
    bool initialized; // True after the first `pull' call.
    int normalizing_upshift; // Amount to post-shift irreversible outputs.
    kd_lifting_step steps[4]; // Array of L_max lifting steps in analysis order
    kd_line_cosets augend; // Augend for the next vertical synthesis step
    kd_line_cosets new_state; // Will become the next vertical step's state.
    int y_out_next, y_out_max;
    int y_in_next, y_in_max;
    bool unit_height; // Sequences of length 1 must be processed differently.
    int x_out_min, x_out_max;
    int x_in_min, x_in_max;
    int low_width, high_width; // Width of horizontal low- and high-pass bands
	int low_height,high_height;
	kde_flow_control* param_flow;
    bool unit_width;
    bool empty; // True only if the resolution has no area.
	int num_comps;
  };
class bgm_LL_synthesis : public kdu_pull_ifc_base {
  public: // Member functions
    bgm_LL_synthesis(kdu_subband subband, kdu_sample_allocator *allocator,
                 kde_flow_control* param_flow,
				 bbgm_io& bgm_interface_,kdu_pull_ifc* param_synthesis);
  protected: // These functions implement their namesakes in the base class
    virtual ~bgm_LL_synthesis();
    virtual void pull(kdu_line_buf &line, bool allow_exchange);
  protected: // Internal implementation functions
    
  protected: // Data
    kdu_pull_ifc hor_low;  // Interfaces for acquiring LL and LH samples
    int L_max,level; // Max filter length is 2L_max+1, but also `L_max' = num steps
	kdu_pull_ifc *hor_low_decoders;
	kdu_line_buf lo_line,hi_line;
	bbgm_io& bgm_interface;
    bool reversible,isLast,initialized;
    int y_out_next, y_out_max;
    int y_in_next, y_in_max;
    int x_out_min, x_out_max;
    int x_in_min, x_in_max;
    int low_width, high_width; // Width of horizontal low- and high-pass bands
	int low_height,high_height;
	kde_flow_control* param_flow;
    
  };
#endif // SYNTHESIS_LOCAL_H
