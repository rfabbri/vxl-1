/*****************************************************************************/
// File: kdu_utils.h [scope = CORESYS/COMMON]
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
   Provides some handy in-line functions.
******************************************************************************/

#ifndef KDU_UTILS_H
#define KDU_UTILS_H

#include <assert.h>
#include "kdu_elementary.h"

/* ========================================================================= */
/*                            Convenient Inlines                             */
/* ========================================================================= */

/*****************************************************************************/
/* INLINE                           kdu_read                                 */
/*****************************************************************************/

static inline int
  kdu_read(kdu_byte * &bp, kdu_byte *end, int nbytes) // throws(kdu_byte *)
  /* This function reads an integer quantity having an `nbytes' bigendian
     representation from the array identified by `bp'.  During the process,
     `bp' is advanced `nbytes' positions.  If this pushes it past the `end'
     pointer, the function throws an exception of type `kdu_byte *'. */
{
  int val;

  assert(nbytes <= 4);
  if ((end-bp) < nbytes)
    throw bp;
  val = *(bp++);
  if (nbytes > 1)
    val = (val<<8) + *(bp++);
  if (nbytes > 2)
    val = (val<<8) + *(bp++);
  if (nbytes > 3)
    val = (val<<8) + *(bp++);
  return val;
}

/*****************************************************************************/
/* INLINE                          ceil_ratio                                */
/*****************************************************************************/

static inline int
  ceil_ratio(int num, int den)
  /* Returns the ceiling function of the ratio `num' / `den', where
     all quantities are required to be non-negative. */
{
  assert(den > 0);
  if (num < 0)
    return -((-num)/den);
  else
    return (num+den-1)/den;
}

/*****************************************************************************/
/* INLINE                          floor_ratio                               */
/*****************************************************************************/

static inline int
  floor_ratio(int num, int den)
  /* Returns the floor function of the ratio `num' / `den', where
     all quantities are required to be non-negative. */
{
  assert(den > 0);
  if (num < 0)
    return -((-num+den-1)/den);
  else
    return num/den;
}

#endif // KDU_UTILS_H
