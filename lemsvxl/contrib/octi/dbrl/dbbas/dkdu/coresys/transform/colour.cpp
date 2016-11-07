/*****************************************************************************/
// File: colour.cpp [scope = CORESYS/TRANSFORMS]
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
   Implements forward and reverse colour transformations: both reversible (RCT)
and irreversible (ICT = RGB to YCbCr).
******************************************************************************/

#include <assert.h>
#include "kdu_sample_processing.h"

// Set things up for the inclusion of assembler optimized routines
// for specific architectures.  The reason for this is to exploit
// the availability of SIMD type instructions on many modern processors.

#if defined KDU_PENTIUM_MSVC
#  define KDU_SIMD_OPTIMIZATIONS
#  include "msvc_colour_mmx_local.h" // Contains all asm commands in-line
static int simd_exists = msvc_colour_mmx_exists();
#elif defined KDU_PENTIUM_GCC
#  define KDU_SIMD_OPTIMIZATIONS
#  include "gcc_colour_mmx_local.h" // Declares functs in separate .s file
static int simd_exists = gcc_colour_mmx_exists();
#endif // KDU_PENTIUM_GCC

#define ALPHA_R 0.299 // These are exact expressions from which the
#define ALPHA_G 0.587 // ICT forward and reverse transform coefficients
#define ALPHA_B 0.114 // may be expressed.

/*****************************************************************************/
/* EXTERN                    kdu_convert_rgb_to_ycc                          */
/*****************************************************************************/

void
  kdu_convert_rgb_to_ycc(kdu_line_buf &c1, kdu_line_buf &c2, kdu_line_buf &c3)
{
  int n = c1.get_width();
  assert((c2.get_width() == n) && (c3.get_width() == n));
  assert((c1.is_absolute() == c2.is_absolute()) &&
         (c1.is_absolute() == c3.is_absolute()));

  if (c1.get_buf32() != NULL)
    { // All lines must have a 32-bit representation.
      kdu_sample32 *sp1 = c1.get_buf32();
      kdu_sample32 *sp2 = c2.get_buf32();
      kdu_sample32 *sp3 = c3.get_buf32();

      assert((sp1 != NULL) && (sp2 != NULL) && (sp3 != NULL));
      if (!c1.is_absolute())
        { // Irreversible transform (YCbCr to RGB)
          double x_y, x_cb, x_cr;
          double x_r, x_g, x_b;
#define CB_FACT (1/(2*(1-ALPHA_B)))
#define CR_FACT (1/(2*(1-ALPHA_R)))

          for (; n > 0; n--, sp1++, sp2++, sp3++)
            {
              x_r = sp1->fval;  x_g = sp2->fval;  x_b = sp3->fval;
              x_y = ALPHA_R*x_r + ALPHA_G*x_g + ALPHA_B*x_b;
              x_cb = CB_FACT*(x_b-x_y);
              x_cr = CR_FACT*(x_r-x_y);
              sp1->fval = (float) x_y;
              sp2->fval = (float) x_cb;
              sp3->fval = (float) x_cr;
            }
        }
      else
        { // Must be 32-bit absolute integers, so use RCT.
          kdu_int32 x_y, x_db, x_dr;
          kdu_int32 x_r, x_g, x_b;

          for (; n > 0; n--, sp1++, sp2++, sp3++)
            {
              x_r = sp1->ival;  x_g = sp2->ival;  x_b = sp3->ival;
              x_y = (x_r + x_g+x_g + x_b) >> 2;
              x_db = x_b - x_g;
              x_dr = x_r - x_g;
              sp1->ival = x_y;  sp2->ival = x_db;  sp3->ival = x_dr;
            }
        }
    }
  else
    { // Working with 16-bit representations.
      kdu_sample16 *sp1 = c1.get_buf16();
      kdu_sample16 *sp2 = c2.get_buf16();
      kdu_sample16 *sp3 = c3.get_buf16();
    
      assert((sp1 != NULL) && (sp2 != NULL) && (sp3 != NULL));
      if (!c1.is_absolute())
        { // 16-bit normalized integers, so use YCbCr transform.
          kdu_int32 x_y, x_cb, x_cr;
          kdu_int32 x_r, x_g, x_b;
#define ALPHA_R14 ((kdu_int32)(0.5+ALPHA_R*(1<<14)))
#define ALPHA_G14 ((kdu_int32)(0.5+ALPHA_G*(1<<14)))
#define ALPHA_B14 ((kdu_int32)(0.5+ALPHA_B*(1<<14)))
#define CB_FACT14 ((kdu_int32)(0.5 + CB_FACT*(1<<14)))
#define CR_FACT14 ((kdu_int32)(0.5 + CR_FACT*(1<<14)))

          for (; n > 0; n--, sp1++, sp2++, sp3++)
            {
              x_r = sp1->ival;  x_g = sp2->ival;  x_b = sp3->ival;
              x_y = (ALPHA_R14*x_r+ALPHA_G14*x_g+ALPHA_B14*x_b+(1<<13))>>14;
              x_cb = (CB_FACT14*(x_b-x_y) + (1<<13)) >> 14;
              x_cr = (CR_FACT14*(x_r-x_y) + (1<<13)) >> 14;
              sp1->ival = (kdu_int16) x_y;
              sp2->ival = (kdu_int16) x_cb;
              sp3->ival = (kdu_int16) x_cr;
            }
        }
      else
        { // 16-bit absolute integers, so use RCT.
          kdu_int16 x_y, x_db, x_dr;
          kdu_int16 x_r, x_g, x_b;

          for (; n > 0; n--, sp1++, sp2++, sp3++)
            {
              x_r = sp1->ival;  x_g = sp2->ival;  x_b = sp3->ival;
              x_y = (x_r + x_g+x_g + x_b) >> 2;
              x_db = x_b - x_g;
              x_dr = x_r - x_g;
              sp1->ival = x_y;  sp2->ival = x_db;  sp3->ival = x_dr;
            }
        }
    }
}

