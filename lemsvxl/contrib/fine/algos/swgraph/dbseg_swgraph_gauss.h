#ifndef _GAUSS_H
#define _GAUSS_H

#include "Matrix.h"
#include <vector>
using namespace std;

class Gauss_model{    //gaussian model, abstract, with logprob
public:
    double    binsize;
    double    mean;
    double    var;

    Gauss_model(){
        binsize=1;
    }
    Gauss_model(double bin_size){
        binsize=bin_size;
    }
    double    logprob_norm();            // the log gaussian probability, unadjusted by the bin size
    double    logprob_norm(double x);    // the log gaussian probability, unadjusted by the bin size
    double    prob_norm(double x);    // the gaussian probability, unadjusted by the bin size
    double    logprob_bin();
    double    logprob_bin(double x);
    double    prob_bin(double x);        // the gaussian probability, adjusted by the bin size
    double    prob_unnorm(double x);        // unnormalized prob
    virtual double Get_var(){return var;}
    virtual double Get_mean(){return mean;}
    virtual double Get_n(){return 0;} // the number of samples of this gaussian
//    void    Draw(double min, double max, int n, CDC* dc, int dx, int dy);
//    virtual bool changed()=0;
};

template<class Tp>
class Gauss_1d:public Gauss_model{    //gaussian fit of samples, incremental
public:
    Tp        sum1,sumx,sumxx;
    double  weight;                        // make all the samples weigh less or more
    bool    bchanged;

    Gauss_1d():Gauss_model(){
        sum1=0;sumx=0;sumxx=0;weight=1.;
        bchanged=true;
    }
    Gauss_1d(double bin_size):Gauss_model(bin_size){
        sum1=0;sumx=0;sumxx=0;weight=1.;
        bchanged=true;
    }
    bool    operator==(Gauss_1d<Tp> &x);
    void    operator=(Gauss_1d<Tp> &x);

    void    Add_sample(Tp x);
    void    Subtract_sample(Tp x);
    void    Add_sample(Tp x, Tp n);
    void    Subtract_sample(Tp x, Tp n);
    void    Add_model(Gauss_1d<Tp> *m);
    void    Subtract_model(Gauss_1d<Tp> *m);
    void    Add_model(Gauss_1d<Tp> *m, Tp n);
    void    Subtract_model(Gauss_1d<Tp> *m,Tp n);

    double    Get_n(){return sum1*weight;}
    double    Get_mean(){if (sum1>0) return sumx/(double)sum1;return 0;}
    virtual double    Get_var(){if (bchanged) Update();return var;}
    double    logprob(double n, double mu, double var){if (var<5*binsize) var=5*binsize;
            return -(sumxx-2*mu*sumx+n*mu*mu)/2./var+n*log(pow(binsize,2)/(2.*PI*var))/2.;}
    double    logprob(){return Gauss_model::logprob_bin();}
    double    logprob_add(Gauss_1d<Tp> *G);
    double    logprobsimple(double x);
    double    SymKL(Gauss_1d *G){
        double m1,m2,v1,v2;
        m1=Get_mean();v1=Get_var();        
        m2=G->Get_mean();v2=G->Get_var();
        return ::SymKL(m1,v1,m2,v2);
    }
    double    KL(Gauss_1d *G){
        double m1,m2,v1,v2;
        m1=Get_mean();v1=Get_var();        
        m2=G->Get_mean();v2=G->Get_var();
        return ::KL(m1,v1,m2,v2);
    }

    double    sample(){return Get_mean()+nrand()*sqrt(Get_var());}
    bool    changed(){return bchanged;}
    void    Update();
    void    Zero(){sum1=0;sumx=sumxx=0;bchanged=true;}
};

template<class Tp>
class Gauss_1d_extra:public Gauss_1d<Tp>{    //gaussian with extras for statistics,(mean, var)
                                        // the gaussian is this+extra, the extras are added in th extra
public:
    Gauss_1d<Tp> extra;

    Gauss_1d_extra(double bin_size):Gauss_1d<Tp>(bin_size){
        extra.binsize=bin_size;
    }
    double    logprob();
    void    Add_extra(Tp i);        //add data d to this model    
    void    Subtract_extra(Tp i);    //subtract data d from this model
    void    Add_extra(Tp i, int n);        //add data d to this model    
    void    Subtract_extra(Tp i, int n);    //subtract data d from this model
    void    Add_extra(Gauss_1d<Tp> *m);
    void    Subtract_extra(Gauss_1d<Tp> *m);
    void    Zero(){Gauss_1d<Tp>::Zero();extra.Zero();}
    double    Get_mean(){if (sum1+extra.sum1>0) return (sumx+extra.sumx)/(double)(sum1+extra.sum1);return 0;}
    virtual double    Get_var(){if (bchanged) Update();return var;}
    void    Update();
    int        Get_n(){return sum1;}
};

