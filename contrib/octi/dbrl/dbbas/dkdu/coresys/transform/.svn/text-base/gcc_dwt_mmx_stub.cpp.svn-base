/*****************************************************************************/
// File: gcc_dwt_mmx_stub.cpp [scope = CORESYS/TRANSFORMS]
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
   This file contains stubs of all the relevant functions whose assembler
code actually appears in "gcc_dwt_mmx.s".  You should never actually
include this file in a build. It was used to generate "gcc_dwt_mmx.s"
by compiling with gcc -S (stops before assembling the output). The resulting
function stubs are then simply filled out with the relevant assembly
instructions: these are AT&T permutations of the instructions found in
"msvc_dwt_mmx_local.h".
******************************************************************************/

/* ========================================================================= */
/*                            Test for MMX Support                           */
/* ========================================================================= */

/*****************************************************************************/
/* EXTERN                      gcc_dwt_mmx_exists                            */
/*****************************************************************************/

int gcc_dwt_mmx_exists()
  {
    int mmx_exists = 1;
    // Assembly code is inserted here.
    return mmx_exists;
  }

/* ========================================================================= */
/*                        Now for the MMX functions                          */
/* ========================================================================= */

/*****************************************************************************/
/* EXTERN                    simd_irrev_v_synth                              */
/*****************************************************************************/

void
  simd_irrev_v_synth(short int *src1, short int *src2, short int *dst,
                     int samples, int i_lambda, short int remainder,
                     short int pre_offset)
{
  int quads = (samples+3)>>2;
  if (quads <= 0)
    return;
  short int q_lambda[4], q_offset[4];
  q_lambda[0]=q_lambda[1]=q_lambda[2]=q_lambda[3] = remainder;
  q_offset[0]=q_offset[1]=q_offset[2]=q_offset[3] = pre_offset;
  if (i_lambda == -2)
    { // Integer part of lifting step factor is -2.
      // The actual lifting factor here should be -1.586134
      src1=src2; src2=dst; // Replace these with actual assembly code
    }
  else if ((i_lambda == 0) && (remainder < 0))
    { // Integer part of lifting step factor is 0 and factor is negative.
      // The actual lifting factor here should be -0.05298.  This is the one
      // which is sensitive to overflow/underflow in the initial accumulation.
      q_lambda[0] = q_lambda[1] = q_lambda[2] = q_lambda[3] = q_lambda[0]<<3;
      q_offset[0]=q_offset[1]=q_offset[2]=q_offset[3] = 4; // A post-offset

      src1=src2; src2=dst; // Replace these with actual assembly code
    }
  else if (i_lambda == 0)
    { // Integer part of lifting step factor is 0 and factor is positive.
      // The actual lifting factor here should be 0.443507

      src1=src2; src2=dst; // Replace these with actual assembly code
    }
  else if (i_lambda == 1)
    { // Integer part of lifting step factor is 1.
      // The actual lifting factor here should be 0.882911
      src1=src2; src2=dst; // Replace these with actual assembly code
    }
}

/*****************************************************************************/
/* EXTERN                      simd_rev_v_synth                              */
/*****************************************************************************/

void
  simd_rev_v_synth(short int *src1, short int *src2, short int *dst,
                   int samples, int downshift, int i_lambda)
{
  int quads = (samples+3)>>2;
  if (quads <= 0)
    return;
  short int q_offset[4];
  q_offset[0] = q_offset[1] = q_offset[2] = q_offset[3] =
    (short int)((1<<downshift)>>1);

  if (i_lambda == 1)
    {
      src1=src2; src2=dst; // Replace these with actual assembly code
    }
  else if (i_lambda == -1)
    {
      src1=src2; src2=dst; // Replace these with actual assembly code
    }
}

/*****************************************************************************/
/* EXTERN                    simd_irrev_h_synth                              */
/*****************************************************************************/

void
  simd_irrev_h_synth(short int *src, short int *dst,
                     int samples, int i_lambda, short int remainder,
                     short int pre_offset)
  /* The current implementation supports only the CDF 9/7 transform.  For
     an explanation, see the comments appearing with `mmx_irrev_v_synth'. */
{
  int quads = (samples+3)>>2;
  if (quads <= 0)
    return;
  short int q_lambda[4], q_offset[4];
  q_lambda[0]=q_lambda[1]=q_lambda[2]=q_lambda[3] = remainder;
  q_offset[0]=q_offset[1]=q_offset[2]=q_offset[3] = pre_offset;
  if (i_lambda == -2)
    { // Integer part of lifting step factor is -2.
      // The actual lifting factor here should be -1.586134
      src=dst; // Replace these with actual assembly code
    }
  else if ((i_lambda == 0) && (remainder < 0))
    { // Integer part of lifting step factor is 0 and factor is negative.
      // The actual lifting factor here should be -0.05298.  This is the one
      // which is sensitive to overflow/underflow in the initial accumulation.
      q_lambda[0] = q_lambda[1] = q_lambda[2] = q_lambda[3] = q_lambda[0]<<3;
      q_offset[0]=q_offset[1]=q_offset[2]=q_offset[3] = 4; // A post-offset

      src=dst; // Replace these with actual assembly code
    }
  else if (i_lambda == 0)
    { // Integer part of lifting step factor is 0 and factor is positive.
      // The actual lifting factor here should be 0.443507
      src=dst; // Replace these with actual assembly code
    }
  else if (i_lambda == 1)
    { // Integer part of lifting step factor is 1.
      // The actual lifting factor here should be 0.882911
      src=dst; // Replace these with actual assembly code
    }
}

/*****************************************************************************/
/* EXTERN                     simd_rev_h_synth                               */
/*****************************************************************************/

void
  simd_rev_h_synth(short int *src, short int *dst,
                   int samples, int downshift, int i_lambda)
{
  int quads = (samples+3)>>2;
  if (quads <= 0)
    return;
  short int q_offset[4];
  q_offset[0] = q_offset[1] = q_offset[2] = q_offset[3] =
    (short int)((1<<downshift)>>1);

  if (i_lambda == 1)
    {
      src=dst; // Replace these with actual assembly code
    }
  else if (i_lambda == -1)
    {
      src=dst; // Replace these with actual assembly code
    }
}

/*****************************************************************************/
/* EXTERN                        simd_interleave                             */
/*****************************************************************************/

void
  simd_interleave(short int *src1, short int *src2,
                  short int *dst, int pairs)
{
  int octets = (pairs+3)>>2;
  if (octets <= 0)
    return;
  src1 = src2; src2 = dst; // Replace these with actual assembly code
}

/*****************************************************************************/
/* EXTERN                   simd_upshifted_interleave                        */
/*****************************************************************************/

void
  simd_upshifted_interleave(short int *src1, short int *src2,
                            short int *dst, int pairs, int upshift)
{
  int octets = (pairs+3)>>2;
  if (octets <= 0)
    return;
  src1 = src2; src2 = dst; // Replace these with actual assembly code
}
