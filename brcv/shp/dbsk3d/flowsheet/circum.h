#ifndef _SHOCK3D_CIRCUM_H_
#define _SHOCK3D_CIRCUM_H_

#include "sheet_flow_mbuck_mres.h"

/* #ifndef CIRCUM_EXACT
   #define CIRCUM_EXACT */
#ifdef CIRCUM_EXACT
#undef CIRCUM_EXACT
#endif

///int
///tricircumcenter3d(double a[3], double b[3], double c[3],
      ///double circumcenter[3], double *pXi, double *pEta);
///int
///tetcircumcenter(double a[3], double b[3], double c[3], double d[3],
    ///double circumcenter[3], double *pXi, double *pEta,
    ///double *pZeta);
int
tri_circum_3d(double a[3], double b[3], double c[3], double circumcenter[3]);

int
tet_circum(double a[3], double b[3], double c[3], double d[3],
     double circumcenter[3]);

#ifndef D_EPSILON
#define D_EPSILON 0.00001
#endif

#endif /* _SHOCK3D_CIRCUM_H_ */
