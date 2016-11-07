/*****************************************************************************/
// File: kdu_kernels.h [scope = CORESYS/COMMON]
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
   Defines services for deriving important properties of DWT kernels from
the relevant identifier.  The implementation of the "kdu_kernels" class may
easily be extended to any odd-length symmetric DWT kernels without affecting
the way in which the derived information is accessed and interpreted by the
rest of the system.
******************************************************************************/

#ifndef KDU_KERNELS_H
#define KDU_KERNELS_H

#include <assert.h>

class kdu_kernels;

/*****************************************************************************/
/*                               kdu_kernel_type                             */
/*****************************************************************************/

enum kdu_kernel_type {
    KDU_ANALYSIS_LOW,
    KDU_ANALYSIS_HIGH,
    KDU_SYNTHESIS_LOW,
    KDU_SYNTHESIS_HIGH
  };

/*****************************************************************************/
/*                                 kdu_kernels                               */
/*****************************************************************************/

class kdu_kernels {
  /* Service object which provides various useful summary quantities
     concerning the DWT and its kernels. The object currently accepts
     only the two kernels defined by Part 1 of the JPEG2000 standard, but
     may be trivially extended to work with any odd-length symmetric DWT
     kernels. */
  public: // Member functions
    kdu_kernels(int kernel_id, bool reversible);
      /* `kernel_id' must be one of "Ckernels_W5X3" or "Ckernels_W9X7".
         The `reversible' flag only affects subband normalization gains,
         which are both set to 0 if `reversible' is true -- see below. */
    ~kdu_kernels();
    int get_id() { return kernel_id; }
      /* Returns one of "Ckernels_W5X3" or "Ckernels_W9X7". */
    float *
      get_lifting_factors(int &num_steps, float &dc_scale, float &nyq_scale);
      /* Returns a complete lifting factorization of the kernels.  Since the
         kernels are symmetric, with odd least dissimilar lengths (lengths
         differ by 2), the lifting steps each involve symmetric 2-tap filters,
         characterized by a single factor.  The function returns the value
         of this single factor for each successive lifting step in the
         forward (analysis) transform.  For the inverse transform, the steps
         are applied in reverse order and the factors negated.  The number
         of lifting steps is returned via the `num_steps' argument,
         while the factors are contained in the returned array.
            The `low_scale' and `high_scale' arguments are used to return the
         factors by which the low-pass and high-pass subband samples must
         be scaled after all lifting steps are complete.  For reversible
         transforms, these are guaranteed to be both equal to 1.0.  For
         irreversible transforms, the scale factors are computed to ensure
         that the DC gain of the low-pass analysis kernels and the Nyquist
         gain of the high-pass analysis kernels are both equal to 1. */
    bool get_lifting_downshift(int step_idx, int &downshift)
      { assert((step_idx >= 0) && (step_idx < num_steps));
        if (downshifts == NULL)
          return false;
        downshift = downshifts[step_idx]; return true; }
      /* Returns false if the transform is irreversible. Otherwise,
         returns true and sets the `downshift' argument to the smallest
         non-negative integer such that the lifting factor for the
         indicated lifting step is an integer multiple of 2^{-`downshift'}. */
    float *get_impulse_response(kdu_kernel_type which, int &half_length);
      /* Returns the equivalent linearized impulse responses of the symmetric
         odd length DWT kernels.  The `which' argument indicates which
         impulse response is required (analysis or synthesis, low- or
         high- pass, as defined for `kdu_kernel_type'.).
            The `half_length' argument is used to return the value of the
         length parameter, L, such that the relevant impulse response has
         length 2L+1.  The returned array contains L+1 filter taps, starting
         with the central tap. The other taps may be deduced, if necessary,
         from the symmetry properties. */
    double get_energy_gain(kdu_kernel_type which, int level_idx);
      /* Returns the energy gain factor associated with the one dimensional
         synthesis basis vectors (sequences) for samples in the
         low- or high-pass subband at the indicated DWT level.  The `which'
         argument must be one of KDU_SYNTHESIS_LOW or KDU_SYNTHESIS_HIGH.
            A `level_idx' of 0 refers to no transform at all, in which
         case the function returns 1.0 for low-pass and 0.0 for high-pass
         kernels.
            The function does not explicitly compute the analysis or
         synthesis vectors for more than a certain number of DWT levels,
         beyond which point it relies upon the fact that the vectors converge
         to samplings of underlying continuous wavelet and scaling functions.
         This allows a large number of DWT levels to be specified without
         consuming inordinate amounts of memory calculating the kernels.  For
         an explanation of these convergence properties, see Section 17.3 of
         the book by Taubman and Marcellin.
            To determine the energy gain for 2D subbands, the 1D energy gains
         for each direction may be multiplied. */
    double *get_bibo_gains(int level_idx, double &low_gain, double &high_gain);
      /* Returns the BIBO (Bounded-Input-Bounded-Output) analysis gain from
         the input 1D transform's input sequence to the various subbands
         and intermediate lifting step results produced at the output.  The
         `low_gain' and `high_gain' arguments are used to return the BIBO
         gain from the input sequence to the low- and high-pass subbands,
         respectively, at DWT level `level_idx'.  The function returns
         a pointer to an array with one entry per lifting step, whose
         entries hold the BIBO gain from the input sequence through to the
         output of that lifting step in the indicated DWT level.  A `level_idx'
         value of 0 refers to no transform at all, in which case the function
         returns a NULL pointer, setting `low_gain' to 1.0 and `high_gain'
         to 0.0.
            The BIBO gain of a linear system is the ratio between the
         maximum absolute value of the output sequence and the maximum
         absolute value of the input sequence.  As such the BIBO gain figures
         are useful for determining the optimal placement of the binary
         point in fixed point DWT implementations.
            We note here that beyond a certain number of DWT levels, the
         function does not explicitly calculate the analysis vectors and
         their BIBO gains.  Instead, it relies upon the fact that the analysis
         vectors converge to samplings of the dual wavelet and scaling
         functions which are continuous.  This enables it to accurately
         predict gains for very deep DWT's without expending substantial
         memory resources.  Again, for a discussion of these properties,
         see Section 17.3 of the book by Taubman and Marcellin. */
  private: // Data
    int kernel_id;
    int num_steps;
    float *lifting_factors;
    int *downshifts; // NULL for irreversible transforms.
    float low_scale, high_scale;
    int low_analysis_L, high_analysis_L; // Half lengths.
    float *low_analysis_taps, *high_analysis_taps; // 2-sided arrays
    int low_synthesis_L, high_synthesis_L; // Half lengths.
    float *low_synthesis_taps, *high_synthesis_taps; // 2-sided arrays
  private: // More data fields; these are used for vector expansion.
    int max_expansion_levels; // max levels to expand analysis/synth vectors
    int work_L; // Large enough to hold expanded analysis or synthesis vectors
    float *work1, *work2; // Both 2-sided arrays from -`work_L' to +`work_L'
    double *bibo_step_gains; // Array with `num_steps' entries.
  };

#endif // KDU_KERNELS_H
