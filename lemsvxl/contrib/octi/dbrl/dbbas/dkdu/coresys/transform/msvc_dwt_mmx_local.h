/*****************************************************************************/
// File: msvc_dwt_mmx_local.h [scope = CORESYS/TRANSFORMS]
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
   Implements various critical functions for the inverse DWT using in-line
MMX assembler code.
******************************************************************************/

#ifndef MSVC_DWT_MMX_LOCAL_H
#define MSVC_DWT_MMX_LOCAL_H

/* ========================================================================= */
/*                            Test for MMX Support                           */
/* ========================================================================= */

/*****************************************************************************/
/* STATIC                     msvc_dwt_mmx_exists                            */
/*****************************************************************************/

static int msvc_dwt_mmx_exists()
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
/* INLINE                    simd_irrev_v_synth                              */
/*****************************************************************************/

inline void
  simd_irrev_v_synth(short int *src1, short int *src2, short int *dst,
                     int samples, int i_lambda, short int remainder,
                     short int pre_offset)
  /* Current implementation supports only the CDF 9/7 transform, although
     minor modifications would see it supporting other transforms.  The
     key difficulty in providing a generic implementation is that we
     must be careful to exploit the limited dynamic range capabilities
     offered by the 16x16 signed multiply (this is executed simultaneously
     on 4 samples at a time) where the least significant 16 bits of the
     result are summarily discarded, without any attempt at rounding.  As
     a result, multiplication factors must lie in the range -0.5 to 0.5 and
     larger factors must be synthesized from smaller factors and integers.
     Although it is most efficient to add the two inputs to each lifting
     step filter and then perform the multiplication, accumulating the result
     into the sequence which is being updated, this order of events may see
     occasional violations of the 16-bit fixed-point representation.  In
     particular, it can be shown that overflow or underflow is possible only
     when the outputs of the first lifting step are added together as part
     of the second lifting step.  To avoid this problem, this lifting step
     is performed differently: the multiplication (by a small factor) is
     performed first and the results are added together.  A more general
     implementation, not tailored to any particular transform kernels, would
     have to waste one extra bit of dynamic range allowing for the possibility
     of overflow/underflow in the accumulation step. */
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
      __asm
        {         
          MOV ECX,quads     // Set up counter used for  looping
          MOV EAX,src1
          MOV EBX,src2
          MOV EDX,dst
          MOVQ MM0,q_lambda
          MOVQ MM1,q_offset
loop_minus2:
          MOVQ MM2,[EAX]     // Start with source sample 1
          PADDSW MM2,[EBX]   // Add source sample 2
          MOVQ MM3,[EDX]
          PADDSW MM3,MM2     // Here is a -1 contribution
          PADDSW MM3,MM2     // Here is another -1 contribution
          PADDSW MM2,MM1     // Add pre-offset for rounding
          PMULHW MM2,MM0     // Multiply by lambda and discard 16 LSB's
          PSUBSW MM3,MM2     // Final contribution
          MOVQ [EDX],MM3
          ADD EAX,8
          ADD EBX,8
          ADD EDX,8
          SUB ECX,1
          JNZ loop_minus2
          EMMS               // Clear MMX registers for use by FPU
        }
    }
  else if ((i_lambda == 0) && (remainder < 0))
    { // Integer part of lifting step factor is 0 and factor is negative.
      // The actual lifting factor here should be -0.05298.  This is the one
      // which is sensitive to overflow/underflow in the initial accumulation.
      q_lambda[0] = q_lambda[1] = q_lambda[2] = q_lambda[3] = q_lambda[0]<<3;
      q_offset[0]=q_offset[1]=q_offset[2]=q_offset[3] = 4; // A post-offset
      __asm
        {         
          MOV ECX,quads     // Set up counter used for looping
          MOV EAX,src1
          MOV EBX,src2
          MOV EDX,dst
          MOVQ MM0,q_lambda
          MOVQ MM1,q_offset
loop_0neg:
          MOVQ MM2,[EAX]     // Start with source samples 1
          PMULHW MM2,MM0     // Multiply by lambda and discard 16 LSB's
          PXOR MM4,MM4
          PSUBSW MM4,[EBX]   // Load negated source samples 2
          PMULHW MM4,MM0     // Multiply by lambda and discard 16 LSB's
          MOVQ MM3,[EDX]
          PSUBSW MM2,MM4     // Accumulate non-negated scaled sources.
          PADDSW MM2,MM1     // Add post-offset for rounding
          PSRAW MM2,3        // Divide by 8 (we've been working with 8x lambda)
          PSUBSW MM3,MM2     // Update destination samples
          MOVQ [EDX],MM3
          ADD EAX,8
          ADD EBX,8
          ADD EDX,8
          SUB ECX,1
          JNZ loop_0neg
          EMMS               // Clear MMX registers for use by FPU
        }
    }
  else if (i_lambda == 0)
    { // Integer part of lifting step factor is 0 and factor is positive.
      // The actual lifting factor here should be 0.443507
      __asm
        {         
          MOV ECX,quads     // Set up counter used for looping
          MOV EAX,src1
          MOV EBX,src2
          MOV EDX,dst
          MOVQ MM0,q_lambda
          MOVQ MM1,q_offset
loop_0pos:
          MOVQ MM2,[EAX]     // Start with source sample 1
          PADDSW MM2,[EBX]   // Add source sample 2
          MOVQ MM3,[EDX]
          PADDSW MM2,MM1     // Add pre-offset for rounding
          PMULHW MM2,MM0     // Multiply by lambda and discard 16 LSB's
          PSUBSW MM3,MM2     // Final contribution
          MOVQ [EDX],MM3
          ADD EAX,8
          ADD EBX,8
          ADD EDX,8
          SUB ECX,1
          JNZ loop_0pos
          EMMS               // Clear MMX registers for use by FPU
        }
    }
  else if (i_lambda == 1)
    { // Integer part of lifting step factor is 1.
      // The actual lifting factor here should be 0.882911
      __asm
        {         
          MOV ECX,quads     // Set up counter used for looping
          MOV EAX,src1
          MOV EBX,src2
          MOV EDX,dst
          MOVQ MM0,q_lambda
          MOVQ MM1,q_offset
loop_plus1:
          MOVQ MM2,[EAX]     // Start with source sample 1
          PADDSW MM2,[EBX]   // Add source sample 2
          MOVQ MM3,[EDX]
          PSUBSW MM3,MM2     // Here is a +1 contribution
          PADDSW MM2,MM1     // Add pre-offset for rounding
          PMULHW MM2,MM0     // Multiply by lambda and discard 16 LSB's
          PSUBSW MM3,MM2     // Final contribution
          MOVQ [EDX],MM3
          ADD EAX,8
          ADD EBX,8
          ADD EDX,8
          SUB ECX,1
          JNZ loop_plus1
          EMMS               // Clear MMX registers for use by FPU
        }
    }
  else
    assert(0);
}

