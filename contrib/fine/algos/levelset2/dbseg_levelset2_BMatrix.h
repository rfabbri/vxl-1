/*

  File: BMatrix.h
  
*/
#pragma once

#include "dbseg_levelset2_Mh.h"

using namespace std;


class BMatrix : public vnl_matrix<double>
{
    
public:
    BMatrix();
    BMatrix(int r, int c);
    BMatrix(const BMatrix& rhs); // copy constructor
    ~BMatrix();
    
    // Functions:
public:
    void init(int r, int c);
    void setVal(int r, int c, double val);
    double getVal(int r, int c) const;
    
    int getNumRows() const;
    int getNumCols() const;
    
    void print();
    void graph();
    
    void writeplot(char* path);
    
    void plot(char *str);
    
    void splot(char *str);
    
    void kill();
    

    BMatrix* copy();
    
    void multTest();
    
    double convolve(const BMatrix &mat) const;
    
    void transpose();
    void invert();
    double det();
    
    BMatrix* SubMatrix(int sr, int sc, int size); 
    
    void CalcNorm();
    void Normalize();
    
    void IntKer();

    BMatrix* operator*(BMatrix &m);
    void operator *(double val);

    
    BMatrix* operator +(BMatrix &m);
    BMatrix* operator -(BMatrix &m);
    
    void operator =(BMatrix& rhs);

private:
    double round(double val);
    
    // Variables
public:
    double norm;
//protected:
    int nrows, ncols;
    vnl_matrix<double> m;
    //double* m;
    bool rowMaj;
    
    double cp;
    
        
};

typedef BMatrix * BMatrixPtr;


