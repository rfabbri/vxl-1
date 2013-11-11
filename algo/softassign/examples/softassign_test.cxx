//---------------------------------------------------------------------
// This is algo/softassign/softassign_main.cxx
//:
// \file
// \brief the program to test the softassign algorithm
//
// \author
//  O.C. Ozcanli - January 21, 2004
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vul/vul_arg.h>
#include <vul/vul_psfile.h>

#include <softassign/softassign.h>

#include <vcl_vector.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_ctime.h>
#include <vcl_cstdlib.h>

int main( int argc, char* argv[] )
{
  //vul_arg<vcl_string> in_1("-file1","first cem file");
  //vul_arg<vcl_string> in_2("-file2","second cem file");
  //vul_arg_parse(argc, argv);

  //static vcl_string filename1 = in_1();
  //static vcl_string filename2 = in_2();

  int i, j;

  vcl_clock_t t1, t2;
  softassign sa;
  vcl_srand(15);

  double range = 10;
  double **A, **M;
  int row_no = 5;
  int col_no = 3;

  A = new double*[row_no+1];
  M = new double*[row_no+1];

  for (i = 0; i<row_no+1; i++) {
    A[i] = new double[col_no+1];
    M[i] = new double[col_no+1];
  }

  for (i = 0; i<row_no; i++)
    for (j = 0; j<col_no; j++)
      A[i][j] = ((double)vcl_rand() / (double)(RAND_MAX+1)) * 100;

  for (i = 0; i<row_no+1; i++)
    A[i][col_no] = 0;
  for (j = 0; j<col_no+1; j++)
    A[row_no][j] = 0;

  sa.print_M(A, row_no+1, col_no+1);

  /*for (i = 0; i<row_no+col_no; i++)
    for (j = 0; j<col_no+row_no; j++)
      AE[i][j] = 0;// + 0.01*((double)vcl_rand() / (double)(RAND_MAX+1));

  for (i = 0; i<row_no; i++)
    for (j = 0; j<col_no; j++)
      AE[i][j] = A[i][j];

  //vcl_cout << "enlarged A\n";
  //sa.print_M(AE, row_no+col_no, col_no+row_no);
  */

  double T = 100;
  while (T > 0) {
    sa.set_temperature(T);
    sa.get_assignment_Rangarajan(A, M, row_no+1, col_no+1);
    vcl_cout << "Matrix of Rangarajan at T: " << T << "\n";
    sa.print_M(M, row_no+1, col_no+1);
    T -= 10;
  }

  sa.subtruct_max(A, row_no+1, col_no+1);
  sa.print_M(A, row_no+1, col_no+1);
  T = 20;
  sa.set_temperature(T);

  sa.get_assignment_Rangarajan(A, M, row_no+1, col_no+1);
  vcl_cout << "assignment matrix:\n";
  sa.print_M(M, row_no+1, col_no+1);

  sa.get_assignment_Rangarajan(A, A, row_no+1, col_no+1);
  vcl_cout << "assignment matrix as A:\n";
  sa.print_M(A, row_no+1, col_no+1);


  /*while (T > 1.1) {
    T -= 1;
    sa.set_temperature(T);
    sa.get_assignment(M, M, row_no, col_no);

    vcl_cout << "Matrix of softassign at T: " << T << "\n";
    sa.print_M(M, row_no, col_no);
  }*/

  //sa.get_optimum_permutation(AE, ME, row_no+col_no, col_no+row_no);

//  vcl_cout << "Optimum permutation matrix for enlarged A is:\n";
//  sa.print_M(ME, row_no+col_no, col_no+row_no);
  //vcl_cout << "solution of the problem is:\n";
  //sa.print_M(ME, row_no, col_no);

  return 0;
}


