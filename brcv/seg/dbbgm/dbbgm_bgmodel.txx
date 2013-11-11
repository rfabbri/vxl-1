#ifndef dbbgm_bgmodel_txx_
#define dbbgm_bgmodel_txx_

#include <dbbgm/dbbgm_bgmodel.h>
#include <vil/vil_image_view.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>

#define DISTFROMEAN 2.5
//The initial standarddev given when only a single point is currently in the model.
#define MEANBIT   0
#define INITIALSTANDARDDEV 20
#define  MINSTANDARDDEV 6
#define MAXVAR 393999
#define INITIALWEIGHT   0.2f
#define pi 3.141592654
#define WEIGHTMAX 1.0
#define WEIGHTMIN 0


template<class T>
dbbgm_bgmodel<T>::dbbgm_bgmodel()
{
}
template<class T>
dbbgm_bgmodel<T>::dbbgm_bgmodel(vil_image_view<T> img,int nummodes, int numframes)
{
        curr_img_=img;
        updatefrozen=false;
        no_of_modes = nummodes;
        L=numframes;
        ni=img.ni();
        nj=img.nj();
        currentwinner.resize(img.nj(),img.ni());//JLM
        initialstd_=15;
        model.initialize(nummodes,ni,nj);
        docalculations();
        MINWEIGHT=0.75;
}
template<class T>
dbbgm_bgmodel<T>::dbbgm_bgmodel(dbbgm_model_data modelin)
{
        model=modelin;
        no_of_modes=modelin.nmodes;
        ni=modelin.isobserved.cols();
                nj=modelin.isobserved.rows();
                currentwinner.resize(nj,ni);//JLM
}
template<class T>
dbbgm_bgmodel<T>::~dbbgm_bgmodel()
{
}
template<class T>
bool dbbgm_bgmodel<T>::updateModel(vil_image_view<T>  img)
{
        curr_img_=img;
        return docalculations();
    
}
template<class T>
void dbbgm_bgmodel<T>::setintialstddev(float initialstddev)
{
        initialstd_=initialstddev;
}
template<class T>
void dbbgm_bgmodel<T>::setminweight(float minweight)
{
        MINWEIGHT=minweight;
}
template<class T>
void dbbgm_bgmodel<T>::setlframes(int lframes)
{
        L=lframes;
}
template<class T>
bool dbbgm_bgmodel<T>::docalculations()
{
        if(!curr_img_)
                return false;
         currentwinner.fill(-1);
    int lframes;
    float pthresh=vcl_exp(-DISTFROMEAN*DISTFROMEAN/2);

    for(int y=0;y<nj;y++)
    {
        for(int x=0;x<ni;x++)
        {
           
            //if(curr_img_(x,y)>0.5)
            {
                
                model.no_of_observations(y,x)++;
                bool done = false;
                                
                        
                if (model.no_of_observations(y,x)>L)
                    lframes=L;
                else
                    lframes=model.no_of_observations(y,x);

                double pmax=0.0;
                int maxMode=-1;
            
            //: iterating over no of modes used
            for(int j=0; j<model.usedmodels(y,x); j++)
            {
                double pij=getprobabilty(model.mean[j](y,x),model.standarddev[j](y,x),curr_img_(x,y));
                if(pij> pthresh)
                {
                    done=true;
                    
                    if(pij>pmax)
                    {
                        pmax=pij;
                        maxMode=j;
                    }
                }
            }
            
                        for(int j=0; j<model.usedmodels(y,x); j++)
            {
                                if(!updatefrozen)
                {
                    if(j==maxMode)
                    {
                    ++model.samplecount[j](y,x);
                    model.weight[j](y,x)=(float)model.samplecount[j](y,x)/(float)model.no_of_observations(y,x);
                    model.mean[j](y,x)+=(int)((curr_img_(x,y)- (float)model.mean[j](y,x))/(float)model.samplecount[j](y,x));
                    model.standarddev[j](y,x)+=(vcl_sqrt(((float)curr_img_(x,y)- (float)model.mean[j](y,x))*(curr_img_(x,y)- (float)model.mean[j](y,x)))-(float)model.standarddev[j](y,x))/(float)model.samplecount[j](y,x);
                    if(model.standarddev[j](y,x)<MINSTANDARDDEV)
                                                model.standarddev[j](y,x)=MINSTANDARDDEV;
                    }
                    else
                    {
                                                model.weight[j](y,x)=(float)model.samplecount[j](y,x)/(float)model.no_of_observations(y,x);

                    }
                    
                }
            }
            currentwinner(y,x)=maxMode;
            float tweight;
            float tstandarddev;
            int tmean;
            int tsamplecount;
            //: sort the models according to the weight/standardeviation;
            for(int j=0; j<model.usedmodels(y,x)-1; j++)
            {
                for(int k=j+1;k<model.usedmodels(y,x); k++)
                {
                    if(model.weight[j](y,x)/model.standarddev[j](y,x)<
                        model.weight[k](y,x)/model.standarddev[k](y,x))
                    {
                        tweight =model.weight[j](y,x);
                        tstandarddev = model.standarddev[j](y,x);
                        tsamplecount = model.samplecount[j](y,x);
                        tmean = model.mean[j](y,x);

                        model.weight[j](y,x)=model.weight[k](y,x);
                        model.standarddev[j](y,x)=model.standarddev[k](y,x);
                        model.samplecount[j](y,x)=model.samplecount[k](y,x);
                        model.mean[j](y,x)=model.mean[k](y,x);

                        model.weight[k](y,x)=tweight;
                        model.standarddev[k](y,x)=tstandarddev;
                        model.samplecount[k](y,x)=tsamplecount;
                        model.mean[k](y,x)=tmean;
                    }
                }
            }
            //: initialize a new mode by throwing out the least probable mode
            if(!done && !updatefrozen)
            {
                if(no_of_modes>model.usedmodels(y,x))
                    ++model.usedmodels(y,x);
                currentwinner(y,x)=model.usedmodels(y,x)-1;
                model.standarddev[(model.usedmodels(y,x)-1)](y,x)    = (float)initialstd_;
                model.mean[(model.usedmodels(y,x)-1)](y,x)        = (int)curr_img_(x,y);
                model.samplecount[model.usedmodels(y,x)-1](y,x)    = 1;

                
                model.weight[(model.usedmodels(y,x)-1)](y,x)  = INITIALWEIGHT;
                
            }
        }

    }
}
    return true;
}
template<class T>
void dbbgm_bgmodel<T>::freeze()
{
        updatefrozen=true;
}

