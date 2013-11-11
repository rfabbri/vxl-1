#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NUMERIC_H_
#define _NUMERIC_H_

#define INFINITE_SOLUTIONS 2

/**************************complex.h ********************************/
#ifndef _NR_COMPLEX_H_
#define _NR_COMPLEX_H_

#ifndef _FCOMPLEX_DECLARE_T_
typedef struct FCOMPLEX {double r,i;} fcomplex;
#define _FCOMPLEX_DECLARE_T_
#endif /* _FCOMPLEX_DECLARE_T_ */

#if defined(__STDC__) || defined(ANSI) || defined(NRANSI) /* ANSI */

fcomplex Cadd(fcomplex a, fcomplex b);
fcomplex Csub(fcomplex a, fcomplex b);
fcomplex Cmul(fcomplex a, fcomplex b);
fcomplex Complex(double re, double im);
fcomplex Conjg(fcomplex z);
fcomplex Cdiv(fcomplex a, fcomplex b);
double Cabs(fcomplex z);
fcomplex Csqrt(fcomplex z);
fcomplex RCmul(double x, fcomplex a);

#else /* ANSI */
/* traditional - K&R */

fcomplex Cadd();
fcomplex Csub();
fcomplex Cmul();
fcomplex Complex();
fcomplex Conjg();
fcomplex Cdiv();
double Cabs();
fcomplex Csqrt();
fcomplex RCmul();

#endif /* ANSI */

#endif /* _NR_COMPLEX_H_ */



typedef struct {
  double m,b0;
  int type;
}LinearEq;

typedef struct {
  double a,b,c;
}LinearEq2;

typedef struct {
  double a,b,c,d,e,f;
}QuadEq2; 

typedef struct {
  double a,b,c;
}QuadEq1; 

typedef struct {
  double A,B,C,D,E;
}QuartEq; 

typedef struct {
  double a1, a2, a3, a4;
  double b1, b2, b3, b4;
  double c1, c2, c3, c4;
  double d1, d2, d3, d4;
  double e1, e2, e3, e4;
  double f1;
}QuartEq2; 

/**************************complex.h ********************************/


void laguer(fcomplex a[], int m, fcomplex *x, double eps, int polish);
void zroots(fcomplex a[],  int m,fcomplex roots[],int polish);

int solution_of_quad_equation(double A, double B, double C, double *root);
int solution_of_quart_equation(double A, double B, double C, double D, double E,
                   double *roots);
int solve_quad_system_two(QuadEq2 *quadeq2A, QuadEq2 *quadeq2B, double *roots1, 
              double *roots2);


#endif /* NUMERIC */

#ifdef __cplusplus
}
#endif

