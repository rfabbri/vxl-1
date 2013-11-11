//---------------------------------------------------------------------
// This is brcv/rec/dbasn/dbasn_softasgn.h
//:
// \file
// \brief class to implement dbasn_softasgn given a MxN cost matrix  {Aai}
//        find {Mai} that maximizes the energy    sum_ai { Aai*Mai} 
//        This is a modified version of the original dbasn_softasgn algorithm
//        that handles common subgraph isomorphism problem
//      Returns the optimum doubly stochastic matrix that maximizes the
//      assignment energy at A GIVEN TEMPERATURE
//      T is the input temperature
//
// \author
//  O.C. Ozcanli - March 26, 2004
//
// \verbatim
//  Modifications
//      M.C. Chang - April 26, 2004
//      Remove some functions that are not required here.
//      Handle the memory releasing and other fix.
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef dbasn_softasgn_h_
#define dbasn_softasgn_h_

// Note that in 32-bit VC++, long double and double are in fact the same!!
// So no improvement! Bad!!
#define DBASN_USE_LONG_DOUBLE   1

//: Copy the array M2 (of specified size) to M1.
void copy_M (double **M1, double **M2, const int row, const int col);
void copy_M (double **M1, long double **M2, const int row, const int col);
void copy_M (long double **M1, long double **M2, const int row, const int col);

void print_M (double **M, const int row, const int col);
void print_M_bin (double **M, const int row, const int col);

void normalize_M_rows (double **M, const int row, const int col);
void normalize_M_rows (long double **M, const int row, const int col);

void normalize_M_cols (double **M, const int row, const int col);
void normalize_M_cols (long double **M, const int row, const int col);

bool test_M_converge (double **M1, double **M2, 
                      const int row, const int col, const double e);
bool test_M_converge (long double **M1, long double **M2, 
                      const int row, const int col, const double e);

void get_norm_row_M_no_slack (double** M, double** normM, const int row, const int col);

//: Rangarajan's softassign main class
class dbasn_softasgn
{
public:
  int         M_row_;
  int         M_col_;

  //: To store M to the exponential
  //  Use long double for ME_, ME2_.
  long double**        ME_;          
  //: Compare to convergence in to interations.
  long double**        ME2_;

  //####### Constructor/Destructor #######
  dbasn_softasgn (int M_row, int M_col);
  ~dbasn_softasgn ();

  //: return false if numerical explosion happens.
  bool run_assign (double** inputM, double** outputM, 
                   const double& T, const int max_iter, const float eS);  
};


#endif
