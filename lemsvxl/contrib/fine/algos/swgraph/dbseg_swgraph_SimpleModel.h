#ifndef _SIMPLEMODEL_H
#define _SIMPLEMODEL_H
#pragma warning(disable:4786)

#include "matrix.h"
#include "gauss.h"
#include "histogram.h"
#include "irmm_rastercolor.h"

#define OBCOEF 15.
#define COMPL  35.

class Model{
public:
    int    type;    //0=image model,1=motion_model, 2=mixture
    int id;    
    int nnodes;

    Model(int type1, int index){
        type=type1;
        id=index;
        nnodes=0;
    }

    virtual __int64 Get_sumd(){return 0;}
    virtual __int64 Get_sumdd(){return 0;}
    virtual int        Get_type(){return type;}
    virtual void    Update(){};    

    virtual void    Add_item(int i){nnodes++;};//add region i to this model    
    virtual void    Subtract_item(int i){nnodes--;};//subtract region i from this model
    virtual void    Add_model(Model *m){nnodes+=m->nnodes;};
    virtual void    Subtract_model(Model *m){nnodes-=m->nnodes;};
    virtual double    KL(Model *m){return 0;}
    virtual double    SymKL(Model *m);

    virtual double    loglik()=0;    //the prob of the model based on current regions
    virtual int        Get_npix()=0;    //number of pixels assigned to this model
};

class Simple_Image_Model:public Gauss_1d<__int64>{
public:                    //simple image model with gaussian noise 
    Matrixu *I;
    int id;

    Simple_Image_Model(Matrixu *I);
    Simple_Image_Model(int l, Matrixu *I);
    Simple_Image_Model(int l,int xx,int yy, int ws, Matrixu &I, Matrixi &L);

    virtual double    loglik();
    virtual void    Add_item(int i);        //add pixel i to this model    
    virtual void    Subtract_item(int i);    //subtract pixel i from this model
    double    prob_item(int i);

    void    printparams(char *st);
    void    print_values(char *st, int x, int y);
    int        Get_npix(){return sum1;}
    void    Set_sample(int i, Matrixu &Is);
};

#define LBINS 100
#define UBINS 30
#define VBINS 30
#define UBOUND 100
#define VBOUND 130

class Color_model_abstract:public Model, public histogram_sparse_3d{
public:
    double min_var[3];

    Color_model_abstract():Model(0,0),histogram_sparse_3d(LBINS,UBINS,VBINS){
        min_var[0]=10;min_var[1]=8;min_var[2]=8;
        Set_bounds(0,-UBOUND,-VBOUND,100,UBOUND,VBOUND);
    }
    Color_model_abstract(int index):Model(0,index),histogram_sparse_3d(LBINS,UBINS,VBINS){
        min_var[0]=10;min_var[1]=8;min_var[2]=8;
        Set_bounds(0,-UBOUND,-VBOUND,100,UBOUND,VBOUND);
    }

    void    Add_item(int i);//add pixel i to this model    
    void    Subtract_item(int i);//subtract pixel i from this model
    void    Add_model(Model *m);
    void    Subtract_model(Model *m);
    void    Set_motion_noise();

    double    loglik();    //the prob of the model based on current regions
    double    loglik_bound(double e0);    //the prob of the model based on current regions, bound by e0 in entropy
    double    loglik(int i);
    double    loglik(Color_model_abstract *cm);    //the prob of the model based on current regions
    int        Get_npix(){return Get_nsamples();}    //number of pixels assigned to this model
    double    KL(Model *m);
    double    SymKL1(histogram_sparse_3d *h);
    double    KL1(histogram_sparse_3d *h);
    void    printparams(char *st);
    double  prob_item(int i);

    virtual    Matrixd    Get_d(int i)=0;
};
class Color_model:public Color_model_abstract{
public:
    Raster_Color<uchar> *I;

    Color_model(Raster_Color<uchar> *Ic){
        I=Ic;
    }
    Color_model(int index,Raster_Color<uchar> *Ic):Color_model_abstract(index){
        I=Ic;
    }
    Color_model(int l,int xx,int yy, int ws, Raster_Color<uchar> &I, Matrixi &L);

    void    Add_pix(int x,int y);
    void    Subtract_pix(int x,int y);
    void    Set_sample(int i, Raster_Color<uchar> &Is);

    int        Get_l(int i);
    int        Get_u(int i);
    int        Get_v(int i);
    int        Get_l(int x, int y);
    int        Get_u(int x, int y);
    int        Get_v(int x, int y);
    Matrixd Get_d(int x, int y);
    Matrixd Get_d(int i);
    void    print_values(char *st, int x, int y);
};

#endif

