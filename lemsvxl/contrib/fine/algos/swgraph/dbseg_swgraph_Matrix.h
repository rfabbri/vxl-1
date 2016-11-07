// NAME:  Matrix class 
// 
// SYNOPSIS:
// 
// DESCRIPTION:
//    A simple Matrix class with <double> as the data type:
//            - overloaded operators for *, +, -
//            - memeber accesing using (): indexing starts from 0
//            - matrix operations like:    T()      - transpose
//                                        Inv() - inverse
//                                        det() - determinant    
//          - Zero(row,col) - creats null matrix
//          - Identity(n)   - creats Identity matrix of nxn
//
//          The Inversion and det are based on Numerical Recipes.
//
// Author : Zhuowen Tu
// FILES:
//            Matrix.h
//

#ifndef _MATRIX_H
#define _MATRIX_H
#pragma warning(disable:4786)

#include <cmath>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <ios>
#include <functional>
using namespace std;

#define LPI    3.14159265359
#ifndef PI
#define PI 3.1415926535897931
#endif

//typedef unsigned char Octet;
typedef unsigned char uchar;
typedef double* pdouble;


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


template<class Tp>
class Simple_Matrix
{

public:
        //CONSTRUCTION
        Simple_Matrix();
        Simple_Matrix( int row, int col);
        Simple_Matrix( int row, int col,const Tp *pdata);
        Simple_Matrix(const Simple_Matrix& x);
        ~Simple_Matrix();
        void Init(const Simple_Matrix& x);
        void Init(int row, int col);
        
        //OPERATORS
        Simple_Matrix<Tp>&        operator=(const Simple_Matrix<Tp>& x );
        Tp&                operator()(const int row, const int col);
        Tp&                operator()(const long index);
        Tp&                operator[](const long index);
        Tp                operator[](const long index)const;
        void            Copy( const Simple_Matrix<Tp> & array );
        void            Copy(const Tp * pData, int nSize, int nGrowBy = -1);
        
        int                SetDimension(const int row, const int col=0);
        void            GetDimension(int& row, int& col) const; 
        long            rows() const;
        long            cols() const;
        int                nx() const{return ncol;};
        int                ny() const{return nrow;};
        inline long        size(void) const {return rows()*cols();};
        inline long        GetSize(void) const {return rows()*cols();};
        virtual int        MatrixFree();
        
        inline bool        Valid(const int row, const int col);
        inline bool        Valid(const int row);
        inline Tp        Data(const int row, const int col) const;
        inline Tp        Data(const int index) const;
        inline    void    init_value(Tp value);
        virtual void    GetPos(const long index, long &j, long &i);
        Simple_Matrix    T() const;
        Simple_Matrix    rot90(int K=1) const;
        Simple_Matrix    Crop(int x0, int y0, int x1, int y1);
        Tp *            GetData() const{return data;}
        inline Tp        GetAt(int nIndex) const{return Data(nIndex);}
        // Iterator functions
                

protected:
        virtual int    MatrixAlloc(const long lSize);
        virtual int MatrixAlloc(const int row, const int col);

public:
        int        nrow, ncol ;
        Tp        *data ;

        class iterator{
        public:
        //    Simple_Matrix<Tp> *parent;
            int i;
            int nx;

            void    operator++(){i++;}
            bool    operator!=(const iterator &p){return (i!=p.i);}
            bool    operator<(const iterator &p){return (i<p.i);}
            iterator & operator=(const iterator &p){
                if(this!=&p){i=p.i;nx=p.nx;}
                return *this;
            };
            int        Get_x(){return i%nx;}
            int        Get_y(){return i/nx;}
            int        Get_i(int x,int y){return y*nx+x;}
            void    Set(int nx1){nx=nx1;i=0;}
            void    Set(int i1, int nx1){nx=nx1;i=i1;}
            void    Set(int x, int y, int nx1){nx=nx1;i=Get_i(x,y);}
        };
        class roi_iterator:public iterator{
        public:    // Region of Interest iterator, iterates through a window in Edge_Matrix parent
            int x0,y0,x1,y1;
            void operator++(){
                int xi=Get_x(),yi=Get_y();
                if (xi+1>=x1){i=Get_i(x0,yi+1);    }
                else{i++;}
            }
            bool valid(){int x=Get_x(),y=Get_y();return (x0<=x&&x<x1&&y0<=y&&y<y1);}
            roi_iterator & operator=(const roi_iterator &p){
                if(this!=&p){i=p.i;nx=p.nx;x0=p.x0;y0=p.y0;x1=p.x1;y1=p.y1;}
                return *this;
            };
            void    Set(int x_0, int y_0, int x_1, int y_1, int nx1){
                nx=nx1;    x0=x_0;y0=y_0;x1=x_1;y1=y_1;i=Get_i(x0,y0);}
        };
        Simple_Matrix<Tp>::iterator begin(){Simple_Matrix<Tp>::iterator mi;mi.i=0;return mi;}
        Simple_Matrix<Tp>::iterator end(){Simple_Matrix<Tp>::iterator mi;mi.i=rows()*cols();return mi;}
        Simple_Matrix<Tp>::roi_iterator begin(int x0,int y0,int x1,int y1){
            Simple_Matrix<Tp>::roi_iterator mi;
            mi.x0=x0;mi.y0=y0;mi.x1=x1;mi.y1=y1;
            mi.i=y0*nx()+x0;mi.nx=nx();
            return mi;
        }
        Simple_Matrix<Tp>::roi_iterator end(int x0,int y0,int x1,int y1){
            Simple_Matrix<Tp>::roi_iterator mi;
            mi.x0=x0;mi.y0=y0;mi.x1=x1;mi.y1=y1;
            mi.i=(y1-1)*nx()+x0;mi.nx=nx();
            return mi;
        }
};

template<class Tp>
class Matrix:public Simple_Matrix<Tp>
{

public:
        //CONSTRUCTION
    Matrix(){
        Simple_Matrix<Tp>();
    }
    Matrix( int row, int col){
        Init(row,col);
    }
    Matrix( int row, int col,const Tp *pdata):Simple_Matrix<Tp>(row,col,pdata){
    }
    Matrix(const Matrix& x){
        Init(x);
    }
    Matrix(const Simple_Matrix& x){
        Init(x);
    }
        
        //OPERATORS
         bool            operator==(const Simple_Matrix<Tp>& x );
        friend std::istream& operator>>(std::istream& s, Matrix& x);
        friend std::ostream& operator<<(std::ostream& s, Matrix& x);
//        operator>>(std::ofstream &strm);
        operator<<(std::ifstream &strm);
        operator>>(FILE &strm);
        operator<<(FILE &strm);


//        virtual bool    Save(char *fname);

        
        virtual void    Identity(void);                    // set this matrix to be I matrix
        double            Det() const;    
        Tp                Trace() const;    
        double            DetNoZero() const;
           Matrix            Inv() const;
        virtual void    Zero(void);                        // set this matrix to be zero matrix
                Tp        SubPixel(const double x, const double y) const;
                bool    IsLocalMax(int x, int y);
        virtual Tp        GetMin(void) const;
        virtual Tp        GetMax(void) const;
        virtual Tp        GetMax(int &maxi) const;
                Tp        Max(int nStartIdx, int nNumElems) const;
        virtual Tp        GetMin(int &mini) const;
        virtual double    Product(void) const;
                double    Product(int nStartIdx, int nNumElems) const;
        virtual double    Sum(void) const;
                double    Sum(int nStartIdx, int nNumElems) const;
                double    SumAbs(int nStartIdx, int nNumElems) const;
                double    SumAbs() const;
        virtual double    SumSq(void) const;
        virtual double    SumPow(double d) const;
        virtual Tp        Mean(void) const;
        virtual Tp        Var(void) const;
        virtual Matrix<Tp>    mabs(void) const;
        virtual Tp        DotProduct(const Matrix<Tp> &kern, const long jshift=0, const long ishift=0);
        virtual Tp        DotProductR(const Matrix<Tp> &kern, const long jshift=0, const long ishift=0);
        virtual Tp        Dist(const Matrix<Tp> &a);

        // convolution
        virtual Matrix    Conv(const Simple_Matrix<Tp>& kern);        // two dimensioanl convolution
//        virtual Matrix<irmm_Complex>    ConvFFT(const Matrix& kern);    // two dimensioanl convolution using FFT
        virtual Matrix    ConvHor(const Simple_Matrix<Tp>& kern);    // horizontal convolution 
        virtual Matrix    ConvVer(const Simple_Matrix<Tp>& kern);    // vertical convolution
        virtual Matrix    Conv2(const Simple_Matrix<Tp>& kern);    
        virtual void    NormToZero(void);                // make sum up to zero
        virtual bool    SameColValue(Tp &x, const int i=0);
        virtual Matrix<Tp>    Rotate(const double angle_degree);    // rotate the matrix
                Matrix    Enlarge(int ntimes);
                Matrix    Reduce(int ntimes);

