////////////////////////////////////////////////////////////////////////
//irmm_Raster.h
//This is the description of classes of image library including
//======================================================================
//        Gaussian filtering
//        segmentation of color image using mean shift algorithm
//=======================================================================
//    Author: Zhuowen Tu
////////////////////////////////////////////////////////////////////////
#ifndef _IRMM_RASTER_H
#define _IRMM_RASTER_H
 
#include <cassert>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <climits>
#include <fstream>
#include <strstream>
#include <memory>
#include <ctime>
#include <cstring>
#include <cstdio>

#include "Matrix.h"
#include "Vector.h"
#include "mcPublic.h"
#include "irmm_LinkList.h"

#define        ORG_SIZE

#define Max_rects        50

#ifndef MIN
#define MIN(x,y)        ((x) <= (y) ? (x) : (y))
#endif

#define Min3(x,y,z)     (MIN((x),(y)) <= (z) ? MIN((x),(y)) : (z))

#define bzero(b,len)     memset(b,'\0',len)
#define bcopy(b1,b2,len) memmove(b2,b1,len)


#define nil 0

class irmm_Window
{
public:
    long    width;        // width of the window
    long    height;        // height of the window

public:
    // construction & destruction
    irmm_Window(){width=10; height=10;};
    irmm_Window(long h, long w){width=w; height=h;};

    // operations
    void Set(long h, long w){width=w; height=h;};
};

// this is a general rectangle class that captures the bound
// of region
class irmm_Rect
{
public:
    long    left,right;
    long    top,bottom;
    // consturction & destruction
    irmm_Rect(){left=0;right=0;top=0;bottom=0;};
    inline    bool    InRect(const int row, const int col) const;
    inline    bool    InRect(const irmm_Rect& rect) const;
    inline    bool    Overlap(const irmm_Rect& rect) const;
    inline    void    Set(long lLeft,long lTop,long lRight,long lBottom)
        {left=lLeft;top=lTop;right=lRight;bottom=lBottom;};
    inline    void    BoundRow(const int row);
    inline    void    BoundCol(const int col);
    inline    long    Width(void){return right-left+1;};
    inline    long    Height(void){return bottom-top+1;};
    inline    void    SetBound(int i, int j);
};

inline void    irmm_Rect::SetBound(int i, int j)
{
    if (i<left)        left    = i;
    if (i>right)    right    = i;
    if (j<top)        top        = j;
    if (j>bottom)    bottom    = j;
}

inline bool    irmm_Rect::InRect(const int row, const int col) const
{
    if (row<top || row>bottom || col<left || col>right)
        return false;
    else
        return true;
}

inline    bool irmm_Rect::Overlap(const irmm_Rect& rect) const
{
    return    InRect(rect.top, rect.left) || InRect(rect.top, rect.right) ||
            InRect(rect.bottom, rect.left) || InRect(rect.bottom, rect.right);
}

inline bool    irmm_Rect::InRect(const irmm_Rect& rect) const
{
    int row1=rect.top,        col1=rect.left;
    int row2=rect.bottom,    col2=rect.left;
    int row3=rect.top,        col3=rect.right;
    int row4=rect.bottom,    col4=rect.right;

    if (InRect(row1,col1) && InRect(row2,col2) && InRect(row3,col3) && InRect(row4,col4))
        return true;
    else
        return false;
}

inline    void irmm_Rect::BoundRow(const int row)
{
    if (row<top) top = row;
    if (row>bottom) bottom = row;
}

inline    void irmm_Rect::BoundCol(const int col)
{
    if (col<left) left = col;
    if (col>right) right = col;
}


#define GAUSS 0
#define FIRST_DERIV 1
#define SECOND_DERIV 2

static int row_neighbors[8]={1,0,-1,-1,-1,0,1,1};
static int col_neighbors[8]={-1,-1,-1,0,1,1,1,0};

static int row_neighbors4[4]={0,-1,0,1};
static int col_neighbors4[4]={-1,0,1,0};

static int row_neighbors4n[4]={1,-1,-1,1};
static int col_neighbors4n[4]={-1,-1,1,1};

/////////////////////////////////////////////////////////
// this is the class of raster image in one channel
template<class Tp>
class Raster: public Matrix<Tp>
{
public:
    // construction
    Raster();
    Raster(const Raster<Tp>& r);
    ~Raster();

    // operators
    Raster<Tp>&        operator=(const Raster<Tp>& r);
    Raster&            operator=(const Matrix<Tp>& r);
    Raster&            operator|(const Raster& r);
    Raster&            operator&(const Raster& r);

