#include "simplemodel.h"
#include "functions.h"
#include "gauss.h"


double Model::SymKL(Model *m){
    return KL(m)+m->KL(this);
}

//
//    Simple_Image_Model Class
//
Simple_Image_Model::Simple_Image_Model(int la, Matrixu *Im){
    id=la;
    I=Im;
    Zero();
}
Simple_Image_Model::Simple_Image_Model(Matrixu *Im){
    id=0;
    I=Im;
    Zero();
}
Simple_Image_Model::Simple_Image_Model(int la,int xx,int yy, int ws, Matrixu &Im, Matrixi &L){    
    int x,y,x0,y0,x1,y1,nx=Im.nx();
    id=la;
    I=&Im;
    Zero();
    x0=xx-ws;if (x0<0) x0=0;
    y0=yy-ws;if (y0<0) y0=0;
    x1=xx+ws+1;if (x1>Im.nx()) x1=Im.nx();
    y1=yy+ws+1;if (y1>Im.ny()) y1=Im.ny();
    for (y=y0;y<y1;y++)
        for (x=x0;x<x1;x++){
            if (L(y,x)==id+1)
                Add_item(y*nx+x);
        }
}
double Simple_Image_Model::loglik(){
    return Gauss_1d<__int64>::logprob();
}
double    Simple_Image_Model::prob_item(int i){
    int d=I->Data(i);
    return prob_bin(d);
}
void Simple_Image_Model::Set_sample(int i, Matrix<uchar> &Is){
    int nx=Is.cols();
    int d=sample();
    if (d<0) d=0;
    if (d>255) d=255;
    Is(i)=d;
}

void Simple_Image_Model::Add_item(int i){
    int d=I->Data(i);
    Add_sample(d);
}
void Simple_Image_Model::Subtract_item(int i){
    int d=I->Data(i);
    Subtract_sample(d);
}
void Simple_Image_Model::printparams(char *st){
    int n=Get_npix();
    sprintf(st,"%d,%1.1f,%1.1f",n,Get_mean(),Get_var());
}
void Simple_Image_Model::print_values(char *st, int x, int y){
    sprintf(st,"%d",I->Data(y,x));
}
/*
void Simple_Image_Model::Draw(CDC* dc, int wx, int wy){
    Gauss_1d<__int64>::Draw(10,255,15,dc,wx,wy);
}
*/

//
// Color_model_abstract
// 
void Color_model_abstract::Set_motion_noise(){
    Set_nbins(2*LBINS,2*UBINS,2*VBINS);
    Set_bounds(-100,-2*UBOUND,-2*VBOUND,100,2*UBOUND,2*VBOUND);
}

void    Color_model_abstract::Add_item(int i){//add pixel i to this model    
    Matrixd m=Get_d(i);
    histogram_abstract_3d::Add(m(0),m(1),m(2));
}
void    Color_model_abstract::Subtract_item(int i){//subtract pixel i from this model
    Matrixd m=Get_d(i);
    histogram_abstract_3d::Subtract(m(0),m(1),m(2));
}
void    Color_model_abstract::Add_model(Model *m){
    Color_model_abstract *cm=(Color_model_abstract *)m;
    (*this)+=cm;
}
void    Color_model_abstract::Subtract_model(Model *m){
    Color_model_abstract *cm=(Color_model_abstract *)m;
    (*this)-=cm;
}

double Color_model_abstract::loglik(){
    return -Get_nsamples()*entropy();
}
double Color_model_abstract::loglik_bound(double e0){
    double e=entropy();
    if (e<e0) e=e0;
    return -Get_nsamples()*e;
}
double    Color_model_abstract::loglik(Color_model_abstract *cm){
    histogram_sparse_3d::iterator it;
    double sum=0,n=cm->Get_nsamples();
    if (n>0){
//        check();
        for (it=begin();it!=end();++it){
            int k=(*it).second;
            int bin=(*it).first;
            double d=cm->Get_freq(bin);
            sum+=k*log(d);
        }
    }
    return sum;
}
double    Color_model_abstract::loglik(int i){
    //log lik of pixel i    
    Matrixd m=Get_d(i);
    int bin=Get_bin(m(0),m(1),m(2));
    double d=Get_freq(bin);
    return log(d);
}
double    Color_model_abstract::prob_item(int i){
    Matrixd m=Get_d(i);
    int bin=Get_bin(m(0),m(1),m(2));
    return Get_freq(bin);
}

