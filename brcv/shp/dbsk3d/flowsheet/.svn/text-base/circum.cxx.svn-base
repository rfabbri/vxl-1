/*------------------------------------------------------------*\
 * File: circum.c
 *
 * Functions:
 *  tricircumcenter3d
 *  tetcircumcenter
 *  tri_circum_3d
 *  tet_circum
 *
 * History:
 *  #0: April 1998, by Jonathan R Shewchuk <jrs+@cs.cmu.edu>
 *  #1: March 2000, used/edited by F.Leymarie
 *  #2: Modified to test for offshoots to infinity.
 *  #3: Oct. 2001: tri_circum_3d & tet_circum are simplified
 *    versions for faster calls.
 *
\*------------------------------------------------------------*/

#include "circum.h"

/*------------------------------------------------------------*\
 * Function: tricircumcenter3d
 *
 * Usage:  Find the circumcenter of a triangle in 3D.
 *  The result is returned both in terms of XYZ coordinates
 *  and Xi-Eta coordinates, relative to the triangle's
 *  point `a' (i.e., `a' is the origin of both coordinate systems).
 *  Hence, the XYZ coordinates returned are NOT absolute;
 *  one must ADD the COORDINATES of `a' to find the absolute
 *  coordinates of the circumcircle. However, this means that the
 *  result is frequently more accurate than would be possible if
 *  absolute coordinates were returned, due to limited floating-point
 *  precision. In general, the circumradius can be computed much more
 *  accurately.
 *
 *  The Xi-Eta coordinate system is defined in terms of the
 *  triangle. Point `a' is the origin of the coordinate system.
 *  The edge `ab' extends one unit along the Xi axis. The edge `ac'
 *  extends one unit along the Eta axis. These coordinate values
 *  are useful for linear interpolation.
 *
 *  If `xi' is NULL on input, the Xi-Eta coordinates will
 *  NOT be computed.
 *
 *  Inputs: a[3], b[3] & c[3] : 3 (double) floating points (triangle)
 *  Ouputs: circumcenter[3] : 3D coords. relative to a[3]
 *    Xi & Eta : coords in the plane of the triangle.
 *
 *  Returns TRUE is reasonable result,
 *    FALSE if offshoots to infinity.
 *
\*------------------------------------------------------------*/
#if 0
int
tricircumcenter3d(double a[3], double b[3], double c[3],
      double circumcenter[3], double *pXi, double *pEta)
{
  static double dEpsilon = (double) E_EPSILON;
  double xba, yba, zba, xca, yca, zca;
  double balength, calength;
  double xcrossbc, ycrossbc, zcrossbc;
  double denominator;
  double xcirca, ycirca, zcirca;
  double dLength;

  /* Use coordinates relative to point `a' of the triangle. */
  xba = b[0] - a[0];
  yba = b[1] - a[1];
  zba = b[2] - a[2];
  xca = c[0] - a[0];
  yca = c[1] - a[1];
  zca = c[2] - a[2];
  /* Squares of lengths of the edges incident to `a'. */
  balength = xba * xba + yba * yba + zba * zba;
  calength = xca * xca + yca * yca + zca * zca;

  /* Cross product of these edges. */
#ifdef CIRCUM_EXACT
  /* Use orient2d() from http://www.cs.cmu.edu/~quake/robust.html     */
  /*   to ensure a correctly signed (and reasonably accurate) result, */
  /*   avoiding any possibility of division by zero.                  */
  xcrossbc = orient2d(b[1], b[2], c[1], c[2], a[1], a[2]);
  ycrossbc = orient2d(b[2], b[0], c[2], c[0], a[2], a[0]);
  zcrossbc = orient2d(b[0], b[1], c[0], c[1], a[0], a[1]);
#else
  /* Take your chances with floating-point roundoff. */
  xcrossbc = yba * zca - yca * zba;
  ycrossbc = zba * xca - zca * xba;
  zcrossbc = xba * yca - xca * yba;
#endif

  /* Calculate the denominator of the formulae. */
  /* denominator = 0.5 / (xcrossbc * xcrossbc + ycrossbc * ycrossbc +
     zcrossbc * zcrossbc); */
  /* Modified by F.Leymarie to remove offshoots to infinity */

  /* dEpsilon = (double) D_EPSILON; */
  dLength = xcrossbc * xcrossbc + ycrossbc * ycrossbc + zcrossbc * zcrossbc;
  if(fabs(dLength) < dEpsilon) return(FALSE); /* Shoots at infinity */
  
  denominator = 0.5 / dLength;

  /* Calculate offset (from `a') of circumcenter. */
  xcirca = ((balength * yca - calength * yba) * zcrossbc -
            (balength * zca - calength * zba) * ycrossbc) * denominator;
  ycirca = ((balength * zca - calength * zba) * xcrossbc -
            (balength * xca - calength * xba) * zcrossbc) * denominator;
  zcirca = ((balength * xca - calength * xba) * ycrossbc -
            (balength * yca - calength * yba) * xcrossbc) * denominator;
  circumcenter[0] = xcirca;
  circumcenter[1] = ycirca;
  circumcenter[2] = zcirca;

  if(pXi != (double *) NULL) {
    /* To interpolate a linear function at the circumcenter, define a   */
    /*  coordinate system with a Xi-axis directed from `a' to `b' and   */
    /*  an Eta-axis directed from `a' to `c'. The values for Xi and Eta */
    /*  are computed by Cramer's Rule for solving systems of linear     */
    /*  equations.                                                      */

    /* There are three ways to do this calculation - using xcrossbc, */
    /*   ycrossbc, or zcrossbc.  Choose whichever has the largest    */
    /*   magnitude, to improve stability and avoid division by zero. */
    if(((xcrossbc >= ycrossbc) ^ (-xcrossbc > ycrossbc)) &&
       ((xcrossbc >= zcrossbc) ^ (-xcrossbc > zcrossbc))) {
      *pXi = (ycirca * zca - zcirca * yca) / xcrossbc;
      *pEta = (zcirca * yba - ycirca * zba) / xcrossbc;
    }
    else if((ycrossbc >= zcrossbc) ^ (-ycrossbc > zcrossbc)) {
      *pXi = (zcirca * xca - xcirca * zca) / ycrossbc;
      *pEta = (xcirca * zba - zcirca * xba) / ycrossbc;
    }
    else {
      *pXi = (xcirca * yca - ycirca * xca) / zcrossbc;
      *pEta = (ycirca * xba - xcirca * yba) / zcrossbc;
    }
  }

  return(TRUE);
}
#endif
/*------------------------------------------------------------*\
 * Function: tetcircumcenter
 *
 * Usage:  Find the circumcenter of a tetrahedron.
 *
 *  The result is returned both in terms of XYZ coordinates and
 *  Xi-Eta-Zeta coordinates, relative to the tetrahedron's point
 *  `a' (i.e., `a' is the origin of both coordinate systems).
 *  Hence, the XYZ coordinates returned are NOT absolute;
 *  one must ADD the COORDINATES of `a' to find the absolute
 *  coordinates of the circumsphere. However, this means that
 *  the result is frequently more accurate than would be possible
 *  if absolute coordinates were returned, due to limited
 *  floating-point precision. In general, the circumradius can
 *  be computed much more accurately.
 *
 *  The Xi-Eta-Zeta coordinate system is defined in terms of the
 *  tetrahedron. Point `a' is the origin of the coordinate system.
 *  The edge `ab' extends one unit along the Xi axis. The edge `ac'
 *  extends one unit along the Eta axis. The edge `ad' extends one
 *  unit along the Zeta axis. These coordinate values are useful
 *  for linear interpolation.
 *
 *  If `Xi' is NULL on input, the Xi-Eta-Zeta coordinates will not
 *  be computed.
 *
 *  Inputs: a[3], b[3], c[3] & d[3] : 4 (double) floating points (tet)
 *  Ouputs: circumcenter[3] : 3D coords. relative to a[3]
 *    Xi, Eta & Zeta : coords in the frame of the tet.
 *
 *  Returns TRUE is reasonable result, FALSE if offshoots
 *    to infinity.
 *
\*------------------------------------------------------------*/
#if 0
int
tetcircumcenter(double a[3], double b[3], double c[3], double d[3],
    double circumcenter[3], double *pXi, double *pEta,
    double *pZeta)
{
  static double dEpsilon = (double) E_EPSILON;
  double xba, yba, zba, xca, yca, zca, xda, yda, zda;
  double balength, calength, dalength;
  double xcrosscd, ycrosscd, zcrosscd;
  double xcrossdb, ycrossdb, zcrossdb;
  double xcrossbc, ycrossbc, zcrossbc;
  double denominator;
  double xcirca, ycirca, zcirca;
  double dLength;

  /* Use coordinates relative to point `a' of the tetrahedron. */
  xba = b[0] - a[0];
  yba = b[1] - a[1];
  zba = b[2] - a[2];
  xca = c[0] - a[0];
  yca = c[1] - a[1];
  zca = c[2] - a[2];
  xda = d[0] - a[0];
  yda = d[1] - a[1];
  zda = d[2] - a[2];
  /* Squares of lengths of the edges incident to `a'. */
  balength = xba * xba + yba * yba + zba * zba;
  calength = xca * xca + yca * yca + zca * zca;
  dalength = xda * xda + yda * yda + zda * zda;
  /* Cross products of these edges. */
  xcrosscd = yca * zda - yda * zca;
  ycrosscd = zca * xda - zda * xca;
  zcrosscd = xca * yda - xda * yca;
  xcrossdb = yda * zba - yba * zda;
  ycrossdb = zda * xba - zba * xda;
  zcrossdb = xda * yba - xba * yda;
  xcrossbc = yba * zca - yca * zba;
  ycrossbc = zba * xca - zca * xba;
  zcrossbc = xba * yca - xca * yba;

  /* Calculate the denominator of the formulae. */
#ifdef CIRCUM_EXACT
  /* Use orient3d() from http://www.cs.cmu.edu/~quake/robust.html     */
  /*   to ensure a correctly signed (and reasonably accurate) result, */
  /*   avoiding any possibility of division by zero.                  */
  denominator = 0.5 / orient3d(b, c, d, a);
#else
  /* Take your chances with floating-point roundoff. */
  /* denominator = 0.5 / (xba * xcrosscd + yba * ycrosscd + zba * zcrosscd); */
  /* Modified by F.Leymarie to remove offshoots to infinity */

  /* dEpsilon = (double) D_EPSILON; */
  dLength = xba * xcrosscd + yba * ycrosscd + zba * zcrosscd;
  if(fabs(dLength) < dEpsilon) return(FALSE); /* Shoots at infinity */
  denominator = 0.5 / dLength;

#endif

  /* Calculate offset (from `a') of circumcenter. */
  xcirca = (balength * xcrosscd + calength * xcrossdb + dalength * xcrossbc) *
           denominator;
  ycirca = (balength * ycrosscd + calength * ycrossdb + dalength * ycrossbc) *
           denominator;
  zcirca = (balength * zcrosscd + calength * zcrossdb + dalength * zcrossbc) *
           denominator;
  circumcenter[0] = xcirca;
  circumcenter[1] = ycirca;
  circumcenter[2] = zcirca;
  
  if(pXi != (double *) NULL) {
    /* To interpolate a linear function at the circumcenter, define a   */
    /*  coordinate system with a Xi-axis directed from `a' to `b',      */
    /*  an Eta-axis directed from `a' to `c', and a Zeta-axis directed  */
    /*  from `a' to `d'.  The values for Xi, Eta, and Zeta are computed */
    /*  by Cramer's Rule for solving systems of linear equations.       */
    *pXi = (xcirca * xcrosscd + ycirca * ycrosscd + zcirca * zcrosscd) *
           (2.0 * denominator);
    *pEta = (xcirca * xcrossdb + ycirca * ycrossdb + zcirca * zcrossdb) *
           (2.0 * denominator);
    *pZeta = (xcirca * xcrossbc + ycirca * ycrossbc + zcirca * zcrossbc) *
            (2.0 * denominator);
  }

  return(TRUE);
}
#endif
/*------------------------------------------------------------*\
 * Function: tri_circum_3d
 *
 * Usage:  Find the circumcenter of a triangle in 3D.
 *  The result is returned both in terms of XYZ coordinates only
 *  (we drop the Xi-Eta coordinates), relative to point `a'
 *  (i.e., `a' is the origin of both coordinate systems).
 *  Hence, the XYZ coordinates returned are NOT absolute;
 *  one must ADD the COORDINATES of `a' to find the absolute
 *  coordinates of the circumcircle. However, this means that the
 *  result is frequently more accurate than would be possible if
 *  absolute coordinates were returned, due to limited floating-point
 *  precision. In general, the circumradius can be computed much more
 *  accurately.
 *
 *  Inputs: a[3], b[3] & c[3] : 3 (double) floating points (triangle)
 *  Ouputs: circumcenter[3] : 3D coords. relative to a[3]
 *
 *  Returns TRUE is reasonable result,
 *    FALSE if offshoots to infinity.
 *
\*------------------------------------------------------------*/