template<class Tp>
class Gauss_multi;

template<class Tp>
class Gauss_4multi:public Gauss_model{ //adapter from Gauss_model to gauss_multi
public:
    Gauss_multi<Tp> *parent;
    int l;

    Gauss_4multi(double binsize,int l,Gauss_multi<Tp> *parent);

    bool    changed();
    double    Get_var();
    double    Get_mean();
    double    Get_n();
};

template<class Tp>
class Gauss_multi{    //gaussian fit of samples, incremental
public:
    vector<Gauss_4multi<Tp>  *>    component;

    Matrixd U,V;            //rotation to make them indep (U=V')
    Matrixd lambda;            //diagonal ver of covariance matrix
    Matrixd lambdainv;        //diagonal ver of inv covariance matrix
    Matrix<double> mu;
    Matrix<double> var;
    Matrix<double> varinv,adjvarinv;
    Matrix<double> sigma;
    double    det_var,log_det_var;
    double    binsize;

    Tp        nsamples;
    Matrix<Tp> sumx,sumxx;
    int        ndim;
    bool    changed;

    Gauss_multi();
    Gauss_multi(int n);
    Gauss_multi(int n,double bin_size);
    ~Gauss_multi();
    void Init(int n,double bin_size);

    //Incremental operations
    void    Add_sample(Matrix<Tp> &x);
    void    Subtract_sample(Matrix<Tp> &x);
    void    Add_sample(Matrix<Tp> &x,Tp n);
    void    Subtract_sample(Matrix<Tp> &x,Tp n);
    void    Add_model(Gauss_multi<Tp> *m, Tp n);
    void    Subtract_model(Gauss_multi<Tp> *m, Tp n);

    Matrixd    Get_mean(){if (changed) Update();return mu;}
    Matrixd    Get_var(){if (changed) Update();return var;}
    Matrixd    GetAdjVar(){if (changed) Update();return U*lambda*V.T();}
    Matrixd    Get_varinv(){if (changed) Update();return adjvarinv;}
    double    Get_var(int i){if (changed) Update();return lambda(i,i);}
    double    Get_mean(int i){if (changed) Update();return mu(i);}
    double    Get_n(){return nsamples;}
    double    Get_sumsq_eig();

    void    Set(Matrixd &mean,Matrixd &variance);
    double    logprob();
    double    logprob(Matrixd &x);
    void    sample(Matrix<double> &x);
    void    Update();
    void    Update_muvar();
    void    Update_vlambda();
    double    SymKL(Gauss_multi<Tp> *G);
    void    Zero();
};

class Gauss_pdf{
public:
    Matrix<double> mu;
    Matrix<double> sigma;
    Gauss_pdf(){mu.SetDimension(0,0);sigma.SetDimension(0,0);}
    Matrix<double> sample();
    double prob(Matrix<double> &x);
    double pmf(double precision);
    void prints(char *text);
    Gauss_pdf & operator=(const Gauss_pdf &p){
        if(this!=&p){
            mu=p.mu;
            sigma=p.sigma;
        }
        return *this;
    };
};
typedef Gauss_pdf *gaussp;
class mixgauss{
public:
    int n;                    //number of gaussians in the mixture
    Gauss_pdf *g;
    double *coeff;            //assume coeffs sum to 1;
    mixgauss(){
        g=NULL;coeff=NULL;
    }
    void create(int n);
    Matrix<double> sample();
    double prob(Matrix<double> &x);
};
void Multiply(Matrixd &mu1,Matrixd &var1inv, Matrixd &mu2, Matrixd &var2inv, Matrixd &mu, Matrixd &var);
void Multiply(Matrixd &mu1,Matrixd &var1inv,double coeff1, Matrixd &mu2, Matrixd &var2inv, Matrixd &mu, Matrixd &var);
void getImageParams(int mbx, int mby, int xc, int yc, double Temp, Matrixu &M, Matrixu &I1, Gauss_pdf &s);
Matrix<float> gaussker(double xd, double yd,double sigmax, double sigmay);
Matrix<float> gaussker(double xd, double yd,double sigmax, double sigmay, double angle);
Matrix<float> gausspmf(double xd, double yd, Matrix<double> sigma);
double GaussDotProd (double mu1, double var1, double mu2, double var2);
double KL(double mu1, double var1, double mu2, double var2);
double KL(Matrixd &mu1, Matrixd &var1, Matrixd &mu, Matrixd &var);
double KL1(Matrixd &mu1, Matrixd &var1, Matrixd &mu, Matrixd &varinv);
double KL(Matrixd &prob,double x0, double y0, Matrixd &mu2, Matrixd &var2);
double KL(Matrixd &prob, double mu, double var);
double SymKL(double mu1, double var1, double mu2, double var2);
double SymKL(Matrixd &mu1, Matrixd &var1, Matrixd &mu2, Matrixd &var2);
Matrixd Adjust_var(Matrixd &var, double min);
Matrixd Adjust_var(Matrixd &var, double *min);
void    Project_Affine(Matrixd &mu6, Matrixd &var6inv, Matrixd &x, Matrixd &mu, Matrixd &var);
void    Project_Affine1(Matrixd &mu6, Matrixd &var6inv, Matrixd &x, Matrixd &mu, Matrixd &varinv);