double    Color_model_abstract::KL(Model *m){
    Color_model_abstract *cm=(Color_model_abstract *)m;
    return histogram_sparse_3d::KL(cm);
}

double    Color_model_abstract::SymKL1(histogram_sparse_3d *h){
    Matrixd mu1,var1,mu2,var2;

    mu1=Get_mean();
    var1=Get_var();
    var1(0,0)+=0.01;var1(1,1)+=0.01;var1(2,2)+=0.01;
    var1=Adjust_var(var1,min_var);
    mu2=h->Get_mean();
    var2=h->Get_var()+0.01;
    var2(0,0)+=0.01;var2(1,1)+=0.01;var2(2,2)+=0.01;
    var2=Adjust_var(var2,min_var);
    return ::SymKL(mu1,var1,mu2,var2);
}
double    Color_model_abstract::KL1(histogram_sparse_3d *h){
    Matrixd mu1,var1,mu2,var2;

    mu1=Get_mean();
    var1=Get_var();
    var1(0,0)+=0.01;var1(1,1)+=0.01;var1(2,2)+=0.01;
    var1=Adjust_var(var1,min_var);
    mu2=h->Get_mean();
    var2=h->Get_var();
    var2(0,0)+=0.01;var2(1,1)+=0.01;var2(2,2)+=0.01;
    var2=Adjust_var(var2,min_var);
    return ::KL(mu1,var1,mu2,var2);
}

void Color_model_abstract::printparams(char *st){
    Matrixd l,m=Get_mean();
    char st1[255];
    sprintf(st1,"%d,%1.1f",Get_npix(),entropy());
    strcpy(st,st1);
    Get_eig(l);
    sprintf(st1,",%1.1f,%1.1f,%1.1f,%1.1f,%1.1f,%1.1f",m(0),m(1),m(2),l(0),l(1),l(2));
    strcat(st,st1);
}
//
// Color_model
// 
Color_model::Color_model(int l,int xx,int yy, int ws, Raster_Color<uchar> &Im, Matrixi &L):Color_model_abstract(l){    
    int x,y,x0,y0,x1,y1;
    I=&Im;
    Zero();
    x0=xx-ws;if (x0<0) x0=0;
    y0=yy-ws;if (y0<0) y0=0;
    x1=xx+ws+1;if (x1>Im.cols()) x1=Im.cols();
    y1=yy+ws+1;if (y1>Im.rows()) y1=Im.rows();
    for (y=y0;y<y1;y++)
        for (x=x0;x<x1;x++){
            if (L(y,x)==l+1)
                Add_pix(x,y);
        }
}
void Color_model::Set_sample(int i, Raster_Color<uchar> &Is){
    Matrixd m;
    int nx=Is.cols();
    sample(m);
    Is.SetPixelInLUV(i/nx,i%nx,m(0),m(1),m(2));
}

Matrixd        Color_model::Get_d(int i){
    Matrixd m(3,1);
    m(0)=I->L(i);
    m(1)=I->U(i);
    m(2)=I->V(i);
    return m;
}
int        Color_model::Get_l(int i){
    return I->L(i);
}
int        Color_model::Get_u(int i){
    return I->U(i);
}
int        Color_model::Get_v(int i){
    return I->V(i);
}
int        Color_model::Get_l(int x, int y){
    return Get_l(y*I->cols()+x);
}
int        Color_model::Get_u(int x, int y){
    return Get_u(y*I->cols()+x);
}
int        Color_model::Get_v(int x, int y){
    return Get_v(y*I->cols()+x);
}
Matrixd Color_model::Get_d(int x, int y){
    return Get_d(y*I->cols()+x);
}

void Color_model::Add_pix(int x,int y){
    int i=y*I->cols()+x;
    histogram_abstract_3d::Add(Get_l(i),Get_u(i),Get_v(i));
}
void Color_model::Subtract_pix(int x,int y){
    int i=y*I->cols()+x;
    histogram_abstract_3d::Subtract(Get_l(i),Get_u(i),Get_v(i));
}
void Color_model::print_values(char *st, int x, int y){
    int i=Get_bin(Get_l(x,y),Get_u(x,y),Get_v(x,y));
    Matrixd m=Get_bin_center(i);
    sprintf(st,"%d,%d,%d,%1.1f,%1.1f,%1.1f",Get_l(x,y),Get_u(x,y),Get_v(x,y),m(0),m(1),m(2));
}