int
tri_circum_3d(double a[3], double b[3], double c[3], double circumcenter[3])
{
  static double dEpsilon = (double) E_EPSILON;
  double xba, yba, zba, xca, yca, zca;
  double balength, calength;
  double xcrossbc, ycrossbc, zcrossbc;
  double denominator;
  double xcirca, ycirca, zcirca;
  double dLength;

  /* Use coordinates relative to point `a' of the triangle. */
  xba = b[0] - a[0];
  yba = b[1] - a[1];
  zba = b[2] - a[2];
  xca = c[0] - a[0];
  yca = c[1] - a[1];
  zca = c[2] - a[2];
  /* Squares of lengths of the edges incident to `a'. */
  balength = xba * xba + yba * yba + zba * zba;
  calength = xca * xca + yca * yca + zca * zca;

  /* Cross product of these edges. */
#ifdef CIRCUM_EXACT
  /* Use orient2d() from http://www.cs.cmu.edu/~quake/robust.html     */
  /*   to ensure a correctly signed (and reasonably accurate) result, */
  /*   avoiding any possibility of division by zero.                  */
  xcrossbc = orient2d(b[1], b[2], c[1], c[2], a[1], a[2]);
  ycrossbc = orient2d(b[2], b[0], c[2], c[0], a[2], a[0]);
  zcrossbc = orient2d(b[0], b[1], c[0], c[1], a[0], a[1]);
#else
  /* Take your chances with floating-point roundoff. */
  xcrossbc = yba * zca - yca * zba;
  ycrossbc = zba * xca - zca * xba;
  zcrossbc = xba * yca - xca * yba;
#endif

  /* Calculate the denominator of the formulae. */
  /* denominator = 0.5 / (xcrossbc * xcrossbc + ycrossbc * ycrossbc +
     zcrossbc * zcrossbc); */
  /* Modified by F.Leymarie to remove offshoots to infinity */

  /* dEpsilon = (double) D_EPSILON; */
  dLength = xcrossbc * xcrossbc + ycrossbc * ycrossbc + zcrossbc * zcrossbc;
  if(fabs(dLength) < dEpsilon) return(FALSE); /* Shoots at infinity */
  
  denominator = 0.5 / dLength;

  /* Calculate offset (from `a') of circumcenter. */
  xcirca = ((balength * yca - calength * yba) * zcrossbc -
            (balength * zca - calength * zba) * ycrossbc) * denominator;
  ycirca = ((balength * zca - calength * zba) * xcrossbc -
            (balength * xca - calength * xba) * zcrossbc) * denominator;
  zcirca = ((balength * xca - calength * xba) * ycrossbc -
            (balength * yca - calength * yba) * xcrossbc) * denominator;
  circumcenter[0] = xcirca;
  circumcenter[1] = ycirca;
  circumcenter[2] = zcirca;

  return(TRUE);
}

