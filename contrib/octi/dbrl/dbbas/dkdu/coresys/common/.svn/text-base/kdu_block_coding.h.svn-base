/*****************************************************************************/
// File: kdu_block_coding.h [scope = CORESYS/COMMON]
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
   Uniform interface to embedded block coding (encoding and decoding) services.
******************************************************************************/

#ifndef KDU_BLOCK_CODING_H
#define KDU_BLOCK_CODING_H

#include <assert.h>
#include "kdu_compressed.h"

// Defined here:

class kdu_block_encoder_base;
class kdu_block_decoder_base;
class kdu_block_encoder;
class kdu_block_decoder;

/* ========================================================================= */
/*                      Class and Structure Definitions                      */
/* ========================================================================= */

/*****************************************************************************/
/*                          kdu_block_encoder_base                           */
/*****************************************************************************/

class kdu_block_encoder_base {
  /* Acts as an abstract base class for the hidden `state' object associated
     with the public `kdu_block_encoder' object. */
  protected:
    friend class kdu_block_encoder;
    virtual ~kdu_block_encoder_base() { return; }
    virtual void encode(kdu_block *block, bool reversible, double msb_wmse,
                        kdu_uint16 estimated_slope_threshold) = 0;
  };

/*****************************************************************************/
/*                            kdu_block_encoder                              */
/*****************************************************************************/

class kdu_block_encoder {
  public: // Member functions
    KDU_EXPORT
      kdu_block_encoder();
    ~kdu_block_encoder() { delete state; }
    void encode(kdu_block *block, bool reversible=false,
                double msb_wmse=0.0F, kdu_uint16 estimated_slope_threshold=0)
      { /* Encodes a single block of samples.
              On entry, `block->num_passes' indicates the number of coding
           passes which are to be processed and `block->missing_msbs' the
           number of most significant bit-planes which are known to be zero.
           The function should process all coding passes, unless this
           is not possible given the available implementation precision, or
           a non-zero `estimated_slope_threshold' argument allows it to
           determine that some passes can be skipped (see below).
              The samples must be in the `block->samples' buffer, organized
           in raster scan order.  The sample values themselves are expected
           to have a sign-magnitude representation, with the most significant
           magnitude bit-plane appearing in bit position 30 and the sign (1 for
           -ve) in bit position 31.
              On exit, the `block->byte_buffer' and `block->pass_lengths'
           arrays should be filled out, although note that the `num_passes'
           value may have been reduced, if the function was able to determine
           that some passes would almost certainly be discarded during rate
           allocation later on (only if `estimated_slope_threshold' != 0).
              If `msb_wmse' is non-zero, the block processor is expected to
           generate distortion-length slope information and perform a convex
           hull analysis, writing the results to the `block->pass_slopes'
           array.  Otherwise, the `block->pass_slopes' array's contents will
           not be touched -- no assumption is made concerning their contents.
              The `reversible' argument is irrelevant unless distortion-length
           slopes are to be estimated here, in which case `msb_wmse' must
           also be non-zero.  Whether the subband sample indices represent
           reversibly transformed image data or irreversibly transformed and
           quantized image data has a subtle impact on the generation of
           rate-distortion information.
              If non-zero, the `estimated_slope_threshold' argument indicates
           an expected lower bound on the distortion-length slope threshold
           which is likely to be selected by the PCRD-opt rate control
           algorithm (the logarithmic representation is identical to that
           associated with the `block->pass_slopes' array).  This enables
           some coding passes which are highly unlikely to be included in the
           final compressed representation to be skipped, thereby saving
           processing time.  The capability is available only if `msb_wmse'
           is also non-zero so that distortion-length slope values are to
           be estimated. */
        state->encode(block,reversible,msb_wmse,estimated_slope_threshold);
      }
  private: // Data
    kdu_block_encoder_base *state;
  };

/*****************************************************************************/
/*                          kdu_block_decoder_base                           */
/*****************************************************************************/

class kdu_block_decoder_base {
  /* Acts as an abstract base class for the hidden `state' object associated
     with the public `kdu_block_decoder' object. */
  protected:
    friend class kdu_block_decoder;
    virtual ~kdu_block_decoder_base()
      { return; }
    virtual void decode(kdu_block *block) = 0;
  };

/*****************************************************************************/
/*                            kdu_block_decoder                              */
/*****************************************************************************/

class kdu_block_decoder {
  public: // Member functions
    KDU_EXPORT
      kdu_block_decoder();
    ~kdu_block_decoder() { delete state; }
    void decode(kdu_block *block)
      { /* Decodes a single block of samples.  The dimensions of the block
           are in `block->size' -- none of the geometric transformation
           flags in `block->size' have any meaning here.
              On entry, `block->num_passes' indicates the number of coding
           passes which are to be decoded and `block->missing_msbs' the
           number of most significant bit-planes which are to be skipped.
           The function processes all coding passes, unless unable to do so
           for reasons of available implementation precision limitations, or
           an error was detected by error resilience mechanisms and corrected
           by discarding passes believed to be erroneous.
              On exit, the decoded samples are in the `block->sample_buffer'
           array, organized in scan-line order.  The sample values themselves
           have a sign-magnitude representation, with the most significant
           magnitude bit-plane appearing in bit position 30 and the sign
           (1 for -ve) in bit position 31.  At each sample location, if p is
           the index of the least significant decoded magnitude bit and the
           sample is non-zero, the function sets bit p-1.  This has the effect
           of both signalling the location of the least significant decoded
           magnitude bit-plane and also implementing the default mid-point
           rounding rule for dequantization.
              The value of `K_max_prime' identifies the maximum number of
           magnitude bit-planes (including missing MSB's) which could have
           been coded.  Knowledge of this value allows the function to
           determine whether or not all coding passes have been decoded,
           without truncation -- this in turn, affects the behaviour of
           the error resilience mechanisms. */
        state->decode(block);
      }
  private: // Data
    kdu_block_decoder_base *state;
  };

#endif // KDU_BLOCK_CODING_H
