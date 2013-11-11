//---------------------------------------------------------------------
// This is algo/softassign/softassign.cxx
//:
// \file
// \brief class to implement softassign given a MxN cost matrix  {Aai}
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

#include <softassign/softassign.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h>   // for rand() function

  //: Constructor
softassign::softassign() : MI(100), T(1000) {}

void softassign::set_temperature(double T_) { T  = T_; }
void softassign::set_max_iteration(int MI_) { MI = MI_; }

//: A is the benefit matrix of the CURRENT TEMPERATURE 
//  M is the assignment matrix that is to be found (it's rows and cols will sum to a value 
//  LESS THAN OR EQUAL TO 1)
//  M is assumed to be allocated properly. 
void softassign::get_assignment(double **A, double**M, int row_no, int col_no) {

  int i, j, iter;
  double beta = 1/T;

  vcl_srand(5);

  current_M = new double*[row_no];

  for (i = 0; i<row_no; i++)
    current_M[i] = new double[col_no];

  for (i = 0; i<row_no; i++)
    for (j = 0; j<col_no; j++)
      M[i][j] = vcl_exp(beta*A[i][j]);

  vcl_cout << "Benefit matrix A:\n";
  print_M(A, row_no, col_no);
  vcl_cout << "initial M after exponential operation\n";
  print_M(M, row_no, col_no);

  iter = 0;
  while (iter <= MI) {
    normalize_rows(M, row_no, col_no);
    //vcl_cout << "iter: " << iter << " after rows are normalized:\n";
    //print_M(M, row_no, col_no);
    normalize_cols(M, row_no, col_no);
    //vcl_cout << "after cols are normalized:\n";
    //print_M(M, row_no, col_no);
    if (almost_doubly_stochastic(M, row_no, col_no))
      break;
    iter++;
  }

  //vcl_cout << "Final M after " << iter << " iterations :\n";
  //print_M(M, row_no, col_no);

}

void softassign::subtruct_max(double **A, int Gn, int gn) {
  int i,j;
  double max = 0;

  for (i = 0; i<Gn; i++)
    for (j = 0; j<gn; j++)
      if (A[i][j] > max)
        max = A[i][j];

  for (i = 0; i<Gn; i++)
    for (j = 0; j<gn; j++)
      A[i][j] -= max;

}

void softassign::get_assignment_Rangarajan(double **A, double**M, int row_no, int col_no) {
  double **ME, **MEI;
  int i,j, iter;
  double beta = 1/T;

  ME = new double*[row_no];
  MEI = new double*[row_no];
  for (i = 0; i<row_no; i++) {
    ME[i] = new double[col_no];
    MEI[i] = new double[col_no];
  }

  for (i = 0; i<row_no; i++)
    for (j = 0; j<col_no; j++)
      ME[i][j] = vcl_exp(beta*A[i][j]);

  //vcl_cout << "initial ME after exponential operation\n";
  //print_M(ME, row_no, col_no);

  iter = 0;
  while (iter <= MI) {
    set_matrix(MEI, ME, row_no, col_no);
    if (row_no > col_no) {
      normalize_cols(ME, row_no, col_no-1);
      normalize_rows(ME, row_no-1, col_no);
    } else {
      normalize_rows(ME, row_no-1, col_no);
      //print_M(ME, row_no, col_no);
      normalize_cols(ME, row_no, col_no-1);
      //print_M(ME, row_no, col_no);
    }
    //if (almost_doubly_stochastic(ME, row_no, col_no))
    if (converge(ME, MEI, row_no, col_no,e_C))
      break;
    iter++;
  }

  for (i = 0; i<row_no; i++)
    for (j = 0; j<col_no; j++)
      M[i][j] = ME[i][j];

  //vcl_cout << "\nFinal ME after " << iter << " iterations:\n";
  //print_M(ME, row_no, col_no);

}

int softassign::converge(double **M1, double **M2, int row_no, int col_no, const double e) {
  int i, j;

  double sum = 0.0;
  for (i = 0; i<row_no; i++)
    for (j = 0; j<col_no; j++) {
      if (M1[i][j] > M2[i][j])
        sum += M1[i][j] - M2[i][j];
      else
        sum += M2[i][j] - M1[i][j];
    }

  if (sum < e) return 1;
  else return 0;
}

//: find all the permutations and return the one that maximizes assignment energy
//: assume that A is square matrix
void softassign::get_optimum_permutation(double **A, double **M, int row_no, int col_no) {
  int i, j;
  int *a;

  double **temp, energy, max;
  temp = new double*[row_no];
  for (i = 0; i<row_no; i++)
    temp[i] = new double[col_no];

  a = new int[row_no];

  for (i = 0; i<row_no; i++)
    a[i] = i;

  for (i = 0; i<row_no; i++)
    for (j = 0; j<col_no; j++) {
      M[i][j] = 0;
      temp[i][j] = 0;
    }


  max = 0;
  do {
    for (i = 0; i<row_no; i++) {
      temp[i][a[i]] = 1;
    }

    //vcl_cout << "\nNext M:\n";
    //print_M(temp, row_no, col_no);

    energy = current_energy(A, temp, row_no, col_no);
    if (max < energy) {
      max = energy;
      set_matrix(M, temp, row_no, col_no);
    }

    //vcl_cout << "\nenergy: " << energy << " \n";

    for (i = 0; i<row_no; i++)
      for (j = 0; j<col_no; j++)
        temp[i][j] = 0;

  } while (vcl_next_permutation(a, a+row_no));
}

