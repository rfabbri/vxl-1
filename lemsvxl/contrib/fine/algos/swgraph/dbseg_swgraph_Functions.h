#pragma warning(disable:4786)
#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

#include <vector>
#include <set>
#include <map>
#include "matrix.h"
#include "point.h"

using namespace std;
#ifndef PI
#define PI 3.1415926535897931
#endif
#ifndef EXP1
#define EXP1 2.7182818284590452353602874713527
#endif

double    getlogTemp(double startT,double end_t,long step, double leveloff);
double    nrand();
double    randdbl(double d);
int        randint(int n);
bool    inbounds(int rows,int cols,int i,int j);
bool    SampleProb(double p);
double    G(double x, double s);
Matrixd    Sqrt(Matrix<double> &M);
int        SolveSystem(Matrix<double>& A, Matrix<double>& x, Matrix<double>& B);
int        ppgauss(Matrix<double>& A, Matrix<double>& x, Matrix<double>& B);
int        ppgauss(Matrix<long double>& A, Matrix<long double>& x, Matrix<long double>& B);
int        jacobi(double **a, int n, double *d, double **v, int *nrot);
void    svbksb(double **u,double *w,double **v,int m,int n,double *b,double *x);


void    DrawEdges(Matrix<int> &L, Matrix<int> &E,int value);
void    Get_edge(int pdir,int x, int y, int side, int &x0, int &y0, int &x1, int &y1);
void    Merge_Layer_Regions(Matrix<int> &I1, Matrix<int> &I2, Matrix<int> &I);
void    Merge_Layer_Regions1(Matrix<int> &I1, Matrix<int> &I2, Matrix<int> &I);
// i/o
void    print    (char *filename, map<int,int> &m);
void    print    (char *filename, vector<int> &m);
void    print    (char * filename,vector<double> &m);
void    print    (char * filename,Matrix<double> &M);
void    print3    (char * filename,Matrix<double> &M);
void    print10    (char * filename,Matrix<double> &M);
void    print    (char * filename,Matrix<float> &M);
void    print   (char * filename,Matrix<int> &M);
void    print   (char * filename,Matrix<__int64> &M);
void    print    (char * filename,Matrix<uchar> &M);
void    sprint    (char * str,Matrix<double> &M);
void    sprint    (char * str,Matrix<float> &M);
bool    dlmread(char *filename, Matrixd &M, char dlm=',');
bool    dlmread(char *filename, Matrixf &M, char dlm=',');
bool    dlmread(char *filename, Matrixi &M, char dlm=',');
bool    dlmread(char *filename, Matrix<__int64> &M, char dlm=',');
void    lineparse(char *line, char dlm, vector<int> &v);
void    lineparse(char *line,int nmax, char dlm, Matrixf &M);
void    lineparse(char *line,int nmax, char dlm, Matrixd &M);
void    lineparse(char *line,int nmax, char dlm, Matrixi &M);
void    lineparse(char *line,int nmax, char dlm, Matrix<__int64> &M);
//logging
void    clearfile(char * filename);
void    log2file(char * filename,vector<double> &m);
void    log2file(char * filename,double d);
void    log2filei(char * filename,int d);
void    log2filei(char * filename,int d, int d1);
void    log2file(char * filename,double d1,double d2);
void    log2file(char * filename,double d1,double d2, double d3);
void    log2file(char * filename,double d1,double d2, double d3, double d4);
void    log2file(char * filename,char *st);
void    log2file(char * filename,set<int> &s);
void    log2file(char * filename,vector<Point3di> &s);
void    log2file(char * filename,Simple_Matrix<double> &M);
void    log2file(char * filename,Matrixf &M);
void    log2file(char * filename,Matrixi &M);
void    log2file(char * filename,Matrix<__int64> &M);

template<class Tp>
void assign_matrixu(Matrix<uchar> &Mu,Matrix<Tp> &M){
    int i,j,r=M.rows(),c=M.cols();
    int f;
    Mu.SetDimension(r,c);
    for(i=0;i<r;i++)
        for(j=0;j<c;j++){
            f=M.Data(i,j);
            if (f<0) f=0;
            if (f>255) f=255;
            Mu(i,j)=(char)(f);
        }
}

#endif

