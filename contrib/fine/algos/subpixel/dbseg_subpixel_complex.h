#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NR_COMPLEX_H_
#define _NR_COMPLEX_H_

#ifndef _DCOMPLEX_DECLARE_T_
typedef struct DCOMPLEX {double r,i;} dcomplex;
#define _DCOMPLEX_DECLARE_T_
#endif /* _DCOMPLEX_DECLARE_T_ */

#if defined(__STDC__) || defined(ANSI) || defined(NRANSI) /* ANSI */

dcomplex Cadd(dcomplex a, dcomplex b);
dcomplex Csub(dcomplex a, dcomplex b);
dcomplex Cmul(dcomplex a, dcomplex b);
dcomplex Complex(double re, double im);
dcomplex Conjg(dcomplex z);
dcomplex Cdiv(dcomplex a, dcomplex b);
double Cabs(dcomplex z);
dcomplex Csqrt(dcomplex z);
dcomplex RCmul(double x, dcomplex a);

#else /* ANSI */
/* traditional - K&R */

dcomplex Cadd();
dcomplex Csub();
dcomplex Cmul();
dcomplex Complex();
dcomplex Conjg();
dcomplex Cdiv();
double Cabs();
dcomplex Csqrt();
dcomplex RCmul();

#endif /* ANSI */

#endif /* _NR_COMPLEX_H_ */

#ifdef __cplusplus
}
#endif

