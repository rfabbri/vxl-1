/*****************************************************************************/
/* File: gcc_dwt_mmx.s [scope = CORESYS/TRANSFORMS]                          */
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
/* EXTERN                      gcc_dwt_mmx_exists                            */
/*****************************************************************************/

	.align 16
.globl gcc_dwt_mmx_exists
gcc_dwt_mmx_exists:
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
	movl	-4(%ebp), %eax
	movl	%ebp, %esp
	popl	%ebp
	ret
.LFE1:
.Lfe1:

/*****************************************************************************/
/* EXTERN                    simd_irrev_v_synth                              */
/*****************************************************************************/

	.align 16
.globl simd_irrev_v_synth
simd_irrev_v_synth:
.LFB2:
	pushl	%ebp
.LCFI3:
	movl	%esp, %ebp
.LCFI4:
	subl	$24, %esp
.LCFI5:
	movl	28(%ebp), %eax
	movw	%ax, -2(%ebp)
	movl	32(%ebp), %eax
	movw	%ax, -4(%ebp)
	movl	20(%ebp), %eax
	addl	$3, %eax
	sarl	$2, %eax
	movl	%eax, -8(%ebp)      # Base of `quads'
	cmpl	$0, -8(%ebp)
	jg	.L4
	jmp	.L3
	.p2align 4,,7
.L4:
	movzwl	-2(%ebp), %eax
	movw	%ax, -10(%ebp)
	movw	%ax, -12(%ebp)
	movw	%ax, -14(%ebp)
	movw	%ax, -16(%ebp)      # Base of `q_lambda'
	movl	-4(%ebp), %eax
	movw	%ax, -18(%ebp)
	movw	%ax, -20(%ebp)
	movw	%ax, -22(%ebp)
	movw	%ax, -24(%ebp)      # Base of `q_offset'
	cmpl	$-2, 24(%ebp)
	jne	.L5
    /* Special MMX code begins here */
          MOV -8(%ebp),%ECX     # Set up counter used for  looping
          MOV 8(%ebp),%EAX      # Load `src1' pointer
          MOV 12(%EBP),%EBX     # Load `src2' pointer
          MOV 16(%EBP),%EDX     # Load `dst' pointer
          MOVQ -16(%ebp),%MM0   # Load `q_lambda'
          MOVQ -24(%ebp),%MM1   # Load `q_offset'
.Liv_loop_minus2:
          MOVQ (%EAX),%MM2     # Start with source sample 1
          PADDSW (%EBX),%MM2   # Add source sample 2
          MOVQ (%EDX),%MM3
          PADDSW %MM2,%MM3     # Here is a -1 contribution
          PADDSW %MM2,%MM3     # Here is another -1 contribution
          PADDSW %MM1,%MM2     # Add pre-offset for rounding
          PMULHW %MM0,%MM2     # Multiply by lambda and discard 16 LSB's
          PSUBSW %MM2,%MM3     # Final contribution
          MOVQ %MM3,(%EDX)
          ADD $8,%EAX
          ADD $8,%EBX
          ADD $8,%EDX
          SUB $1,%ECX
          JNZ .Liv_loop_minus2
          EMMS               # Clear MMX registers for use by FPU
    /* End of special MMX code; resume stub code generated by gcc */
	jmp	.L12
	.p2align 4,,7
.L5:
	cmpl	$0, 24(%ebp)
	jne	.L7
	cmpw	$0, -2(%ebp)
	jns	.L7
	movl	-16(%ebp), %eax
	sall	$3, %eax
	movw	%ax, -10(%ebp)
	movw	%ax, -12(%ebp)
	movw	%ax, -14(%ebp)
	movw	%ax, -16(%ebp)
	movw	$4, -18(%ebp)
	movw	$4, -20(%ebp)
	movw	$4, -22(%ebp)
	movw	$4, -24(%ebp)
    /* Special MMX code begins here: i_lambda=0 and remainder < 0 */
          MOV -8(%ebp),%ECX     # Set up counter used for  looping
          MOV 8(%ebp),%EAX      # Load `src1' pointer
          MOV 12(%EBP),%EBX     # Load `src2' pointer
          MOV 16(%EBP),%EDX     # Load `dst' pointer
          MOVQ -16(%ebp),%MM0   # Load `q_lambda'
          MOVQ -24(%ebp),%MM1   # Load `q_offset'