template<class Tp>
bool Gauss_1d<Tp>::operator==(Gauss_1d<Tp> &x){
    if (sum1!=x.sum1)
        return false;
    if (sumx!=x.sumx)
        return false;
    if (sumxx!=x.sumxx)
        return false;
    return true;
}
template<class Tp>
void Gauss_1d<Tp>::operator=(Gauss_1d<Tp> &x){
    if (this!=&x){
        sum1=x.sum1;
        sumx=x.sumx;
        sumxx=x.sumxx;
        var=x.var;
        bchanged=x.bchanged;
        weight=x.weight;
    }
}

template<class Tp>
void    Gauss_1d<Tp>::Add_sample(Tp x){
    sum1++;
    sumx+=x;
    sumxx+=x*x;
    bchanged=true;
}
template<class Tp>
void    Gauss_1d<Tp>::Subtract_sample(Tp x){
    sum1--;
    sumx-=x;
    sumxx-=x*x;
    bchanged=true;
}
template<class Tp>
void    Gauss_1d<Tp>::Add_sample(Tp x, Tp n){
    sum1+=n;
    sumx+=x*n;
    sumxx+=x*x*n;
    bchanged=true;
}
template<class Tp>
void    Gauss_1d<Tp>::Subtract_sample(Tp x, Tp n){
    sum1-=n;
    sumx-=x*n;
    sumxx-=x*x*n;
    bchanged=true;
}
template<class Tp>
void Gauss_1d<Tp>::Add_model(Gauss_1d<Tp> *m){
    sum1+=m->sum1;
    sumx+=m->sumx;
    sumxx+=m->sumxx;
    bchanged=true;
}
template<class Tp>
void Gauss_1d<Tp>::Subtract_model(Gauss_1d<Tp> *m){
    sum1-=m->sum1;
    sumx-=m->sumx;
    sumxx-=m->sumxx;
    bchanged=true;
}
template<class Tp>
void Gauss_1d<Tp>::Add_model(Gauss_1d<Tp> *m,Tp n){
    sum1+=m->sum1*n;
    sumx+=m->sumx*n;
    sumxx+=m->sumxx*n;
    bchanged=true;
}
template<class Tp>
void Gauss_1d<Tp>::Subtract_model(Gauss_1d<Tp> *m, Tp n){
    sum1-=m->sum1*n;
    sumx-=m->sumx*n;
    sumxx-=m->sumxx*n;
    bchanged=true;
}

template<class Tp>
void    Gauss_1d<Tp>::Update(){
    if (sum1>1)
        var=(sumxx-(sumx*sumx)/(double)sum1)/(sum1-1.);
    else
        var=0;
    if (var<5*binsize)
        var=5*binsize;
    bchanged=false;
}

