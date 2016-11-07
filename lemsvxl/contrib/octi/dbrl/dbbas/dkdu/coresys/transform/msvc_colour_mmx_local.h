/*****************************************************************************/
// File: msvc_colour_mmx_local.h [scope = CORESYS/TRANSFORMS]
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
   Provides MMX implementations of the reverse colour transformations: both
reversible (RCT) and irreversible (ICT = RGB to YCbCr).
******************************************************************************/

#ifndef MSVC_COLOUR_MMX_LOCAL_H
#define MSVC_COLOUR_MMX_LOCAL_H

/* ========================================================================= */
/*                            Test for MMX Support                           */
/* ========================================================================= */

/*****************************************************************************/
/* STATIC                    msvc_colour_mmx_exists                          */
/*****************************************************************************/

static int msvc_colour_mmx_exists()
  {
    int mmx_exists = 1;
    __asm
      {
        MOV EAX,1
        CPUID
        TEST EDX,00800000h
        JNZ mmx_exists_label
        MOV mmx_exists,0
mmx_exists_label:
      }
    return mmx_exists;
  }

/* ========================================================================= */
/*                        Now for the MMX functions                          */
/* ========================================================================= */

/*****************************************************************************/
/* INLINE                      simd_inverse_ict                              */
/*****************************************************************************/

inline void
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
  __asm
    {
      MOV EDX,quads      // Set up counter used for looping
      MOV EAX,src1
      MOV EBX,src2
      MOV ECX,src3
      MOVQ MM4,q_offset_1
      MOVQ MM5,q_offset_2
      MOVQ MM6,q_CRfactR
      MOVQ MM7,q_CBfactB
loop_inverse_ict:
      MOVQ MM0,[EAX] // Load luminance (Y)
      MOVQ MM1,[ECX] // Load chrominance (Cr)
      MOVQ MM2,MM1   // Prepare to form Red output in MM2
      PADDSW MM2,MM4 // +1 here similar to adding 2^15 before dividing by 2^16
      PMULHW MM2,MM6 // Multiply by 0.402*2^16 (CRfactR) and divide by 2^16
      PADDSW MM2,MM1 // Add Cr again to make the factor equivalent to 1.402
      PADDSW MM2,MM0 // Add in luminance to get Red
      MOVQ [EAX],MM2 // Save Red channel
      MOVQ MM2,MM1   // Prepare to form Cr*(-0.714136) in MM2 (will free MM1)
      PADDSW MM2,MM5 // +2 here similar to adding 2^15 before dividing by 2^16
      PMULHW MM2,q_CRfactG // Multiply by 0.285864*2^16 and divide by 2^16
      PSUBSW MM2,MM1 // Subtract Cr leaves us with the desired result
      PADDSW MM2,MM0 // Add Y to scaled Cr forms most of Green result in MM2
      MOVQ MM1,[EBX] // Load chrominance (Cb)
      MOVQ MM3,MM1   // Prepare to form Blue output in MM3
      PSUBSW MM3,MM5 // +2 here similar to adding 2^15 before dividing by 2^16
      PMULHW MM3,MM7 // Multiply by -0.228*2^16 (CBfactB) and divide by 2^16
      PADDSW MM3,MM1 // Gets 0.772*Cb to MM3
      PADDSW MM3,MM1 // Gets 1.772*Cb to MM3
      PADDSW MM3,MM0 // Add in luminance to get Blue
      MOVQ [ECX],MM3 // Save Blue channel
      PSUBSW MM1,MM5 // +2 here similar to adding 2^15 before dividing by 2^16
      PMULHW MM1,q_CBfactG // Multiply by -0.344136*2^16 and divide by 2^16
      PADDSW MM2,MM1 // Completes the Green channel in MM2
      MOVQ [EBX],MM2
      ADD EAX,8
      ADD EBX,8
      ADD ECX,8
      SUB EDX,1
      JNZ loop_inverse_ict
      EMMS // Clear MMX registers for use by FPU
    }
}

/*****************************************************************************/
/* INLINE                       simd_inverse_rct                             */
/*****************************************************************************/

inline void
  simd_inverse_rct(short int *src1, short int *src2, short int *src3,
                   int samples)
{
  int quads = (samples+3)>>2;
  if (quads <= 0)
    return;
  __asm
    {
      MOV EDX,quads      // Set up counter used for looping
      MOV EAX,src1
      MOV EBX,src2
      MOV ECX,src3
loop_inverse_rct:
      MOVQ MM1,[EBX] // Load chrominance (Db)
      MOVQ MM2,[ECX] // Load chrominance (Dr)
      MOVQ MM3,MM1
      PADDSW MM3,MM2
      PSRAW MM3,2 // Forms (Db+DR)>>2
      MOVQ MM0,[EAX] // Load luminance (Y).
      PSUBSW MM0,MM3 // Convert Y to Green channel
      MOVQ [EBX],MM0
      PADDSW MM2,MM0 // Convert Dr to Red channel
      MOVQ [EAX],MM2
      PADDSW MM1,MM0 // Convert Db to Blue channel
      MOVQ [ECX],MM1
      ADD EAX,8
      ADD EBX,8
      ADD ECX,8
      SUB EDX,1
      JNZ loop_inverse_rct
      EMMS // Clear MMX registers for use by FPU
    }
}

#endif // MSVC_COLOUR_MMX_LOCAL_H