.Liv_loop_0neg:
          MOVQ (%EAX),%MM2      # Start with source samples 1
          PMULHW %MM0,%MM2      # Multiply by lambda and discard 16 LSB's
          PXOR %MM4,%MM4
          PSUBSW (%EBX),%MM4    # Load negated source samples 2
          PMULHW %MM0,%MM4      # Multiply by lambda and discard 16 LSB's
          MOVQ (%EDX),%MM3
          PSUBSW %MM4,%MM2      # Accumulate non-negated scaled sources.
          PADDSW %MM1,%MM2      # Add post-offset for rounding
          PSRAW $3,%MM2         # Divide by 8 (we've been working with 8x lambda)
          PSUBSW %MM2,%MM3      # Update destination samples
          MOVQ %MM3,(%EDX)
          ADD $8,%EAX
          ADD $8,%EBX
          ADD $8,%EDX
          SUB $1,%ECX
          JNZ .Liv_loop_0neg
          EMMS                  # Clear MMX registers for use by FPU
    /* End of special MMX code; resume stub code generated by gcc */
	jmp	.L12
	.p2align 4,,7
.L7:
	cmpl	$0, 24(%ebp)
	jne	.L9
    /* Special MMX code begins here */
          MOV -8(%ebp),%ECX     # Set up counter used for  looping
          MOV 8(%ebp),%EAX      # Load `src1' pointer
          MOV 12(%EBP),%EBX     # Load `src2' pointer
          MOV 16(%EBP),%EDX     # Load `dst' pointer
          MOVQ -16(%ebp),%MM0   # Load `q_lambda'
          MOVQ -24(%ebp),%MM1   # Load `q_offset'
.Liv_loop_0pos:
          MOVQ (%EAX),%MM2      # Start with source sample 1
          PADDSW (%EBX),%MM2    # Add source sample 2
          MOVQ (%EDX),%MM3
          PADDSW %MM1,%MM2      # Add pre-offset for rounding
          PMULHW %MM0,%MM2      # Multiply by lambda and discard 16 LSB's
          PSUBSW %MM2,%MM3      # Final contribution
          MOVQ %MM3,(%EDX)
          ADD $8,%EAX
          ADD $8,%EBX
          ADD $8,%EDX
          SUB $1,%ECX
          JNZ .Liv_loop_0pos
          EMMS                  # Clear MMX registers for use by FPU
    /* End of special MMX code; resume stub code generated by gcc */
	jmp	.L12
	.p2align 4,,7
.L9:
	cmpl	$1, 24(%ebp)
	jne	.L12
    /* Special MMX code begins here */
          MOV -8(%ebp),%ECX     # Set up counter used for  looping
          MOV 8(%ebp),%EAX      # Load `src1' pointer
          MOV 12(%EBP),%EBX     # Load `src2' pointer
          MOV 16(%EBP),%EDX     # Load `dst' pointer
          MOVQ -16(%ebp),%MM0   # Load `q_lambda'
          MOVQ -24(%ebp),%MM1   # Load `q_offset'
.Liv_loop_plus1:
          MOVQ (%EAX),%MM2      # Start with source sample 1
          PADDSW (%EBX),%MM2    # Add source sample 2
          MOVQ (%EDX),%MM3
          PSUBSW %MM2,%MM3      # Here is a +1 contribution
          PADDSW %MM1,%MM2      # Add pre-offset for rounding
          PMULHW %MM0,%MM2      # Multiply by lambda and discard 16 LSB's
          PSUBSW %MM2,%MM3      # Final contribution
          MOVQ %MM3,(%EDX)
          ADD $8,%EAX
          ADD $8,%EBX
          ADD $8,%EDX
          SUB $1,%ECX
          JNZ .Liv_loop_plus1
          EMMS                  # Clear MMX registers for use by FPU
    /* End of special MMX code; resume stub code generated by gcc */
.L12:
.L3:
	movl	%ebp, %esp
	popl	%ebp
	ret
.LFE2:
.Lfe2:

/*****************************************************************************/
/* EXTERN                      simd_rev_v_synth                              */
/*****************************************************************************/

	.align 16
.globl simd_rev_v_synth
simd_rev_v_synth:
.LFB3:
	pushl	%ebp
.LCFI6:
	movl	%esp, %ebp
.LCFI7:
	subl	$16, %esp
.LCFI8:
	movl	20(%ebp), %eax
	addl	$3, %eax
	sarl	$2, %eax
	movl	%eax, -4(%ebp)      # Base of `quads'
	cmpl	$0, -4(%ebp)
	jg	.L14
	jmp	.L13
	.p2align 4,,7