//
// Gauss_1d_extra
//
template<class Tp>
double    Gauss_1d_extra<Tp>::logprob(){
    double n=Get_n();
    double mu=Get_mean();
    double var=Get_var();
    return -(sumxx-2*mu*sumx+n*mu*mu)/2./var+n*log(pow(binsize,2)/(2.*PI*var))/2.;
}
template<class Tp>
void    Gauss_1d_extra<Tp>::Add_extra(Tp x){
    extra.Add_sample(x);
    bchanged=true;
}
template<class Tp>
void    Gauss_1d_extra<Tp>::Subtract_extra(Tp x){
    extra.Subtract_sample(x);
    bchanged=true;
}
template<class Tp>
void    Gauss_1d_extra<Tp>::Add_extra(Tp x, int n){
    extra.Add_sample(x,n);
    bchanged=true;
}
template<class Tp>
void    Gauss_1d_extra<Tp>::Subtract_extra(Tp x, int n){
    extra.Subtract_sample(x,n);
    bchanged=true;
}
template<class Tp>
void Gauss_1d_extra<Tp>::Add_extra(Gauss_1d<Tp> *m){
    extra.Add_model(m);    
    bchanged=true;
}
template<class Tp>
void Gauss_1d_extra<Tp>::Subtract_extra(Gauss_1d<Tp> *m){
    extra.Subtract_model(m);
    bchanged=true;
}
template<class Tp>
void    Gauss_1d_extra<Tp>::Update(){
    double s1=sum1+extra.sum1;
    double sx=sumx+extra.sumx;
    double sxx=sumxx+extra.sumxx;
    if (s1>1)
        var=(sxx-(sx*sx)/s1)/(s1-1.);
    else
        var=0;
    if (var<5*binsize)
        var=5*binsize;
    bchanged=false;
}

template<class Tp>
Gauss_4multi<Tp>::Gauss_4multi(double binsize,int label,Gauss_multi<Tp> *owner):Gauss_model(binsize){
    parent=owner;
    l=label;
}

template<class Tp>
bool Gauss_4multi<Tp>::changed(){
    return parent->changed;
}
template<class Tp>
double Gauss_4multi<Tp>::Get_var(){
    return parent->Get_var(l);
}
template<class Tp>
double Gauss_4multi<Tp>::Get_mean(){
    return parent->Get_mean(l);
}

template<class Tp>
double Gauss_4multi<Tp>::Get_n(){
    return parent->Get_n();
}

template<class Tp>
Gauss_multi<Tp>::Gauss_multi(){
    ndim=0;
    changed=true;
    nsamples=0;
    binsize=1;
}
template<class Tp>
Gauss_multi<Tp>::Gauss_multi(int n){
    ndim=n;
    component.reserve(n);
    for (int i=0;i<ndim;i++)
        component.push_back(new Gauss_4multi<Tp>(1,i,this));
    changed=true;
    nsamples=0;
    sumx.SetDimension(ndim,1);sumx.Zero();
    sumxx.SetDimension(ndim,ndim);sumxx.Zero();
    binsize=1;
}
template<class Tp>
Gauss_multi<Tp>::Gauss_multi(int n,double bin_size){
    Init(n,bin_size);
}
template<class Tp>
Gauss_multi<Tp>::~Gauss_multi(){
    for (int i=0;i<ndim;i++){
        delete component[i];
    }
}
template<class Tp>
void Gauss_multi<Tp>::Init(int n,double bin_size){
    ndim=n;
    component.reserve(n);
    for (int i=0;i<ndim;i++)
        component.push_back(new Gauss_4multi<Tp>(bin_size,i,this));
    binsize=bin_size;
    changed=true;
    nsamples=0;
    sumx.SetDimension(ndim,1);sumx.Zero();
    sumxx.SetDimension(ndim,ndim);sumxx.Zero();
}
template<class Tp>
void Gauss_multi<Tp>::Zero(){
    changed=true;
    nsamples=0;
    sumx.SetDimension(ndim,1);sumx.Zero();
    sumxx.SetDimension(ndim,ndim);sumxx.Zero();
}

template<class Tp>
double    Gauss_multi<Tp>::logprob(){
    int i;
    double sum=0;
    for (i=0;i<ndim;i++){
        sum+=component[i]->logprob();
    }
    return sum;
}
template<class Tp>
double    Gauss_multi<Tp>::logprob(Matrixd &x){
    Matrixd y=x-mu;
    Matrixd z=y.T()*adjvarinv*y;
    return -z(0)-log(binsize*binsize/2/PI)*ndim/2.-log_det_var/2.;
}

template<class Tp>
void Gauss_multi<Tp>::Add_sample(Matrix<Tp> &x){
    nsamples++;
    sumx=sumx+x;
    sumxx=sumxx+x*x.T();
    changed=true;
}
template<class Tp>
void Gauss_multi<Tp>::Subtract_sample(Matrix<Tp> &x){
    nsamples--;
    sumx=sumx-x;
    sumxx=sumxx-x*x.T();
    changed=true;
}
template<class Tp>
void Gauss_multi<Tp>::Add_model(Gauss_multi<Tp> *m, Tp n){
    nsamples+=m->nsamples*n;
    sumx=sumx+m->sumx*n;
    sumxx=sumxx+m->sumxx*n;
    changed=true;
}
template<class Tp>
void Gauss_multi<Tp>::Subtract_model(Gauss_multi<Tp> *m, Tp n){
    nsamples-=m->nsamples*n;
    sumx=sumx-m->sumx*n;
    sumxx=sumxx-m->sumxx*n;
    changed=true;
}