    // access
    Tp                Get(const int Row, const int Col) const;
    void            Set(const int Row, const int Col, Tp& value);
    inline int        width(){return cols();};
    inline int        height(){return rows();};
    inline void        Zero(void){init_value(0);};
    
    // operations
    virtual Raster<uchar> ToVisibleRaster(const bool bBinary=false);
    
    // Gaussian filtering
    virtual void    DoGauss(double sigma, int option=0);

    // merge dangling pixels
    virtual    void    MergeDanglingPixels(void);
    virtual int        ResetLabels(const bool bMerge=true,Vector<short> *pvc_original_id=NULL);
    virtual bool    DanglingPixel(const int row, const int col);
    virtual int        FindUnion(Matrix<long> &union1, Matrix<long> &union2);
    virtual int        GetRepresentative(Matrix<long> &union1, const long j, const long i,long &m, long &n);

protected:
};

template<class Tp1, class Tp2> void assign_raster(Raster<Tp1>& x1,const Raster<Tp2>& x2);
void ResetLabels(Raster<int> &image);

static const double     Un_prime = (double)0.1978;
static const double     Vn_prime = (double)0.4683;
static const double     Lt = (double)0.008856;

// Constants for LUV transformation 
static const double     Xn = (double)0.9505;
static const double     Yn = (double)1.0;
static const double     Zn = (double)1.0888;

//==========================================================
// Class : Raster
//==========================================================

//-----------------------------------------------------------
// construction and destruction

/////////////////////////////////////////////////////////////
template<class Tp>
Raster<Tp>::Raster()
{
}

template<class Tp>
Raster<Tp>::Raster(const Raster<Tp>& r)
{
    (*this) = r;
}

/////////////////////////////////////////////////////////////
template<class Tp>
Raster<Tp>::~Raster()
{
}

//-----------------------------------------------------------
// operators
/////////////////////////////////////////////////////////////
template<class Tp>
Raster<Tp>&    Raster<Tp>::operator=(const Raster<Tp>& r)
{
    *((Matrix<Tp>*) this) = *((Matrix<Tp>*) &r);
    
    return *this;
}

template<class Tp>
Raster<Tp>&    Raster<Tp>::operator=(const Matrix<Tp>& r)
{
    *((Matrix<Tp>*) this) = *((Matrix<Tp>*) &r);
    
    return *this;
}

template<class Tp1, class Tp2>
void assign_raster(Raster<Tp1>& x1,const Raster<Tp2>& x2)
{
    int i=0;
    assign_matrix(*((Matrix<Tp1>*) &x1),*((Matrix<Tp2>*) &x2));
}

////////////////////////////////////////////////////////////////
template<class Tp>
Raster<Tp>& Raster<Tp>::operator|(const Raster<Tp>& r)
{
    if (this != &r && rows() == r.rows() && cols() == r.cols())
    {
        for (int i = 0; i < rows(); i++)
            for (int j = 0; j < cols(); j++)
                ((Matrix<uchar>)(*this))(i, j) = ((Matrix<uchar>)(*this))(i, j) | r(i, j);
    }
    return *this;
}

////////////////////////////////////////////////////////////////
template<class Tp>
Raster<Tp>& Raster<Tp>::operator&(const Raster<Tp>& r)
{
    if (this != &r && rows() == r.rows() && cols() == r.cols())
    {
        for (int i = 0; i < rows(); i++)
            for (int j = 0; j < cols(); j++)
                (*this)(i, j) = (*this)(i, j) & r(i, j);
    }
    return *this;
}


//-----------------------------------------------------------
// access

////////////////////////////////////////////////////////////////
template<class Tp>
Tp Raster<Tp>::Get(const int Row, const int Col) const
{
    return Data(Row, Col);
}

////////////////////////////////////////////////////////////////
template<class Tp>
void Raster<Tp>::Set(const int Row, const int Col, Tp& value)
{
    if (Row < rows() && Col < cols())
        (*this)(Row, Col) = value;
}



template<class Tp>
void Raster<Tp>::DoGauss(double sigma, int option)
{
    Matrix<Tp>    kernel;
    Raster<Tp>    r;

    kernel.SetGaussian(2,sigma,option);
    // make the kernel to sum up to zero
    kernel.NormToZero();

    (*this) = Conv(kernel);
}