        // kernel generation
        virtual void    SetGaussian(int dim=1, double sigma=2, int derivative=0);    // Gaussian kernel
        virtual void    SetGabor(const double k, const double sigma, const double theta=0 /* in degree */);
        virtual void    SetLaplacianOfGaussian(const double sigma);
                Matrix    GaussianBlur(double sigma);


        Matrix<Tp>&        operator=(const Simple_Matrix<Tp>& x );
        friend bool        operator==(const Matrix<Tp>& x, const Simple_Matrix<Tp>& y );

        Matrix<int>        operator>(const Tp y);
        Matrix&            operator^(const Simple_Matrix<Tp> y);

#define DEFOP(OP) \
    Matrix operator OP (const Tp val); 
    DEFOP(+);
    DEFOP(-);
    DEFOP(*);
    DEFOP(/);
    DEFOP(^);
#undef DEFOP

    // computed assignment (all element-wise)
#define DEFOP(OP) \
    Matrix& operator OP (const Tp& val); \
    Matrix& operator OP (const Matrix& that);
    DEFOP(+=);
    DEFOP(-=);
    DEFOP(*=);
    DEFOP(/=);
    DEFOP(^=);    // exponentiation, not xor
#undef DEFOP

    // binary operators (all element-wise)
#define DEFOP(OP) \
    friend Matrix<Tp> operator OP (const Simple_Matrix<Tp>& a, const Simple_Matrix<Tp>& b); \
    friend Matrix<Tp> operator OP (const Simple_Matrix<Tp>& a,  Tp b); \
    friend Matrix<Tp> operator OP ( Tp a, const Simple_Matrix& b);
    DEFOP(+);
    DEFOP(-);
    DEFOP(*); // M*N is for matrix product, the rest are elementwise
    DEFOP(&); // & instead of * for product. * is for matrix product
    DEFOP(/);
    DEFOP(^);    // exponentiation, not xor
#undef DEFOP

    // binary operators (all element-wise)
#define DEFOP(OP) \
    friend Matrix<int> operator OP (const Simple_Matrix& a, const Simple_Matrix& b); \
    friend Matrix<int> operator OP (const Simple_Matrix& a, Tp b); \
    friend Matrix<int> operator OP (Tp a, const Matrix& b);
    DEFOP(<);
    DEFOP(>);
    DEFOP(<=);
    DEFOP(>=);
    DEFOP(&&);
    DEFOP(||);
#undef DEFOP

    // unary operators
#define DEFOP(OP) \
    friend Matrix operator OP (const Matrix& a);
    DEFOP(!);
#undef DEFOP

    // misc binary functions
#define DEFOP(OP) \
    friend Matrix OP (const Matrix& a, const Matrix& b); \
    friend Matrix OP (const Matrix& a, double b); \
    friend Matrix OP (double a, const Matrix& b);
    DEFOP(rem);
    DEFOP(mod);
    DEFOP(atan2);
#undef DEFOP
                
        // FFT functions
//        virtual Matrix<irmm_Complex> FFT(void);
//        virtual Matrix<irmm_Complex> IFFT(void);

        // pdf functions
        Tp                PdfNorm1(const Matrix &x);
        Tp                PdfNorm2(const Matrix &x);

protected:
        friend int        LUdcmp(Matrix& X, Matrix& indx, double& d);
        friend int        LUbksb(Matrix& X, Matrix& indx, Matrix& B);
        friend void        Eig(Matrix& a, Matrix& eigenvalue, Matrix& eigenvector);
        friend void        SVD(Matrix& a, Matrix& U, Matrix& D, Matrix& V);
};

typedef Matrix<float> Matrixf;
typedef Matrix<double> Matrixd;
typedef Matrix<int> Matrixi;
typedef Matrix<uchar> Matrixu;



inline int  IsIntegerValue(bool &bintg, const double value);
//inline void    FFT_1D(Matrix<irmm_Complex> & H, const int sign=1,const int t=0);
//inline void    FFT_shift(Matrix<irmm_Complex> & H, const int t=0);
//inline void    IFFT_shift(Matrix<irmm_Complex> & H, const int t=0);

//template<class Tp1, class Tp2> void assign_matrix(Simple_Matrix<Tp1>& x1,const Simple_Matrix<Tp2>& x2);

template<class Tp>
struct lex_less : public binary_function<Matrix<Tp>,Matrix<Tp>, bool> {
    // lexicographic order
    lex_less(){}
    bool operator()(const Matrix<Tp> &p,const  Matrix<Tp>& q) const{
        int i,np=p.size(),nq=q.size();
        if (np!=nq)
            return np<nq;
        for (i=0;i<np;i++){
            if (p.Data(i)>q.Data(i))
                return false;
            if (p.Data(i)<q.Data(i))
                return true;
        }
        return false;
    }
};

template<class Tp1, class Tp2>
void assign_matrix(Simple_Matrix<Tp1>& x1, const Simple_Matrix<Tp2>& x2)
{
    x1.SetDimension(x2.rows(), x2.cols());

    for(int i = 0; i < x2.rows(); i++)
        for(int j = 0; j < x2.cols(); j++)
            x1(i, j) = (Tp1)(x2.Data(i, j));
}

template<class Tp1, class Tp2>
void assign_matrix(Matrix<Tp1>& x1, const Matrix<Tp2>& x2)
{
    x1.SetDimension(x2.rows(), x2.cols());

    for(int i = 0; i < x2.rows(); i++)
        for(int j = 0; j < x2.cols(); j++)
            x1(i, j) = (Tp1)(x2.Data(i, j));
}


template<class Tp>
Simple_Matrix<Tp>::Simple_Matrix()
{  //null matrix
  nrow = 0; ncol = 0;
  data = NULL ;
}

template<class Tp>
Simple_Matrix<Tp>::Simple_Matrix(int row, int col)
{    
    Simple_Matrix();
    Init(row,col);
}
template<class Tp>
Simple_Matrix<Tp>::Simple_Matrix(int row, int col,const Tp *pdata)
{    
    Simple_Matrix();
    nrow = row;
    ncol = col;
    data=pdata;
}
template<class Tp>
void Simple_Matrix<Tp>::Init(int row, int col)
{    
    //initialize Simple_Matrix
    if (row > 0 && col >0)
    {
        nrow = row;
        ncol = col;
        if (MatrixAlloc(nrow, ncol) )
        {
    //        cerr<<"\n Memory allocation Error ! " ;
            exit(-1);
        }
    }
}

template<class Tp>
Simple_Matrix<Tp>::Simple_Matrix(const Simple_Matrix& x)
{//copy constructor
    Init(x);
}
template<class Tp>
void Simple_Matrix<Tp>::Init(const Simple_Matrix& x)
{

    nrow = x.nrow; ncol = x.ncol;
    MatrixAlloc(nrow, ncol);  //allocate memory
    
    for (int i = 0; i < rows(); i++)
        for (int j = 0; j < cols(); j++)
            (*this)(i, j) = x.Data(i, j);
}
  

template<class Tp>
Simple_Matrix<Tp>::~Simple_Matrix()
{  //delete the data
    MatrixFree(); //free the memory
}

template<class Tp>
int Simple_Matrix<Tp>::MatrixFree()
{  //null Simple_Matrix
      
    if (data != NULL)
        delete[] data;           // STEP 2: DELETE THE ROWS
    
    data = NULL;
    nrow = ncol = 0;

    return 0;
}

template<class Tp>
int Simple_Matrix<Tp>::MatrixAlloc(const long lSize)
{
      try
      {                                 //test for exceptions
          data = new Tp[lSize];    //allocate memory for rows
      }
      catch (double*)
      {  // ENTER THIS BLOCK ONLY IF xalloc IS THROWN.
        return -1;
      }
      return 0;
}

template<class Tp>
int Simple_Matrix<Tp>::MatrixAlloc(const int row, const int col)
{
    return MatrixAlloc(((long)row) * col);
}


template<class Tp>
long Simple_Matrix<Tp>::rows() const
{ 
    return nrow;
}

template<class Tp>
long Simple_Matrix<Tp>::cols() const
{ 
    return ncol;
}

template<class Tp>
inline bool    Simple_Matrix<Tp>::Valid(const int row, const int col)
{
    if (row>=0 && row<rows() && col>=0 && col<cols())
        return true;
    else
        return false;
}

template<class Tp>
inline bool    Simple_Matrix<Tp>::Valid(const int row)
{
    if (row>=0 && row<size())
        return true;
    else
        return false;
}

template<class Tp>
int Simple_Matrix<Tp>::SetDimension(const int row, const int col)
{  //set the dimension and allocates the memory of the Simple_Matrix

    if ((nrow!=row)||(ncol!=col)){
         //delete the existing data;
        MatrixFree();

        nrow = row;
        ncol = col;
        
        if(ncol == 0)            //if default column: square Simple_Matrix
            ncol = nrow;

        MatrixAlloc(row, col);
    }

    return 0; //could be used to return the success
                  // or failure of allocation
}