/*****************************************************************************/
/* INLINE                      simd_rev_v_synth                              */
/*****************************************************************************/

inline void
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
      __asm
        {
          MOV ECX,quads      // Set up counter used for looping
          MOV EAX,src1
          MOV EBX,src2
          MOV EDX,dst
          MOVQ MM0,q_offset
          MOVD MM1,downshift
loop_plus1:
          MOVQ MM2,MM0       // start with the offset
          PADDSW MM2,[EAX]   // add 1'st source sample
          PADDSW MM2,[EBX]   // add 2'nd source sample
          MOVQ MM3,[EDX]
          PSRAW MM2,MM1      // shift rigth by the `downshift' value
          PSUBSW MM3,MM2     // subtract from dest sample
          MOVQ [EDX],MM3
          ADD EAX,8
          ADD EBX,8
          ADD EDX,8
          SUB ECX,1
          JNZ loop_plus1
          EMMS // Clear MMX registers for use by FPU
        }
    }
  else if (i_lambda == -1)
    {
      __asm
        {
          MOV ECX,quads      // Set up counter used for looping
          MOV EAX,src1
          MOV EBX,src2
          MOV EDX,dst
          MOVQ MM0,q_offset
          MOVD MM1,downshift
loop_minus1:
          MOVQ MM2,MM0       // start with the offset
          PSUBSW MM2,[EAX]   // subtract 1'st source sample
          PSUBSW MM2,[EBX]   // subtract 2'nd source sample
          MOVQ MM3,[EDX]
          PSRAW MM2,MM1      // shift rigth by the `downshift' value
          PSUBSW MM3,MM2     // subtract from dest sample
          MOVQ [EDX],MM3
          ADD EAX,8
          ADD EBX,8
          ADD EDX,8
          SUB ECX,1
          JNZ loop_minus1
          EMMS // Clear MMX registers for use by FPU
        }
    }
  else
    assert(0);
}

