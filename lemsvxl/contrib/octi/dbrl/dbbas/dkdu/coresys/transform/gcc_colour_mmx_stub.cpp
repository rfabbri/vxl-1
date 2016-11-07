/*****************************************************************************/
// File: gcc_colour_mmx_stub.cpp [scope = CORESYS/TRANSFORMS]
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
code actually appears in "gcc_colour_mmx.s".  You should never actually
include this file in a build. It was used to generate "gcc_colour_mmx.s"
by compiling with gcc -S (stops before assembling the output). The resulting
function stubs are then simply filled out with the relevant assembly
instructions: these are AT&T permutations of the instructions found in
"msvc_dwt_mmx_local.h".
******************************************************************************/

/* ========================================================================= */
/*                            Test for MMX Support                           */
/* ========================================================================= */

/*****************************************************************************/
/* EXTERN                     gcc_colour_mmx_exists                          */
/*****************************************************************************/

int gcc_colour_mmx_exists()
  {
    int mmx_exists = 1;
    // Assembly code is inserted in here.
    return mmx_exists;
  }

/* ========================================================================= */
/*                        Now for the MMX functions                          */
/* ========================================================================= */

/*****************************************************************************/
/* EXTERN                      simd_inverse_ict                              */
/*****************************************************************************/

void
  simd_inverse_ict(short int *src1, short int *src2, short int *src3,
                   int samples)
{
  int quads = (samples+3)>>2;
  if (quads <= 0)
    return;
  short int q_offset_1[4]={1,1,1,1};
  short int q_offset_2[4]={2,2,2,2};
  short int q_CRfactR[4], q_CBfactB[4], q_CRfactG[4], q_CBfactG[4];
  q_CRfactR[0] = q_CRfactR[1] = q_CRfactR[2] = q_CRfactR[3] =
    (short int)(0.402 * (1<<16)); // Actual factor is 1.402
  q_CBfactB[0] = q_CBfactB[1] = q_CBfactB[2] = q_CBfactB[3] =
    (short int)(-0.228 * (1<<16)); // Actual factor is 1.772
  q_CRfactG[0] = q_CRfactG[1] = q_CRfactG[2] = q_CRfactG[3] =
    (short int)(0.285864 * (1<<16)); // Actual factor is -0.714136
  q_CBfactG[0] = q_CBfactG[1] = q_CBfactG[2] = q_CBfactG[3] =
    (short int)(-0.344136 * (1<<16)); // Actual factor is -0.344136

  // Assembly code is inserted in here.
}

/*****************************************************************************/
/* EXTERN                       simd_inverse_rct                             */
/*****************************************************************************/

void
  simd_inverse_rct(short int *src1, short int *src2, short int *src3,
                   int samples)
{
  int quads = (samples+3)>>2;
  if (quads <= 0)
    return;

  // Assembly code is inserted in here.
}