template<class Tp>
void Simple_Matrix<Tp>::GetDimension(int& row, int& col) const
{  //report the dimension of the Simple_Matrix
     row = nrow;
     col = ncol;
}


  
template<class Tp>
Tp& Simple_Matrix<Tp>::operator ()(const int row, const int col)
{    
#ifdef _DEBUG
    assert(row < rows() && row >= 0);
    assert(col < cols() && col >= 0);
#endif

    return data[((long)row) * cols() + col];
}

template<class Tp>
Tp& Simple_Matrix<Tp>::operator ()(const long index)
{    
#ifdef _DEBUG
    assert(index<((long)rows()) * cols() && index>=0);
#endif

    return data[index];
}
template<class Tp>
Tp& Simple_Matrix<Tp>::operator [](const long index)
{    
#ifdef _DEBUG
    assert(index<((long)rows()) * cols() && index>=0);
#endif

    return data[index];
}
template<class Tp>
Tp Simple_Matrix<Tp>::operator [](const long index) const
{    
#ifdef _DEBUG
    assert(index<((long)rows()) * cols() && index>=0);
#endif

    return data[index];
}

template<class Tp>
Tp Simple_Matrix<Tp>::Data(const int row, const int col) const
{
#ifdef _DEBUG
    assert(row < rows() && row >= 0);
    assert(col < cols() && col >= 0);
#endif

    return data[((long)row) * cols() + col];
}
template<class Tp>
Tp Simple_Matrix<Tp>::Data(const int index) const
{
#ifdef _DEBUG
    assert(index>=0 && index<size());
#endif

    return data[index];
}

template<class Tp>
Simple_Matrix<Tp>&    Simple_Matrix<Tp>::operator=(const Simple_Matrix<Tp>& x)
{//performs assignment statement

    //beware of self assignments : x = x
    if ( this != &x )  
    {
        register i,j;

        if (rows()!=x.rows() || cols()!=x.cols())
        {
            //delete the existing data;
            MatrixFree();

            nrow = x.nrow; ncol = x.ncol;
            MatrixAlloc(nrow, ncol); //allocate memory
        }

        for(i=0; i<nrow; i++)
            for(j=0; j<ncol; j++)
                (*this)(i, j) = (Tp)(x.Data(i, j));
    }
    return *this;
}
template<class Tp>
void Simple_Matrix<Tp>::Copy( const Simple_Matrix<Tp> & array )
{
    SetDimension(array.rows(),array.cols());
    Copy(array.GetData(),array.size());
}

template<class Tp>
void Simple_Matrix<Tp>::Copy(const Tp * pData, int nSize, int nGrowBy)
{
    memcpy( m_pArray->GetData(), pData, nSize * sizeof(Tp) );
}

template<class Tp>
Simple_Matrix<Tp> Simple_Matrix<Tp>::T() const
{//transposes of a Simple_Matrix

    Simple_Matrix temp(ncol,nrow); //intialize a temp Simple_Matrix

    for (int i = 0; i < ncol; i++)
        for (int j = 0; j < nrow; j++)
            temp(i, j) = Data(j, i); //transpose elements

     return temp;
}
template<class Tp>
Simple_Matrix<Tp> Simple_Matrix<Tp>::rot90(int K) const{
//      ROT90  Rotate Simple_Matrix 90 degrees.
//    ROT90() is the 90 degree counterclockwise rotation of Simple_Matrix A.
//    ROT90(K) is the K*90 degree rotation of A, K = +-1,+-2,...
 
//    Example,
//        A = [1 2 3      B = rot90(A) = [ 3 6
//             4 5 6 ]                     2 5
//                                         1 4 ]
    int i,j;
    Simple_Matrix temp; //intialize a temp Simple_Matrix
    int k = K%4;
    if (k < 0)
        k += 4;
    if (k==1){
        temp.SetDimension(ncol,nrow);
        for (i = 0; i < ncol; i++)
            for (j = 0; j < nrow; j++)
                temp(i, j) = Data(j, ncol-i-1); //rotate elements

         return temp;
    }
    if (k==2){
        temp.SetDimension(nrow,ncol);
        for (i = 0; i <nrow ; i++)
            for (j = 0; j < ncol; j++)
                temp(i, j) = Data(nrow-i-1, ncol-j-1); //rotate elements

         return temp;
    }
    if (k==3){
        temp.SetDimension(ncol,nrow);
        for (i = 0; i < ncol; i++)
            for (j = 0; j < nrow; j++)
                temp(i, j) = Data(nrow-j-1, i); //rotate elements

         return temp;
    }
    temp.SetDimension(nrow,ncol);
    for (i = 0; i <nrow ; i++)
        for (j = 0; j < ncol; j++)
            temp(i, j) = Data(i, j); 

     return temp;    
}


template<class Tp>
void Simple_Matrix<Tp>::GetPos(const long index, long &j, long &i)
{
    j = index/(cols());
    i = index-j*cols();
}

template<class Tp>
inline    void Simple_Matrix<Tp>::init_value(Tp value)
{
    register i,n;
    n=rows()*cols();

    if (data != NULL)
    {
        for (i = 0; i < n; i++)
                (*this)(i) = value;
    }
}
template<class Tp>
Simple_Matrix<Tp>    Simple_Matrix<Tp>::Crop(int x0, int y0, int x1, int y1){
    Simple_Matrix<Tp> M;
    int i,j,ny1=y1-y0,nx1=x1-x0;
    M.SetDimension(ny1,nx1);
    for (i=0;i<ny1;i++)
        for (j=0;j<nx1;j++){
            M(i,j)=Data(i+y0,j+x0);
        }
    return M;
}

template<class Tp>
Matrix<Tp>&    Matrix<Tp>::operator=(const Simple_Matrix<Tp>& x)
{//performs assignment statement

    //beware of self assignments : x = x
    if ( this != &x )  
    {
        register i,j;

        if (rows()!=x.rows() || cols()!=x.cols())
        {
            //delete the existing data;
            MatrixFree();

            nrow = x.nrow; ncol = x.ncol;
            MatrixAlloc(nrow, ncol); //allocate memory
        }

        for(i=0; i<nrow; i++)
            for(j=0; j<ncol; j++)
                (*this)(i, j) = (Tp)(x.Data(i, j));
    }
    return *this;
}
template<class Tp>
bool    Matrix<Tp>::operator==(const Simple_Matrix<Tp>& x)
{    
    int i,j;
    for(i=0; i<nrow; i++)
        for(j=0; j<ncol; j++)
            if ((*this)(i, j) != x.Data(i, j))
                return false;
    return true;
}
/*
template<class Tp>
Matrix<Tp>::operator>>(std::ofstream &strm)
{
    // dimensions
    strm << rows() << "\n";
    strm << cols() << "\n";
    // data
    for (long j=0; j<size(); j++)
        strm << (*this)(j) << "\n";
}
*/
template<class Tp>
Matrix<Tp>::operator>>(FILE &strm)
{
    long lleft,lgo;

    fwrite(&(nrow),sizeof(nrow),1,&strm);
    fwrite(&(ncol),sizeof(ncol),1,&strm);
    
    lleft = size();
    lgo = 0;
    // data
    while (lleft>0)
    {
        if (lleft>10000)
        {
            fwrite(&(data[lgo]),sizeof(Tp),10000,&strm);
            lgo += 10000;
            lleft -= 10000;
        }
        else
        {
            fwrite(&(data[lgo]),sizeof(Tp),lleft,&strm);
            lleft = 0;
        }
    }
}

template<class Tp>
Matrix<Tp>::operator<<(std::ifstream &strm)
{
    int row_read,col_read;

    MatrixFree();

    // dimensions
    strm >> row_read;
    strm >> col_read;
    if (row_read>0 && col_read>0)
        SetDimension(row_read,col_read);
    // data
    for (long j=0; j<size(); j++)
        strm >> (*this)(j);
}

template<class Tp>
Matrix<Tp>::operator<<(FILE &strm)
{
    MatrixFree();

    // dimensions
    fread(&(nrow),sizeof(nrow),1,&strm);
    fread(&(ncol),sizeof(ncol),1,&strm);

    if (nrow>0 && ncol>0)
        SetDimension(nrow,ncol);

    long lleft,lgo;

    lleft = size();
    lgo = 0;
    // data
    while (lleft>0)
    {
        if (lleft>10000)
        {
            fread(&(data[lgo]),sizeof(Tp),10000,&strm);
            lgo += 10000;
            lleft -= 10000;
        }
        else
        {
            fread(&(data[lgo]),sizeof(Tp),lleft,&strm);
            lleft = 0;
        }
    }
}