/*------------------------------------------------------------*\
 * Function: tet_circum
 *
 * Usage:  Find the circumcenter of a tetrahedron.
 *
 *  The result is returned both in terms of XYZ coordinates
 *  relative to the tetrahedron's point `a' (i.e., `a' is
 *  the origin of both coordinate systems). Hence, the XYZ
 *  coordinates returned are NOT absolute; one must ADD the
 *  COORDINATES of `a' to find the absolute coordinates of
 *  the circumsphere. However, this means that the result is
 *  frequently more accurate than would be possible if
 *  absolute coordinates were returned, due to limited
 *  floating-point precision. In general, the circumradius
 *  can be computed much more accurately.
 *
 *  Inputs: a[3], b[3], c[3] & d[3] : 4 (double) floating points (tet)
 *  Ouputs: circumcenter[3] : 3D coords. relative to a[3]
 *
 *  Returns TRUE is reasonable result, FALSE if offshoots
 *    to infinity.
 *
\*------------------------------------------------------------*/

int
tet_circum(double a[3], double b[3], double c[3], double d[3],
     double circumcenter[3])
{
  static double dEpsilon = (double) E_EPSILON;
  double xba, yba, zba, xca, yca, zca, xda, yda, zda;
  double balength, calength, dalength;
  double xcrosscd, ycrosscd, zcrosscd;
  double xcrossdb, ycrossdb, zcrossdb;
  double xcrossbc, ycrossbc, zcrossbc;
  double denominator;
  double xcirca, ycirca, zcirca;
  double dLength;

  /* Use coordinates relative to point `a' of the tetrahedron. */
  xba = b[0] - a[0];
  yba = b[1] - a[1];
  zba = b[2] - a[2];
  xca = c[0] - a[0];
  yca = c[1] - a[1];
  zca = c[2] - a[2];
  xda = d[0] - a[0];
  yda = d[1] - a[1];
  zda = d[2] - a[2];
  /* Squares of lengths of the edges incident to `a'. */
  balength = xba * xba + yba * yba + zba * zba;
  calength = xca * xca + yca * yca + zca * zca;
  dalength = xda * xda + yda * yda + zda * zda;
  /* Cross products of these edges. */
  xcrosscd = yca * zda - yda * zca;
  ycrosscd = zca * xda - zda * xca;
  zcrosscd = xca * yda - xda * yca;
  xcrossdb = yda * zba - yba * zda;
  ycrossdb = zda * xba - zba * xda;
  zcrossdb = xda * yba - xba * yda;
  xcrossbc = yba * zca - yca * zba;
  ycrossbc = zba * xca - zca * xba;
  zcrossbc = xba * yca - xca * yba;

  /* Calculate the denominator of the formulae. */
#ifdef CIRCUM_EXACT
  /* Use orient3d() from http://www.cs.cmu.edu/~quake/robust.html     */
  /*   to ensure a correctly signed (and reasonably accurate) result, */
  /*   avoiding any possibility of division by zero.                  */
  denominator = 0.5 / orient3d(b, c, d, a);
#else
  /* Take your chances with floating-point roundoff. */
  /* denominator = 0.5 / (xba * xcrosscd + yba * ycrosscd + zba * zcrosscd); */
  /* Modified by F.Leymarie to remove offshoots to infinity */

  /* dEpsilon = (double) D_EPSILON; */
  dLength = xba * xcrosscd + yba * ycrosscd + zba * zcrosscd;
  if(fabs(dLength) < dEpsilon) return(FALSE); /* Shoots at infinity */
  denominator = 0.5 / dLength;

#endif

  /* Calculate offset (from `a') of circumcenter. */
  xcirca = (balength * xcrosscd + calength * xcrossdb + dalength * xcrossbc) *
           denominator;
  ycirca = (balength * ycrosscd + calength * ycrossdb + dalength * ycrossbc) *
           denominator;
  zcirca = (balength * zcrosscd + calength * zcrossdb + dalength * zcrossbc) *
           denominator;
  circumcenter[0] = xcirca;
  circumcenter[1] = ycirca;
  circumcenter[2] = zcirca;
  
  return(TRUE);
}


/* ----- EoF ---- */
