/*****************************************************************************/
// File: gcc_colour_mmx_local.h [scope = CORESYS/TRANSFORMS]
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
   Declares functions which are implemented in separate assembler files.
The assembly code contains MMX implementations of the reverse colour
transformations: both the reversible (RCT) and the irreversible (ICT = RGB to
YCbCr) for use with Gcc and Gnu AS.
******************************************************************************/

#ifndef GCC_COLOUR_MMX_LOCAL_H
#define GCC_COLOUR_MMX_LOCAL_H

extern "C" {
  int
    gcc_colour_mmx_exists();
  void
    simd_inverse_ict(short int *src1, short int *src2, short int *src3,
                     int samples);
  void
    simd_inverse_rct(short int *src1, short int *src2, short int *src3,
                     int samples);
}

#endif // GCC_COLOUR_MMX_LOCAL_H