#define DEFOP(OP) \
    template<class Tp>\
    Matrix<Tp> Matrix<Tp>::operator OP (const Tp b) { \
        Matrix<Tp> c (rows(), cols()); \
        int n=rows()*cols();\
        for (int i = 0; i < n; i++) { \
            c.data[i] = Data(i) OP b; \
        } \
        return c; \
    } 
DEFOP(+);
DEFOP(-);
DEFOP(*);
DEFOP(/);
#undef DEFOP

template<class Tp>
Matrix<int> Matrix<Tp>::operator > (const Tp y)
{  //multiply by a scalar constant
    Matrix<int> temp(rows(), cols()); //intialize the temporary matrix
    int n=size();
    for (int i = 0; i < n; i++)
        temp(i) = (int) (Data(i)>y); //compare element by element
    return temp; //return the matrix
}
template<class Tp>
Matrix<Tp>&    Matrix<Tp>::operator^(const Simple_Matrix<Tp> y)
{
#ifdef _DEBUG
    assert(rows()==y.rows() && cols()==y.cols());
#endif

    for (long i=0; i<size(); i++)
        (*this)(i) = (*this)(i)*y.Data(i);
    return *this;
}
template<class Tp>
Matrix<Tp>    Matrix<Tp>::operator ^(const Tp y)
{
    Matrix<Tp>    temp(rows(),cols()); //intialize the temporary matrix
    long i, n=size();
    for (i=0; i<n; i++)
        temp(i) = pow(Data(i),y);
    return temp;
}

/*
template<class Tp>
bool Matrix<Tp>::Save(char *fname)
{
    std::ofstream    stream;
    
    stream.open(fname, std::ios_base::out);
    (*this) >> stream;
    if (stream.is_open())
    {    
        stream.close();
        return true;
    }
    else
        return false;
}
*/
  
template<class Tp>
std::istream&  operator>>(std::istream& s, Simple_Matrix<Tp>& x)
{  //read matrix from a stream

    for(int i = 0; i < x.rows(); i++)
        for(int j = 0; j<x.cols(); j++)
            s>>x(i,j);

      return s;
}

template<class Tp>
std::ostream& operator<<(std::ostream& s, Simple_Matrix<Tp>& x)
{
    for (int i=0; i<x.cols(); i++)
        for (int j=0; j<x.rows(); j++)
            s << x.Data(j,i) << "\n";
    return s;
}

 // set this matrix to be I matrix
template<class Tp>
void Matrix<Tp>::Identity(void)
{
     for (int i = 0; i < rows();  i++)
        for (int j = 0; j < cols();  j++)
        {
            if( i==j) 
                (*this)(i,j) = 1;
            else 
                (*this)(i,j) = 0;
        }
}


// set this matrix to be zero matrix                    
template<class Tp>
void Matrix<Tp>::Zero(void)                        
{
     for (int i = 0; i < rows();  i++)
        for (int j = 0; j < cols();  j++)
         (*this)(i,j) = 0;
}


////////////////////////////////////////////////////////////////
template<class Tp>
Tp Matrix<Tp>::SubPixel(const double x, const double y) const
{    //interpolates the pixels, 
    int ix=(int)x,iy=(int)y;
    int ny=rows(),nx=cols();
    double a,p1,p2;
    Tp lt,rt,lb,rb; //lt=left top, rb=right bottom and so on

    if ((ix<0)||(ix>=nx)||(iy<0)||(iy>=ny))    
        return 0;

    lt=Data(iy,ix);

    if (ix+1<nx)    
        rt=Data(iy,ix+1);
    else 
        rt=0;
    if (iy+1<ny) 
        lb=Data(iy+1,ix);
    else 
        lb=0;
    if ((ix+1<nx)&&(iy+1<ny)) 
        rb=Data(iy+1,ix+1);
    else 
        rb=0;
    a=x-ix;
    p1=(1-a)*lt+a*rt;
    p2=(1-a)*lb+a*rb;
    a=y-iy;
    return (Tp)((1-a)*p1+a*p2);
}

template<class Tp>
bool Matrix<Tp>::IsLocalMax(int xi, int yi){
    int x0=xi-1,x1=xi+1;
    int y0=yi-1,y1=yi+1;
    int x,y;
    float d=Data(yi,xi);
    if (x0<0) x0=0;
    if (x1>=nx()) x1=nx()-1;
    if (y0<0) y0=0;
    if (y1>=ny()) y1=ny()-1;
    for (y=y0;y<=y1;y++)
        for (x=x0;x<=x1;x++){
            if ((x!=xi)||(y!=yi)){
                if (Data(y,x)>=d)
                    return false;
            }
        }
    return true;
}
template<class Tp>
Matrix<Tp>    Matrix<Tp>::Enlarge(int ntimes){
    Matrix<Tp> M;
    int i,j,ny1=ntimes*nrow,nx1=ntimes*ncol;
    M.SetDimension(ny1,nx1);
    for (i=0;i<ny1;i++)
        for (j=0;j<nx1;j++){
            M(i,j)=Data(i/ntimes,j/ntimes);
        }
    return M;
}
template<class Tp>
Matrix<Tp> Matrix<Tp>::Reduce(int ntimes){
    int nx1=nx(),ny1=ny();
    int x,y,nnx=nx1/ntimes,nny=ny1/ntimes;
    if (nx1%ntimes!=0)
        nnx++;
    if (ny1%ntimes!=0)
        nny++;
    Matrix<Tp> B;
    B.SetDimension(nny,nnx);
    for(x=0;x<nnx;x++)
        for(y=0;y<nny;y++)
            B(y,x)=Data(y*ntimes,x*ntimes);
    return B;
}

///////////////////////////////////////////////////
// two dimensioanl convolution
template<class Tp>
Matrix<Tp> Matrix<Tp>::Conv(const Simple_Matrix<Tp>& kern)
{
    Matrix<Tp>    I;
    long i,j,i1,j1,i2,j2,m1,n1,m2,n2;

    I.SetDimension(rows(), cols());

    I.Zero();
    m1 = kern.rows();
    n1 = kern.cols();
    
    m2 = rows();
    n2 = cols();

    for (j=0; j<m2; j++)
        for (i=0; i<n2; i++)
        {
            for (j1=0; j1<m1; j1++)
                for (i1=0; i1<n1; i1++)
                {
                    j2 = j-(j1-(m1-1)/2);
                    i2 = i-(i1-(n1-1)/2);
                    if (j2>=0 && j2<m2 &&
                        i2>=0 && i2<n2)
                        I(j,i) = I(j,i)+Data(j2,i2)*kern.Data(j1,i1);
                }
        }
    return I;
}

// Blur by gaussian kernel 
template<class Tp>
Matrix<Tp> Matrix<Tp>::GaussianBlur(double sigma){
    Matrix<Tp> GaussKer,C;
    GaussKer.SetGaussian(1,sigma);
    C=ConvHor(GaussKer);
    return C.ConvVer(GaussKer.T());
}

// horizontal convolution 
template<class Tp>
Matrix<Tp> Matrix<Tp>::ConvHor(const Simple_Matrix<Tp>& kern)
{
    Matrix<Tp>    I;

    I.SetDimension(rows(), cols());
    long i,j,k,l,m;

    I.Zero();
    l = kern.cols();
    for (j=0; j<rows(); j++)
        for (i=0; i<cols(); i++)
        {
            for (k=0; k<l; k++)
            {
                m = i-(k-(l-1)/2);
                if (m>=0 && m<cols())
                    I(j,i) = I(j,i)+Data(j,m)*kern.Data(0,k);
            }
        }
    return I;
}

// vertical convolution
template<class Tp>
Matrix<Tp> Matrix<Tp>::ConvVer(const Simple_Matrix<Tp>& kern)
{
    Matrix<Tp>    I;

    // get tranpose of matrix
    I = T();
    I = I.ConvHor(kern.T());
    return *((Matrix<Tp> *)&I.T());
}

template<class Tp>
Matrix<Tp> Matrix<Tp>::Conv2(const Simple_Matrix<Tp>& kern)
{
    Matrix<Tp> I;

    I = ConvHor(kern);
    I = I.ConvVer(kern.T());
    return I;
}

inline double    gauss(double x, double sigma)
    {return exp(-x*x/(2*sigma*sigma)) / (sigma*sqrt(2*LPI));};
inline double    dgauss(double x, double sigma)
    {return -x*gauss(x,sigma)/(sigma*sigma);};
inline double    d2gauss(double x, double sigma)
    {return (x*x/pow(sigma,4)-1/pow(sigma,2))*gauss(x,sigma);};