.L14:
	movzbl	24(%ebp), %ecx
	movl	$1, %eax
	sall	%cl, %eax
	sarl	%eax
	movw	%ax, -10(%ebp)
	movzwl	-10(%ebp), %eax
	movw	%ax, -10(%ebp)
	movw	%ax, -12(%ebp)
	movw	%ax, -14(%ebp)
	movw	%ax, -16(%ebp)      # Base of `q_offset'
	cmpl	$1, 28(%ebp)
	jne	.L15
    /* Special MMX code begins here */
          MOV -4(%ebp),%ECX     # Set up counter used for looping
          MOV 8(%ebp),%EAX      # Load `src1' pointer
          MOV 12(%ebp),%EBX     # Load `src2' pointer
          MOV 16(%ebp),%EDX     # Load `dst' pointer
          MOVQ -16(%ebp),%MM0   # Load `q_offset'
          MOVD 24(%ebp),%MM1    # Load `downshift'
.Lrv_loop_plus1:
          MOVQ %MM0,%MM2        # start with the offset
          PADDSW (%EAX),%MM2    # add 1'st source sample
          PADDSW (%EBX),%MM2    # add 2'nd source sample
          MOVQ (%EDX),%MM3
          PSRAW %MM1,%MM2       # shift rigth by the `downshift' value
          PSUBSW %MM2,%MM3      # subtract from dest sample
          MOVQ %MM3,(%EDX)
          ADD $8,%EAX
          ADD $8,%EBX
          ADD $8,%EDX
          SUB $1,%ECX
          JNZ .Lrv_loop_plus1
          EMMS                  # Clear MMX registers for use by FPU
    /* End of special MMX code; resume stub code generated by gcc */
	jmp	.L18
	.p2align 4,,7
.L15:
	cmpl	$-1, 28(%ebp)
	jne	.L18
    /* Special MMX code begins here */
          MOV -4(%ebp),%ECX     # Set up counter used for looping
          MOV 8(%ebp),%EAX      # Load `src1' pointer
          MOV 12(%ebp),%EBX     # Load `src2' pointer
          MOV 16(%ebp),%EDX     # Load `dst' pointer
          MOVQ -16(%ebp),%MM0   # Load `q_offset'
          MOVD 24(%ebp),%MM1    # Load `downshift'
.Lrv_loop_minus1:
          MOVQ %MM0,%MM2        # start with the offset
          PSUBSW (%EAX),%MM2    # subtract 1'st source sample
          PSUBSW (%EBX),%MM2    # subtract 2'nd source sample
          MOVQ (%EDX),%MM3
          PSRAW %MM1,%MM2       # shift rigth by the `downshift' value
          PSUBSW %MM2,%MM3      # subtract from dest sample
          MOVQ %MM3,(%EDX)
          ADD $8,%EAX
          ADD $8,%EBX
          ADD $8,%EDX
          SUB $1,%ECX
          JNZ .Lrv_loop_minus1
          EMMS                  # Clear MMX registers for use by FPU
    /* End of special MMX code; resume stub code generated by gcc */
.L18:
.L13:
	movl	%ebp, %esp
	popl	%ebp
	ret
.LFE3:
.Lfe3:

/*****************************************************************************/
/* EXTERN                    simd_irrev_h_synth                              */
/*****************************************************************************/

	.align 16
.globl simd_irrev_h_synth
simd_irrev_h_synth:
.LFB4:
	pushl	%ebp
.LCFI9:
	movl	%esp, %ebp
.LCFI10:
	subl	$24, %esp
.LCFI11:
	movl	24(%ebp), %eax
	movw	%ax, -2(%ebp)
	movl	28(%ebp), %eax
	movw	%ax, -4(%ebp)
	movl	16(%ebp), %eax
	addl	$3, %eax
	sarl	$2, %eax
	movl	%eax, -8(%ebp)     # Base of `quads'
	cmpl	$0, -8(%ebp)
	jg	.L20
	jmp	.L19
	.p2align 4,,7