template<class T>
void dbbgm_bgmodel<T>::unfreeze()
{
        updatefrozen=false;
}
template<class T>
bool dbbgm_bgmodel<T>::isbackground(int i, int j)
{
        
    if(currentwinner(j,i)==0)
        return model.weight[(currentwinner(j,i))](j,i)>=MINWEIGHT;
    else 
        return false;
    
}
template<class T>
vil_image_view<T> dbbgm_bgmodel<T>::writeforeground()
{
        vil_image_view<T> fg;
        fg.fill(255);
        fg.set_size(ni,nj);
        for(int i=0; i<ni; i++)
                for(int j=0; j<nj; j++)
        { 
            if(!isbackground(i,j) && currentwinner(j,i)>0)
            {
                if(model.weight[0](j,i)>=MINWEIGHT)
                                        fg(i,j)=255;
             }
        }
    return fg;
}
template<class T>
vil_image_view<T> dbbgm_bgmodel<T>::detectforeground()
{
        
    if(!curr_img_)
         return vil_image_view<T>();
    model.isobserved.fill(false);
    currentwinner.fill(-1);
    vil_image_view<T> fg;
    fg.set_size(ni,nj);
    fg.fill(0);
    float pthresh=vcl_exp(-DISTFROMEAN*DISTFROMEAN/2);
    for(int y=0;y<nj;y++)
    {
        for(int x=0;x<ni;x++)
        {
            if(curr_img_(x,y)>0.5)
            {
                bool done = false;
                model.isobserved(y,x)=true;
                float pmax=0.0;
                unsigned short maxMode=-1;
            
                //: iterating over no of modes used
                for(int j=0; j<model.usedmodels(y,x); j++)
                            {
                                    double pij=getprobabilty((float)model.mean[j](y,x),(float)model.standarddev[j](y,x),curr_img_(x,y));
                                        if(pij> pthresh)
                                        {
                                                done=true;
                                            if(pij>pmax)
                                                {
                                                        pmax=pij;
                                                        maxMode=j;
                                                }
                                        }
                                }
                                
                                currentwinner(y,x)=maxMode;
                        }
                        if(!isbackground(x,y) && model.isobserved(y,x))
            {
                if(model.weight[0](y,x)>=MINWEIGHT)
                                        fg(x,y)=255;
            }
                }
     }   
     return fg;
}
template<class T>
vil_image_view<T> dbbgm_bgmodel<T>::writebackground(int k)
{
    assert(k<no_of_modes);
    vil_image_view<T> bkg;
    bkg.set_size(ni,nj);
    for(int i=0; i<ni; i++)
            for(int j=0; j<nj; j++)
                       bkg(i,j) =(T)(model.mean[k](j,i));
    return bkg;
}
template<class T>
vil_image_view<T> dbbgm_bgmodel<T>::writeweightimage(int k)
{
        assert(k<no_of_modes);

    vil_image_view<T> bkg;
        bkg.set_size(ni,nj);
        for(int i=0; i<ni; i++)
                for(int j=0; j<nj; j++)
          bkg(i,j) =(T)(model.weight[k](j,i));
       
    return bkg;
}
template<class T>
float dbbgm_bgmodel<T>::getprobabilty(float mu, float sigma,float x)
{
    
        return vcl_exp(-(x-mu)*(x-mu)/(2*sigma*sigma));
}

#define DBBGM_BGMODEL_INSTANTIATE(T) \
template class dbbgm_bgmodel<T >; 
#endif // dbbgm_bgmodel_txx_