template<class Tp>
void Gauss_multi<Tp>::Add_sample( Matrix<Tp> &x, Tp n){
    nsamples+=n;
    sumx=sumx+x*n;
    sumxx=sumxx+x*x.T()*n;
    changed=true;
}
template<class Tp>
void Gauss_multi<Tp>::Subtract_sample(Matrix<Tp> &x, Tp n){
    nsamples-=n;
    sumx=sumx-x*n;
    sumxx=sumxx-x*x.T()*n;
    changed=true;
}
template<class Tp>
void Gauss_multi<Tp>::sample(Matrix<double> &x){
    Matrix<double> w;
    int i,r;
    r=mu.size();
    w.SetDimension(r,1);
    for (i=0;i<r;i++)
        w(i)=nrand();
    x=mu+sigma*w;
//    return prob(x);
}

template<class Tp>
void Gauss_multi<Tp>::Update(){
    Update_muvar();
    Update_vlambda();
    changed=false;
}
template<class Tp>
void Gauss_multi<Tp>::Set(Matrixd &mean,Matrixd &variance){
    mu=mean;
    var=variance;
    varinv=var.Inv();
    nsamples=1000;
    assign_matrix(sumx,mu*nsamples);
    assign_matrix(sumxx,mu*mu.T()*nsamples+var*(nsamples-1));
    Update_vlambda();
    changed=false;
}

template<class Tp>
void Gauss_multi<Tp>::Update_muvar(){
    double n=Get_n();
    if (n>0){
        assign_matrix(mu,sumx);
        mu=mu/n;
    }
    else{
        mu.SetDimension(ndim,1);
        mu.Zero();
    }
    switch((int)Get_n()){
    case 0:
        var.SetDimension(ndim,ndim);
        var.Identity();var=var*100;
        break;
    case 1:
        var.SetDimension(ndim,ndim);
        var.Identity();var=var*10;
        break;
    default:
        Matrixd sumX,sumXX;
        assign_matrix(sumX,sumx);
        assign_matrix(sumXX,sumxx);
        var=(sumXX-(sumX*sumX.T())/n)/(n-1.);
    }
    varinv=var.Inv();
}
template<class Tp>
void Gauss_multi<Tp>::Update_vlambda(){
    Matrixd sqrtlambda;
    for (int i=0;i<ndim;i++){
        var(i,i)+=component[i]->binsize/1000.;
    }
    SVD(var,U,lambda,V);
    lambdainv.SetDimension(ndim,ndim);lambdainv.Zero();
    sqrtlambda.SetDimension(ndim,ndim);sqrtlambda.Zero();
    det_var=1;
    for (i=0;i<ndim;i++){
        if (lambda(i,i)<5*component[i]->binsize)
            lambda(i,i)=5*component[i]->binsize;
        det_var*=lambda(i,i);
        lambdainv(i,i)=1./lambda(i,i);
        sqrtlambda(i,i)=sqrt(lambda(i,i));
    }
    log_det_var=log(det_var);
    adjvarinv=U*lambdainv*V.T();
    sigma=U*sqrtlambda*V.T();
}
template<class Tp>
double    Gauss_multi<Tp>::Get_sumsq_eig(){
    int i;
    double sum=0;
    if (changed)
        Update();
    for (i=0;i<ndim;i++){
        sum+=lambda(i,i);
    }
    return sum;
}

template<class Tp>
double    Gauss_multi<Tp>::SymKL(Gauss_multi<Tp> *G){
    Matrixd v1=GetAdjVar(),v2=G->GetAdjVar();
    Matrixd v1i=Get_varinv(),v2i=G->Get_varinv();
    Matrixd mu1=Get_mean(),mu2=G->Get_mean();
    Matrixd rez=(mu1-mu2).T()*(v1i+v2i)*(mu1-mu2)/2.+(v1-v2)*(v2i-v1i)/2;
//              (mu1-mu2)*(mu1-mu2)*(1./var1+1./var2)/2.+0.5*(var1-var2)*(var1-var2)/var1/var2;
    return rez(0);
}
#endif

