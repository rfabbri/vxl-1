///////////////////////////////////////////
// vector class
#ifndef _VECTOR_H
#define _VECTOR_H

#include "Matrix.h"


template<class Tp>
class Vector : public Matrix<Tp>
{
public:
    // construction & destruction
    Vector(){};
    Vector(const int size){SetRowVector(size);};
    ~Vector(){};

    // operators
    Vector<Tp>& operator=(const Matrix<Tp>& x );
    Tp&                operator()(const long index);
    Tp&                operator()(const int row, const int col);
    friend Vector    operator+(const Vector& x, const Vector& y );
    friend Vector    operator-(const Vector& x, const Vector& y );
    friend Vector    operator*(const Vector& x, const Vector& y );
    friend Vector    operator*(const Matrix& x, const Vector& y );
    friend Vector    operator*(const Vector& x, const Matrix& y );

    operator>>(std::ofstream &strm);
    operator<<(std::ifstream &strm);
    operator>>(FILE &strm);
    operator<<(FILE &strm);

    Vector        operator+(const Tp y);
    Vector        operator-(const Tp y);
    Vector        operator*(const Tp y);
    Vector        operator/(const Tp y);

    // operations
    void    SetRowVector(const int size);
    void    SetColVector(const int size);
    bool    RowVector(void);
    bool    ColVector(void);

    double    Norm(const double l);
    Tp        Projection(const Vector &a);
    Tp        DotProduct(const Vector &a);
    
    void    CopyContent(const Vector<Tp> &b, const int num);
};

////////////////////////////////////////////
// operators
template<class Tp>
Vector<Tp>& Vector<Tp>::operator=(const Matrix<Tp>& x )
{
    //beware of self assignments : x = x
    if ( this != &x )  
    {
        register i;

        if (rows()!=x.rows() || cols()!=x.cols())
            SetDimension(x.rows(),x.cols());

        for(i=0; i<size(); i++)
                (*this)(i) = (Tp)(x.Data(i));
    }
    return *this;
}

template<class Tp>
Vector<Tp>::operator>>(std::ofstream &strm)
{
    // dimensions
    strm << rows() << "\n";
    strm << cols() << "\n";
    // data
    for (long j=0; j<size(); j++)
        strm << (*this)(j) << "\n";
}

template<class Tp>
Vector<Tp>::operator>>(FILE &strm)
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
Vector<Tp>::operator<<(std::ifstream &strm)
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
Vector<Tp>::operator<<(FILE &strm)
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

template<class Tp>
Vector<Tp> operator+ (const Vector<Tp>& x, const Vector<Tp>& y)
{  //add two matrices

    Vector<Tp> temp;
    
    temp.SetDimension(x.rows(), x.cols()); //intialize the temporary matrix
    for(int i = 0; i < x.rows(); i++)
        for(int j = 0; j < x.cols(); j++)
            temp(i, j) = x.Data(i, j) + y.Data(i, j); //add element by element

      return temp; //return the new matrix
}
  

template<class Tp>
Vector<Tp> Vector<Tp>::operator+ (const Tp y)
{  
    //add a constant to a matrices
    Vector<Tp> temp;
    
    temp.SetDimension(rows(), cols()); //intialize the temporary matrix

    for(int i = 0; i < nrow; i++)
        for(int j = 0; j < ncol; j++)
            temp(i, j) = (*this)(i,j)+y;  //add element by element

      return temp; //return the matrix
}

  
template<class Tp>
Vector<Tp> operator - (const Vector<Tp>& x, const Vector<Tp>& y)
{  //subtract two Vector
    Vector<Tp> temp;
    
    temp.SetDimension(x.rows(),x.cols()); //intialize the temporary Vector

    for (int i = 0; i < x.nrow; i++)
        for (int j = 0; j<x.ncol; j++)
            temp(i, j) = x.Data(i, j)- y.Data(i, j); //sub element by element

    return temp; //return the new Vector
}

  
template<class Tp>
Vector<Tp> Vector<Tp>::operator - (const Tp y)
{  //subtract a constant from a Vector
    Vector<Tp> temp(rows(), cols()); //intialize the temporary Vector

    for (int i = 0; i < nrow; i++)
        for (int j = 0; j < ncol; j++ )
            temp(i,j) = (*this)(i, j)-y; //subtract element by element

      return temp; //return the Vector
}

  
template<class Tp>
Vector<Tp> operator * (const Vector<Tp>& x, const Vector<Tp>& y)
{  //multiply two matrices
    Tp            sum;
    Vector<Tp>    temp(x.nrow,y.ncol); //intialize the temporary Vector

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


template<class Tp>
Vector<Tp> operator * (const Matrix<Tp>& x, const Vector<Tp>& y )
{  //multiply two matrices
    Tp            sum;
    Vector<Tp>    temp;
    
    temp.SetDimension(x.rows(),y.cols()); //intialize the temporary Vector

    for (int i=0; i<x.rows(); i++)
        for (int j=0; j<y.cols(); j++)
        {
            sum = 0;
            for(int k=0; k<x.cols(); k++)
                sum = sum+x.Data(i, k)*y.Data(k, j); //multiply

            temp(i,j) = sum ;
        }

      return temp; //return the result of multiplication
}

template<class Tp>
Vector<Tp> operator * (const Vector<Tp>& x, const Matrix<Tp>& y )
{  //multiply two matrices
    Tp            sum;
    Vector<Tp>    temp;
    
    temp.SetDimension(x.rows(),y.cols()); //intialize the temporary Vector
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
  
template<class Tp>
Vector<Tp> Vector<Tp>::operator * (const Tp y)
{  //multiply by a scalar constant
    Vector<Tp> temp;
    
    temp.SetDimension(rows(), cols()); //intialize the temporary Vector

    for (int i = 0; i < nrow; i++)
        for (int j = 0; j < ncol; j++)
            temp(i,j) = (*this)(i, j)*y; //scale element by element

      return temp; //return the Vector
}

template<class Tp>
Vector<Tp> Vector<Tp>::operator / (const Tp y)
{  //multiply by a scalar constant
    Vector<Tp> temp;
    
    temp.SetDimension(rows(), cols()); //intialize the temporary Vector

    for (int i = 0; i < nrow; i++)
        for (int j = 0; j < ncol; j++)
            temp(i,j) = (*this)(i, j)/y; //scale element by element

      return temp; //return the Vector
}
 
template<class Tp>
Tp& Vector<Tp>::operator ()(const int row, const int col)
{    
#ifdef _DEBUG
    assert(row < rows() && row >= 0);
    assert(col < cols() && col >= 0);
#endif

    return data[((long)row) * cols() + col];
}

template<class Tp>
Tp& Vector<Tp>::operator ()(const long index)
{    
#ifdef _DEBUG
    assert(index<((long)rows()) * cols() && index>=0);
#endif

    return data[index];
}

template<class Tp>
void Vector<Tp>::SetRowVector(const int size)
{
    SetDimension(size,1);
}

template<class Tp>
void Vector<Tp>::SetColVector(const int size)
{
    SetDimension(1,size);
}

template<class Tp>
bool Vector<Tp>::RowVector(void)
{
    if (cols()==1 && rows()>0)
        return true;
    else
        return false;
}

template<class Tp>
bool Vector<Tp>::ColVector(void)
{
    if (cols()>0 && rows()==1)
        return true;
    else
        return false;
}

template<class Tp>
Tp Vector<Tp>::Projection(const Vector<Tp> &a)
{
#ifdef _DEBUG
    assert(rows()==a.rows() && cols()==a.cols());
#endif

    Tp sum=0;
    for (int i=0; i<size(); i++)
        sum = sum+Data(i)*a.Data(i);
    return sum;
}

template<class Tp>
Tp Vector<Tp>::DotProduct(const Vector<Tp> &a)
{
#ifdef _DEBUG
    assert(rows()==a.cols() && cols()==a.rows());
#endif

    Tp sum=0;
    for (int i=0; i<size(); i++)
        sum = sum+Data(i)*a.Data(i);
    return sum;
}

template<class Tp>
double Vector<Tp>::Norm(const double l)
{
    int        i;
    double    sum = 0.0;

#ifdef _DEBUG
    assert(cols()>0);
#endif

    for (i=0; i<rows(); i++)
        sum = sum+pow(fabs((double)Data(i)),l);
    sum = pow(sum,1.0/l);
    return sum;
}

template<class Tp>
void Vector<Tp>::CopyContent(const Vector<Tp> &b, const int num)
{
    for (int i=0; i<num; i++)
        (*this)(i) = b.Data(i);
}

#endif

