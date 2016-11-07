#ifndef _SAMPLER_H
#define _SAMPLER_H

#pragma warning(disable:4786)
#include "Matrix.h"
#include <map>
#include <vector>
#include <cstdio>
using namespace std;

class sampler_basic{
    //samples from a probability distrib
public:
    sampler_basic(){}
    double    GetMean();
    void    MakeUniform();
    double    sample(int &i);
    double    entropy();
    double    KL(sampler_basic *h);
    void    Save_p(char *filename);
    double    dotprod(sampler_basic *s);

    virtual int    size()const=0;
    virtual double prob(int i)=0;
    virtual void Set_p(int i, double value)=0;
};

class msampler:public sampler_basic,public Matrix<double>{
public:
    double sum;
    msampler(){}
    msampler(Matrixd &M);
    int        size ()const{return Matrix<double>::size();}
    void    Set_p(int i, double value){(*this)(i)=value;}
    double    sample(int &i){return sampler_basic::sample(i);}
    double    prob(int i){return Data(i)/sum;}
    double    sample(int &x, int &y);
    double    prob(int x, int y);
    void    Normalize();
    void    logprob2prob(double temp);
    void    logprob2prob(double temp, Matrixd &logprobs);
    void    prob2prob(double temp);
    Matrixd    Get_mean();
    Matrixd    Get_var();
    Matrixd    Get_var(Matrixd &mu);
    double    Get_mean1d();
    double    Get_var1d();
    double    Max(){return GetMax()/sum;}
};
class vsampler:public sampler_basic, public vector<double>{
public:
    int        size (){return vector<double>::size();}
    double    prob(int i) {return (*this)[i];}
    void    Set_p(int i, double value){(*this)[i]=value;}
    double    sample(int &i){return sampler_basic::sample(i);}
    void    Normalize();
};


#endif

