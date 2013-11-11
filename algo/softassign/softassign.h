//---------------------------------------------------------------------
// This is algo/softassign/softassign.cxx
//:
// \file
// \brief class to implement softassign given a MxN cost matrix  {Aai}
//        find {Mai} that maximizes the energy    sum_ai { Aai*Mai} 
//        This is a modified version of the original softassign algorithm
//        that handles common subgraph isomorphism problem
//      Returns the optimum doubly stochastic matrix that maximizes the
//      assignment energy at A GIVEN TEMPERATURE
//      T is the input temperature
//
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

#include <vcl_vector.h>

//: constants of the algorithm
//#define e_B (0.5)
#define e_C (0.00005)
#define ETHRES (0.5)

//: main class
class softassign
{
public:
  //: Constructor
  softassign();

  void get_assignment(double **A, double**M, int row_no, int col_no);
  //: adds slack variables and normalizes the rows and columns differently
  void get_assignment_Rangarajan(double **A, double**M, int row_no, int col_no);
  void set_temperature(double T);
  void set_max_iteration(int MI);    // return the current M as solution if max number of iterations is reached

  void get_optimum_permutation(double **A, double **M, int row_no, int col_no);

  double current_energy(double **A, double **M, int row_no, int col_no);

  void fprint_M(double **M, int row_no, int col_no);

  void print_M(double **M, int row_no, int col_no);

  //: just make the strictly row dominant matrix into a permutation matrix by rounding next to nearest integer
  void cleanup(double **M, int row_no, int col_no);

  //: returns 1 if the matrix is strictly row_dominant
  int row_dominant(double **M, int row_no, int col_no);
  int converge(double **M1, double **M2, int row_no, int col_no, const double e);

  //: find max element of the matrix and subtruct from each of its elements 
  void subtruct_max(double **A, int Gn, int gn);
private:

  // function that sets current_M_init matrix to current_M
  void set_matrix(double **M1, double **M2, int row_no, int col_no);

  int almost_doubly_stochastic(double **M, int row_no, int col_no);

  void normalize_rows(double **M, int row_no, int col_no);
  void normalize_cols(double **M, int row_no, int col_no);

  double T; // temperature
  int MI;
  double **current_M;
};