//////////////////////////////////////////////////////
// kernel generation
/*   dim : dimension 1: one
                     2: two
     sigma: sigmal value
     direvite: 0: 
               1: first
               2: second
*/
template<class Tp>
void Matrix<Tp>::SetGaussian(int dim, double sigma, int derivative)
{
    Matrix<Tp>    op;
    int            size,i,j,u;
    double        x,y;

    size = ((int)sigma)*6+1;
    u = (size-1)/2;
    
    assert(size > 0);

    // gaussian kernel
    if (dim == 1)
    {
        op.SetDimension(1,size);
        op.Zero();
        for (i=0; i<size; i++)
        {
            x = double(i-u);
            switch (derivative)
            {
            case 0:    // Gaussian function
                    op(0,i) = (Tp)gauss(x,sigma);
                    break;
            case 1:    // First derivative of Gaussian
                    op(0,i) = (Tp)dgauss(x,sigma);
                    break;
            case 2: // Second derivative of Gaussian
                    op(0,i) = (Tp)d2gauss(x,sigma);
                    break;
            }
        }
    }
    else
    {
        double    std2=sigma*sigma;
        double    h1;

        op.SetDimension(size,size);
        op.Zero();

        switch(derivative)
        {
        case 0:
            for (j=0; j<size; j++)
                for (i=0; i<size; i++)
                {
                    x = double(j-u);
                    y = double(i-u);

                    h1 = exp(-(x*x+y*y)/(2*std2));
                    op(j,i) = (Tp)(h1/(2*LPI*(pow(sigma,2))));
                }
            break;
        case 2:
            for (j=0; j<size; j++)
                for (i=0; i<size; i++)
                {
                    x = double(j-u);
                    y = double(i-u);

                    h1 = exp(-(x*x+y*y)/(2*std2));
                    op(j,i) = (Tp)(h1*(x*x+y*y-2*std2)/(2*LPI*(pow(sigma,6))));
                }
            break;
        }
    }
    *this = op;
}

// Gabor Filter
// make gabor filter                                                   2
//               k*k                k*k       2                  sigma
//  Pk(x) = --------------exp(- -------------x )(exp(ikx)-exp(- --------)
//          (sigma*sigma)       (sigma*sigma)                      2
//
template<class Tp>
void Matrix<Tp>::SetGabor(const double k, const double sigma, 
                          const double theta /* in degree */)
{
    int half_size,w,h,i,j;

    // size of the filter
    if (k>1e-10)
        half_size = (int)floor(sigma/k*1.5);
    else
        half_size = (int)floor(sigma*1.5);
    w = 2*half_size+1;
    h = 2*half_size+1;

    // initilization
    // gaussian kernel
    Matrix<Tp>    F(1,w),G;
    double        x,sigma_s,k_s;
    
    k_s = k*k;
    sigma_s = sigma*sigma;
    for (i=0; i<w; i++)
    {
        x = (double)(i-half_size);
        x = x*x;
        F(i) = (Tp)(k_s/sigma_s*exp(-k_s/(2*sigma_s)*x));
    }
    G = F.T()*F;
    
    // sinusoid kernel
    // to grad
    Matrix<Tp>        P(h,w);
    Matrix<double>    r(2,2),temp(2,1),u;
    double            angle;
    angle = theta/180*LPI;
    r(0,0) = cos(angle);
    r(0,1) = -sin(angle);
    r(1,0) = sin(angle);
    r(1,1) = cos(angle);

    for (j=0; j<h; j++)
        for (i=0; i<w; i++)
        {
            temp(0) = j-half_size;
            temp(1) = i-half_size;
            u = r*temp;
              P(j,i) = (Tp)(cos(u(0)*k));
        }
    
    // Gabor filter
    (*this) = G^P;
    (*this) = (*this)/Sum();
}

template<class Tp>
void Matrix<Tp>::SetLaplacianOfGaussian(const double sigma)
{
    int        half_size,w,h,i,j;
    double    x,y;

    half_size = (int)floor(sigma/2.0);
    w = 2*half_size+1;
    h = 2*half_size+1;
    
    SetDimension(h,w);
    for (j=0; j<h; j++)
        for (i=0; i<w; i++)
        {
            x = j-half_size; y = i-half_size;
            (*this)(j,i) = (Tp)((x*x+y*y-sigma*sigma)*exp(-(x*x+y*y)/(2.0*sigma*sigma))/pow(sigma,2.0));
        }
    Tp sum=(Tp)Sum();
    for (j=0; j<h; j++)
        for (i=0; i<w; i++)
            (*this)(j,i) = (Tp)(((*this)(j,i)-sum/((double)size()))*10.0);
}

template<class Tp>
Tp Matrix<Tp>::GetMin(void) const
{
    Tp value;

    if (size()>0)
    {
        long i;
        value = Data(0);
        for (i=1; i<size(); i++)
            if (Data(i)<value)
                value = Data(i);
    }

    return value;
}
template<class Tp>
Tp Matrix<Tp>::GetMin(int &mini) const
{
    Tp value;
    if (size()>0)
    {
        long i;
        mini=0;
        value = Data(0);
        for (i=1; i<size(); i++)
            if (Data(i)<value){
                value = Data(i);
                mini=i;
            }
    }

    return value;
}

template<class Tp>
Tp Matrix<Tp>::GetMax(void) const
{
    Tp value;

    if (size()>0)
    {
        long i;
        value = Data(0);
        for (i=1; i<size(); i++)
            if (Data(i)>value)
                value = Data(i);
    }

    return value;
}
template<class Tp>
Tp Matrix<Tp>::GetMax(int &maxi) const
{
    Tp value;
    if (size()>0)
    {
        long i;
        maxi=0;
        value = Data(0);
        for (i=1; i<size(); i++)
            if (Data(i)>value){
                value = Data(i);
                maxi=i;
            }
    }

    return value;
}
template<class Tp>
Tp Matrix<Tp>::Max(int nStartIdx, int nNumElems) const
{
    ASSERT( 0 <= nStartIdx && nStartIdx < GetSize() );
    ASSERT( nStartIdx + nNumElems <= GetSize() );

    const Tp *    pData = GetData() + nStartIdx;
    Tp            tMax  = *pData++;
    Tp            tTmp  = tMax;        

    while( --nNumElems )
    {
        tTmp = *(pData++);
        tMax = max(tMax, tTmp);
    }
    return tMax;
}
template<class Tp>
double Matrix<Tp>::SumAbs(int nStartIdx, int nNumElems) const
{
    ASSERT( 0 <= nStartIdx && nStartIdx < GetSize() );
    ASSERT( nStartIdx + nNumElems <= GetSize() );

    const T *    pData = GetData() + nStartIdx;
    double        dSum  = fabs(*pData++);

    while( --nNumElems )
    {
        dSum += fabs(*pData++);
    }
    return dSum;
}

template<class Tp>
double Matrix<Tp>::SumAbs() const
{
    return SumAbs(0, GetSize());
}

template<class Tp>
double Matrix<Tp>::Product(void) const
{
    double    value;
    value = 1;
    for (long i=0; i<size(); i++)
        value = value*Data(i);

    return value;
}
template<class Tp>
double Matrix<Tp>::Product(int nStartIdx, int nNumElems) const
{
    ASSERT( 0 <= nStartIdx && nStartIdx < GetSize() );
    ASSERT( nStartIdx + nNumElems <= GetSize() );

    const T *    pData = GetData() + nStartIdx;
    double        dProduct  = *pData++;

    while( --nNumElems )
    {
        dProduct *= *pData++;
    }
    return dProduct;
}
template<class Tp>
double Matrix<Tp>::Sum(void) const
{
    double    value;
    value = 0;
    for (long i=0; i<size(); i++)
        value = value+Data(i);

    return value;
}
template<class Tp>
double Matrix<Tp>::Sum(int nStartIdx, int nNumElems) const
{
    ASSERT( 0 <= nStartIdx && nStartIdx < GetSize() );
    ASSERT( nStartIdx + nNumElems <= GetSize() );

    const Tp *    pData = GetData() + nStartIdx;
    double        dSum  = *pData++;

    while( --nNumElems )
    {
        dSum += *pData++;
    }
    return dSum;
}
template<class Tp>
double Matrix<Tp>::SumSq(void) const
{
    double    value;
    value = 0;
    for (long i=0; i<size(); i++)
        value = value+data[i]*data[i];

    return value;
}
template<class Tp>
double Matrix<Tp>::SumPow(double d) const
{
    double    value;
    value = 0;
    for (long i=0; i<size(); i++)
        value = value+pow(data[i],d);

    return value;
}
// make sum up to zero
template<class Tp>
void Matrix<Tp>::NormToZero(void)
{
    Tp        sum=Sum(),g,z;
    long    i;
    
    z = 0;
    g = (Tp)((sum)/((double)size()));
    if (sum != z)
        for (i=1; i<size(); i++)
            (*this)(i) = (Tp)(Data(i)-g);
}

template<class Tp>
Tp Matrix<Tp>::Mean(void) const
{
    Tp    value;
    long temp;

    value = 0;
    temp = size();
    if (temp>0)
        value = (Tp)(Sum()/temp);
    return value;
}
template<class Tp>
Tp Matrix<Tp>::Var(void) const
{
    Tp    value,mean=Mean();
    long i,len;

    value = 0;
    len = size();
    if (len>1){
        for (i=0;i<len;i++)
            value += (Tp)((Data(i)-mean)*(Data(i)-mean));
        value=(Tp)(value/(len-1));
    }
    return value;
}

