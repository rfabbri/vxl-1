//---------------------------------------------------------------------
// This is brcv/rec/dbasn/dbasn_softasgn.cxx
//:
// \file
// \brief class to implement dbasn_softasgn given a MxN cost matrix  {Aai}
//        find {Mai} that maximizes the energy    sum_ai { Aai*Mai} 
//
// \author
//  O.C. Ozcanli - March 26, 2004
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h>   // for rand() function
#include <vcl_vector.h>
#include <vcl_cfloat.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_math.h>

#include <dbasn/dbasn_softasgn.h>

//: Copy the array M2 (of specified size) to M1.
void copy_M (double **targetM, double **fromM, const int row, const int col) 
{
  for (int i=0; i<row; i++) {
    for (int j=0; j<col; j++)  {
      assert (vnl_math_isnan(fromM[i][j]) == false);
      targetM[i][j] = fromM[i][j];
    }
  }
}

void copy_M (double **targetM, long double **fromM, const int row, const int col)
{
  for (int i=0; i<row; i++) {
    for (int j=0; j<col; j++)  {
      assert (vnl_math_isnan(fromM[i][j]) == false);
      targetM[i][j] = double (fromM[i][j]);
    }
  }
}

void copy_M (long double **targetM, long double **fromM, const int row, const int col)
{
  for (int i=0; i<row; i++) {
    for (int j=0; j<col; j++)  {
      assert (vnl_math_isnan(fromM[i][j]) == false);
      targetM[i][j] = fromM[i][j];
    }
  }
}

void print_M (double **M, const int row, const int col) 
{
  vul_printf (vcl_cout,"Matrix value*100. row: %d, col: %d\n", row, col);
  for (int i=0; i<row; i++) {
    for (int j=0; j<col; j++)
      vul_printf (vcl_cout, "%2.0lf ", M[i][j]*100);
    vul_printf (vcl_cout, "\n");
  }
  ///vul_printf (vcl_cout,"\n");
}

void print_M_bin (double **M, const int row, const int col)
{
  vcl_cout << "Matrix row: " << row << " col: " << col << vcl_endl;
  for (int i=0; i<row; i++) {
    for (int j=0; j<col; j++)
      vul_printf (vcl_cout, "%d ", (int) M[i][j]);
    vul_printf (vcl_cout, "\n");
  }
  vul_printf (vcl_cout,"\n");
}

void normalize_M_rows (double **M, const int row, const int col) 
{
  int a, i;
  double sum;
  for (a = 0; a<row; a++) {
    sum = 0.0;
    for(i = 0; i<col; i++) 
      sum += M[a][i];
    for (i = 0; i<col; i++)
      M[a][i] /= sum;
  }
}
void normalize_M_rows (long double **M, const int row, const int col) 
{
  int a, i;
  long double sum;
  for (a = 0; a<row; a++) {
    sum = 0.0;
    for(i = 0; i<col; i++) 
      sum += M[a][i];
    for (i = 0; i<col; i++)
      M[a][i] /= sum;
  }
}

void normalize_M_cols (double **M, const int row, const int col) 
{
  int a, i;
  double sum;
  for (i = 0; i<col; i++) {
    sum = 0.0;
    for(a = 0; a<row; a++) 
      sum += M[a][i];
    for (a = 0; a<row; a++)
      M[a][i] /= sum;
  }
}

void normalize_M_cols (long double **M, const int row, const int col) 
{
  int a, i;
  long double sum;
  for (i = 0; i<col; i++) {
    sum = 0.0;
    for(a = 0; a<row; a++) 
      sum += M[a][i];
    for (a = 0; a<row; a++)
      M[a][i] /= sum;
  }
}

bool test_M_converge (double **M1, double **M2, 
                      const int row, const int col, const double e) 
{
  int i, j;
  double sum = 0.0;

  for (i = 0; i<row; i++)
    for (j = 0; j<col; j++)
      sum += vcl_fabs (M1[i][j] - M2[i][j]);

  if (sum < e) 
    return true;
  else 
    return false;
}

bool test_M_converge (long double **M1, long double **M2, 
                      const int row, const int col, const double e) 
{
  int i, j;
  long double sum = 0.0;

  for (i = 0; i<row; i++)
    for (j = 0; j<col; j++)
      sum += vcl_fabs (M1[i][j] - M2[i][j]);

  if (sum < e) 
    return true;
  else 
    return false;
}

void get_norm_row_M_no_slack (double** M, double** normM, const int row, const int col)
{
  int r, c;

  for (r=0; r<row-1; r++) {
    double sum = 0;
    for (c=0; c<col-1; c++)
      sum += M[r][c];
    for (c=0; c<col-1; c++)
      normM[r][c] = M[r][c] / sum;
  }
}

//###############################################################

//: Constructor
dbasn_softasgn::dbasn_softasgn (int M_row, int M_col)
{
  M_row_ = M_row;
  M_col_ = M_col;

  //allocate memory here
#ifdef DBASN_USE_LONG_DOUBLE
  ME_ = new long double*[M_row];
  ME2_ = new long double*[M_row];
  for (int i=0; i<M_row; i++) {
    ME_[i] = new long double[M_col];
    ME2_[i] = new long double[M_col];
  }
#else
  ME_ = new double*[M_row];
  ME2_ = new double*[M_row];
  for (int i=0; i<M_row; i++) {
    ME_[i] = new double[M_col];
    ME2_[i] = new double[M_col];
  }
#endif
}

dbasn_softasgn::~dbasn_softasgn ()
{
  //: release memory here
  for (int i=0; i<M_row_; i++) {
    delete []ME_[i];
    delete []ME2_[i];
  }
  delete []ME_;
  delete []ME2_;
}

bool dbasn_softasgn::run_assign (double** inputM, double** outputM, 
                                 const double& T, const int max_iter, const float eS)
{
  const double beta = 1 / T;

  for (int i = 0; i<M_row_; i++) {
    for (int j = 0; j<M_col_; j++) {
      ME_[i][j] = vcl_exp (beta*inputM[i][j]);
      //return false if exp. explosion happens.
      if (vnl_math_isfinite(ME_[i][j]) == false)
        return false;
    }
  }

  int iter = 0;
  while (iter <= max_iter) {
    copy_M (ME2_, ME_, M_row_, M_col_);
    normalize_M_cols (ME_, M_row_, M_col_-1); //not including the slack column
    normalize_M_rows (ME_, M_row_-1, M_col_); //not including the slack row

    if (test_M_converge(ME_, ME2_, M_row_, M_col_, eS)) 
      break;
    iter++;
  }
  copy_M (outputM, ME_, M_row_, M_col_);

  return true;
}