.L20:
	movzwl	-2(%ebp), %eax
	movw	%ax, -10(%ebp)
	movw	%ax, -12(%ebp)
	movw	%ax, -14(%ebp)
	movw	%ax, -16(%ebp)     # Base of `q_lambda'
	movl	-4(%ebp), %eax
	movw	%ax, -18(%ebp)
	movw	%ax, -20(%ebp)
	movw	%ax, -22(%ebp)
	movw	%ax, -24(%ebp)     # Base of `q_offset'
	cmpl	$-2, 20(%ebp)
	jne	.L21
    /* Special MMX code begins here */
          MOV -8(%ebp),%ECX    # Set up counter used for looping
          MOV 8(%ebp),%EAX     # Load `src' pointer
          MOV 12(%ebp),%EDX    # Load `dst' pointer
          MOVQ -16(%ebp),%MM0  # Load `q_lambda'
          MOVQ -24(%ebp),%MM1  # Load `q_offset'
.Lih_loop_minus2:
          MOVQ (%EAX),%MM2
          PADDSW 2(%EAX),%MM2   # Add source sample 2
          MOVQ (%EDX),%MM3
          PADDSW %MM2,%MM3     # Here is a -1 contribution
          PADDSW %MM2,%MM3     # Here is another -1 contribution
          PADDSW %MM1,%MM2     # Add pre-offset for rounding
          PMULHW %MM0,%MM2     # Multiply by lambda and discard 16 LSB's
          PSUBSW %MM2,%MM3     # Final contribution
          MOVQ %MM3,(%EDX)
          ADD $8,%EAX
          ADD $8,%EDX
          SUB $1,%ECX
          JNZ .Lih_loop_minus2
          EMMS                 # Clear MMX registers for use by FPU
    /* End of special MMX code; resume stub code generated by gcc */
	jmp	.L28
	.p2align 4,,7
.L21:
	cmpl	$0, 20(%ebp)
	jne	.L23
	cmpw	$0, -2(%ebp)
	jns	.L23
	movl	-16(%ebp), %eax
	sall	$3, %eax
	movw	%ax, -10(%ebp)
	movw	%ax, -12(%ebp)
	movw	%ax, -14(%ebp)
	movw	%ax, -16(%ebp)
	movw	$4, -18(%ebp)
	movw	$4, -20(%ebp)
	movw	$4, -22(%ebp)
	movw	$4, -24(%ebp)
    /* Special MMX code begins here */
          MOV -8(%ebp),%ECX    # Set up counter used for looping
          MOV 8(%ebp),%EAX     # Load `src' pointer
          MOV 12(%ebp),%EDX    # Load `dst' pointer
          MOVQ -16(%ebp),%MM0  # Load `q_lambda'
          MOVQ -24(%ebp),%MM1  # Load `q_offset'
.Lih_loop_0neg:
          MOVQ (%EAX),%MM2
          PMULHW %MM0,%MM2     # Multiply by lambda and discard 16 LSB's
          PXOR %MM4,%MM4
          PSUBSW 2(%EAX),%MM4  # Load negated source samples 2
          PMULHW %MM0,%MM4     # Multiply by lambda and discard 16 LSB's
          MOVQ (%EDX),%MM3
          PSUBSW %MM4,%MM2     # Accumulate non-negated scaled sources.
          PADDSW %MM1,%MM2     # Add post-offset for rounding
          PSRAW $3,%MM2        # Divide by 8 (we've been working with 8x lambda)
          PSUBSW %MM2,%MM3     # Final contribution
          MOVQ %MM3,(%EDX)
          ADD $8,%EAX
          ADD $8,%EDX
          SUB $1,%ECX
          JNZ .Lih_loop_0neg
          EMMS                 # Clear MMX registers for use by FPU
    /* End of special MMX code; resume stub code generated by gcc */
	jmp	.L28
	.p2align 4,,7
.L23:
	cmpl	$0, 20(%ebp)
	jne	.L25
    /* Special MMX code begins here */
          MOV -8(%ebp),%ECX    # Set up counter used for looping
          MOV 8(%ebp),%EAX     # Load `src' pointer
          MOV 12(%ebp),%EDX    # Load `dst' pointer
          MOVQ -16(%ebp),%MM0  # Load `q_lambda'
          MOVQ -24(%ebp),%MM1  # Load `q_offset'
.Lih_loop_0pos:
          MOVQ (%EAX),%MM2
          PADDSW 2(%EAX),%MM2 # Add source sample 2
          MOVQ (%EDX),%MM3
          PADDSW %MM1,%MM2     # Add pre-offset for rounding
          PMULHW %MM0,%MM2     # Multiply by lambda and discard 16 LSB's
          PSUBSW %MM2,%MM3     # Final contribution
          MOVQ %MM3,(%EDX)
          ADD $8,%EAX
          ADD $8,%EDX
          SUB $1,%ECX
          JNZ .Lih_loop_0pos
          EMMS                 # Clear MMX registers for use by FPU
    /* End of special MMX code; resume stub code generated by gcc */
	jmp	.L28
	.p2align 4,,7
