/*****************************************************************************/
/* File: gcc_colour_mmx.s [scope = CORESYS/TRANSFORMS]                       */
/* Version: Kakadu, V2.2                                                     */
/* Author: David Taubman                                                     */
/* Last Revised: 20 June, 2001                                               */
/*****************************************************************************/
/* Copyright 2001, David Taubman, The University of New South Wales (UNSW)   */
/* The copyright owner is Unisearch Ltd, Australia (commercial arm of UNSW)  */
/* Neither this copyright statement, nor the licensing details below         */
/* may be removed from this file or dissociated from its contents.           */
/*****************************************************************************/
/*****************************************************************************/
/* Licensee: Book Owner */
/* License number: 99999 */
/* The Licensee has been granted a NON-COMMERCIAL license to the contents of */
/* this source file, said Licensee being the owner of a copy of the book, */
/* "JPEG2000: Image Compression Fundamentals, Standards and Practice," by */
/* Taubman and Marcellin (Kluwer Academic Publishers, 2001).  A brief summary */
/* of the license appears below.  This summary is not to be relied upon in */
/* preference to the full text of the license agreement, which was accepted */
/* upon breaking the seal of the compact disc accompanying the above-mentioned */
/* book. */
/* 1. The Licensee has the right to Non-Commercial Use of the Kakadu software, */
/*    Version 2.2, including distribution of one or more Applications built */
/*    using the software, provided such distribution is not for financial */
/*    return. */
/* 2. The Licensee has the right to personal use of the Kakadu software, */
/*    Version 2.2. */
/* 3. The Licensee has the right to distribute Reusable Code (including */
/*    source code and dynamically or statically linked libraries) to a Third */
/*    Party, provided the Third Party possesses a license to use the Kakadu */
/*    software, Version 2.2, and provided such distribution is not for */
/*    financial return. */
/*****************************************************************************/
/*****************************************************************************/
/* Description:                                                              */
/*   Assembler file for use with GCC builds on Pentium platforms (e.g.,      */
/* Linux).  You need only include this is the core system was built with     */
/* KDU_PENTIUM_GCC defined.                                                  */
/*****************************************************************************/

	.text

/*****************************************************************************/
/* EXTERN                     gcc_colour_mmx_exists                          */
/*****************************************************************************/

	.align 16
.globl gcc_colour_mmx_exists
gcc_colour_mmx_exists:
.LFB1:
	pushl	%ebp
.LCFI0:
	movl	%esp, %ebp
.LCFI1:
	subl	$4, %esp
.LCFI2:
	movl	$1, -4(%ebp)
    /* Here is where we insert the special MMX code. */
        MOV $1,%EAX
        CPUID
        TEST $0x800000,%EDX
        JNZ .Lmmx_exists_label
        MOVL $0,-4(%EBP)
.Lmmx_exists_label:
    /* Here is where the GCC generated stub code resumes. */
	movl	-4(%ebp), %eax /* Return value is in EAX. */
	movl	%ebp, %esp
	popl	%ebp
	ret
.LFE1:
.Lfe1:

/*****************************************************************************/
/* EXTERN                      simd_inverse_ict                              */
/*****************************************************************************/

	.align 16
.globl simd_inverse_ict
simd_inverse_ict:
.LFB2:
	pushl	%ebp
.LCFI3:
	movl	%esp, %ebp
.LCFI4:
	subl	$56, %esp
.LCFI5:
	movl	20(%ebp), %eax
	addl	$3, %eax
	sarl	$2, %eax
	movl	%eax, -4(%ebp)      # Base of `quads'
	cmpl	$0, -4(%ebp)
	jg	.L4
	jmp	.L3
	.p2align 4,,7
.L4:
	movw	$1, -16(%ebp)       # Base of `q_offset_1'
	movw	$1, -14(%ebp)
	movw	$1, -12(%ebp)
	movw	$1, -10(%ebp)       
	movw	$2, -24(%ebp)       # Base of `q_offset_2'
	movw	$2, -22(%ebp)
	movw	$2, -20(%ebp)
	movw	$2, -18(%ebp)       
	movw	$26345, -26(%ebp)
	movw	$26345, -28(%ebp)
	movw	$26345, -30(%ebp)
	movw	$26345, -32(%ebp)   # Base of `q_CRfactR'
	movw	$-14942, -34(%ebp)
	movw	$-14942, -36(%ebp)
	movw	$-14942, -38(%ebp)
	movw	$-14942, -40(%ebp)  # Base of `q_CBfactB'
	movw	$18734, -42(%ebp)
	movw	$18734, -44(%ebp)
	movw	$18734, -46(%ebp)
	movw	$18734, -48(%ebp)   # Base of `q_CRfactG'
	movw	$-22553, -50(%ebp)
	movw	$-22553, -52(%ebp)
	movw	$-22553, -54(%ebp)
	movw	$-22553, -56(%ebp)  # Base of `q_CBfactG'
    /* Here is where we add the special MMX code */
      MOV -4(%ebp),%EDX
      MOV 8(%ebp),%EAX      # Load the `src1' pointer
      MOV 12(%ebp),%EBX     # Load the `src2' pointer
      MOV 16(%ebp),%ECX     # Load the `src3' pointer
      MOVQ -16(%ebp),%MM4
      MOVQ -24(%ebp),%MM5
      MOVQ -32(%ebp),%MM6
      MOVQ -40(%ebp),%MM7
.Lloop_inverse_ict:
      MOVQ (%EAX),%MM0      # Load luminance (Y)
      MOVQ (%ECX),%MM1      # Load chrominance (Cr)
      MOVQ %MM1,%MM2        # Prepare to form Red output in MM2
      PADDSW %MM4,%MM2      # +1 here similar to adding 2^15 before dividing by 2^16
      PMULHW %MM6,%MM2      # Multiply by 0.402*2^16 (CRfactR) and divide by 2^16
      PADDSW %MM1,%MM2      # Add Cr again to make the factor equivalent to 1.402
      PADDSW %MM0,%MM2      # Add in luminance to get Red
      MOVQ %MM2,(%EAX)      # Save Red channel
      MOVQ %MM1,%MM2        # Prepare to form Cr*(-0.714136) in MM2 (will free MM1)
      PADDSW %MM5,%MM2      # +2 here similar to adding 2^15 before dividing by 2^16
      PMULHW -48(%ebp),%MM2 # Multiply by 0.285864*2^16 and divide by 2^16
      PSUBSW %MM1,%MM2      # Subtract Cr leaves us with the desired result
      PADDSW %MM0,%MM2      # Add Y to scaled Cr forms most of Green result in MM2
      MOVQ (%EBX),%MM1      # Load chrominance (Cb)
      MOVQ %MM1,%MM3        # Prepare to form Blue output in MM3
      PSUBSW %MM5,%MM3      # +2 here similar to adding 2^15 before dividing by 2^16
      PMULHW %MM7,%MM3      # Multiply by -0.228*2^16 (CBfactB) and divide by 2^16
      PADDSW %MM1,%MM3      # Gets 0.772*Cb to MM3
      PADDSW %MM1,%MM3      # Gets 1.772*Cb to MM3
      PADDSW %MM0,%MM3      # Add in luminance to get Blue
      MOVQ %MM3,(%ECX)      # Save Blue channel
      PSUBSW %MM5,%MM1      # +2 here similar to adding 2^15 before dividing by 2^16
      PMULHW -56(%ebp),%MM1 # Multiply by -0.344136*2^16 and divide by 2^16
      PADDSW %MM1,%MM2      # Completes the Green channel in MM2
      MOVQ %MM2,(%EBX)
      ADD $8,%EAX
      ADD $8,%EBX
      ADD $8,%ECX
      SUB $1,%EDX
      JNZ .Lloop_inverse_ict
      EMMS                  # Clear MMX registers for use by FPU
    /* Here is where the GCC generated stub code resumes */
.L3:
	movl	%ebp, %esp
	popl	%ebp
	ret
.LFE2:
.Lfe2:

/*****************************************************************************/
/* EXTERN                       simd_inverse_rct                             */
/*****************************************************************************/

	.align 16
.globl simd_inverse_rct
simd_inverse_rct:
.LFB3:
	pushl	%ebp
.LCFI6:
	movl	%esp, %ebp
.LCFI7:
	subl	$4, %esp
.LCFI8:
	movl	20(%ebp), %eax
	addl	$3, %eax
	sarl	$2, %eax
	movl	%eax, -4(%ebp)  # Base of `quads'
	cmpl	$0, -4(%ebp)
	jg	.L8
	jmp	.L6
	.p2align 4,,7
.L8:
    /* Here is where we add the special MMX code */
      MOV -4(%ebp),%EDX
      MOV 8(%ebp),%EAX      # Load the `src1' pointer
      MOV 12(%ebp),%EBX     # Load the `src2' pointer
      MOV 16(%ebp),%ECX     # Load the `src3' pointer
.Lloop_inverse_rct:
      MOVQ (%EBX),%MM1      # Load chrominance (Db)
      MOVQ (%ECX),%MM2      # Load chrominance (Dr)
      MOVQ %MM1,%MM3
      PADDSW %MM2,%MM3
      PSRAW $2,%MM3         # Forms (Db+DR)>>2
      MOVQ (%EAX),%MM0      # Load luminance (Y).
      PSUBSW %MM3,%MM0      # Convert Y to Green channel
      MOVQ %MM0,(%EBX)
      PADDSW %MM0,%MM2      # Convert Dr to Red channel
      MOVQ %MM2,(%EAX)
      PADDSW %MM0,%MM1      # Convert Db to Blue channel
      MOVQ %MM1,(%ECX)
      ADD $8,%EAX
      ADD $8,%EBX
      ADD $8,%ECX
      SUB $1,%EDX
      JNZ .Lloop_inverse_rct
      EMMS                  # Clear MMX registers for use by FPU
    /* Here is where the GCC generated stub code resumes */
.L6:
	movl	%ebp, %esp
	popl	%ebp
	ret
.LFE3:
.Lfe3:
