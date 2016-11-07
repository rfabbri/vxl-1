/*****************************************************************************/
// File: block_decoder.cpp [scope = CORESYS/CODING]
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
   Implements the embedded block decoding algorithm, including the decoding
passes themselves, as well as error detection and concealment capabilities.
No more than 30 bit-planes will be decoded for any code-block.  The low
level services offered by the MQ arithmetic coder appear in "mq_decoder.cpp"
and "mq_decoder.h".
******************************************************************************/

#include <assert.h>
#include <string.h>
#include "kdu_messaging.h"
#include "kdu_block_coding.h"
#include "block_coding_common.h"
#include "mq_decoder.h"

static kdu_byte *significance_luts[4] =
  {lh_sig_lut, hl_sig_lut, lh_sig_lut, hh_sig_lut};

#define EXTRA_DECODE_CWORDS 3 // Number of extra context-words between stripes.

// Set things up for the inclusion of assembler optimized routines for
// specific architectures.  Optimizing compilers should generally do a
// pretty good job with the C++ code contained here.  Further optimizations
// are probably only worthwhile for processors with very few registers such
// as the X86 family of processors.

#ifdef KDU_PENTIUM_MSVC
#  define KDU_ASM_OPTIMIZATIONS
#  include "msvc_block_decode_asm.h"
static int mmx_exists = msvc_decoder_mmx_exists();
#endif

/* ========================================================================= */
/*                   Local Class and Structure Definitions                   */
/* ========================================================================= */

/*****************************************************************************/
/*                             kd_block_decoder                              */
/*****************************************************************************/

class kd_block_decoder : public kdu_block_decoder_base {
  /* Although we can supply a constructor and a virtual destructor in the
     future, we have no need for these for the moment. */
  protected:
    void decode(kdu_block *block);
  private: // Internal implementation
    void reset_states()
      { // See Table 12.1 in the book by Taubman and Marcellin
        for (int n=0; n < 18; n++)
          states[n].init(0,0);
        states[KAPPA_SIG_BASE].init(4,0);
        states[KAPPA_RUN_BASE].init(3,0);
      }
  private: // Data
    mq_decoder coder;
    mqd_state states[18];
  };


/* ========================================================================= */
/*             Binding of MQ and Raw Symbol Coding Services                  */
/* ========================================================================= */

#define USE_FAST_MACROS // Comment this out if you want functions instead.

#ifdef USE_FAST_MACROS
#  define _mq_check_out_(coder)                                     \
     register kdu_int32 D; register kdu_int32 C; register kdu_int32 A; \
     register kdu_int32 t; kdu_int32 temp; kdu_byte *store; int S;     \
     coder.check_out(A,C,D,t,temp,store,S)
#  define _mq_check_in_(coder)                                      \
     coder.check_in(A,C,D,t,temp,store,S)
#  define _mq_dec_(coder,symbol,state)                              \
     _mq_decode_(symbol,state,A,C,D,t,temp,store,S)
#  define _mq_dec_run_(coder,run)                                   \
     _mq_decode_run_(run,A,C,D,t,temp,store,S)

#  define _raw_check_out_(coder)                                    \
     register kdu_int32 t; register kdu_int32 temp; kdu_byte *store;   \
     coder.check_out(t,temp,store)
#  define _raw_check_in_(coder)                                     \
     coder.check_in(t,temp,store)
#  define _raw_dec_(coder,symbol)                                   \
     _raw_decode_(symbol,t,temp,store)
#else // Do not use fast macros
#  define _mq_check_out_(coder)
#  define _mq_check_in_(coder)
#  define _mq_dec_(coder,symbol,state) coder.mq_decode(symbol,state)
#  define _mq_dec_run_(coder,run) coder.mq_decode_run(run)

#  define _raw_check_out_(coder)
#  define _raw_check_in_(coder)
#  define _raw_dec_(coder,symbol) coder.raw_decode(symbol)
#endif // USE_FAST_MACROS


/* ========================================================================= */
/*                 Machine Independent coding pass functions                 */
/* ========================================================================= */

/*****************************************************************************/
/* STATIC                   decode_sig_prop_pass_raw                         */
/*****************************************************************************/

static void
  decode_sig_prop_pass_raw(mq_decoder &coder, int p, bool causal,
                           kdu_int32 *samples, kdu_int32 *contexts,
                           int width, int num_stripes, int context_row_gap)
{
  /* Ideally, register storage is available for 7 32-bit integers. Two
     are declared inside the "_raw_check_out_" macro.  The order of priority
     for these registers corresponds roughly to the order in which their
     declarations appear below.  Unfortunately, none of these register
     requests are likely to be honored by the register-starved X86 family
     of processors, but the register declarations may prove useful to
     compilers for other architectures or for hand optimizations of
     assembly code. */
  register kdu_int32 *cp = contexts;
  register int c;
  register kdu_int32 cword;
  _raw_check_out_(coder); // Declares t and temp as registers.
  register kdu_int32 *sp = samples;
  register kdu_int32 sym;
  kdu_int32 one_point_five = 1<<p; one_point_five += (one_point_five>>1);
  int r, width_by2=width+width, width_by3=width_by2+width;

  assert((context_row_gap - width) == EXTRA_DECODE_CWORDS);
  for (r=num_stripes; r > 0; r--, cp += EXTRA_DECODE_CWORDS, sp += width_by3)
    for (c=width; c > 0; c--, sp++, cp++)
      {
        if (*cp == 0)
          continue;
        cword = *cp;
        if ((cword & (NBRHD_MASK<<0)) && !(cword & (SIG_PROP_MEMBER_MASK<<0)))
          { // Process first row of stripe column (row 0)
            _raw_dec_(coder,sym);
            if (!sym)
              { cword |= (PI_BIT<<0); goto row_1; }
            // Decode sign bit
            _raw_dec_(coder,sym);
            // Broadcast neighbourhood context changes
            if (!causal)
              {
                cp[-context_row_gap-1] |=(SIGMA_BR_BIT<<9);
                cp[-context_row_gap  ] |=(SIGMA_BC_BIT<<9)|(sym<<NEXT_CHI_POS);
                cp[-context_row_gap+1] |=(SIGMA_BL_BIT<<9);
              }
            cp[-1] |= (SIGMA_CR_BIT<<0);
            cp[1]  |= (SIGMA_CL_BIT<<0);
            cword |= (SIGMA_CC_BIT<<0) | (PI_BIT<<0) | (sym<<CHI_POS);
            sp[0] = (sym<<31) + one_point_five;
          }
row_1:
        if ((cword & (NBRHD_MASK<<3)) && !(cword & (SIG_PROP_MEMBER_MASK<<3)))
          { // Process second row of stripe column (row 1)
            _raw_dec_(coder,sym);
            if (!sym)
              { cword |= (PI_BIT<<3); goto row_2; }
            // Decode sign bit
            _raw_dec_(coder,sym);
            // Broadcast neighbourhood context changes
            cp[-1] |= (SIGMA_CR_BIT<<3);
            cp[1]  |= (SIGMA_CL_BIT<<3);
            cword |= (SIGMA_CC_BIT<<3) | (PI_BIT<<3) | (sym<<(CHI_POS+3));
            sp[width] = (sym<<31) + one_point_five;
          }
row_2:
        if ((cword & (NBRHD_MASK<<6)) && !(cword & (SIG_PROP_MEMBER_MASK<<6)))
          { // Process third row of stripe column (row 2)
            _raw_dec_(coder,sym);
            if (!sym)
              { cword |= (PI_BIT<<6); goto row_3; }
            // Decode sign bit
            _raw_dec_(coder,sym);
            // Broadcast neighbourhood context changes
            cp[-1] |= (SIGMA_CR_BIT<<6);
            cp[1]  |= (SIGMA_CL_BIT<<6);
            cword |= (SIGMA_CC_BIT<<6) | (PI_BIT<<6) | (sym << (CHI_POS+6));
            sp[width_by2] = (sym<<31) + one_point_five;
          }
row_3:
        if ((cword & (NBRHD_MASK<<9)) && !(cword & (SIG_PROP_MEMBER_MASK<<9)))
          { // Process fourth row of stripe column (row 3)
            _raw_dec_(coder,sym);
            if (!sym)
              { cword |= (PI_BIT<<9); goto done; }
            // Decode sign bit
            _raw_dec_(coder,sym);
            // Broadcast neighbourhood context changes
            cp[context_row_gap-1] |= SIGMA_TR_BIT;
            cp[context_row_gap  ] |= SIGMA_TC_BIT | (sym<<PREV_CHI_POS);
            cp[context_row_gap+1] |= SIGMA_TL_BIT;
            cp[-1] |= (SIGMA_CR_BIT<<9);
            cp[1]  |= (SIGMA_CL_BIT<<9);
            cword |= (SIGMA_CC_BIT<<9) | (PI_BIT<<9) | (sym<<(CHI_POS+9));
            sp[width_by3] = (sym<<31) + one_point_five;
          }
done:
        *cp = cword;
      }

  _raw_check_in_(coder);
}

/*****************************************************************************/
/* STATIC                    decode_sig_prop_pass                            */
/*****************************************************************************/

static void
  decode_sig_prop_pass(mq_decoder &coder, mqd_state states[],
                       int p, bool causal, int orientation,
                       kdu_int32 *samples, kdu_int32 *contexts,
                       int width, int num_stripes, int context_row_gap)
{
  /* Ideally, register storage is available for 12 32-bit integers. Four
     are declared inside the "_mq_check_out_" macro.  The order of priority
     for these registers corresponds roughly to the order in which their
     declarations appear below.  Unfortunately, none of these register
     requests are likely to be honored by the register-starved X86 family
     of processors, but the register declarations may prove useful to
     compilers for other architectures or for hand optimizations of
     assembly code. */
  register kdu_int32 *cp = contexts;
  register int c;
  register kdu_int32 cword;
  _mq_check_out_(coder); // Declares A, C, D and t as registers.
  register kdu_int32 sym;
  register kdu_int32 val;
  register  kdu_byte *sig_lut = significance_luts[orientation];
  register kdu_int32 *sp = samples;
  register mqd_state *state_ref;
  kdu_int32 one_point_five = 1<<p; one_point_five += (one_point_five>>1);
  int r, width_by2=width+width, width_by3=width_by2+width;

  assert((context_row_gap - width) == EXTRA_DECODE_CWORDS);
  for (r=num_stripes; r > 0; r--, cp += EXTRA_DECODE_CWORDS, sp += width_by3)
    for (c=width; c > 0; c--, sp++, cp++)
      {
        if (*cp == 0)
          { // Invoke speedup trick to skip over runs of all-0 neighbourhoods
            for (cp+=3; *cp == 0; cp+=3, c-=3, sp+=3);
            cp-=3;
            continue;
          }
        cword = *cp;
        if ((cword & (NBRHD_MASK<<0)) && !(cword & (SIG_PROP_MEMBER_MASK<<0)))
          { // Process first row of stripe column (row 0)
            state_ref = states+KAPPA_SIG_BASE+sig_lut[cword & NBRHD_MASK];
            _mq_dec_(coder,sym,*state_ref);
            if (!sym)
              { cword |= (PI_BIT<<0); goto row_1; }
            // Decode sign bit
            sym = cword & ((CHI_BIT>>3) | (SIGMA_CC_BIT>>3) |
                           (CHI_BIT<<3) | (SIGMA_CC_BIT<<3));
            sym >>= 1; // Shift down so that top sigma bit has address 0
            sym |= (cp[-1] & ((CHI_BIT<<0) | (SIGMA_CC_BIT<<0))) >> (1+1);
            sym |= (cp[ 1] & ((CHI_BIT<<0) | (SIGMA_CC_BIT<<0))) >> (1-1);
            sym |= (sym >> (CHI_POS-1-SIGMA_CC_POS)); // Interleave chi & sigma
            val = sign_lut[sym & 0x000000FF];
            state_ref = states + KAPPA_SIGN_BASE + (val>>1);
            _mq_dec_(coder,sym,*state_ref);
            sym ^= (val & 1); // Sign bit recovered in LSB.
            // Broadcast neighbourhood context changes
            if (!causal)
              {
                cp[-context_row_gap-1] |=(SIGMA_BR_BIT<<9);
                cp[-context_row_gap  ] |=(SIGMA_BC_BIT<<9)|(sym<<NEXT_CHI_POS);
                cp[-context_row_gap+1] |=(SIGMA_BL_BIT<<9);
              }
            cp[-1] |= (SIGMA_CR_BIT<<0);
            cp[1]  |= (SIGMA_CL_BIT<<0);
            cword |= (SIGMA_CC_BIT<<0) | (PI_BIT<<0) | (sym<<CHI_POS);
            sp[0] = (sym<<31) + one_point_five;
          }
row_1:
        if ((cword & (NBRHD_MASK<<3)) && !(cword & (SIG_PROP_MEMBER_MASK<<3)))
          { // Process second row of stripe column (row 1)
            state_ref = states+KAPPA_SIG_BASE+sig_lut[(cword>>3) & NBRHD_MASK];
            _mq_dec_(coder,sym,*state_ref);
            if (!sym)
              { cword |= (PI_BIT<<3); goto row_2; }
            // Decode sign bit
            sym = cword & ((CHI_BIT<<0) | (SIGMA_CC_BIT<<0) |
                           (CHI_BIT<<6) | (SIGMA_CC_BIT<<6));
            sym >>= 4; // Shift down so that top sigma bit has address 0
            sym |= (cp[-1] & ((CHI_BIT<<3) | (SIGMA_CC_BIT<<3))) >> (4+1);
            sym |= (cp[ 1] & ((CHI_BIT<<3) | (SIGMA_CC_BIT<<3))) >> (4-1);
            sym |= (sym >> (CHI_POS-1-SIGMA_CC_POS)); // Interleave chi & sigma
            val = sign_lut[sym & 0x000000FF];
            state_ref = states + KAPPA_SIGN_BASE + (val>>1);
            _mq_dec_(coder,sym,*state_ref);
            sym ^= (val & 1); // Sign bit recovered in LSB.
            // Broadcast neighbourhood context changes
            cp[-1] |= (SIGMA_CR_BIT<<3);
            cp[1]  |= (SIGMA_CL_BIT<<3);
            cword |= (SIGMA_CC_BIT<<3) | (PI_BIT<<3) | (sym<<(CHI_POS+3));
            sp[width] = (sym<<31) + one_point_five;
          }
row_2:
        if ((cword & (NBRHD_MASK<<6)) && !(cword & (SIG_PROP_MEMBER_MASK<<6)))
          { // Process third row of stripe column (row 2)
            state_ref = states+KAPPA_SIG_BASE+sig_lut[(cword>>6) & NBRHD_MASK];
            _mq_dec_(coder,sym,*state_ref);
            if (!sym)
              { cword |= (PI_BIT<<6); goto row_3; }
            // Decode sign bit
            sym = cword & ((CHI_BIT<<3) | (SIGMA_CC_BIT<<3) |
                           (CHI_BIT<<9) | (SIGMA_CC_BIT<<9));
            sym >>= 7; // Shift down so that top sigma bit has address 0
            sym |= (cp[-1] & ((CHI_BIT<<6) | (SIGMA_CC_BIT<<6))) >> (7+1);
            sym |= (cp[ 1] & ((CHI_BIT<<6) | (SIGMA_CC_BIT<<6))) >> (7-1);
            sym |= (sym >> (CHI_POS-1-SIGMA_CC_POS)); // Interleave chi & sigma
            val = sign_lut[sym & 0x000000FF];
            state_ref = states + KAPPA_SIGN_BASE + (val>>1);
            _mq_dec_(coder,sym,*state_ref);
            sym ^= (val & 1); // Sign bit recovered in LSB.
            // Broadcast neighbourhood context changes
            cp[-1] |= (SIGMA_CR_BIT<<6);
            cp[1]  |= (SIGMA_CL_BIT<<6);
            cword |= (SIGMA_CC_BIT<<6) | (PI_BIT<<6) | (sym << (CHI_POS+6));
            sp[width_by2] = (sym<<31) + one_point_five;
          }
row_3:
        if ((cword & (NBRHD_MASK<<9)) && !(cword & (SIG_PROP_MEMBER_MASK<<9)))
          { // Process fourth row of stripe column (row 3)
            state_ref = states+KAPPA_SIG_BASE+sig_lut[(cword>>9) & NBRHD_MASK];
            _mq_dec_(coder,sym,*state_ref);
            if (!sym)
              { cword |= (PI_BIT<<9); goto done; }
            // Decode sign bit
            sym = cword & ((CHI_BIT<<6) | (SIGMA_CC_BIT<<6) |
                                0       | (SIGMA_CC_BIT<<12));
            sym >>= 10; // Shift down so that top sigma bit has address 0
            if (cword < 0) // Use the fact that NEXT_CHI_BIT = 31
              sym |= CHI_BIT<<(12-10);
            sym |= (cp[-1] & ((CHI_BIT<<9) | (SIGMA_CC_BIT<<9))) >> (10+1);
            sym |= (cp[ 1] & ((CHI_BIT<<9) | (SIGMA_CC_BIT<<9))) >> (10-1);
            sym |= (sym >> (CHI_POS-1-SIGMA_CC_POS)); // Interleave chi & sigma
            val = sign_lut[sym & 0x000000FF];
            state_ref = states + KAPPA_SIGN_BASE + (val>>1);
            _mq_dec_(coder,sym,*state_ref);
            sym ^= (val & 1); // Sign bit recovered in LSB.
            // Broadcast neighbourhood context changes
            cp[context_row_gap-1] |= SIGMA_TR_BIT;
            cp[context_row_gap  ] |= SIGMA_TC_BIT | (sym<<PREV_CHI_POS);
            cp[context_row_gap+1] |= SIGMA_TL_BIT;
            cp[-1] |= (SIGMA_CR_BIT<<9);
            cp[1]  |= (SIGMA_CL_BIT<<9);
            cword |= (SIGMA_CC_BIT<<9) | (PI_BIT<<9) | (sym<<(CHI_POS+9));
            sp[width_by3] = (sym<<31) + one_point_five;
          }
done:
        *cp = cword;
      }

  _mq_check_in_(coder);
}

/*****************************************************************************/
/* STATIC                    decode_mag_ref_pass_raw                         */
/*****************************************************************************/

static void
  decode_mag_ref_pass_raw(mq_decoder &coder, int p, bool causal,
                          kdu_int32 *samples, kdu_int32 *contexts,
                          int width, int num_stripes, int context_row_gap)
{
  /* Ideally, register storage is available for 7 32-bit integers.
     Four 32-bit integers are declared inside the "_raw_check_out_" macro.
     The order of priority for these registers corresponds roughly to the
     order in which their declarations appear below.  Unfortunately, none
     of these register requests are likely to be honored by the
     register-starved X86 family of processors, but the register
     declarations may prove useful to compilers for other architectures or
     for hand optimizations of assembly code. */
  register kdu_int32 *cp = contexts;
  register int c;
  register kdu_int32 cword;
  _raw_check_out_(coder); // Declares t and temp as registers.
  register kdu_int32 *sp = samples;
  register kdu_int32 sym;
  kdu_int32 half_lsb = (1<<p)>>1;
  int r, width_by2=width+width, width_by3=width_by2+width;

  assert((context_row_gap - width) == EXTRA_DECODE_CWORDS);
  for (r=num_stripes; r > 0; r--, cp += EXTRA_DECODE_CWORDS, sp += width_by3)
    for (c=width; c > 0; c--, sp++, cp++)
      {
        if ((*cp & ((MU_BIT<<0)|(MU_BIT<<3)|(MU_BIT<<6)|(MU_BIT<<9))) == 0)
          { // Invoke speedup trick to skip over runs of all-0 neighbourhoods
            for (cp+=2; *cp == 0; cp+=2, c-=2, sp+=2);
            cp-=2;
            continue;
          }
        cword = *cp;
        if (cword & (MU_BIT<<0))
          { // Process first row of stripe column
            _raw_dec_(coder,sym);
            sym = (1-sym)<<p;
            sym ^= sp[0];
            sym |= half_lsb;
            sp[0] = sym;
          }
        if (cword & (MU_BIT<<3))
          { // Process second row of stripe column
            _raw_dec_(coder,sym);
            sym = (1-sym)<<p;
            sym ^= sp[width];
            sym |= half_lsb;
            sp[width] = sym;
          }
        if (cword & (MU_BIT<<6))
          { // Process third row of stripe column
            _raw_dec_(coder,sym);
            sym = (1-sym)<<p;
            sym ^= sp[width_by2];
            sym |= half_lsb;
            sp[width_by2] = sym;
          }
        if (cword & (MU_BIT<<9))
          { // Process fourth row of stripe column
            _raw_dec_(coder,sym);
            sym = (1-sym)<<p;
            sym ^= sp[width_by3];
            sym |= half_lsb;
            sp[width_by3] = sym;
          }
      }

  _raw_check_in_(coder);
}

/*****************************************************************************/
/* STATIC                     decode_mag_ref_pass                            */
/*****************************************************************************/

static void
  decode_mag_ref_pass(mq_decoder &coder, mqd_state states[],
                      int p, bool causal, kdu_int32 *samples,
                      kdu_int32 *contexts, int width, int num_stripes,
                      int context_row_gap)
{
  /* Ideally, register storage is available for 11 32-bit integers.
     Four 32-bit integers are declared inside the "_mq_check_out_" macro.
     The order of priority for these registers corresponds roughly to the
     order in which their declarations appear below.  Unfortunately, none
     of these register requests are likely to be honored by the
     register-starved X86 family of processors, but the register
     declarations may prove useful to compilers for other architectures or
     for hand optimizations of assembly code. */
  register kdu_int32 *cp = contexts;
  register int c;
  register kdu_int32 cword;
  _mq_check_out_(coder); // Declares A, C, D and t as registers.
  register kdu_int32 *sp = samples;
  register mqd_state *state_ref;
  register kdu_int32 sym;
  register kdu_int32 val;
  kdu_int32 half_lsb = (1<<p)>>1;
  int r, width_by2=width+width, width_by3=width_by2+width;

  states += KAPPA_MAG_BASE;
  assert((context_row_gap - width) == EXTRA_DECODE_CWORDS);
  for (r=num_stripes; r > 0; r--, cp += EXTRA_DECODE_CWORDS, sp += width_by3)
    for (c=width; c > 0; c--, sp++, cp++)
      {
        if ((*cp & ((MU_BIT<<0)|(MU_BIT<<3)|(MU_BIT<<6)|(MU_BIT<<9))) == 0)
          { // Invoke speedup trick to skip over runs of all-0 neighbourhoods
            for (cp+=2; *cp == 0; cp+=2, c-=2, sp+=2);
            cp-=2;
            continue;
          }
        cword = *cp;
        if (cword & (MU_BIT<<0))
          { // Process first row of stripe column
            val = sp[0];
            sym = (val & KDU_INT32_MAX) >> p;
            state_ref = states;
            if (sym < 4)
              {
                if (cword & (NBRHD_MASK<<0))
                  state_ref++;
              }
            else
              state_ref += 2;
            _mq_dec_(coder,sym,*state_ref);
            val ^= ((1-sym)<<p);
            val |= half_lsb;
            sp[0] = val;
          }
        if (cword & (MU_BIT<<3))
          { // Process second row of stripe column
            val = sp[width];
            sym = (val & KDU_INT32_MAX) >> p;
            state_ref = states;
            if (sym < 4)
              {
                if (cword & (NBRHD_MASK<<3))
                  state_ref++;
              }
            else
              state_ref += 2;
            _mq_dec_(coder,sym,*state_ref);
            val ^= ((1-sym)<<p);
            val |= half_lsb;
            sp[width] = val;
          }
        if (cword & (MU_BIT<<6))
          { // Process third row of stripe column
            val = sp[width_by2];
            sym = (val & KDU_INT32_MAX) >> p;
            state_ref = states;
            if (sym < 4)
              {
                if (cword & (NBRHD_MASK<<6))
                  state_ref++;
              }
            else
              state_ref += 2;
            _mq_dec_(coder,sym,*state_ref);
            val ^= ((1-sym)<<p);
            val |= half_lsb;
            sp[width_by2] = val;
          }
        if (cword & (MU_BIT<<9))
          { // Process fourth row of stripe column
            val = sp[width_by3];
            sym = (val & KDU_INT32_MAX) >> p;
            state_ref = states;
            if (sym < 4)
              {
                if (cword & (NBRHD_MASK<<9))
                  state_ref++;
              }
            else
              state_ref += 2;
            _mq_dec_(coder,sym,*state_ref);
            val ^= ((1-sym)<<p);
            val |= half_lsb;
            sp[width_by3] = val;
          }
      }

  _mq_check_in_(coder);
}

/*****************************************************************************/
/* STATIC                     decode_cleanup_pass                            */
/*****************************************************************************/

static void
  decode_cleanup_pass(mq_decoder &coder, mqd_state states[],
                      int p, bool causal, int orientation,
                      kdu_int32 *samples, kdu_int32 *contexts,
                      int width, int num_stripes, int context_row_gap)
{
  /* Ideally, register storage is available for 12 32-bit integers and
     at least the first 32-bit word of the `run_state' variable.  Four
     32-bit integers are declared inside the "_mq_check_out_" macro.  The
     order of priority for these registers corresponds roughly to the
     order in which their declarations appear below.  Unfortunately, none
     of these register requests are likely to be honored by the
     register-starved X86 family of processors, but the register
     declarations may prove useful to compilers for other
     architectures or for hand optimizations of assembly code. */
  register kdu_int32 *cp = contexts;
  register int c;
  register kdu_int32 cword;
  _mq_check_out_(coder); // Declares A, C, D and t as registers.
  register mqd_state run_state = states[KAPPA_RUN_BASE]; // 64-bit register
  register kdu_int32 sym;
  register kdu_int32 val;
  register kdu_byte *sig_lut = significance_luts[orientation];
  register kdu_int32 *sp = samples;
  register mqd_state *state_ref;
  kdu_int32 one_point_five = 1<<p; one_point_five += (one_point_five>>1);
  int r, width_by2=width+width, width_by3=width_by2+width;

  assert((context_row_gap - width) == EXTRA_DECODE_CWORDS);
  for (r=num_stripes; r > 0; r--, cp += EXTRA_DECODE_CWORDS, sp += width_by3)
    for (c=width; c > 0; c--, sp++, cp++)
      {
        if (*cp == 0)
          { // Enter the run mode.
#ifdef USE_FAST_MACROS // Try to skip over four stripe columns at once
            if ((cp[3] == 0) && ((run_state.p_bar_mps & 1) == 0))
              {
                D -= run_state.p_bar_mps<<2;
                if (D >= 0)
                  { // Succeeded in skipping 4 columns at once!
                    cp += 3; c -= 3; sp += 3;
                    continue;
                  }
                D += run_state.p_bar_mps<<2; // Put back the change.
              }
#endif // USE_FAST_MACROS
            _mq_dec_(coder,sym,run_state);
            if (!sym)
              continue;
            _mq_dec_run_(coder,sym); // Returns run length in `sym'.
            cword = *cp;
            switch (sym) {
              case 0: goto row_0_significant;
              case 1: goto row_1_significant;
              case 2: goto row_2_significant;
              case 3: goto row_3_significant;
              }
          }
        cword = *cp;
        if (!(cword & (CLEANUP_MEMBER_MASK<<0)))
          { // Process first row of stripe column (row 0)
            state_ref = states+KAPPA_SIG_BASE+sig_lut[cword & NBRHD_MASK];
            _mq_dec_(coder,sym,*state_ref);
            if (!sym)
              goto row_1;
row_0_significant:
            // Decode sign bit
            sym = cword & ((CHI_BIT>>3) | (SIGMA_CC_BIT>>3) |
                           (CHI_BIT<<3) | (SIGMA_CC_BIT<<3));
            sym >>= 1; // Shift down so that top sigma bit has address 0
            sym |= (cp[-1] & ((CHI_BIT<<0) | (SIGMA_CC_BIT<<0))) >> (1+1);
            sym |= (cp[ 1] & ((CHI_BIT<<0) | (SIGMA_CC_BIT<<0))) >> (1-1);
            sym |= (sym >> (CHI_POS-1-SIGMA_CC_POS)); // Interleave chi & sigma
            val = sign_lut[sym & 0x000000FF];
            state_ref = states + KAPPA_SIGN_BASE + (val>>1);
            _mq_dec_(coder,sym,*state_ref);
            sym ^= (val & 1); // Sign bit recovered in LSB.
            // Broadcast neighbourhood context changes
            if (!causal)
              {
                cp[-context_row_gap-1] |=(SIGMA_BR_BIT<<9);
                cp[-context_row_gap  ] |=(SIGMA_BC_BIT<<9)|(sym<<NEXT_CHI_POS);
                cp[-context_row_gap+1] |=(SIGMA_BL_BIT<<9);
              }
            cp[-1] |= (SIGMA_CR_BIT<<0);
            cp[1]  |= (SIGMA_CL_BIT<<0);
            cword |= (SIGMA_CC_BIT<<0) | (sym<<CHI_POS);
            sp[0] = (sym<<31) + one_point_five;
          }
row_1:
        if (!(cword & (CLEANUP_MEMBER_MASK<<3)))
          { // Process second row of stripe column (row 1)
            state_ref = states+KAPPA_SIG_BASE+sig_lut[(cword>>3) & NBRHD_MASK];
            _mq_dec_(coder,sym,*state_ref);
            if (!sym)
              goto row_2;
row_1_significant:
            // Decode sign bit
            sym = cword & ((CHI_BIT<<0) | (SIGMA_CC_BIT<<0) |
                           (CHI_BIT<<6) | (SIGMA_CC_BIT<<6));
            sym >>= 4; // Shift down so that top sigma bit has address 0
            sym |= (cp[-1] & ((CHI_BIT<<3) | (SIGMA_CC_BIT<<3))) >> (4+1);
            sym |= (cp[ 1] & ((CHI_BIT<<3) | (SIGMA_CC_BIT<<3))) >> (4-1);
            sym |= (sym >> (CHI_POS-1-SIGMA_CC_POS)); // Interleave chi & sigma
            val = sign_lut[sym & 0x000000FF];
            state_ref = states + KAPPA_SIGN_BASE + (val>>1);
            _mq_dec_(coder,sym,*state_ref);
            sym ^= (val & 1); // Sign bit recovered in LSB.
            // Broadcast neighbourhood context changes
            cp[-1] |= (SIGMA_CR_BIT<<3);
            cp[1]  |= (SIGMA_CL_BIT<<3);
            cword |= (SIGMA_CC_BIT<<3) | (sym<<(CHI_POS+3));
            sp[width] = (sym<<31) + one_point_five;
          }
row_2:
        if (!(cword & (CLEANUP_MEMBER_MASK<<6)))
          { // Process third row of stripe column (row 2)
            state_ref = states+KAPPA_SIG_BASE+sig_lut[(cword>>6) & NBRHD_MASK];
            _mq_dec_(coder,sym,*state_ref);
            if (!sym)
              goto row_3;
row_2_significant:
            // Decode sign bit
            sym = cword & ((CHI_BIT<<3) | (SIGMA_CC_BIT<<3) |
                           (CHI_BIT<<9) | (SIGMA_CC_BIT<<9));
            sym >>= 7; // Shift down so that top sigma bit has address 0
            sym |= (cp[-1] & ((CHI_BIT<<6) | (SIGMA_CC_BIT<<6))) >> (7+1);
            sym |= (cp[ 1] & ((CHI_BIT<<6) | (SIGMA_CC_BIT<<6))) >> (7-1);
            sym |= (sym >> (CHI_POS-1-SIGMA_CC_POS)); // Interleave chi & sigma
            val = sign_lut[sym & 0x000000FF];
            state_ref = states + KAPPA_SIGN_BASE + (val>>1);
            _mq_dec_(coder,sym,*state_ref);
            sym ^= (val & 1); // Sign bit recovered in LSB.
            // Broadcast neighbourhood context changes
            cp[-1] |= (SIGMA_CR_BIT<<6);
            cp[1]  |= (SIGMA_CL_BIT<<6);
            cword |= (SIGMA_CC_BIT<<6) | (sym << (CHI_POS+6));
            sp[width_by2] = (sym<<31) + one_point_five;
          }
row_3:
        if (!(cword & (CLEANUP_MEMBER_MASK<<9)))
          { // Process fourth row of stripe column (row 3)
            state_ref = states+KAPPA_SIG_BASE+sig_lut[(cword>>9) & NBRHD_MASK];
            _mq_dec_(coder,sym,*state_ref);
            if (!sym)
              goto done;
row_3_significant:
            // Decode sign bit
            sym = cword & ((CHI_BIT<<6) | (SIGMA_CC_BIT<<6) |
                                0       | (SIGMA_CC_BIT<<12));
            sym >>= 10; // Shift down so that top sigma bit has address 0
            if (cword < 0) // Use the fact that NEXT_CHI_BIT = 31
              sym |= CHI_BIT<<(12-10);
            sym |= (cp[-1] & ((CHI_BIT<<9) | (SIGMA_CC_BIT<<9))) >> (10+1);
            sym |= (cp[ 1] & ((CHI_BIT<<9) | (SIGMA_CC_BIT<<9))) >> (10-1);
            sym |= (sym >> (CHI_POS-1-SIGMA_CC_POS)); // Interleave chi & sigma
            val = sign_lut[sym & 0x000000FF];
            state_ref = states + KAPPA_SIGN_BASE + (val>>1);
            _mq_dec_(coder,sym,*state_ref);
            sym ^= (val & 1); // Sign bit recovered in LSB.
            // Broadcast neighbourhood context changes
            cp[context_row_gap-1] |= SIGMA_TR_BIT;
            cp[context_row_gap  ] |= SIGMA_TC_BIT | (sym<<PREV_CHI_POS);
            cp[context_row_gap+1] |= SIGMA_TL_BIT;
            cp[-1] |= (SIGMA_CR_BIT<<9);
            cp[1]  |= (SIGMA_CL_BIT<<9);
            cword |= (SIGMA_CC_BIT<<9) | (sym<<(CHI_POS+9));
            sp[width_by3] = (sym<<31) + one_point_five;
          }
done:
        cword |= (cword << (MU_POS - SIGMA_CC_POS)) &
                 ((MU_BIT<<0)|(MU_BIT<<3)|(MU_BIT<<6)|(MU_BIT<<9));
        cword &= ~((PI_BIT<<0)|(PI_BIT<<3)|(PI_BIT<<6)|(PI_BIT<<9));
        *cp = cword;
      }

  states[KAPPA_RUN_BASE] = run_state;
  _mq_check_in_(coder);
}