.L25:
	cmpl	$1, 20(%ebp)
	jne	.L28
    /* Special MMX code begins here */
          MOV -8(%ebp),%ECX    # Set up counter used for looping
          MOV 8(%ebp),%EAX     # Load `src' pointer
          MOV 12(%ebp),%EDX    # Load `dst' pointer
          MOVQ -16(%ebp),%MM0  # Load `q_lambda'
          MOVQ -24(%ebp),%MM1  # Load `q_offset'
.Lih_loop_plus1:
          MOVQ (%EAX),%MM2
          PADDSW 2(%EAX),%MM2  # Add source sample 2
          MOVQ (%EDX),%MM3
          PSUBSW %MM2,%MM3     # Here is a +1 contribution
          PADDSW %MM1,%MM2     # Add pre-offset for rounding
          PMULHW %MM0,%MM2     # Multiply by lambda and discard 16 LSB's
          PSUBSW %MM2,%MM3     # Final contribution
          MOVQ %MM3,(%EDX)
          ADD $8,%EAX
          ADD $8,%EDX
          SUB $1,%ECX
          JNZ .Lih_loop_plus1
          EMMS                 # Clear MMX registers for use by FPU
    /* End of special MMX code; resume stub code generated by gcc */
.L28:
.L19:
	movl	%ebp, %esp
	popl	%ebp
	ret
.LFE4:
.Lfe4:

/*****************************************************************************/
/* EXTERN                     simd_rev_h_synth                               */
/*****************************************************************************/

	.align 16
.globl simd_rev_h_synth
simd_rev_h_synth:
.LFB5:
	pushl	%ebp
.LCFI12:
	movl	%esp, %ebp
.LCFI13:
	subl	$16, %esp
.LCFI14:
	movl	16(%ebp), %eax
	addl	$3, %eax
	sarl	$2, %eax
	movl	%eax, -4(%ebp)   # Base of `quads'
	cmpl	$0, -4(%ebp)
	jg	.L30
	jmp	.L29
	.p2align 4,,7
.L30:
	movzbl	20(%ebp), %ecx   # Load `downshift'
	movl	$1, %eax
	sall	%cl, %eax
	sarl	%eax
	movw	%ax, -10(%ebp)
	movzwl	-10(%ebp), %eax
	movw	%ax, -10(%ebp)
	movw	%ax, -12(%ebp)
	movw	%ax, -14(%ebp)
	movw	%ax, -16(%ebp)   # Base of `q_offset'
	cmpl	$1, 24(%ebp)
	jne	.L31
    /* Special MMX code begins here */
          MOV -4(%ebp),%ECX      # Set up counter used for looping
          MOV 8(%ebp),%EAX       # Load `src' pointer
          MOV 12(%ebp),%EDX      # Load `dst' pointer
          MOVQ -16(%ebp),%MM0    # Load `q_offset'
          MOVD 20(%ebp),%MM1     # Load `downshift'
.Lrh_loop_plus1:
          MOVQ %MM0,%MM2         # start with the offset
          PADDSW (%EAX),%MM2     # add 1'st source sample
          PADDSW 2(%EAX),%MM2    # add 2'nd source sample
          MOVQ (%EDX),%MM3
          PSRAW %MM1,%MM2        # shift rigth by the `downshift' value
          PSUBSW %MM2,%MM3       # subtract from dest sample
          MOVQ %MM3,(%EDX)
          ADD $8,%EAX
          ADD $8,%EDX
          SUB $1,%ECX
          JNZ .Lrh_loop_plus1
          EMMS                   # Clear MMX registers for use by FPU
    /* End of special MMX code; resume stub code generated by gcc */
	jmp	.L34
	.p2align 4,,7
.L31:
	cmpl	$-1, 24(%ebp)
	jne	.L34
    /* Special MMX code begins here */
          MOV -4(%ebp),%ECX      # Set up counter used for looping
          MOV 8(%ebp),%EAX       # Load `src' pointer
          MOV 12(%ebp),%EDX      # Load `dst' pointer
          MOVQ -16(%ebp),%MM0    # Load `q_offset'
          MOVD 20(%ebp),%MM1     # Load `downshift'
.Lrh_loop_minus1:
          MOVQ %MM0,%MM2         # start with the offset
          PSUBSW (%EAX),%MM2     # subtract 1'st source sample
          PSUBSW 2(%EAX),%MM2    # subtract 2'nd source sample
          MOVQ (%EDX),%MM3
          PSRAW %MM1,%MM2        # shift rigth by the `downshift' value
          PSUBSW %MM2,%MM3       # subtract from dest sample
          MOVQ %MM3,(%EDX)
          ADD $8,%EAX
          ADD $8,%EBX
          ADD $8,%EDX
          SUB $1,%ECX
          JNZ .Lrh_loop_minus1
          EMMS                   # Clear MMX registers for use by FPU
    /* End of special MMX code; resume stub code generated by gcc */
.L34:
.L29:
	movl	%ebp, %esp
	popl	%ebp
	ret
.LFE5:
.Lfe5:

/*****************************************************************************/
/* EXTERN                        simd_interleave                             */
/*****************************************************************************/

	.align 16
.globl simd_interleave
simd_interleave:
.LFB6:
	pushl	%ebp
.LCFI15:
	movl	%esp, %ebp
.LCFI16:
	subl	$4, %esp
.LCFI17:
	movl	20(%ebp), %eax
	addl	$3, %eax
	sarl	$2, %eax
	movl	%eax, -4(%ebp)   # Base of `octets'
	cmpl	$0, -4(%ebp)
	jg	.L36
	jmp	.L35
	.p2align 4,,7
.L36:
    /* Special MMX code begins here */
      MOV -4(%ebp),%ECX     # Set up counter used for looping
      MOV 8(%ebp),%EAX      # Load `src1' pointer
      MOV 12(%ebp),%EBX     # Load `src2' pointer
      MOV 16(%ebp),%EDX     # Load `dst' pointer
.Lloop_ilv:
      MOVQ (%EAX),%MM0
      MOVQ %MM0,%MM2
      MOVQ (%EBX),%MM1
      PUNPCKLWD %MM1,%MM2
      MOVQ %MM2,(%EDX)
      PUNPCKHWD %MM1,%MM0
      MOVQ %MM0,8(%EDX)
      ADD $8,%EAX
      ADD $8,%EBX
      ADD $16,%EDX
      SUB $1,%ECX
      JNZ .Lloop_ilv
      EMMS                  # Clear MMX registers for use by FPU
    /* End of special MMX code; resume stub code generated by gcc */
.L35:
	movl	%ebp, %esp
	popl	%ebp
	ret
.LFE6:
.Lfe6:

/*****************************************************************************/
/* EXTERN                   simd_upshifted_interleave                        */
/*****************************************************************************/

	.align 16
.globl simd_upshifted_interleave
simd_upshifted_interleave:
.LFB7:
	pushl	%ebp
.LCFI18:
	movl	%esp, %ebp
.LCFI19:
	subl	$4, %esp
.LCFI20:
	movl	20(%ebp), %eax
	addl	$3, %eax
	sarl	$2, %eax
	movl	%eax, -4(%ebp)     # Base of `octets'
	cmpl	$0, -4(%ebp)
	jg	.L39
	jmp	.L38
	.p2align 4,,7
.L39:
    /* Special MMX code begins here */
      MOV -4(%ebp),%ECX      # Set up counter used for looping
      MOV 8(%ebp),%EAX       # Load `src1' pointer
      MOV 12(%ebp),%EBX      # Load `src2' pointer
      MOV 16(%ebp),%EDX      # Load `dst' pointer
      MOVD 24(%ebp),%MM3     # Load `upshift'
.Lloop_shift_ilv:
      MOVQ (%EAX),%MM0
      PSLLW %MM3,%MM0
      MOVQ %MM0,%MM2
      MOVQ (%EBX),%MM1
      PSLLW %MM3,%MM1
      PUNPCKLWD %MM1,%MM2
      MOVQ %MM2,(%EDX)
      PUNPCKHWD %MM1,%MM0
      MOVQ %MM0,8(%EDX)
      ADD $8,%EAX
      ADD $8,%EBX
      ADD $16,%EDX
      SUB $1,%ECX
      JNZ .Lloop_shift_ilv
      EMMS                   # Clear MMX registers for use by FPU
    /* End of special MMX code; resume stub code generated by gcc */
.L38:
	movl	%ebp, %esp
	popl	%ebp
	ret
.LFE7:
.Lfe7:
