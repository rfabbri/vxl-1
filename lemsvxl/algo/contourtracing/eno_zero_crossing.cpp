/************************************************************************
 *                  *
 *       Copyright 1996, Brown University, Providence, RI    *
 *                  *
 *  Permission to use and modify this software and its documentation  *
 *  for any purpose other than its incorporation into a commercial  *
 *  product is hereby granted without fee. Recipient agrees not to  *
 *  re-distribute this software or any modifications of this    *
 *  software without the permission of Brown University. Brown    *
 *  University makes no representations or warrantees about the    *
 *  suitability of this software for any purpose.  It is provided  *
 *  "as is" without express or implied warranty. Brown University  *
 *  requests notification of any modifications to this software or  *
 *  its documentation. Notice should be sent to:      *
 *                    *
 *  To:                  *
 *        Software Librarian            *
 *        Laboratory for Engineering Man/Machine Systems,    *
 *        Division of Engineering, Box D,        *
 *        Brown University            *
 *        Providence, RI 02912            *
 *        (401) 863-2118            *
 *        Software_Librarian@lems.brown.edu        *
 *                    *
 *  We will acknowledge all electronic notifications.      *
 *                   *
 ************************************************************************/

#include "1d_eno_types.h"


int
enozerox_check_in_interval(ENO_ZeroCrossing_t *zerox,
         ENO_Polynomial_t const* poly,
         int index,double start,double end)
{
  int in_interval = 0;
  double loc = zerox->loc[index];

  /* If we ignored the leading term of the polynomial, I'm going to be
      a little lenient about the end points. This is necessary because
      the zero crossings shift slightly when you ignore the leading
      polynomial term, possibly placing them outside the interval by a
      very small amount. If this is the case, I'm clamping the return
      location to the appropriate end point */
  if (IS_ALMOST_ZERO(poly->coeff[kSecondOrderIndex])){
    if (IS_ALMOST_EQUAL(loc,start,eno_IntervalTolerance)){
      in_interval++;
      zerox->loc[index] = start;
    }
    else if (IS_ALMOST_EQUAL(loc,end,eno_IntervalTolerance)){
      in_interval++;
      zerox->loc[index] = end; /*was start;  Seth made this change, 7-3-97*/
    }
  }
  
  /* haven't classified location as in interval, is it strictly
     in the interval?*/
  if (!in_interval && IS_IN_INTERVAL(loc,start,end)){
    in_interval++;
  }
  return in_interval;
}

int
enozerox_from_polynomial(ENO_ZeroCrossing_t *zerox,
                         ENO_Polynomial_t const* poly)
{
    double const
      a = poly->coeff[kSecondOrderIndex], /* a */
      b = poly->coeff[kFirstOrderIndex],  /* b */
      c = poly->coeff[kZeroOrderIndex];   /* c */
    double
      discriminant;
    
    discriminant = (b * b) - (4.0 * a * c);
    
    /** initialize assuming no roots found **/
    zerox->cnt = 0;

    if ( discriminant < 0.0){
        /* roots are imaginary, nothing to do */

    } else if (IS_ALMOST_ZERO(a)){
        /* the polynomial is a straight line, so at most one root */
        if (!IS_ALMOST_ZERO(b)){
    double intercept =  -c / b;
    zerox->cnt = 1;
    zerox->loc[0] = intercept;
        }
    } else {
        /* two real roots */
      double root_minus = (-b - sqrt(discriminant))/(2.0 * a);
      double root_plus  = (-b + sqrt(discriminant))/(2.0 * a);
      zerox->cnt = 2;
      zerox->loc[0] = root_minus;
      zerox->loc[1] = root_plus;
    }
    return zerox->cnt;
}

int
enozerox_from_polynomial_in_interval(ENO_ZeroCrossing_t *zeros,
                                     ENO_Polynomial_t *poly,
                                     double start, double end)
{
    int root_cnt;
    int valid_root_cnt = 0;

    root_cnt = enozerox_from_polynomial(zeros,poly);

    if (root_cnt == 0){
        /* only imaginary roots */
    }
    if (root_cnt >= 1){
      if (enozerox_check_in_interval(zeros,poly,0,start,end)){
  valid_root_cnt++;
      }
    }
    if (root_cnt >= 2){
      if (enozerox_check_in_interval(zeros,poly,1,start,end)){
            valid_root_cnt++;
            /* if this the first valid root, store it in loc[0] */
            if (valid_root_cnt == 1){
                zeros->loc[0] = zeros->loc[1];
            }
        }
    }
    zeros->cnt = valid_root_cnt;
    return valid_root_cnt;
}

void enozerox_sort(ENO_ZeroCrossing_t *zerox)
{
    double tmp;
    if (zerox->cnt > 1){
        if (zerox->loc[0] > zerox->loc[1]){
            tmp = zerox->loc[0];
            zerox->loc[0] = zerox->loc[1];
            zerox->loc[1] = tmp;
        }
    }
}

        
void enozerox_remove_duplicates(ENO_ZeroCrossing_t *zerox)
{
    if (zerox->cnt >1){
        if (is_almost_equal(zerox->loc[0],zerox->loc[1],eno_NearZeroValue)){
            zerox->cnt--;
        }
    }
}

void enozerox_combine(ENO_ZeroCrossing_t *dest,
                      ENO_ZeroCrossing_t *z1, ENO_ZeroCrossing_t *z2)
{
    int ii,kk;
    int found;

    dest->cnt = 0;

    
    for (ii=0; ii < z1->cnt ; ii++){
        for(found=0 , kk=dest->cnt-1; (kk>=0) && !found; kk--){
    found = is_almost_equal(dest->loc[kk],z1->loc[ii],eno_NearZeroValue);
        }
        if (!found){
            dest->loc[dest->cnt++] = z1->loc[ii];
        }
        if (dest->cnt > kENOMaxZerox){
            dmessage2("WHOA!! z1 how can you have %d zero crossings?...\n",dest->cnt);
            enozerox_print(z1,stderr);
            enozerox_print(z2,stderr);
            dest->cnt = 0;
            return;
        }
    }
    for (ii=0; ii < z2->cnt ; ii++){
        for(found=0 , kk=dest->cnt-1; (kk>=0) && !found; kk--){
    found =  is_almost_equal(dest->loc[kk],z2->loc[ii],eno_NearZeroValue);
        }
        if (!found){
            dest->loc[dest->cnt++] = z2->loc[ii];
        }
        if (dest->cnt > kENOMaxZerox){
            dmessage2("WHOA!! z2 how can you have %d zero crossings?...\n",dest->cnt);
            enozerox_print(z1,stderr);
            enozerox_print(z2,stderr);
            dest->cnt = 0;
            return;
        }
    }
}


int enozerox_check_for_zerox_in_interval(ENO_Interval_t *prev,
                                         ENO_Interval_t *cur,
                                         ENO_Interval_t *next)
{
    ENO_ZeroCrossing_t zeros;
    
    /* if there is a shock in this interval, use zero crossings of its
       neighboring polynomials */
    if (cur->shock){
        ENO_ZeroCrossing_t prev_zeros,next_zeros;
  double shock_loc = cur->shock->shock_loc;

  /* When there is a shock, Kaleem originally was checking the
     ENTIRE current interval for zerox of the neighboring intervals.

     I'm restricting it to search only PART of the interval for
     zero crossings:
     zerox(prev_interpolant) within (cur_interval_start,shock_loc)
     zerox(next_interpolant) within (shock_loc,cur_interval_end) */
  enozerox_from_polynomial_in_interval(&prev_zeros,&prev->interp.poly,
                                             cur->interp.start,shock_loc);
        enozerox_from_polynomial_in_interval(&next_zeros,&next->interp.poly,
                                             shock_loc,cur->interp.end);
        enozerox_combine(&zeros,&prev_zeros,&next_zeros);
    }
    else{
        /* check for zero crossings using the current interval only */
        enozerox_from_polynomial_in_interval(&zeros,&cur->interp.poly,
                                             cur->interp.start,cur->interp.end);
    }
    if (zeros.cnt > 0){
        enozerox_sort(&zeros);
        enozerox_remove_duplicates(&zeros);
        
        if (cur->zerox == NULL){
            cur->zerox = enoNEW(ENO_ZeroCrossing_t,1);
        }

        enozerox_copy(cur->zerox,&zeros);
#ifdef DEBUG
        fprintf(stderr,"\tfound %d zeros = %g",cur->zerox->cnt,cur->zerox->loc[0]);
        if (zeros.cnt > 1){
            fprintf(stderr,",%g",cur->zerox->loc[1]);
        }
        fprintf(stderr,"\n");
#endif
    }
    
    return zeros.cnt;
}

int
enozerox_print(ENO_ZeroCrossing_t const *zerox,FILE *fp)
{
    int i;
    
    fprintf(fp,"zero(cnt=%d,",zerox->cnt);
    for(i=0; i < zerox->cnt; i++){
        fprintf(fp,"%.15g,",zerox->loc[i]);
    }
    fprintf(fp,")\n");
    return 0;
}
