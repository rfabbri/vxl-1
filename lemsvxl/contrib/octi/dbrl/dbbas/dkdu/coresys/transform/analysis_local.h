/*****************************************************************************/
// File: analysis_local.h [scope = CORESYS/TRANSFORMS]
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
   Local definitions used by "analysis.cpp".  These are not to be included
from any other scope.
******************************************************************************/

#ifndef ANALYSIS_LOCAL_H
#define ANALYSIS_LOCAL_H

#include <assert.h>
#include "kdu_compressed.h"
#include "kdu_sample_processing.h"
#include "transform_local.h"
#include "kdu_roi_processing.h"
#include "compress_local.h"
// Defined here

class kd_analysis;
class kdu_analysis;
/*****************************************************************************/
/*                                kd_analysis                                */
/*****************************************************************************/

class kd_analysis : public kdu_push_ifc_base {
  public: // Member functions
    kd_analysis(kdu_resolution resolution, kdu_sample_allocator *allocator,
                bool use_shorts, float normalization,
                kdu_roi_node *roi);
  protected: // These functions implement their namesakes in the base class
    virtual ~kd_analysis();
    virtual void push(kdu_line_buf &line, bool allow_exchange);
  private: // Internal implementation functions
    void perform_vertical_lifting_step(kd_lifting_step *step);
      /* This function implements a single vertical lifting step, using the
         three lines identified by `step->state', `augend' and `new_state'.
         For an explanation of the algorithm and the interpretation of these
         three quantities, see the notes below. */
    void horizontal_analysis(kd_line_cosets &line);
      /* Performs horizontal analysis, pushing the resulting horizontal subband
         lines on to their respective interfaces.  Uses line.lnum to determine
         whether the line belongs to a vertically low- or high-pass subband.
         Decrements the `output_rows_remaining' counter. */
  private: // Data
	friend class kd_bg_analysis;
    kdu_push_ifc hor_low[2];  // Interfaces for delivering LL and LH samples
    kdu_push_ifc hor_high[2]; // Interfaces for delivering HL and HH samples
    int L_max; // Max filter length is 2L_max+1, but also `L_max' = num steps
    bool reversible;
    bool use_shorts; // Using 16-bit (shorts) or 32-bit representations?
    bool initialized; // Final initialization happens in the first `push' call
    int normalizing_downshift; // Amount to pre-shift irreversible inputs.
    kd_lifting_step steps[4]; // Array of `L_max' lifting steps, in order.
    kd_line_cosets augend; // Holds augend for the next vertical synthesis step
    kd_line_cosets new_state; // Will become the next vertical step's state.
    int y_next, y_max;
    int output_rows_remaining; // Number of vertical subband rows left to make
    bool unit_height; // Sequences of length 1 must be processed differently.
    int x_min, x_max;
    int low_width, high_width; // Width of horizontal low- and high-pass bands
    bool bgm;
	bool unit_width;
    bool empty; // True only if the resolution has no area.
    kdu_roi_level roi_level; // Keep this around only to allow destruction
                             // of ROI processing objects when done.
  };
  /* Notes:
     This object is responsible for performing analysis operations for a
     single 2D DWT stage.  Vertical analysis is performed first, followed
     by horizontal analysis.  The implementation is incremental and aims to
     minimize memory consumption.  New lines of samples for the relevant
     resolution level are pushed one by one via the `push' function.  This
     function invokes the vertical analysis logic, which in turn pushes any
     newly generated vertical subband lines to the `horizontal_analysis'
     function and thence on to the `hor_low' and `hor_high' interfaces.
        Vertical analysis is implemented in a remarkably simple and efficient
     manner.  The implementation is based around the lifting network
     depicted in Figure 17.10 of the book by Taubman and Marcellin. This
     implementation structure may be used to realize any symmetric
     DWT kernel having filters with odd lengths differing by 2 (it is, in fact,
     a canonical factorization for the class of two channel FIR PR subband
     transforms, having linear phase filters with least dissimilar odd
     lengths -- low- and high-pass filter lengths differ by exactly 2).  A
     more thorough explanation of the lifting state machine may be found in
     Section 6.4.4 of the book by Taubman and Marcellin, while the
     implications for boundary extension are described in Section 6.5.3 of
     the same text.
        The vertical analysis implementation assigns a single state (line) to
     each lifting step, which represents the earlier of the two inputs to the
     lifting step's two tap, symmetric filter.  Each lifting step may be
     described in terms of 3 line buffers, which we call `state', `new_state'
     and `augend'.  `new_state' holds the later of the two inputs to the
     lifting step's filter and `augend' holds the samples which will be updated
     by the lifting step.  To perform the lifting step and reshuffle line
     buffers in preparation for the next analysis step, the following simple
     agorithm suffices:
           augend <- augend + lambda*(state+new_state)
           tmp <- state; state <- new_state; new_state <- augend; augend <- tmp
     Once all lifting steps have been performed, `augend' and `new_state' hold
     two new vertical subband rows.  Each row maintains its own line number so
     we can trivially determine which new subband row is low-pass and which is
     high-pass based on the parity of the line number.
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
        `y_next' holds the coordinate (true canvas coordinates) of the
     next input row to be supplied via the `push' function.  The last valid
     input row has coordinate `y_max'.
        'x_min' and `x_max' identify the true canvas coordinates of the first
     and last sample in each input line.
        To minimize memory fragmentation, all line buffers used by this
     object are allocated together from adjacent locations in a single memory
     block provided by the `kdu_sample_allocator' object.  The memory block
     is sized during construction and actual allocation is completed in the
     first `push' call, which is identified by the `initialized' flag being
     false. */