// transform current image to general image in char that could be seen 
// directly 0~255
template<class Tp>
Raster<uchar> Raster<Tp>::ToVisibleRaster(const bool bBinary)
{
    Raster<uchar>    I;
    Tp                v1,v2;
    long            i;

    I.SetDimension(rows(), cols());
    v1 = GetMin();
    v2 = GetMax();
    if (v1 != v2)
    {
        if (!bBinary)
            for (i=0; i<size(); i++)
                I(i) = uchar((double(Data(i)-v1))/(v2-v1)*255);
        else
            // as binary image
        {
            for (i=0; i<size(); i++)
            {
                if (Data(i)>0)
                    I(i) = 255;
                else
                    I(i) = 0;
            }
        }
    }

    return I;
}
template<class Tp>
bool Raster<Tp>::DanglingPixel(const int row, const int col)
{
    int        i,m,n;
    bool    bDangling=false;
    
    if (Valid(row,col))
    {
        bDangling=true;
        for (i=0; i<4; i++)
        {
            n = row+row_neighbors4[i];
            m = col+col_neighbors4[i];
            if (Valid(n,m) && Data(row,col)==Data(n,m))
            {
                bDangling=false;
                break;
            }
        }
        int temp=0;
        if (bDangling)
        {
            for (i=0; i<4; i++)
            {
                n = row+row_neighbors4n[i];
                m = col+col_neighbors4n[i];
                if (Valid(n,m) && Data(row,col)==Data(n,m))
                    temp++;
            }
            if (temp<2)
                bDangling = false;
        }
    }
    return bDangling;
}


template<class Tp>
void Raster<Tp>::MergeDanglingPixels(void)
{
    int            j,i,m,n;

    for (j=0; j<rows(); j++)
        for (i=0; i<cols(); i++)
        {
            m = j+1; n = i-1;
            if (Valid(m,n))
            {
                if (Data(m,n)==Data(j,i) && Data(m,i)!=Data(j,i) && Data(j,n)!=Data(j,i))
                {
                    if (!DanglingPixel(m,i))
                    {
                        (*this)(m,i) = Data(j,i);
                        assert(Data(m,i)!=Data(j,n));
                    }
                }
            }
            m = j+1; n = i+1;
            if (Valid(m,n))
            {
                if (Data(m,n)==Data(j,i) && Data(m,i)!=Data(j,i) && Data(j,n)!=Data(j,i))
                {
                    if (!DanglingPixel(m,i))
                    {
                        (*this)(m,i) = Data(j,i);
                        assert(Data(m,i)!=Data(j,n));
                    }
                }
            }
        }
}

template<class Tp>
int Raster<Tp>::GetRepresentative(Matrix<long> &union1, const long j, const long i,
                                   long &m, long &n)
{
    long k,l,temp;

    k = j; l = i;
    while (union1.Data(k,l)>=0)
    {
        // get its parent
        temp = union1.Data(k,l);
        // get the corresponding position
        union1.GetPos(temp,k,l);
    }
    m = k; n = l;
    return union1.Data(m,n);
}

// return: the largest value in the matrix
template<class Tp>
int Raster<Tp>::FindUnion(Matrix<long> &union1, Matrix<long> &union2)
{
    long    j,i,j1,i1,j2,i2,m,n,m1,n1,m2,n2;
    int        largest_label=-1;

    // initialization
    union1.SetDimension(rows(),cols());
    union1.init_value(-1);
    union2.SetDimension(rows(),cols());
    union2.init_value(1);
    
    for (j=0; j<rows(); j++)
    {
        j=j;
        for (i=0; i<cols(); i++)
        {
            if (largest_label<Data(j,i))
                largest_label=(int)Data(j,i);

            m = j; n = i;
            // upper neighbor
            j1 = j-1;    i1 = i;
            if (Valid(j1,i1) && Data(j1,i1)==Data(j,i))
            {
                GetRepresentative(union1,j1,i1,m1,n1);
                union1(j,i) = m1*cols()+n1;
                union1(m1,n1) = -max(abs(union1(m1,n1)),2);
                union2(j,i) = 0;
                union2(m1,n1) = union2(m1,n1)+1;

                m = m1; n = n1;
            }
            // left neighbor
            j2 = j;        i2 = i-1;
            if (Valid(j2,i2) && Data(j2,i2)==Data(j,i))
            {
                GetRepresentative(union1,j2,i2,m2,n2);
                if (!(m2==m && n2==n))
                {
                    if (abs(union1(m,n))>abs(union1(m2,n2)))
                    {
                        union1(m,n) = -max(abs(union1(m,n)),abs(union1(m2,n2))+1);
                        union1(m2,n2) = m*cols()+n;
                        union2(m,n) = union2(m,n)+union2(m2,n2);
                        union2(m2,n2) = 0;
                    }
                    else
                    {
                        union1(m2,n2) = -max(abs(union1(m2,n2)),abs(union1(m,n))+1);
                        union1(m,n) = m2*cols()+n2;
                        union2(m2,n2) = union2(m,n)+union2(m2,n2);
                        union2(m,n) = 0;
                    }
                }
            }
        }
    }
    return largest_label;
}


