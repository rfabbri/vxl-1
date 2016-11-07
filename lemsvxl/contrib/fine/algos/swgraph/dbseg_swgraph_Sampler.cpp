#pragma warning(disable:4786)
#include "sampler.h"
#include "functions.h"
using namespace std;

//
// sampler_basic class=basic sampler
//


double sampler_basic::sample(int &i){
    double sum1,d1,p;
    int n=size();
    d1=randdbl(1);            //sample
    sum1=0;
    for (i=0;i<n;i++){
        p=prob(i);
        sum1+=p;
        if (d1<=sum1&&p>0){
            return p;
        }
    }
    return 0;
}

double sampler_basic::GetMean(){
    int i,n=size();
    double sumxi;
    sumxi=0;
    for (i=0;i<n;i++){
        sumxi+=prob(i)*i;
    }
    return sumxi;
}

double sampler_basic::entropy(){
    double d,sum=0.;
    int i,n=size();

    for (i=0;i<n;i++){
        d=prob(i);
        if (d>0.0){
            sum += d*log(d);
        }
    }
    return -sum;
}


double sampler_basic::KL(sampler_basic *h){
    //assume they have the same bins
    double p,q,sum=0.;
    int i,n=size();

    for (i=0;i<n;i++){
        p=prob(i);
        q=h->prob(i);
        if (p>0.000000001){
            if (q>0.00000001)
                sum += p*log(p/q);
            else
                sum+=p*log(p/0.00000001);
        }
    }
    return sum;
}


void sampler_basic::Save_p(char *filename){
    int i,n=size();
    FILE *f;

    f=fopen(filename,"w");
    for (i=0;i<n-1;i++){
        fprintf(f,"%f,",prob(i));
    }
    fprintf(f,"%f",prob(n-1));
    fclose(f);
}


void sampler_basic::MakeUniform(){
    int i,n=size();
    double val=1./n;
    for (i=0;i<n;i++)
        Set_p(i,val);
}
double sampler_basic::dotprod(sampler_basic *s){
    int m=size();
    int j;
    double sumi;
    sumi=0;
    for (j=0;j<m;j++)
        sumi+=prob(j)*s->prob(j);
    return sumi;
}


msampler::msampler(Matrixd &M){
    (*(Matrixd *)this)=M;
    sum=M.Sum();
}

double    msampler::sample(int &x, int &y){
    int i,nnx=nx();
    double p=sample(i);
    x=i%nnx;
    y=i/nnx;
    return p;
}
double    msampler::prob(int x, int y){
    int i=y*nx()+x;
    return prob(i);
}
void msampler::logprob2prob(double temp){
    int i,n=nx()*ny();
    double max=GetMax();
    for (i=0;i<n;i++){
        (*this)(i)=exp((Data(i)-max)/temp);
    }
    Normalize();
}
void msampler::logprob2prob(double temp, Matrixd &logprobs){
    int i,n=nx()*ny();
    double max=GetMax();
    logprobs.SetDimension(ny(),nx());
    for (i=0;i<n;i++){
        logprobs(i)=(Data(i)-max)/temp;
        (*this)(i)=exp((Data(i)-max)/temp);
    }
    Normalize();
    logprobs=logprobs-log(sum);
}
void msampler::prob2prob(double temp){
    int i,n=nx()*ny();
    double max=GetMax(),t1=1./temp;
    assert(max!=0);
    for (i=0;i<n;i++){
        (*this)(i)=pow(Data(i)/max,t1);
    }
    Normalize();
}

void msampler::Normalize(){
    sum=Sum();
    if (sum==0){
        (*this)=(*this)+1./size();
        sum=1;
    }
}

Matrixd    msampler::Get_mean(){
    double p,sumx,sumy,sump;
    int x,y,nx1=nx(),ny1=ny();
    Matrixd mu(2,1);
    int i,n=size();
    sumx=sumy=sump=0;
    for (i=0;i<n;i++){
        x=(i%nx1);y=(i/nx1);
        p=prob(i);
        sumx+=p*x;
        sumy+=p*y;
        sump+=p;
    }
    mu(0)=sumx/sump;
    mu(1)=sumy/sump;
    return mu;
}
Matrixd    msampler::Get_var(Matrixd &mu){
    double p,sump,sumxx,sumxy,sumyy,x,y;
    Matrixd var(2,2);
    mu=Get_mean();
    int i,n=size(),ix,iy,nx1=nx(),ny1=ny();
    sumxx=sumxy=sumyy=sump=0;
    for (i=0;i<n;i++){
        p=prob(i);
        ix=i%nx1;iy=i/nx1;
        x=ix-mu(0);y=iy-mu(1);
        sump+=p;
        sumxx+=p*x*x;
        sumxy+=p*x*y;
        sumyy+=p*y*y;
    }
    var(0,0)=sumxx/sump;
    var(0,1)=var(1,0)=sumxy/sump;
    var(1,1)=sumyy/sump;
    return var;
}
Matrixd    msampler::Get_var(){
    Matrixd mu;
    return Get_var(mu);
}
double    msampler::Get_mean1d(){
    double p,sumx,sump;
    int x;
    int i,n=size();
    sumx=sump=0;
    for (i=0;i<n;i++){
        x=i;
        p=prob(i);
        sumx+=p*x;
        sump+=p;
    }
    return sumx/sump;
}
double    msampler::Get_var1d(){
    double p,sump,sumxx,x;
    double mu=Get_mean1d();
    int i,n=size();
    sumxx=sump=0;
    for (i=0;i<n;i++){
        p=prob(i);
        x=i-mu;
        sump+=p;
        sumxx+=p*x*x;
    }
    return sumxx/sump;
}


void Normalize(vector<double> &d){
    int i,n=d.size();
    double sum=0;
    for (i=0;i<n;i++)
        sum+=d[i]*d[i];
    sum=sqrt(sum);
    if (fabs(sum)>1){
        for (i=0;i<n;i++)
            d[i]=d[i]/sum;
    }

}