int softassign::almost_doubly_stochastic(double **M, int row_no, int col_no) {
  int i, j;
  double sum;
  for (i = 0; i<row_no; i++) {
    sum = 0;
    for (j = 0; j<col_no; j++)
      sum = sum + M[i][j];
    //vcl_cout << "row: " << i << " sum: " << sum << "\n";
    if (sum > ETHRES)
      return 0;
  }

  for (j = 0; j<col_no; j++) {
    sum = 0;
    for (i = 0; i<row_no; i++)
      sum = sum + M[i][j];
    //vcl_cout << "col: " <<  j << " sum: " << sum << "\n";
    if (sum > ETHRES)
      return 0;
  }

  return 1;
}

//: returns 1 if the matrix is strictly row_dominant
//: find max in each row and make it 1 and other elements in that row 0
//: for each row add all the elements if for any row, the sum is 0 or 2 then return NO
int softassign::row_dominant(double **M, int row_no, int col_no) {
  int j, i, j_ind;
  double max, sum;
  double **temp;

  temp = new double*[row_no];
  for (i = 0; i<row_no; i++)
    temp[i] = new double[col_no];

  for (i = 0; i<row_no; i++)
    for (j = 0; j<col_no; j++)
      temp[i][j] = M[i][j];

  // find max in each row of the matrix
  for (i = 0; i<row_no; i++) {
    max = 0;
    for (j = 0; j<col_no; j++)
      if (temp[i][j] > max) {
        max = temp[i][j];
        j_ind = j;
      }
    for (j = 0; j<col_no; j++)
      temp[i][j] = 0;
    temp[i][j_ind] = 1;
  }

  for (j = 0; j<col_no; j++) {
    sum = 0;
    for (i = 0; i<row_no; i++)
      sum += temp[i][j];
    if (sum != 1)
      return 0;
  }

  return 1;
}

//: if row_no > col_no, find max in each col and set to 1, set other elements to 0
//  if col_no > row_no, find max in each row and set to 1, set other elements to 0
void softassign::cleanup(double **M, int row_no, int col_no) {
  int j, i,j_ind;
  double max;

  for (i = 0; i<row_no; i++) {
    max = 0;
    for (j = 0; j<col_no; j++)
      if (M[i][j] > max) {
        max = M[i][j];
        j_ind = j;
      }
    for (j = 0; j<col_no; j++)
      M[i][j] = 0;
    M[i][j_ind] = 1;
  }
}

void softassign::normalize_rows(double **M, int row_no, int col_no) {
  int a, i;
  double sum;

  //vcl_cout << "In normalize rows, initial M:\n";
  //print_M(M, row_no, col_no);

  for (a = 0; a<row_no; a++) {
    sum = 0.0;
    for(i = 0; i<col_no; i++)
      sum += M[a][i];

    for (i = 0; i<col_no; i++)  // do not normalize slack
      M[a][i] /= sum;

//    vcl_cout << "after normalization of " << a << " row, M:\n";
//    print_M(M, row_no, col_no);
  }
}

void softassign::normalize_cols(double **M, int row_no, int col_no) {
  int a, i;
  double sum;

  //vcl_cout << "In normalize cols, initial M:\n";
  //print_M(M, row_no, col_no);

  for (i = 0; i<col_no; i++) {
    sum = 0.0;
    for(a = 0; a<row_no; a++)
      sum += M[a][i];

    for (a = 0; a<row_no; a++)
      M[a][i] /= sum;

//    vcl_cout << "after normalization of " << i << " col, M:\n";
//    print_M(M, row_no, col_no);
  }
}

void softassign::set_matrix(double **current_M_init, double **current_M, int row_no, int col_no) {
  int i, j;
  for (i = 0; i<row_no; i++)
    for (j = 0; j<col_no; j++)
      current_M_init[i][j] = current_M[i][j];
}

void softassign::print_M(double **M, int row_no, int col_no) {
  //vcl_cout << "row_no: " << row_no << " col_no: " << col_no << vcl_endl;
  vcl_cout << vcl_endl;
  for (int i = 0; i<row_no; i++) {
    for (int j = 0; j<col_no; j++)
      printf("%.3f\t", M[i][j]);
    vcl_cout << vcl_endl;
  }

}
void softassign::fprint_M(double **M, int row_no, int col_no) {
  int i, j;
  FILE *fp;

  fp = fopen("C:\\matrix.out", "a");

  for (i = 0; i<row_no; i++) {
    fprintf(fp, "j:%d\t", i);
    for (j = 0; j<col_no; j++)
      fprintf(fp, "%.3f\t", M[i][j]);
    fprintf(fp, "\n");
  }
  fclose(fp);

}

double softassign::current_energy(double **A, double **M, int row_no, int col_no) {
  int i, j;

  double energy = 0.0;
  for (i = 0; i< row_no; i++)
    for (j = 0; j<col_no; j++)
      energy += A[i][j]*M[i][j];

  return energy;
}