/*****************************************************************************/
/* EXTERN                    kdu_convert_ycc_to_rgb                          */
/*****************************************************************************/

void
  kdu_convert_ycc_to_rgb(kdu_line_buf &c1, kdu_line_buf &c2, kdu_line_buf &c3,
                         int width)
{
  int n = width;
  if (n < 0)
    n = c1.get_width();
  assert((c1.get_width() >= n) && (c2.get_width() >= n) &&
         (c3.get_width() >= n));
  assert((c1.is_absolute() == c2.is_absolute()) &&
         (c1.is_absolute() == c3.is_absolute()));

  if (c1.get_buf32() != NULL)
    { // Using 32-bit representations.
      kdu_sample32 *sp1 = c1.get_buf32();
      kdu_sample32 *sp2 = c2.get_buf32();
      kdu_sample32 *sp3 = c3.get_buf32();

      assert((sp1 != NULL) && (sp2 != NULL) && (sp3 != NULL));
      if (!c1.is_absolute())
        { // Irreversible transform (YCbCr to RGB)
          double x_y, x_cb, x_cr;
          double x_r, x_g, x_b;
#define CR_FACT_R (2*(1-ALPHA_R))
#define CB_FACT_B (2*(1-ALPHA_B))
#define CR_FACT_G (2*ALPHA_R*(1-ALPHA_R)/ALPHA_G)
#define CB_FACT_G (2*ALPHA_B*(1-ALPHA_B)/ALPHA_G)

          for (; n > 0; n--, sp1++, sp2++, sp3++)
            {
              x_y = sp1->fval;  x_cb = sp2->fval;  x_cr = sp3->fval;
              x_r = x_y + CR_FACT_R*x_cr;
              x_b = x_y + CB_FACT_B*x_cb;
              x_g = x_y - CR_FACT_G*x_cr - CB_FACT_G*x_cb;
              sp1->fval = (float) x_r;
              sp2->fval = (float) x_g;
              sp3->fval = (float) x_b;
            }
        }
      else
        { // 32-bit absolute integers, so use RCT.
          kdu_int32 x_y, x_db, x_dr;
          kdu_int32 x_r, x_g, x_b;
          
          for (; n > 0; n--, sp1++, sp2++, sp3++)
            {
              x_y = sp1->ival;  x_db = sp2->ival;  x_dr = sp3->ival;
              x_g = x_y - ((x_db+x_dr) >> 2);
              x_r = x_g + x_dr;
              x_b = x_g + x_db;
              sp1->ival = x_r;  sp2->ival = x_g;  sp3->ival = x_b;
            }
        }
    }
  else
    { // Using 16-bit representations.
      kdu_sample16 *sp1 = c1.get_buf16();
      kdu_sample16 *sp2 = c2.get_buf16();
      kdu_sample16 *sp3 = c3.get_buf16();

      assert((sp1 != NULL) && (sp2 != NULL) && (sp3 != NULL));
#ifdef KDU_SIMD_OPTIMIZATIONS
      if (simd_exists)
        {
          if (!c1.is_absolute())
            simd_inverse_ict(&(sp1->ival),&(sp2->ival),&(sp3->ival),n);
          else
            simd_inverse_rct(&(sp1->ival),&(sp2->ival),&(sp3->ival),n);
        }
      else
#endif // KDU_SIMD_OPTIMIZATIONS
      if (!c1.is_absolute())
        { // 16-bit normalized integers, so use YCbCr transform
          kdu_int32 x_y, x_cb, x_cr;
          kdu_int32 x_r, x_g, x_b;
#define CR_FACT_R14 ((kdu_int32)(0.5 + CR_FACT_R*(1<<14)))
#define CB_FACT_B14 ((kdu_int32)(0.5 + CB_FACT_B*(1<<14)))
#define CR_FACT_G14 ((kdu_int32)(0.5 + CR_FACT_G*(1<<14)))
#define CB_FACT_G14 ((kdu_int32)(0.5 + CB_FACT_G*(1<<14)))

          for (; n > 0; n--, sp1++, sp2++, sp3++)
            {
              x_y = sp1->ival;  x_cb = sp2->ival; x_cr = sp3->ival;
              x_y <<= 14;
              x_r = x_y + CR_FACT_R14*x_cr;
              x_b = x_y + CB_FACT_B14*x_cb;
              x_g = x_y - CR_FACT_G14*x_cr - CB_FACT_G14*x_cb;
              sp1->ival = (kdu_int16)((x_r + (1<<13))>>14);
              sp2->ival = (kdu_int16)((x_g + (1<<13))>>14);
              sp3->ival = (kdu_int16)((x_b + (1<<13))>>14);
            }
        }
      else
        { // 16-bit absolute integers, so use RCT
          kdu_int16 x_y, x_db, x_dr;
          kdu_int16 x_r, x_g, x_b;
          
          for (; n > 0; n--, sp1++, sp2++, sp3++)
            {
              x_y = sp1->ival;  x_db = sp2->ival;  x_dr = sp3->ival;
              x_g = x_y - ((x_db+x_dr) >> 2);
              x_r = x_g + x_dr;
              x_b = x_g + x_db;
              sp1->ival = x_r;  sp2->ival = x_g; sp3->ival = x_b;
            }
        }
    }
}
