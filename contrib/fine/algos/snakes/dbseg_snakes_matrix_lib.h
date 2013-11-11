//#include <vcl_iostream.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MUTU_MATRIX_H_
#define _MUTU_MATRIX_H_



void MAXsub(int m, int n, double **first, double **second, double **result);

void MAXscale(int m, int n, double scalar, double **matrix, double **result);

void MAXcopy(int m, int n, double **original, double **result);

void MAXtrans(int rows, int columns, double **matrix, double **transpose);

void MAXinit(int rows, int columns, double **matrix);

double ** MAXgen(int rows, int columns);    /* newly added */
void   MAXdelete(int rows, int columns, double **mtx); 

void MAXone(int n, double **matrix);

void MAXmult(int rows, int common, int columns, double **matrix1, 
             double **matrix2, double **output);

#define  SMALL 1.0e-20;

/*
void MAXlud(double **mtx, int size, int phy_size, int outvect[], 
            int output);

void MAXlubks(double **mtx, int size, int phy_size, int outvect[], 
              double output[]);
*/

void MAXinv(double **mtx, int size, double **result);   /* newly added */

void MAXeigen_sym(double **mtx, int size, double eigen_val[], double **eigen_vec);

void MAXprint(double **mtx, int rows, int columns);

#endif  /* _MUTU_MATRIX_H_ */



#ifdef __cplusplus
}
#endif