/*****************************************************************************/
/* INLINE                    simd_irrev_h_synth                              */
/*****************************************************************************/

inline void
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
      __asm
        {         
          MOV ECX,quads     // Set up counter used for looping
          MOV EAX,src
          MOV EDX,dst
          MOVQ MM0,q_lambda
          MOVQ MM1,q_offset
loop_minus2:
          MOVQ MM2,[EAX];
          PADDSW MM2,[EAX+2]   // Add source sample 2
          MOVQ MM3,[EDX]
          PADDSW MM3,MM2     // Here is a -1 contribution
          PADDSW MM3,MM2     // Here is another -1 contribution
          PADDSW MM2,MM1     // Add pre-offset for rounding
          PMULHW MM2,MM0     // Multiply by lambda and discard 16 LSB's
          PSUBSW MM3,MM2     // Final contribution
          MOVQ [EDX],MM3
          ADD EAX,8
          ADD EDX,8
          SUB ECX,1
          JNZ loop_minus2
          EMMS               // Clear MMX registers for use by FPU
        }
    }
  else if ((i_lambda == 0) && (remainder < 0))
    { // Integer part of lifting step factor is 0 and factor is negative.
      // The actual lifting factor here should be -0.05298.  This is the one
      // which is sensitive to overflow/underflow in the initial accumulation.
      q_lambda[0] = q_lambda[1] = q_lambda[2] = q_lambda[3] = q_lambda[0]<<3;
      q_offset[0]=q_offset[1]=q_offset[2]=q_offset[3] = 4; // A post-offset
      __asm
        {         
          MOV ECX,quads     // Set up counter used for looping
          MOV EAX,src
          MOV EDX,dst
          MOVQ MM0,q_lambda
          MOVQ MM1,q_offset
loop_0neg:
          MOVQ MM2,[EAX];
          PMULHW MM2,MM0     // Multiply by lambda and discard 16 LSB's
          PXOR MM4,MM4
          PSUBSW MM4,[EAX+2] // Load negated source samples 2
          PMULHW MM4,MM0     // Multiply by lambda and discard 16 LSB's
          MOVQ MM3,[EDX]
          PSUBSW MM2,MM4     // Accumulate non-negated scaled sources.
          PADDSW MM2,MM1     // Add post-offset for rounding
          PSRAW MM2,3        // Divide by 8 (we've been working with 8x lambda)
          PSUBSW MM3,MM2     // Final contribution
          MOVQ [EDX],MM3
          ADD EAX,8
          ADD EDX,8
          SUB ECX,1
          JNZ loop_0neg
          EMMS               // Clear MMX registers for use by FPU
        }
    }
  else if (i_lambda == 0)
    { // Integer part of lifting step factor is 0 and factor is positive.
      // The actual lifting factor here should be 0.443507
      __asm
        {         
          MOV ECX,quads     // Set up counter used for looping
          MOV EAX,src
          MOV EDX,dst
          MOVQ MM0,q_lambda
          MOVQ MM1,q_offset
loop_0pos:
          MOVQ MM2,[EAX];
          PADDSW MM2,[EAX+2]   // Add source sample 2
          MOVQ MM3,[EDX]
          PADDSW MM2,MM1     // Add pre-offset for rounding
          PMULHW MM2,MM0     // Multiply by lambda and discard 16 LSB's
          PSUBSW MM3,MM2     // Final contribution
          MOVQ [EDX],MM3
          ADD EAX,8
          ADD EDX,8
          SUB ECX,1
          JNZ loop_0pos
          EMMS               // Clear MMX registers for use by FPU
        }
    }
  else if (i_lambda == 1)
    { // Integer part of lifting step factor is 1.
      // The actual lifting factor here should be 0.882911
      __asm
        {         
          MOV ECX,quads     // Set up counter used for looping
          MOV EAX,src
          MOV EDX,dst
          MOVQ MM0,q_lambda
          MOVQ MM1,q_offset
loop_plus1:
          MOVQ MM2,[EAX];
          PADDSW MM2,[EAX+2]   // Add source sample 2
          MOVQ MM3,[EDX]
          PSUBSW MM3,MM2     // Here is a +1 contribution
          PADDSW MM2,MM1     // Add pre-offset for rounding
          PMULHW MM2,MM0     // Multiply by lambda and discard 16 LSB's
          PSUBSW MM3,MM2     // Final contribution
          MOVQ [EDX],MM3
          ADD EAX,8
          ADD EDX,8
          SUB ECX,1
          JNZ loop_plus1
          EMMS               // Clear MMX registers for use by FPU
        }
    }
  else
    assert(0);
}

/*****************************************************************************/
/* INLINE                     simd_rev_h_synth                               */
/*****************************************************************************/

inline void
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
      __asm
        {
          MOV ECX,quads      // Set up counter used for looping
          MOV EAX,src
          MOV EDX,dst
          MOVQ MM0,q_offset
          MOVD MM1,downshift
loop_plus1:
          MOVQ MM2,MM0       // start with the offset
          PADDSW MM2,[EAX]   // add 1'st source sample
          PADDSW MM2,[EAX+2]   // add 2'nd source sample
          MOVQ MM3,[EDX]
          PSRAW MM2,MM1      // shift rigth by the `downshift' value
          PSUBSW MM3,MM2     // subtract from dest sample
          MOVQ [EDX],MM3
          ADD EAX,8
          ADD EDX,8
          SUB ECX,1
          JNZ loop_plus1
          EMMS // Clear MMX registers for use by FPU
        }
    }
  else if (i_lambda == -1)
    {
      __asm
        {
          MOV ECX,quads      // Set up counter used for looping
          MOV EAX,src
          MOV EDX,dst
          MOVQ MM0,q_offset
          MOVD MM1,downshift
loop_minus1:
          MOVQ MM2,MM0       // start with the offset
          PSUBSW MM2,[EAX]   // subtract 1'st source sample
          PSUBSW MM2,[EAX+2]   // subtract 2'nd source sample
          MOVQ MM3,[EDX]
          PSRAW MM2,MM1      // shift rigth by the `downshift' value
          PSUBSW MM3,MM2     // subtract from dest sample
          MOVQ [EDX],MM3
          ADD EAX,8
          ADD EBX,8
          ADD EDX,8
          SUB ECX,1
          JNZ loop_minus1
          EMMS // Clear MMX registers for use by FPU
        }
    }
  else
    assert(0);
}

/*****************************************************************************/
/* INLINE                        simd_interleave                             */
/*****************************************************************************/

inline void
  simd_interleave(short int *src1, short int *src2,
                  short int *dst, int pairs)
{
  int octets = (pairs+3)>>2;
  if (octets <= 0)
    return;
  __asm
    {
      MOV ECX,octets      // Set up counter used for looping
      MOV EAX,src1
      MOV EBX,src2
      MOV EDX,dst
loop_ilv:
      MOVQ MM0,[EAX]
      MOVQ MM2,MM0
      MOVQ MM1,[EBX]
      PUNPCKLWD MM2,MM1
      MOVQ [EDX],MM2
      PUNPCKHWD MM0,MM1
      MOVQ [EDX+8],MM0
      ADD EAX,8
      ADD EBX,8
      ADD EDX,16
      SUB ECX,1
      JNZ loop_ilv
      EMMS // Clear MMX registers for use by FPU
    }
}

/*****************************************************************************/
/* INLINE                   simd_upshifted_interleave                        */
/*****************************************************************************/

inline void
  simd_upshifted_interleave(short int *src1, short int *src2,
                            short int *dst, int pairs, int upshift)
{
  int octets = (pairs+3)>>2;
  if (octets <= 0)
    return;
  __asm
    {
      MOV ECX,octets      // Set up counter used for looping
      MOV EAX,src1
      MOV EBX,src2
      MOV EDX,dst
      MOVD MM3,upshift
loop_shift_ilv:
      MOVQ MM0,[EAX]
      PSLLW MM0,MM3
      MOVQ MM2,MM0
      MOVQ MM1,[EBX]
      PSLLW MM1,MM3
      PUNPCKLWD MM2,MM1
      MOVQ [EDX],MM2
      PUNPCKHWD MM0,MM1
      MOVQ [EDX+8],MM0
      ADD EAX,8
      ADD EBX,8
      ADD EDX,16
      SUB ECX,1
      JNZ loop_shift_ilv
      EMMS // Clear MMX registers for use by FPU
    }
}

#endif // MSVC_DWT_MMX_LOCAL_H