#define MAX_REGION_NUM  5200

#define MAX_REGION_SIZENOTMERGED 15

template<class Tp>
int    Raster<Tp>::ResetLabels(const bool bMerge, Vector<short> *pvc_original_id)
{
    Matrix<long>    union1,union2;
    long            j,i,m,n,m1,n1,index;
    Matrix<int>        delsign;
    int                k,largest_label;
    Vector<int>        rep_row(MAX_REGION_NUM),rep_col(MAX_REGION_NUM),num(MAX_REGION_NUM);
    bool            bOK=false;
    int                size_current_region;

    // initialization
    MergeDanglingPixels();
    if (pvc_original_id != NULL)
    {
        pvc_original_id->SetRowVector(MAX_REGION_NUM);
        pvc_original_id->init_value(-1);
    }

    while (!bOK)
    {
        bOK = true;

        // union1 keeps the position of parent of pixel at (j,i)
        // union2 keeps the number of connected pixles with the same id
        largest_label = FindUnion(union1,union2);

        delsign.SetDimension(rows(),cols());
        delsign.init_value(-1);

        for (i=0; i<MAX_REGION_NUM; i++)
            num(i) = 0;
    
        // find which region should be kept and which one should be merged
        for (j=0; j<rows(); j++)
            for (i=0; i<cols(); i++)
            {
                if (union1(j,i)<0)
                {
                    // get region id
                    k = (int)Data(j,i);
                    if (k>=MAX_REGION_NUM)
                        return MAX_REGION_NUM+1;

                    if (abs(union2(j,i))>num(k))
                    {
                        // keep this region
                        rep_row(k) = j;
                        rep_col(k) = i;
                        num(k) = abs(union2(j,i));
                    }
                }
            }
        for (k=0; k<MAX_REGION_NUM; k++)
        {
            if (num(k)>0)
                delsign(rep_row(k),rep_col(k))=-2;
        }

        for (j=0; j<rows(); j++)
            for (i=0; i<cols(); i++)
            {
                // find its representative
                GetRepresentative(union1,j,i,m,n);
                index = (int)Data(m,n);

                if (delsign(m,n)==-1)
                {
                    bOK = false;
                    size_current_region = union2(m,n);

                    if (bMerge && size_current_region<MAX_REGION_SIZENOTMERGED)
                    {
                        bool bfound=false;
                        // find a neighbor to which this region merges
                        for (k=0; k<4; k++)
                        {
                            m1 = m+row_neighbors4[k];
                            n1 = n+col_neighbors4[k];
                            if (Valid(m1,n1) && Data(m1,n1)!=Data(m,n))
                            {
                                long m2,n2;
                                GetRepresentative(union1,m1,n1,m2,n2);
                                if (delsign(m2,n2)==-2)
                                {
                                    bfound = true;
                                    break;
                                }
                            }
                        }
                        if (!bfound)
                        {
                            for (int jj=0; jj<rows(); jj++)
                            {
                                for (int ii=0; ii<cols(); ii++)
                                {
                                    long mm1,nn1;
                                    GetRepresentative(union1,jj,ii,mm1,nn1);
                                    if (mm1==m && nn1==n)
                                    {
                                        for (k=0; k<4; k++)
                                        {
                                            m1 = jj+row_neighbors4[k];
                                            n1 = ii+col_neighbors4[k];
                                            if (Valid(m1,n1) && Data(m1,n1)!=Data(m,n))
                                            {
                                                long m2,n2;
                                                GetRepresentative(union1,m1,n1,m2,n2);
                                                if (delsign(m2,n2)==-2)
                                                {
                                                    bfound = true;
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                    if (bfound) break;
                                }
                                if (bfound) break;
                            }
                        }
                        if (bfound)
                            delsign(m,n) = (int)Data(m1,n1);
                    }
                    else
                    {
                        // assign this region with a new id
                        delsign(m,n) = (int)(++largest_label);

                        // save its original id
                        if (pvc_original_id != NULL)
                            (*pvc_original_id)(delsign(m,n)) = (int)((*this)(m,n));
                    }
                    if (delsign(m,n)>=0)
                        (*this)(j,i) = (Tp)delsign(m,n); 
                }
                else if (delsign(m,n)>=0)
                    (*this)(j,i) = (Tp)delsign(m,n); 
            }
    }
    return largest_label;
}

#endif

