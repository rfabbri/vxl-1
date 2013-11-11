#pragma warning(disable:4786)
#ifndef _FRAMEMAP_H
#define _FRAMEMAP_H

#include<vector>
#include<set>
#include "matrix.h"
#include "histogram.h"
#include "irmm_raster.h"
#include "irmm_rastercolor.h"
#include "simplemodel.h"
#include "model.h"
#include "sw_framework.h"
using namespace std;


#define COLOR

typedef multimap<int, int, less<int> >    MMapi2i;
typedef map<int, int, less<int> >        Mapi2i;


//typedef Simple_Image_Model ImageModel;
#ifndef COLOR
    typedef Gauss_Image_Model ImageModel;
//    typedef Histogram_Model ImageModel;
#else
    typedef Color_Model ImageModel;
#endif

class FrameMap1{//framework with regions, adjacency, neighborhood,labels
public:

#ifndef COLOR
    Matrix<uchar> mImage;
    Matrix<uchar> mReconstructedImage;
#else
    Raster_Color<uchar> mImage;
    Raster_Color<uchar> mReconstructedImage;
#endif

    Matrix<int>        mLabelImage;
    Raster<int>        ras_region_label;
    int                nx,ny;
    histogram_sparse edges;

    vector<ImageModel *>    vAtomicRegions;            //image attrib in each small region

    vector<double>    cl;            //length of each contour seg

    vector<vector<int> *>    vvRegionPixels;    //the pixels of each small region
    vector<Pointi>    vpRegionCenters;

    FrameMap1(){}
    FrameMap1(char *bmpfilename, char *suffix);
    FrameMap1(char *bmpfilename,Matrixi &M);

    void    Init(char *filename, char *suffix);
    void    Init(char *filename, char *suffix, Matrixi &M);
    void    Init(Matrixi &M);
    void    UpdateAtomicRegions();
    void    UpdateRegionPixels();
    ImageModel *newmodel(int i);

    int        GetPixelRegionId(int x, int y);
    int        GetPixelRegionId(int i);
    int        Get_x(int nPixelId){return nPixelId%nx;}
    int        Get_y(int nPixelId){return nPixelId/nx;}
    int        GetPixelIndex(int x, int y){return y*nx+x;}
    int        GetRegionCount(){return vAtomicRegions.size();}
    int        Get_npix(int nRegionId);
    vector<int> *GetPixels(int nRegionId){return vvRegionPixels[nRegionId];}


    void     GetLabels(vector<int> &N,set<int> &L);
    void     GetLabels(vector<int> &N,MMapi2i &L);
    void    GetRegionsLabeledWith(int l, vector<int> &L);

    double    GetBoundaryLength(int i);
    void    GetBoundaryCsegIds(int i, int j, set<int> &ids);
    
    void    LabelRegion(int i,Matrixi &M, int l);
    void    UpdateRegionCenters();
    void    UpdateLabelImage();
    void    UpdateReconstructedImage();

    virtual    double    GetMean(int rl,int x,int y){return 150;}
    virtual    int     GetLabel(int i){return 0;}
};


class FrameMapWithProb:public FrameMap1, public SW_Framework<ImageModel,ImageModel>{
public:
    char            filename[255];

    double            scale;
#ifndef COLOR
    Matrix<uchar>    mSynthesizedImage;
#else
    Raster_Color<uchar> mSynthesizedImage;
#endif

            FrameMapWithProb(char *filename, char *suffix);
            FrameMapWithProb(char *filename, Matrixi &M);

            double    GetSumPowAreaComponents(int l,double power, int &ncomp){
                return SwendsenWang::GetSumPowAreaComponents(l,power,ncomp);
            };
    virtual int        GetLabel(int i){return SW_Framework<ImageModel,ImageModel>::GetLabel(i);}
            double  loglik(int l);
            double  loglik();
            double    logprior_bnd();
            double    logprior(int l); 
            double    logprior();
    virtual    double    logprob(){return loglik()+logprior();}
    virtual    double    logprob(int l1, int l2);
    virtual void    SetLabel(vector<int> &N, int label);
            double    GetEdgeProb(int i1,int i2);
            int        Get_npix(int i);
            int        Get_size(int i);
            ImageModel *Get_model(int l){return model[l];}
            int        node_size(){return GetRegionCount();}
            bool    IsEmpty(int l);
            int        GetNodeCount(int l);
            ImageModel *Get_node(int i){return vAtomicRegions[i];}
            ImageModel *Make_node(int i);
            ImageModel *Make_model(int i);

            void    Get_log_filename(char *name);

            double    GetEdgeTemp();
            void    UpdateSynthesizedImage();

            double    GetTemp();

            void    GetFileName(char *filename1){strcpy(filename1,filename);}

            Raster<int> GetProposal();
            Matrix<int> GetProposals();
            void    Save(int n);
};

#endif

