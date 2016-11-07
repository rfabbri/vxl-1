#ifdef __cplusplus
extern "C" {
#endif

/* Add free of matrix data type, necessary memory manipulation */
/* 7/9/96                       MITL, Z. Lei                   */

/* Added MAXinverse to compute the inverse of a square matrix */
/* 1/18/96                       Zhibin Lei                   */

/* Added MAXeigen_sym to compute eigen values and vectors of real symmetric matrix*/
/* 1/25/96                       Zhibin Lei                   */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "dbseg_snakes_matrix_lib.h"

//#include <vcl_iostream.h>

/*******************************************************/
void
MAXsub(int m, int n, double **first, double **second, double **result)
{
    int i,j;
    
    for (i=0; i<m; i++)
    for (j=0; j<n; j++)
        result[i][j] = first[i][j] - second[i][j];
}

/*******************************************************/
void
MAXscale(int m, int n, double scalar, double **matrix, double **result)
{
    int i,j;
    
    for (i=0; i<m; i++)
    for (j=0;j<n; j++)
        result[i][j] = scalar*matrix[i][j];
}


/*******************************************************/
void
MAXcopy(int m, int n, double **original, double **result)
{
    int i,j;
    
    for (i=0; i<m; i++)
    for (j=0;j<n; j++)
        result[i][j] = original[i][j];
}



/*******************************************************/
void    
MAXtrans(int rows, int columns, double **matrix, double **transpose)       
{
    int    i,j;
    
    for(i=0;i<rows;i++)
    for(j=0;j<columns;j++)
        transpose[j][i] = matrix[i][j];
}

/******************************************************/
void
MAXinit(int rows, int columns, double **matrix)
{
    int i, j;
    for ( i = 0; i < rows; i++ )
    for ( j = 0; j < columns; j++)
        matrix[i][j] = 0.;
}

/******************************************************/
double **
MAXgen(int rows, int columns)
{
    int i, j;
    double **mtx;
    
    if ( (mtx = (double **) malloc (rows * sizeof(double *))) == NULL ){
    printf("Malloc failed in MAXgen(%d,%d): mtx = (double **)...\n", rows, columns);
    exit(1);
    }
    
    for ( i = 0; i < rows; i++ )
    if ( (mtx[i] = (double *) malloc(columns * sizeof(double))) == NULL ){
        printf("Malloc failed in MAXgen(%d,%d): mtx[%d] = (double *)...\n", rows, columns, i);
        exit(1);
    }
    
    for ( i = 0; i < rows; i++ )
    for ( j = 0; j < columns; j++)
        mtx[i][j] = 0.;

    return mtx;
}

/*******************************************************/
/* free up the memory occupied by the matrix structure */
void
MAXdelete(int rows, int columns, double **mtx)
{
    int i,j;

    for ( i = 0; i < rows; i++ )
    free(mtx[i]);
    free(mtx);
    
    return;
}

/******************************************************/
void
MAXone(int n, double **matrix)
{
    int i, j;
    for ( i = 0; i < n; i++ )
    for ( j = 0; j < n; j++ )
        matrix[i][j] = 0.;
    
    for ( i = 0; i < n; i++ )
    matrix[i][i] = 1.0;
}

/*******************************************************/
void    
MAXmult(int rows, int common, int columns, double **matrix1,
    double **matrix2, double **output)
{
   int    i,j,k;
   
   for(i=0;i<rows;i++){
       for(j=0;j<columns;j++){
       output[i][j] = 0.0;
       for(k=0;k<common;k++)
           output[i][j]+=matrix1[i][k]*matrix2[k][j];
       }
   }
}

/***********************************************************/
void    
MAXlud(double **mtx, int size, int phy_size, int outvect[], int output)
{
    int        maxsize=1000, i, j, k, imax; 
    double  VV[1000], aamax, sum, dnum; 
    
    output = 1;          /* No row interchanges yet*/
    
    for(i=0; i<size; i++){
    aamax=0.0;
    for(j=0; j<size; j++){
        if(fabs(mtx[i][j]) > aamax)
        aamax = fabs(mtx[i][j]);
    }
    if(aamax == 0.){
        fprintf(stderr,"ERROR_LUD: singular matrix");
        exit(0);
    }
    VV[i] = 1.0/aamax;
    }
    
    for(j=0; j<size; j++){
    if(j > 0){
        for(i=0; i<j; i++){
        sum = mtx[i][j];
        if(i > 0){
            for(k=0; k<i; k++)
            sum = sum - (mtx[i][k]*mtx[k][j]);
            mtx[i][j] = sum;
        }
        }
    }
    aamax = 0.;
    for(i=j; i<size; i++){
        sum = mtx[i][j];
        if(j > 0){
        for(k=0; k<j; k++)
            sum = sum - (mtx[i][k]*mtx[k][j]);
        mtx[i][j] = sum;
        }
        dnum = VV[i] * fabs(sum);
        if(dnum >= aamax){
        imax = i;
        aamax = dnum;
        }
    }
    if(j != imax){
        for(k=0; k<size; k++){
        dnum = mtx[imax][k];
        mtx[imax][k]=mtx[j][k];
        mtx[j][k] = dnum;
        }
        output = -output;
        VV[imax] = VV[j];
    }
    outvect[j] = imax;
    if(j != (size-1)){
        if(mtx[j][j] == 0.)
        mtx[j][j] = SMALL;
        dnum = 1.0/mtx[j][j];
        for(i=(j+1); i<size; i++)
        mtx[i][j] = mtx[i][j] * dnum;
    }
    }
    if(mtx[(size-1)][(size-1)] == 0.)
    mtx[(size-1)][(size-1)] = SMALL;
}

/**************************************************************/
void    
MAXlubks(double **mtx, int size, int phy_size, int outvect[], 
     double output[])
{
    int    i, j, ii, ll, k;
    double    sum;
    
    ii = -1;  /*when ii is set to a value >= 0 it is an index to
        the first nonvanishing element of the output*/
    for(i=0; i<size; i++){
    ll = outvect[i];
    sum = output[ll];
    output[ll] = output[i];
    if(ii != -1){
        for(j=ii; j<i; j++)
        sum = sum - (mtx[i][j]*output[j]);
    }
    else if(sum != 0.){
        ii = i;}
    output[i] = sum;
    }
    for(i=(size-1); i>-1; i--){
    sum = output[i];
    if(i < (size-1)){
        for(j=(i+1); j < size; j++)
        sum = sum - (mtx[i][j]*output[j]);
    }
    output[i] = sum/mtx[i][i];
    }
}

/****************************************************************/
void 
MAXinv(double **input, int size, double **result)
{
    //double temp[size];
    double* temp = (double *) malloc(size*sizeof(double));
    double **mtx;  /* this is used as the copy of input matrix input*/
    int  D, i, j;
    int* IND = (int *) malloc(size*sizeof(int));
    /* the call to MAXlud will actually change the input matrix mtx,
       so we should pass to it a copy of input matrix instead */
    mtx = (double **) malloc (size* sizeof(double *));
    for ( i = 0; i < size; i++ )
    mtx[i] = (double *) malloc(size*sizeof(double));
    MAXcopy(size, size, input, mtx);
    MAXlud(mtx, size, size, IND, D);
    for ( j = 0; j < size; j++ ){
    for ( i = 0; i < size; i++ )
        temp[i] = 0.0;
    temp[j] = 1.0;
    MAXlubks(mtx, size, size, IND, temp);
    for ( i = 0; i < size; i++ )
        result[i][j] = temp[i];
    }
}

/****************************************************************/
/* Auxliary code for MAXeigen_sym, from numerical recipies in C */
/* Householder reduction of a real, symmetric matrix a[][]. On output, a is 
   replaced by the orthogonal matrix Q effecting the transformation. d[] returns 
   the diagonal elms, e[] off-diag elms of tridiag matrix, with e[1]=0. 
   Notice the different starting index of array in crecipies and in c, off by 1*/ 
void tred2(double **a, int n, double d[], double e[])
{
    int    l,k,j,i;
    double scale,hh,h,g,f;

    for (i=n;i>=2;i--) {
    l=i-1;
    h=scale=0.0;
    if (l > 1) {
        for (k=1;k<=l;k++)
        scale += fabs(a[i][k]);
        if (scale == 0.0)
        e[i]=a[i][l];
        else {
        for (k=1;k<=l;k++) {
            a[i][k] /= scale;
            h += a[i][k]*a[i][k];
        }
        f=a[i][l];
        g = f>0 ? -sqrt(h) : sqrt(h);
        e[i]=scale*g;
        h -= f*g;
        a[i][l]=f-g;
        f=0.0;
        for (j=1;j<=l;j++) {
            /* Next statement can be omitted if eigenvectors not wanted */
            a[j][i]=a[i][j]/h;
            g=0.0;
            for (k=1;k<=j;k++)
            g += a[j][k]*a[i][k];
            for (k=j+1;k<=l;k++)
            g += a[k][j]*a[i][k];
            e[j]=g/h;
            f += e[j]*a[i][j];
        }
        hh=f/(h+h);
        for (j=1;j<=l;j++) {
            f=a[i][j];
            e[j]=g=e[j]-hh*f;
            for (k=1;k<=j;k++)
            a[j][k] -= (f*e[k]+g*a[i][k]);
        }
        }
    } else
        e[i]=a[i][l];
    d[i]=h;
    }
    /* Next statement can be omitted if eigenvectors not wanted */
    d[1]=0.0;
    e[1]=0.0;
    /* Contents of this loop can be omitted if eigenvectors not
       wanted except for statement d[i]=a[i][i]; */
    for (i=1;i<=n;i++) {
    l=i-1;
    if (d[i]) {
        for (j=1;j<=l;j++) {
        g=0.0;
        for (k=1;k<=l;k++)
            g += a[i][k]*a[k][j];
        for (k=1;k<=l;k++)
            a[k][j] -= g*a[k][i];
        }
    }
    d[i]=a[i][i];
    a[i][i]=1.0;
    for (j=1;j<=l;j++) a[j][i]=a[i][j]=0.0;
    }
}

#define SIGN(a,b) ((b)<0 ? -fabs(a) : fabs(a))

/* QL alg with implicit shifts, to determine the eigenvalues and vectors of a real
   sym tridiag matrix, or of a real, sym matrix previously reduced by tred2. On 
   input, d[] contains diag and e[] subdiag elms. On output d[] return eigenvalues.
   z[][] input and idetical matrix for tridiag matrix, for sym matrix reduced by 
   tred2, z[][] input as the output of tred2, it a[][]. In either case, the kth 
   column of z returns the normalized eigenvector corresponding to d[k] */
void tqli(double d[], double e[], int n, double **z)
{
    int     m,l,iter,i,k;
    double  s,r,p,g,f,dd,c,b;
    /* void nrerror(); */
    
    for (i=2;i<=n;i++) e[i-1]=e[i];
    e[n]=0.0;
    for (l=1;l<=n;l++) {
    iter=0;
    do {
        for (m=l;m<=n-1;m++) {
        dd=fabs(d[m])+fabs(d[m+1]);
        if (fabs(e[m])+dd == dd) break;
        }
        if (m != l) {
        if (iter++ == 30) printf("Too many iterations in TQLI\n");
        g=(d[l+1]-d[l])/(2.0*e[l]);
        r=sqrt((g*g)+1.0);
        g=d[m]-d[l]+e[l]/(g+SIGN(r,g));
        s=c=1.0;
        p=0.0;
        for (i=m-1;i>=l;i--) {
            f=s*e[i];
            b=c*e[i];
            if (fabs(f) >= fabs(g)) {
            c=g/f;
            r=sqrt((c*c)+1.0);
            e[i+1]=f*r;
            c *= (s=1.0/r);
            } else {
            s=f/g;
            r=sqrt((s*s)+1.0);
            e[i+1]=g*r;
            s *= (c=1.0/r);
            }
            g=d[i+1]-p;
            r=(d[i]-g)*s+2.0*c*b;
            p=s*r;
            d[i+1]=g+p;
            g=c*r-b;
            /* Next loop can be omitted if eigenvectors not wanted */
            for (k=1;k<=n;k++) {
            f=z[k][i+1];
            z[k][i+1]=s*z[k][i]+c*f;
            z[k][i]=c*z[k][i]-s*f;
            }
        }
        d[l]=d[l]-p;
        e[l]=g;
        e[m]=0.0;
        }
    } while (m != l);
    }
}

/*******************************************************/
/* Compute the eigen value and eigen vector of a real sym matrix,
   a wrapper around the crecipie code. Input mat mtx with size size, output
   eigen values eigen_val[] and eigen vectors eigen_vec[][], kth column 
   corresponding kth eigen value. (array script as standard c, ie, from 0 */
void 
MAXeigen_sym(double **mtx, int size, double eigen_val[], double **eigen_vec) 
{
    int    i, j;
    //double d[size+1], e[size+1];
    double* d = (double *) malloc((size+1)*sizeof(double));
    double* e = (double *) malloc((size+1)*sizeof(double));
    double **a;
    
    a = MAXgen(size+1, size+1);
    /* because crecipies code array start from 1, so add 1 to c array.
       we'd rather not touch crecipies code */
    for( i = 0; i < size; i++ )
    for ( j = 0; j < size; j++ )
        a[i+1][j+1] = mtx[i][j];
    
    tred2(a, size, d, e);
    tqli(d, e, size, a);
    
    /* copy answers back to the paras */
    for ( i = 0; i < size; i++ ){
    eigen_val[i] = d[i+1];
    for ( j = 0; j < size; j++ )
        eigen_vec[i][j] = a[i+1][j+1];
    }
}


/*******************************************************/
void
MAXprint(double **mtx, int rows, int columns)
{
    int i,j;

    for ( i = 0; i < rows; i++ ){
    for ( j = 0; j < columns; j++)
        printf("%lf   ", mtx[i][j]);
    printf("\n");
    }
}


#ifdef __cplusplus
}
#endif







