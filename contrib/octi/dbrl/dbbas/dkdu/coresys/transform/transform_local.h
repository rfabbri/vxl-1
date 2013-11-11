/*****************************************************************************/
// File: transform_local.h [scope = CORESYS/TRANSFORMS]
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
   Provides local definitions common to both the DWT analysis and the DWT
synthesis implementations in "analysis.cpp" and "synthesis.cpp".
******************************************************************************/

#ifndef TRANSFORM_LOCAL_H
#define TRANSFORM_LOCAL_H

#include <assert.h>
#include "kdu_messaging.h"
#include "kdu_sample_processing.h"

/*****************************************************************************/
/*                             kd_line_cosets                                */
/*****************************************************************************/

struct kd_line_cosets {
  public: // function members
    void pre_create(kdu_sample_allocator *allocator, int even_width,
                    int odd_width, bool absolute, bool use_shorts,bool bgm=false)
      { cosets[0].pre_create(allocator,even_width,absolute,use_shorts,bgm);
        cosets[1].pre_create(allocator,odd_width,absolute,use_shorts,bgm); }
    void create()
      { cosets[0].create(); cosets[1].create(); }
    void destroy()
      { cosets[0].destroy(); cosets[1].destroy(); }
    void activate()
      { cosets[0].activate(); cosets[1].activate(); }
    void deactivate()
      { cosets[0].deactivate(); cosets[1].deactivate(); }
    bool is_active()
      { return cosets[0].is_active(); }
    bool operator!()
      { return !cosets[0]; }
  public: // data members
    kdu_line_buf cosets[2];
    int lnum;
  };
  /* Notes:
     Instances of this structure represent a single line.  The absolute index
     of the line is stored in `lnum', while the data corresponding to the
     even and odd (low and high-pass) sample cosets of the line are stored in
     `cosets[0]' and `cosets[1]', respectively. */

/*****************************************************************************/
/*                         kd_fixpoint_lift_factor                           */
/*****************************************************************************/

struct kd_fixpoint_lift_factor {
    kdu_int32 fix_lambda; // holds round(`lambda'*2^16)
    int i_lambda; // holds round(`lambda')
    kdu_int16 remainder; // holds round((lambda-i_lambda)*2^16)
    kdu_int16 pre_offset; // holds round(2^15 / `remainder')
  };
  /* This structure represents a floating point lifting step factor in terms
     of integer quantities which are useful for fixed-point implementations,
     in which sample values all have 16-bit representations.
     The product, lambda*x, may be approximated in one of two ways:
          a) Using 32-bit integer arithmetic we may use the approximation
                   (fix_lambda*x + (1<<15)) >> 16
          b) Confining ourselves entirely to 16-bit arithmetic we may use
                   i_lambda*x + ((remainder*(x+pre_offset)) >> 16)
     The latter expression is particularly useful for MMX (and probably other
     SIMD instruction sets). */

/*****************************************************************************/
/*                             kd_lifting_step                               */
/*****************************************************************************/

struct kd_lifting_step {
    kd_line_cosets state; // Earlier of the two inputs for vertical step filter
    int augend_parity; // 0 if augend is an even indexed line/sample, else 1.
    float lambda; // Lifting step factor
    int i_lambda; // Integer version of lifting step factor
    int downshift; // `lambda' = `i_lambda'*2^{-`downshift'}
    kd_fixpoint_lift_factor fixpoint; // Used only with irreversible xforms.
  };

#endif // TRANSFORM_LOCAL_H