template<class Tp>
Matrix<Tp> Matrix<Tp>::mabs(void) const
{
    Matrix<Tp> I;
    Tp            z;

    z = 0;
    I.SetDimension(rows(),cols());
    for (long i=0; i<size(); i++)
    {
        if (Data(i)<z)    I(i) = z-Data(i);
        else            I(i) = Data(i);
    }
    return I;
}

template<class Tp>
Tp Matrix<Tp>::DotProduct(const Matrix<Tp> &kern, const long jshift, const long ishift)
{
    Tp        value;
    long    i,j,k,l;

    value = 0;
    if (size()<kern.size())
    {
        for (j=0; j<rows(); j++)
            for (i=0; i<cols(); i++)
            {
                k = j-jshift;
                l = i-ishift;
                if (k>=0 && k<kern.rows() && l>=0 && l<kern.cols())
                    value = value+kern.Data(k,l)*Data(j,i);
            }
    }
    else
    {
        for (k=0; k<kern.rows(); k++)
            for (l=0; l<kern.cols(); l++)
            {
                j = k+jshift;
                i = l+ishift;
                if (j>=0 && j<rows() && i>=0 && i<cols())
                    value = value+kern.Data(k,l)*Data(j,i);
            }
    }
    return value;
}

template<class Tp>
Tp Matrix<Tp>::DotProductR(const Matrix<Tp> &kern, const long jshift, const long ishift)
{
    Tp        value;
    long    i,j,k,l;

    value = 0;
    if (size()<kern.size())
    {
        for (j=0; j<rows(); j++)
            for (i=0; i<cols(); i++)
            {
                k = (j-jshift+2*kern.rows())%kern.rows();
                l = (i-ishift+2*kern.cols())%kern.cols();
                value = value+kern.Data(k,l)*Data(j,i);
            }
    }
    else
    {
        for (k=0; k<kern.rows(); k++)
            for (l=0; l<kern.cols(); l++)
            {
                j = (k+jshift+2*rows())%rows();
                i = (l+ishift+2*cols())%cols();
                value = value+kern.Data(k,l)*Data(j,i);
            }
    }
    return value;
}

template<class Tp>
Tp Matrix<Tp>::PdfNorm1(const Matrix &x)
{
    Tp result=0;

#ifdef _DEBUG
    assert(x.size() == size());
#endif

    for (long i=0; i<size(); i++)
        result += __min(Data(i),x.Data(i));
    return result;
}

template<class Tp>
Tp Matrix<Tp>::PdfNorm2(const Matrix &x)
{
    Tp result=0;

#ifdef _DEBUG
    assert(x.size() == size());
#endif

    for (long i=0; i<size(); i++)
        result += (Data(i)-x.Data(i))*(Data(i)-x.Data(i));
    return result;
}

//Is this value in double really close to an integer?
inline int IsIntegerValue(bool &bintg, const double value)
{
    double a;
    
    bintg = false;
    a = floor(value);
    if (fabs(a-value)<1e-4)
    {
        // floor value
        bintg = true;
        return (int)a;
    }
    else 
    {
        a = ceil(value);
        if (fabs(a-value)<1e-4)
        {
            // ceiling value
            bintg = true;
            return (int)a;
        }
    }
    return -1;
}
// rotate the matrix
// this function is performed on square matrix only
template<class Tp>
Matrix<Tp> Matrix<Tp>::Rotate(const double angle_degree)
{
    // rotation matrix
    // to speed up the computation four variables are used instead of a matrix
    double        m11,m12,m21,m22;
    double        angle_radian;
    Matrix<Tp>    mx_t;

    mx_t.SetDimension(rows(),cols());

    angle_radian = angle_degree/180.0*LPI;
    // cos  -sin
    // sin  cos
    m11 = cos(angle_radian);
    m12 = -sin(angle_radian);
    m21 = sin(angle_radian);
    m22 = cos(angle_radian);

    double    row_center,col_center;
    double    x1,y1,x,y;
    bool    b_rowint,b_colint;
    int        i1,j1;

    row_center = ((double)rows())/2.0;
    col_center = ((double)cols())/2.0;

    int i,j;
    for (j=0; j<rows(); j++)
        for (i=0; i<cols(); i++)
        {
            y1 = j+0.5-row_center;
            x1 = i+0.5-col_center;

            // transformation
            x = m11*x1+m12*y1;
            y = m21*x1+m22*y1;
            // translation
            x += col_center-0.5;
            y += row_center-0.5;
            
            j1 = IsIntegerValue(b_rowint,y);
            i1 = IsIntegerValue(b_colint,x);
            if (b_rowint && b_colint)
            {
                //transformed exactly on a pixel
                if (Valid(j1,i1))
                    mx_t(j,i) = Data(j1,i1);
                else
                    mx_t(j,i) = (Tp)0;
            }
            else
            {
                // find the four corners
                Tp    weights,weight;
                int    row_close[2],col_close[2];

                row_close[0] = (int)floor(y);
                row_close[1] = (int)ceil(y);
                col_close[0] = (int)floor(x);
                col_close[1] = (int)ceil(x);

                mx_t(j,i) = (Tp)0;
                weights = (Tp)0;
                for (int n=0; n<=1; n++)
                    for (int m=0; m<=1; m++)
                    {
                        if (Valid(row_close[n],col_close[m]))
                        {
                            // weight is computed according the inverse of the distance
                            weight = (Tp)(1.0/sqrt(pow(col_close[m]-x,2.0)+pow(row_close[n]-y,2.0)));
                            mx_t(j,i) = mx_t(j,i)+Data(row_close[n],col_close[m])*weight;
                            weights += weight;
                        }
                    }
                if (weights > 0.0)
                    mx_t(j,i) = mx_t(j,i)/weights;
            }
        }

    return mx_t;
}

template<class Tp>
bool operator == (const Matrix<Tp>& x, const Simple_Matrix<Tp>& y)
{  //returns a matrix of zeros and ones
    int n=x.rows()*x.cols();

    for (int i = 0; i < n; i++)
            if(x.Data(i)!= y.Data(i))
                return false;
    return true; 
}

template<class Tp>
Matrix<Tp> operator * (const Simple_Matrix<Tp>& x, const Simple_Matrix<Tp>& y)
{  //multiply two matrices
    Tp            sum;
    Matrix<Tp>    temp(x.nrow,y.ncol); //intialize the temporary matrix

    for (int i = 0; i < x.nrow; i++)
        for (int j = 0; j < y.ncol; j++)
        {
            sum = 0;
            for(int k = 0; k < x.ncol; k++)
                sum = sum+x.Data(i, k)*y.Data(k, j); //multiply

            temp(i, j) = sum ;
        }

      return temp; //return the result of multiplication
}

// binary operators
template<class Tp>
Matrix<Tp>    operator ^(const Simple_Matrix<Tp>& x, Simple_Matrix<Tp> &y)
{
    Matrix<Tp>    temp(x.rows(),x.cols()); //intialize the temporary matrix
    long i, n=x.size();
    for (i=0; i<n; i++)
        temp(i) = pow(x.Data(i),y.Data(i));
    return temp;
}
template<class Tp>
Matrix<Tp>    operator ^(const Simple_Matrix<Tp>& x, const Tp y)
{
    Matrix<Tp>    temp(x.rows(),x.cols()); //intialize the temporary matrix
    long i, n=x.size();
    for (i=0; i<n; i++)
        temp(i) = pow(x.Data(i),y);
    return temp;
}
template<class Tp>
Matrix<Tp>    operator ^(const Tp x, const Simple_Matrix<Tp> &y)
{
    Matrix<Tp>    temp(x.rows(),x.cols()); //intialize the temporary matrix
    long i, n=x.size();
    for (i=0; i<n; i++)
        temp(i) = pow(x,y.Data(i));
    return temp;
}
#define DEFOP(OP) \
    template<class Tp>\
    Matrix<Tp> operator OP (const Simple_Matrix<Tp>& a, const Simple_Matrix<Tp>& b) { \
        Matrix<Tp> c (a.rows(), a.cols()); \
        int n=a.rows()*a.cols();\
        for (int i = 0; i < n; i++) { \
            c.data[i] = a.Data(i) OP b.Data(i); \
        } \
        return c; \
    } 
DEFOP(+);
DEFOP(-);
DEFOP(&);
DEFOP(/);
#undef DEFOP

#define DEFOP(OP) \
    template<class Tp>\
    Matrix<Tp> operator OP (const Simple_Matrix<Tp>& a, Tp b) { \
        Matrix<Tp> c (a.rows(), a.cols()); \
        int n=a.rows()*a.cols();\
        for (int i = 0; i < n; i++) { \
            c.data[i] = a.Data(i) OP b; \
        } \
        return c; \
    } 
