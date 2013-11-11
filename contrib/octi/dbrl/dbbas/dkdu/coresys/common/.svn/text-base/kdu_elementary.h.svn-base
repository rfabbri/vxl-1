/*****************************************************************************/
// File: kdu_elementary.h [scope = CORESYS/COMMON]
// Version: Kakadu, V2.2.2
// Author: David Taubman
// Last Revised: 20 July, 2001
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
   Elementary data types and extensively used constants.  It is possible that
the definitions here might need to be changed for some architectures.
******************************************************************************/

#ifndef KDU_ELEMENTARY_H
#define KDU_ELEMENTARY_H
#include <limits.h>
#define KDU_EXPORT

#ifdef CORESYS_EXPORTS
#  ifdef WIN32
#    undef KDU_EXPORT
#    define KDU_EXPORT __declspec(dllexport)
#  endif
#endif

/*****************************************************************************/
/*                                 8-bit Scalars                             */
/*****************************************************************************/

typedef unsigned char kdu_byte;

/*****************************************************************************/
/*                                 16-bit Scalars                            */
/*****************************************************************************/

typedef short kdu_int16;
typedef unsigned short int kdu_uint16;

#define KDU_INT16_MAX ((kdu_int16) 0x7FFF)
#define KDU_INT16_MIN ((kdu_int16) 0x8000)

/*****************************************************************************/
/*                                 32-bit Scalars                            */
/*****************************************************************************/

#if (INT_MAX == 2147483647)
  typedef int kdu_int32;
  typedef unsigned int kdu_uint32;
#else
# error "Platform does not appear to support 32 bit integers!"
#endif

#define KDU_INT32_MAX ((kdu_int32) 0x7FFFFFFF)
#define KDU_INT32_MIN ((kdu_int32) 0x80000000)

/*****************************************************************************/
/*                              Subband Identifiers         k;                 */
/*****************************************************************************/

#define LL_BAND 0 // DC subband
#define HL_BAND 1 // Horizontally high-pass subband
#define LH_BAND 2 // Vertically high-pass subband
#define HH_BAND 3 // High-pass subband

#endif // KDU_ELEMENTARY_H