class kd_bg_analysis : public kdu_push_ifc_base {
  public: // Member functions
    kd_bg_analysis(kdu_resolution resolution, kdu_sample_allocator *allocator,
                bool use_shorts, float normalization,
                kdu_roi_node *roi,kdc_flow_control* param_flow_,
				bbgm_io& bgm_interface_,kdu_push_ifc* param_analysis);
  protected: // These functions implement their namesakes in the base class
    virtual ~kd_bg_analysis();
    virtual void push(kdu_line_buf &line, bool allow_exchange);
  private: // Internal implementation functions
    void perform_vertical_lifting_step(kd_lifting_step *step);
      /* This function implements a single vertical lifting step, using the
         three lines identified by `step->state', `augend' and `new_state'.
         For an explanation of the algorithm and the interpretation of these
         three quantities, see the notes below. */
    void horizontal_analysis(kd_line_cosets &line);
      /* Performs horizontal analysis, pushing the resulting horizontal subband
         lines on to their respective interfaces.  Uses line.lnum to determine
         whether the line belongs to a vertically low- or high-pass subband.
         Decrements the `output_rows_remaining' counter. */
  private: // Data
    kdu_push_ifc hor_low[2];  // Interfaces for delivering LL and LH samples
    kdu_push_ifc hor_high[2]; // Interfaces for delivering HL and HH samples
    kdu_push_ifc **hor_high_encoders, **hor_low_encoders;
	kdu_push_ifc* param_ana_network;
	kdu_line_buf lo_line,hi_line;
	bbgm_io& bgm_interface;

	int L_max,level; // Max filter length is 2L_max+1, but also `L_max' = num steps
    bool reversible;
	bool isLast;
    bool use_shorts; // Using 16-bit (shorts) or 32-bit representations?
    bool initialized; // Final initialization happens in the first `push' call
    int normalizing_downshift; // Amount to pre-shift irreversible inputs.
    kd_lifting_step steps[4]; // Array of `L_max' lifting steps, in order.
    kd_line_cosets augend; // Holds augend for the next vertical synthesis step
    kd_line_cosets new_state; // Will become the next vertical step's state.
    int y_next, y_max;
    int output_rows_remaining; // Number of vertical subband rows left to make
    bool unit_height; // Sequences of length 1 must be processed differently.
    int x_min, x_max;
    int low_width, high_width; // Width of horizontal low- and high-pass bands
    int low_height,high_height;
	kdc_flow_control* param_flow;
	bool unit_width;
    bool empty; // True only if the resolution has no area.
    kdu_roi_level roi_level; // Keep this around only to allow destruction
                             // of ROI processing objects when done.
  };
#endif // ANALYSIS_LOCAL_H