DEFOP(+);
DEFOP(-);
DEFOP(*);
DEFOP(/);
#undef DEFOP

#define DEFOP(OP) \
    template<class Tp>\
    Matrix<Tp> operator OP (Tp a, const Simple_Matrix<Tp>& b) { \
        Matrix<Tp> c (b.rows(), b.cols()); \
        int n=b.rows()*b.cols();\
        for (int i = 0; i < n; i++) { \
            c.data[i] = a OP b.Data(i); \
        } \
        return c; \
    } 
DEFOP(+);
DEFOP(-);
DEFOP(*);
DEFOP(/);
#undef DEFOP

#define DEFOP(OP) \
    template<class Tp>\
    Matrix<int> operator OP (const Simple_Matrix<Tp>& a, const Simple_Matrix<Tp>& b) { \
        Matrix<int> c (a.rows(), a.cols()); \
        int n=a.rows()*a.cols();\
        for (int i = 0; i < n; i++) { \
            c.data[i] =(int) ( a.Data(i) OP b.Data(i)); \
        } \
        return c; \
    } 
    DEFOP(<);
    DEFOP(>);
    DEFOP(<=);
    DEFOP(>=);
    DEFOP(&&);
    DEFOP(||);
#undef DEFOP

#define DEFOP(OP) \
    template<class Tp>\
    Matrix<int> operator OP (const Simple_Matrix<Tp>& a, Tp b) { \
        Matrix<int> c (a.rows(), a.cols()); \
        int n=a.rows()*a.cols();\
        for (int i = 0; i < n; i++) { \
            c.data[i] = a.Data(i) OP b; \
        } \
        return c; \
    } 
    DEFOP(<);
    DEFOP(>);
    DEFOP(<=);
    DEFOP(>=);
    DEFOP(&&);
    DEFOP(||);
#undef DEFOP

#define DEFOP(OP) \
    template<class Tp>\
    Matrix<int> operator OP (Tp a, const Simple_Matrix<Tp>& b) { \
        Matrix<int> c (b.rows(), b.cols()); \
        int n=b.rows()*b.cols();\
        for (int i = 0; i < n; i++) { \
            c.data[i] = a OP b.Data(i); \
        } \
        return c; \
    } 
    DEFOP(<);
    DEFOP(>);
    DEFOP(<=);
    DEFOP(>=);
    DEFOP(&&);
    DEFOP(||);
#undef DEFOP

/*
Matrix<int>    operator ^(const Simple_Matrix<int>& x, const int y)
{
    Matrix<int>    temp(x.rows(),x.cols()); //intialize the temporary matrix
    long i, n=x.size();
    for (i=0; i<n; i++)
        temp(i) = pow(x.Data(i),y);
    return temp;
}*/
/*template<class Tp>
bool operator == (const Simple_Matrix<Tp>& x, const Tp y)
{  //divide two matrices elem by elem
    int n=x.rows()*x.cols();

    for (int i = 0; i < n; i++)
            if (x.Data(i)!= y)
                return false;
    return true; 
}*/

template<class Tp>
double Matrix<Tp>::Det() const
{//returns the value of the determinanat

    double d; //interchange flag

    Matrix<Tp> temp(*this);         //make a copy of the original matrix
    Matrix<Tp> index(1,temp.rows());

#ifdef _DEBUG
    assert(nrow==ncol);
#endif

    if( LUdcmp(temp,index,d)) //decompose the matrix
        return 0;

    for( int j=0; j<nrow; j++) 
        d *= temp(j,j);
    return d;//fabs(d);
}
template<class Tp>
Tp Matrix<Tp>::Trace() const
{//returns the value of the trace

    Tp d; //interchange flag


#ifdef _DEBUG
    assert(nrow==ncol);
#endif
    
    d=0;
    for( int j=0; j<nrow; j++) 
        d += Data(j,j);
    return d;
}

template<class Tp>
double Matrix<Tp>::DetNoZero() const
{
    double det = Det();
    if (det<1.0)
        return 1.0;
    else
        return det;
}

template<class Tp>
Matrix<Tp> Matrix<Tp>::Inv() const
{//returns the inverse of the matrix

    double    d; //interchange flag
    int        i,j;

    Matrix<Tp> temp(*this);         //make a copy of the original matrix
    Matrix<Tp> col(rows(),1) ;     //temporary variable
    Matrix<Tp> index(1,rows()) ;
    Matrix<Tp> result(rows(),cols());    //the final result

    if( LUdcmp(temp, index, d) )
    {    //decompose the matrix
//        cerr<<"\n\n\t Error Singluar matrix...."<<endl;
        return result;
    }

    for(j=0; j<rows(); j++) 
    {
        col.init_value(0);
        col(j,0) = 1;
        LUbksb(temp,index,col);
        for(i=0; i<nrow; i++)
            result(i,j) = col(i,0);
    }
  return result;
}


#define TINY 1.0e-20;
 
template<class Tp>
int LUdcmp(Matrix<Tp>& X, Matrix<Tp>& indx, double& d)
{
    int i,imax, j, k;
    double big, dum, sum, temp;

    Matrix<Tp> vv(1,X.nrow); //vv stores the implicit scaling of each nrow

    d = 1.0;                        //no nrow interchange yet
    for (i=0;i<X.nrow;i++)    //loop to get implicit scaling information
    {
        big = 0.0;
        for (j=0;j<X.ncol;j++)
            if ((temp=fabs(X(i,j))) > big) big=temp;

        if (big == 0.0)
        {
//             cerr<<"Singular matrix in routine LUDCMP"<<endl;
             return -1;
        }

        vv(0,i)=1.0/big;  //save the scaling
    }

    for (j=0;j<X.nrow;j++) 
    {
        for (i=0;i<j;i++) 
        {
            sum = X(i,j);
            for (k=0;k<i;k++) sum -= X(i,k)*X(k,j);
              X(i,j)=sum;
        }
        big=0.0;
        for (i=j;i<X.nrow;i++)
        {
            sum = X(i,j);
            for (k=0;k<j;k++)
                sum -= X(i,k)*X(k,j);

            X(i,j)=sum;
            if ( (dum=vv(0,i)*fabs(sum)) >= big)
            {
                big=dum;
                imax=i;
            }
        }

        if (j != imax)
        {
            for (k=0;k<X.nrow;k++) 
            {
                dum = X(imax,k);
                X(imax,k)= X(j,k);
                X(j,k)=dum;
            }
            d = -(d);                        // change the parity of d
            vv(0,imax)=vv(0,j);                // Also interchange the scale factor
        }

        indx(0,j) = imax;
        if (X(j,j) == 0.0) X(j,j)=TINY;
        if (j != (X.nrow-1))
        {
            dum = 1.0/(X(j,j));
            for (i=j+1;i<X.nrow;i++) X(i,j) *= dum;
        }
    }
    return 0;
}
#undef TINY

template<class Tp>
int LUbksb(Matrix<Tp>& X, Matrix<Tp>& indx, Matrix<Tp>& B)
{
    int i, ii=-1, ip, j;
    double sum;

    for (i=0;i<X.nrow;i++)
    {
        ip  = (int) indx(0,i);
        sum = B(ip, 0);
        B(ip, 0) = B(i,0);
        if (ii>=0)
            for (j=ii;j<=(i-1);j++) sum -= X(i,j)*B(j,0);
        else if (sum) ii=i;
        B(i,0)=sum;
    }

    for (i=(X.nrow-1);i>=0;i--)
    {
        sum = B(i,0);
        for (j=i+1;j<X.nrow;j++) sum -= X(i,j)*B(j,0);
        B(i,0)=sum/X(i,i);
    }

    return 0;
}

template<class Tp>
bool Matrix<Tp>::SameColValue(Tp &x, const int i)
{
    int        j=0;

    if (rows()>0 && i<cols())
        x = Data(0,i);

    for (j=1; j<rows(); j++)
    {
        if (!(Data(j,i)==Data(0,i)))
            return false;
    }
    return true;
}


template<class Tp>
Tp Matrix<Tp>::Dist(const Matrix<Tp> &a)
{
    Matrix<Tp>    temp;

    if (a.cols()!=1)        return 0;
    if (cols()!=1)            return 0;
    if (rows()!=a.rows())    return 0;
    
    temp = *this-a;
    temp = temp.T()*temp;

    return temp.Data(0);
}

int jacobi(double **a, int n, double *d, double **v, int *nrot);

