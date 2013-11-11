#ifndef _IMGFUNCTIONS_H
#define _IMGFUNCTIONS_H
#pragma warning(disable:4786)

#include "matrix.h"

struct _IplImage;
struct _IplConvKernelFP;
class CImage;

void    print    (char * name,_IplImage *M);
void    print    (char * name,_IplConvKernelFP *k);

void    SaveBmpu    (char * filename,Matrix<uchar> &M);
template<class Tp>
void SaveBmp    (char * filename,Matrix<Tp> &M){
    Matrixu Mu;assign_matrixu(Mu,M);SaveBmpu(filename,Mu);}
void    SaveBmp    (char * filename,Matrixu &R,Matrixu &G,Matrixu &B);
bool    LoadBmp    (char * filename,Matrix<uchar> &M);
bool    LoadBmp    (char * filename,Matrixu &R,Matrixu &G,Matrixu &B);
bool    LoadJpg(char * filename,Matrix<uchar> &M);
bool    LoadJpg(char * filename,Matrixu &R,Matrixu &G,Matrixu &B);
bool    LoadImg(char * filename,Matrix<uchar> &M);

//format conversion
void        Iplf2Image        (_IplImage *I, CImage &J);
void        Ipl2Matrixu        (_IplImage *I, Matrixu &R, Matrixu&G, Matrixu&B);
Matrixu        Ipl2Matrixu        (_IplImage *I);
Matrixf        Ipl2Matrixf        (_IplImage *I);
Matrixf        Iplf2Matrixf    (_IplImage *I);
void        Matrix2Ipl_scale(Matrix<int> &I, _IplImage *Ip);
void        Matrix2Ipl        (Matrix<int> &I, _IplImage *Ip);
void        Matrix2Ipl        (Matrix<__int64> &I, _IplImage *Ip);
void        Matrix2Ipl        (Matrix<uchar> &I, _IplImage *Ip);
void        Matrix2Ipl        (Matrixu &R,Matrixu &G,Matrixu &B, _IplImage *Ip);
void        Matrix2Ipl        (Matrix<float> &I, _IplImage *Ip);
void        Matrix2Ipl        (Matrix<double> &I, _IplImage *Ip);
void        Matrix2Iplf        (Matrix<float> &I, _IplImage *Ip);
void        Matrix2Iplf        (Matrix<uchar> &I, _IplImage *Ip);
_IplImage*    Matrix2Ipl        (Matrix<float> &M);
_IplImage*    Matrix2Iplf        (Matrix<float> &M);
_IplImage*    Matrix2Iplf        (Matrix<uchar> &M);
Matrixf        Matrix2Matrixf    (Matrix<uchar> &M);
Matrixu        Matrix2Matrixu    (Matrix<float> &M);

//kernels

Matrix<double> dog(double sigma);
Matrix<double> doog(double sigma,double orientation,double aspect);
Matrix<double> doog1(double sigma, double orientation,double aspect);
_IplConvKernelFP *ipldog(double xd, double yd,double sigma);
_IplConvKernelFP *ipldoog(double xd, double yd,double sigma,double orientation,double aspect);
_IplConvKernelFP *ipldoog1(double xd, double yd,double sigma,double orientation,double aspect);

#endif