/* ========================================================================= */
/*                             kdu_block_decoder                             */
/* ========================================================================= */

/*****************************************************************************/
/*                   kdu_block_decoder::kdu_block_decoder                    */
/*****************************************************************************/

kdu_block_decoder::kdu_block_decoder()
{
  state = new kd_block_decoder;
}


/* ========================================================================= */
/*                             kd_block_decoder                              */
/* ========================================================================= */

/*****************************************************************************/
/*                         kd_block_decoder::decode                          */
/*****************************************************************************/

void
  kd_block_decoder::decode(kdu_block *block)
{
  // Get dimensions.
  int num_cols = block->size.x;
  int num_rows = block->size.y;
  int num_stripes = (num_rows+3)>>2;
  int num_samples = (num_stripes<<2)*num_cols;
  int context_row_gap = num_cols + EXTRA_DECODE_CWORDS;
  int num_context_words = (num_stripes+2)*context_row_gap+1;
  
  // Prepare enough storage.
  if (block->max_samples < num_samples)
    block->set_max_samples((num_samples > 4096)?num_samples:4096);
  if (block->max_contexts < num_context_words)
    block->set_max_contexts((num_context_words > 1600)?num_context_words:1600);

  // Start timing loop here, if there is any need for one.
  int cpu_counter = block->start_timing();
  bool error_found;
  do {
      error_found = false;

      // Initialize sample and context word contents
      kdu_int32 *samples = block->sample_buffer;
      memset(samples,0,(size_t)(num_samples<<2));
      kdu_int32 *context_words = block->context_buffer + context_row_gap + 1;
      memset(context_words-1,0,(size_t)((num_stripes*context_row_gap+1)<<2));
      if (num_rows & 3)
        {
          kdu_int32 oob_marker;
          if ((num_rows & 3) == 1) // Last 3 rows of last stripe unoccupied
            oob_marker = (OOB_MARKER<<3) | (OOB_MARKER<<6) | (OOB_MARKER<<9);
          else if ((num_rows & 3) == 2) // Last 2 rows of last stripe are empty
            oob_marker = (OOB_MARKER << 6) | (OOB_MARKER << 9);
          else
            oob_marker = (OOB_MARKER << 9);
          kdu_int32 *cp = context_words + (num_stripes-1)*context_row_gap;
          for (int k=num_cols; k > 0; k--)
            *(cp++) = oob_marker;
        }
      if (context_row_gap > num_cols)
        { // Initialize the extra context words between lines to OOB
          kdu_int32 oob_marker =
            OOB_MARKER | (OOB_MARKER<<3) | (OOB_MARKER<<6) | (OOB_MARKER<<9);
          assert(context_row_gap >= (num_cols+3));
          kdu_int32 *cp = context_words + num_cols;
          for (int k=num_stripes; k > 0; k--, cp+=context_row_gap)
            cp[0] = cp[1] = cp[2] = oob_marker; // Need 3 OOB words after line
        }

      // Determine which passes we can decode and where to put them.

      int p_max = 30 - block->missing_msbs; // Index of most significant plane
      int num_passes = 3*p_max-2; // 1 plane for dequantization signalling
      if (num_passes > block->num_passes)
        num_passes = block->num_passes;

      // Now decode the passes one by one.

      int p = p_max; // Bit-plane counter
      int z = 0; // Coding pass index
      int k=2; // Coding pass category; start with cleanup pass
      int segment_start_z;
      int segment_passes = 0; // Num coding passes in current codeword segment.
      bool segment_truncated = false;
      int segment_bytes = 0;
      kdu_byte *buf = block->byte_buffer;
      bool bypass = false;
      bool causal = (block->modes & Cmodes_CAUSAL) != 0;
      bool er_check = (block->modes & Cmodes_ERTERM) &&
                      (block->fussy || block->resilient);
      for (; z < num_passes; z++, k++)
        {
          if (k == 3)
            { k=0; p--; } // Move on to next bit-plane.
          if (segment_passes == 0)
            { // Need to start a new codeword segment.
              segment_start_z = z;
              segment_passes = 3*(block->K_max_prime-block->missing_msbs) - 2;
              if (block->modes & Cmodes_BYPASS)
                {
                  if (z < 10)
                    segment_passes = 10-z;
                  else if (k == 2) // Cleanup pass.
                    { segment_passes = 1; bypass = false; }
                  else
                    {
                      segment_passes = 2;
                      bypass = true;
                    }
                }
              if (block->modes & Cmodes_RESTART)
                segment_passes = 1;
              segment_truncated = false;
              if ((z+segment_passes) > num_passes)
                {
                  segment_truncated = true;
                  segment_passes = num_passes - z;
                }
              segment_bytes = 0;
              for (int n=0; n < segment_passes; n++)
                segment_bytes += block->pass_lengths[z+n];
              coder.start(buf,segment_bytes,!bypass);
              buf += segment_bytes;
            }
          if ((z == 0) || (block->modes & Cmodes_RESET))
            reset_states();
#ifdef KDU_ASM_OPTIMIZATIONS
          if (mmx_exists)
            { // Invoke highly optimized machine-specific forms where available
              if ((k == 0) && !bypass)
                asm_decode_sig_prop_pass(coder,states,p,causal,
                          block->orientation,samples,context_words,
                          num_cols,num_stripes,context_row_gap);
              else if (k == 0)
                decode_sig_prop_pass_raw(coder,p,causal,samples,
                          context_words,num_cols,num_stripes,context_row_gap);
              else if ((k == 1) && !bypass)
                asm_decode_mag_ref_pass(coder,states,p,causal,samples,
                          context_words,num_cols,num_stripes,context_row_gap);
              else if (k == 1)
                decode_mag_ref_pass_raw(coder,p,causal,samples,
                          context_words,num_cols,num_stripes,context_row_gap);
              else
                asm_decode_cleanup_pass(coder,states,p,causal,
                          block->orientation,samples,context_words,
                          num_cols,num_stripes,context_row_gap);
            }
          else
#endif // KDU_ASM_OPTIMIZATIONS
          if ((k == 0) && !bypass)
            decode_sig_prop_pass(coder,states,p,causal,
                          block->orientation,samples,context_words,
                          num_cols,num_stripes,context_row_gap);
          else if (k == 0)
            decode_sig_prop_pass_raw(coder,p,causal,samples,
                          context_words,num_cols,num_stripes,context_row_gap);
          else if ((k == 1) && !bypass)
            decode_mag_ref_pass(coder,states,p,causal,samples,
                          context_words,num_cols,num_stripes,context_row_gap);
          else if (k == 1)
            decode_mag_ref_pass_raw(coder,p,causal,samples,
                          context_words,num_cols,num_stripes,context_row_gap);
          else
            decode_cleanup_pass(coder,states,p,causal,
                          block->orientation,samples,context_words,
                          num_cols,num_stripes,context_row_gap);
          if ((block->modes & Cmodes_SEGMARK) && (k==2))
            {
              kdu_int32 run, segmark;
              coder.mq_decode_run(run); segmark = run<<2;
              coder.mq_decode_run(run); segmark += run;
              if ((segmark != 0x0A) && (block->fussy || block->resilient))
                { // Segmark not detected correctly.
                  error_found = true;
                  block->num_passes = (z > 2)?(z-2):0;
                  break;
                }
            }
          segment_passes--;
          if (segment_passes == 0)
            {
              if (!coder.finish(er_check && !segment_truncated))
                { // Error has been detected in the current codeword segment.
                  error_found = true;
                  block->num_passes = segment_start_z;
                  break;
                }
            }
        }
      if (error_found)
        {
          if (block->fussy)
            { kdu_error e; e << "Encountered incorrectly terminated codeword "
              "segment, or invalid SEGMARK symbol in code-block bit-stream.  "
              "You may like to use the \"resilient\" mode to recover from "
              "and conceal such errors."; }
            else
              { kdu_warning w; w << "Corrupted block bit-stream detected.\n"; }
        }
    } while (error_found || ((--cpu_counter) > 0));
  block->finish_timing();
}