template<class Tp>
void Eig(Matrix<Tp>& mx, Matrix<Tp>& eigenvalue, Matrix<Tp>& eigenvector)
    //computes eigenvalues and eigenvectors of matrix mx
    //eigenvalues are written in a line vector, eigenvectors are column vectors in a matrix
    //mx must be symetric
{
    int i,j,n,*nrot;
    int nx=mx.cols(),ny=mx.rows();
    double **a,*d,**v;
    
    if (nx!=ny)
        return;

    // allocate memory
    a = new pdouble[ny+1];
    for (j=0; j<ny+1; j++)
        a[j] = new double[nx+1];
    
    d = new double[ny+1];

    v = new pdouble[ny+1];
    for (j=0; j<ny+1; j++)
        v[j] = new double[nx+1];
    
    nrot = new int[ny+1];

    n = ny;
    
    eigenvalue.SetDimension(1,n);
    eigenvector.SetDimension(n,n);

    // copy data
    for (j=0; j<ny; j++)
        for (i=0; i<nx; i++)
            a[j+1][i+1] = (double)mx(j,i);
    // compute eigen values and eigen vectors
    jacobi(a,n,d,v,nrot);
    for (j=0; j<ny; j++)
    {    
        eigenvalue(j) = d[j+1];
        for (i=0; i<nx; i++)
             eigenvector(j,i) = (Tp)(v[j+1][i+1]);
    }
    
    // free memory
    for (j=0; j<ny+1; j++)
    {
        delete []a[j];
        delete []v[j];
    }
    delete []a;
    delete []d;
    delete []v;
    delete []nrot;
}

void svdcmp(double **a,int m,int n,double *w,double **v);

template<class Tp>
void SVD(Matrix<Tp>& mx, Matrix<Tp>& U, Matrix<Tp>& D, Matrix<Tp>& V)
{
    int i,j,n,*nrot,m;
    double **a,*d,**v;
    
    // allocate memory
    a = new pdouble[mx.rows()+1];
    for (j=0; j<mx.rows()+1; j++)
        a[j] = new double[mx.cols()+1];
    
    d = new double[mx.rows()+1];

    v = new pdouble[mx.rows()+1];
    for (j=0; j<mx.rows()+1; j++)
        v[j] = new double[mx.cols()+1];
    
    nrot = new int[mx.rows()+1];

    m = mx.rows();
    n = mx.cols();
    
    U.SetDimension(m,n);
    D.SetDimension(n,n);D.Zero();
    V.SetDimension(n,n);

    // copy data
    for (j=0; j<mx.rows(); j++)
        for (i=0; i<mx.cols(); i++)
            a[j+1][i+1] = (double)mx(j,i);
    // compute svd
    svdcmp(a,m,n,d,v);

    for (i=0; i<n; i++){
        D(i,i) = d[i+1];
        for (j=0; j<m; j++)
        {
            U(j,i) = (Tp)(a[j+1][i+1]);
        }
        for (j=0; j<n; j++)
        {
            V(j,i) = (Tp)(v[j+1][i+1]);
        }
    }
    
    // free memory
    for (j=0; j<mx.rows(); j++)
    {
        delete []a[j];
        delete []v[j];
    }
    delete []a;
    delete []d;
    delete []v;
}



/*
////////////////////////////////////////////////////////
// FFT functions
void FFT_1D(Matrix<irmm_Complex> &H, const int sign, const int t)
{
    double n1,k1;
    n1 = (double)H.cols();
    k1 = floor(log(n1)/log(2.0));
    assert(n1==pow(2,k1));
    
    long            n,mmax,m,j,istep,i,nn;
    irmm_Complex    w,wp,temp;
    double            wtemp,theta;
    
    nn= (long)n1;
    n=nn << 1;
    j=1;
    for (i=1; i<n; i+=2)
    {
        if (j > i)
        {
            temp = H(t,(j-1)/2);
            H(t,(j-1)/2) = H(t,(i-1)/2);
            H(t,(i-1)/2) = temp;
        }
        m=n >> 1;
        while (m >= 2 && j > m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    mmax=2;
    while (n > mmax)
    {
        istep=mmax << 1;
        theta=(2*LPI/mmax)*sign;
        wtemp=sin(0.5*theta);
        wp.Set(-2.0*wtemp*wtemp,sin(theta));
        w.Set(1.0,0.0);
        
        for (m=1; m<mmax; m+=2)
        {
            for (i=m; i<=n; i+=istep)
            {
                j=i+mmax;
                temp = w*H(t,(j-1)/2);

                H(t,(j-1)/2) = H(t,(i-1)/2)-temp;
                H(t,(i-1)/2) = H(t,(i-1)/2)+temp;

            }
            w = w*wp+w;
        }
        mmax=istep;
    }
}

void FFT_shift(Matrix<irmm_Complex> & H, int t)
{
    Matrix<irmm_Complex> ttt;
    ttt.SetDimension(1,H.cols());
    ttt.Zero();
    for (long j=0; j<=H.cols()/2; j++)
    {
        if (j==0)
            ttt(0,H.cols()/2) = H(t,j);
        else
        {
            if (H.cols()/2+j >=0 && H.cols()/2+j<H.cols())
                ttt(0,H.cols()/2+j) = H(t,j);
            ttt(0,H.cols()/2-j) = H(t,H.cols()-j);
        }
    }
    for (j=0; j<=H.cols(); j++)
        H(t,j) = ttt(0,j);
}

void IFFT_shift(Matrix<irmm_Complex> & H, int t)
{
    Matrix<irmm_Complex> ttt;
    ttt.SetDimension(1,H.cols());
    ttt.Zero();
    for (long j=0; j<=H.cols()/2; j++)
    {
        if (j==0)
            ttt(0,j) = H(t,H.cols()/2);
        else
        {
            if (H.cols()/2+j >=0 && H.cols()/2+j<H.cols())
                ttt(0,j) = H(t,H.cols()/2+j);
            ttt(0,H.cols()-j) = H(t,H.cols()/2-j);
        }
    }
    for (j=0; j<=H.cols(); j++)
        H(t,j) = ttt(0,j);
}

template<class Tp>
Matrix<irmm_Complex> Matrix<Tp>::FFT(void)
{
    Matrix<irmm_Complex> H;
    
    // expend current matrix to the size that is a power to 2
    double n1,k1,m1;
    k1 = ceil(log((double)rows())/log(2.0));
    n1 = pow(2.0,k1);
    k1 = ceil(log((double)cols())/log(2.0));
    m1 = pow(2.0,k1);

    long    j,i;
    H.SetDimension((long)n1,(long)m1);
    
    for (j=0; j<rows(); j++)
        for (i=0; i<cols(); i++)
            H(j,i) = Data(j,i);
    
    for (j=0; j<H.rows(); j++)
        FFT_1D(H,1,j);
    // transportation
    H = H.T();

    for (j=0; j<H.rows(); j++)
        FFT_1D(H,1,j);

    // transportation
    H = H.T();
    
    return    H;
}

template<class Tp>
Matrix<irmm_Complex> Matrix<Tp>::IFFT(void)
{
    Matrix<irmm_Complex> H;
    
    // expend current matrix to the size that is a power to 2
    double n1,k1,m1;
    k1 = ceil(log((double)rows())/log(2.0));
    n1 = pow(2.0,k1);
    k1 = ceil(log((double)cols())/log(2.0));
    m1 = pow(2.0,k1);

    long    j,i;
    H.SetDimension((long)n1,(long)m1);
    
    for (j=0; j<rows(); j++)
        for (i=0; i<cols(); i++)
            H(j,i) = Data(j,i);

    for (j=0; j<H.rows(); j++)
        FFT_1D(H,-1,j);
    // transportation
    H = H.T();

    for (j=0; j<H.rows(); j++)
        FFT_1D(H,-1,j);

    // transportation
    H = H.T();
    
    return    H;
}

// two dimensioanl convolution using FFT
template<class Tp>
Matrix<irmm_Complex> Matrix<Tp>::ConvFFT(const Matrix<Tp>& kern)
{
    // expend current matrix to the size that is a power to 2
    double n1,k1,m1;
    k1 = ceil(log((double)(__max(rows(),kern.rows())))/log(2.0));
    n1 = pow(2.0,k1);
    k1 = ceil(log((double)(__max(cols(),kern.cols())))/log(2.0));
    m1 = pow(2.0,k1);
    
    Matrix<irmm_Complex>    F,H,rtn;

    rtn.SetDimension(rows(),cols());
    F.SetDimension((long)n1,(long)m1);
    H.SetDimension((long)n1,(long)m1);
    F.Zero();
    H.Zero();

    long j,i;

    // copy data
    for (j=0; j<rows(); j++)
        for (i=0; i<cols(); i++)
            F(j,i) = Data(j,i);
    
    for (j=0; j<kern.rows(); j++)
        for (i=0; i<kern.cols(); i++)
            H(j,i) = kern.Data(j,i);
    
    // FFT transformation
    F = F.FFT();
    H = H.FFT();
    F = F^H;

    // IFFT transformation
    F = F.IFFT();

    for (j=0; j<rows(); j++)
        for (i=0; i<cols(); i++)
            rtn(j,i) = F(j,i);
    return rtn;
}
*/
#endif


