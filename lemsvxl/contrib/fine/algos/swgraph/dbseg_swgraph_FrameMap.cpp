#pragma warning(disable:4786)
#include "FrameMap.h"
#include "functions.h"
#include "gauss.h"
#include "imgfunctions.h"
#include <map>
#include <algorithm>
#include "label.h"
using namespace std; 


FrameMap1::FrameMap1(char *filename, char *suffix){
    Init(filename,suffix);
}
FrameMap1::FrameMap1(char *imgfilename, Matrixi &M){
    Matrix<int>    P,Merge;
    
    LoadImg(imgfilename,mImage);
    nx=mImage.cols();ny=mImage.rows();
    Init(M);
}
void FrameMap1::Init(char *imgfilename, char *suffix){
    LoadImg(imgfilename,mImage);
    nx=mImage.cols();ny=mImage.rows();
    Raster_Color<uchar> mImagec;
    LoadImg(imgfilename,mImagec);
    Init(GetProposal(mImagec,2, imgfilename,suffix));
}
void FrameMap1::Init(char *imgfilename, char *suffix,Matrixi &M){
    Matrix<int>    P,Merge;
    Raster_Color<uchar> mImagec;
    
    LoadImg(imgfilename,mImage);
    LoadImg(imgfilename,mImagec);
    nx=mImage.cols();ny=mImage.rows();

    P=GetProposal(mImagec,1, imgfilename,suffix);
    Merge_Layer_Regions1(M,P,Merge);
    SaveBmp("merge.bmp", Merge);
    Init(Merge);
}
void FrameMap1::Init(Matrixi &M){
    
    if (M.size()>0){
        nx=M.nx();ny=M.ny();
        assign_matrix(ras_region_label,M+1);
        ras_region_label.ResetLabels(false);
        ResetLabels(ras_region_label);
        ras_region_label=ras_region_label-ras_region_label.GetMin()+1;
        Parse_edges(ras_region_label,edges);
        UpdateAtomicRegions();
        UpdateRegionPixels();
    }
    UpdateReconstructedImage();
}

ImageModel *FrameMap1::newmodel(int i){
    return new ImageModel(i,&mImage);
}
int FrameMap1::Get_npix(int i){
    return vAtomicRegions[i]->Get_npix();
}

void FrameMap1::UpdateAtomicRegions(){
    int i,max;
    int x,y;
    max=ras_region_label.GetMax();
    vAtomicRegions.clear();
    vAtomicRegions.reserve(max);
    for (i=0;i<max;i++){
        vAtomicRegions.push_back(newmodel(i));
        vAtomicRegions[i]->nnodes=1;
    }
    for (y=0;y<ny;y++){
        for (x=0;x<nx;x++){
            i=GetPixelRegionId(x,y);
            if (i>=0){
                vAtomicRegions[i]->Add_item(y*nx+x);
            }
        }
    }
/*    for (y=0;y<ny;y++){
        for (x=0;x<nx;x++){
            i=GetImage(x,y);
            if (i>=0){
                double v;
                d=I.Data(y,x);if (d<30) d=30;
                v=sqrt(vAtomicRegions[i].GetVar());
                if (v<5) v=5;
                if (fabs(d-vAtomicRegions[i].GetMean())>3*v){//outlier rejection
                    vAtomicRegions[i].Subtract(x,y,d);
                    if (d>vAtomicRegions[i].GetMean()+3*v) d=vAtomicRegions[i].GetMean()+3*v;
                    if (d<vAtomicRegions[i].GetMean()-3*v) d=vAtomicRegions[i].GetMean()-3*v;
                    vAtomicRegions[i].Add(x,y,d);
                }
            }
        }
    }
*/
}
void FrameMap1::UpdateRegionPixels(){
    int i;
    int x,y;
    for (i=0;i<vAtomicRegions.size();i++){
        vvRegionPixels.push_back(new vector<int>());
    }
    for (y=0;y<ny;y++)
        for (x=0;x<nx;x++){
            i=GetPixelRegionId(x,y);
            if (i>=0){
                vvRegionPixels[i]->push_back(y*nx+x);
            }
        }
}

int    FrameMap1::GetPixelRegionId(int x, int y){
    if (inbounds(ny,nx,y,x)){
        return ras_region_label.Data(y,x)-1;
    }
    else
        return -1;
}
int    FrameMap1::GetPixelRegionId(int i){
    if (i>=0&&i<nx*ny){
        return ras_region_label.Data(i)-1;
    }
    else
        return -1;
}

void FrameMap1::GetLabels(vector<int> &N,set<int> &L){
    int i;
    L.clear();
    for (i=0;i<N.size();i++){
        L.insert(GetLabel(N[i]));
    }
}
void FrameMap1::GetLabels(vector<int> &N,MMapi2i &L){
    int i;
    L.clear();
    for (i=0;i<N.size();i++){
        L.insert(MMapi2i::value_type(GetLabel(N[i]),N[i]));
    }
}

void FrameMap1::LabelRegion(int i,Matrixi &M, int l){
    vector<int> *v=vvRegionPixels[i];
    int j,n=v->size();
    for (j=0;j<n;j++){
        M(v->at(j))=l;
    }
}

void FrameMap1::UpdateLabelImage(){
    int x,y;
    mLabelImage.SetDimension(ny,nx);
    mLabelImage.Zero();
    for (y=0;y<ny;y++)
        for (x=0;x<nx;x++)
            mLabelImage(y,x)=GetLabel(GetPixelRegionId(x,y));
}
void FrameMap1::UpdateRegionCenters(){
    int i,j,n=vAtomicRegions.size(),na,p;
    double sumx,sumy;
    Pointi P;
    vpRegionCenters.clear();
    for (i=0;i<n;i++){
        na=vvRegionPixels[i]->size();
        sumx=sumy=0;
        for (j=0;j<na;j++){
            p=(*vvRegionPixels[i])[j];
            sumx+=p%nx;
            sumy+=p/nx;
        }
        P.x=sumx/na;
        P.y=sumy/na;
        vpRegionCenters.push_back(P);
    }
}

void FrameMap1::UpdateReconstructedImage(){
    int x,y,i;
    Matrixd m;
    mReconstructedImage.SetDimension(ny,nx);
    for (y=0;y<ny;y++)
        for (x=0;x<nx;x++){
            i=GetPixelRegionId(x,y);
#ifndef COLOR
            mReconstructedImage(y,x)=vAtomicRegions[i]->sample(x,y);
#else
            vAtomicRegions[i]->sample(m);
            mReconstructedImage.SetPixelInLUV(y,x,m(0),m(1),m(2));
#endif
//            i=y*nx+x;mReconstructedImage.SetPixelInLUV(y,x,Ic.L(i),Ic.U(i),Ic.V(i));
        }
}


//
//    FrameMapWithProb class
//
FrameMapWithProb::FrameMapWithProb(char *imgfilename, char *suffix):FrameMap1(imgfilename, suffix){
    int max=vAtomicRegions.size();
    Raster_Color<double> Id;
    strcpy(filename,"");strncat(filename,imgfilename,strlen(imgfilename)-4);
    SW_Framework<ImageModel,ImageModel>::Init();
    SwendsenWang::Init(0,edges);
    Update_current_partition();
    sg.Construct(node_size());
    SampleGraph();
    scale=8;
    model[0]->Update();
    nsweeps=15;
    SetTemperatureRange(20.1,3);
    Temp=1;
    UpdateRegionCenters();
}
FrameMapWithProb::FrameMapWithProb(char *imgfilename, Matrixi &M):FrameMap1(imgfilename, M){
    int max=vAtomicRegions.size();
    Raster_Color<double> Id;
    Matrixd Imag,Ix,Iy;

    strcpy(filename,"");strncat(filename,imgfilename,strlen(imgfilename)-4);

    model.clear();
    SW_Framework<ImageModel,ImageModel>::Init();
    SwendsenWang::Init(0,edges);
    Update_current_partition();
    SampleGraph();
    scale=14;
    model[0]->Update();
    SetTemperatureRange(20.1,3);
    Temp=1;
    UpdateRegionCenters();
}
void FrameMapWithProb::Get_log_filename(char *name){
    int i=0;
    FILE *f;
    do{
#ifdef EDGEWT
        sprintf(name,"sw_%1.0f_T%1.0f_sw%d_e%1.4f_r%03d.txt",scale,startTemp,nsweeps,EDGEWT,i);
#else 
        sprintf(name,"sw_%1.0f_T%1.0f_sw%d_r%03d.txt",scale,startTemp,nsweeps,i);
#endif
        f=fopen(name,"r");
        i++;
    }
    while (f!=NULL);
}

double FrameMapWithProb::GetEdgeProb(int i1,int i2){
#ifdef EDGEWT
    return EDGEWT;
#endif
    double d;
    d=vAtomicRegions[i1]->SymKL(vAtomicRegions[i2]);
    d=0.01+0.98*exp(-d/2);
    return d;
}
ImageModel *FrameMapWithProb::Make_node(int i){
    return newmodel(i);
}
ImageModel *FrameMapWithProb::Make_model(int i){
    return newmodel(i);
}
void FrameMapWithProb::Save(int n){
    char st[255];
    UpdateLabelImage();
    sprintf(st,"%sis_%03d.txt",filename,n);
    print(st,mLabelImage);
}


void FrameMapWithProb::UpdateSynthesizedImage(){
    int x,y,i,l;
    Matrixd m;
    mSynthesizedImage.SetDimension(ny,nx);
    for (y=0;y<ny;y++)
        for (x=0;x<nx;x++){
            i=GetPixelRegionId(x,y);
            l=GetLabel(i);
#ifndef COLOR
            mSynthesizedImage(y,x)=model[l]->sample(x,y);
#else
            model[l]->sample(m);
            mSynthesizedImage.SetPixelInLUV(y,x,m(0),m(1),m(2));
#endif
        }
//    mSynthesizedImage.LUVtoRGB();
}
int    FrameMapWithProb::Get_npix(int i){
    return vAtomicRegions[i]->Get_npix();
}
int FrameMapWithProb::Get_size(int i){
    return vAtomicRegions[i]->Get_npix();
}
bool FrameMapWithProb::IsEmpty(int l){
    return model[l]->nnodes==0;
}
int    FrameMapWithProb::GetNodeCount(int l){
    return model[l]->nnodes;
}

double FrameMapWithProb::loglik(int l){
    return model[l]->loglik();
}
double FrameMapWithProb::logprior_bnd(){
    return 0;
}
double FrameMapWithProb::logprob(int l1,int l2){
    return loglik(l1)+loglik(l2)+logprior(l1)+logprior(l2)+logprior_bnd();
}
double FrameMapWithProb::logprior(int l){
    switch (model[l]->Get_npix()){
    case 0:
        return 0;
    default:
        double sum;
        int ncomp;
        sum=GetSumPowAreaComponents(l,0.9,ncomp);
        return -COMPL-OBCOEF*(ncomp-1)-scale*sum;
    }
}
double FrameMapWithProb::loglik(){
    double sum=0;
    int l,n=model.size();
    for (l=0;l<n;l++){
        sum+=loglik(l);
    }
    return sum;
}
double FrameMapWithProb::logprior(){
    double sum=logprior_bnd();
    int l,n=model.size();
    for (l=0;l<n;l++){
        sum+=logprior(l);
    }
    return sum;
}


double    FrameMapWithProb::GetTemp(){
//    return Temp;
    return ::getlogTemp(startTemp,endTemp,step,nsweeps*node_size());
}

double    FrameMapWithProb::GetEdgeTemp(){
    return 1.;
}

void FrameMapWithProb::SetLabel(vector<int> &N, int label){
    if (N.size()==0)
        return;
    int l0=GetLabel(N[0]);

    if ((l0<0)||(l0==label)) return;
    SW_Framework<ImageModel,ImageModel>::SetLabel(N,label);
    Update_current_partition();

    model[l0]->Update();
    model[label]->Update();
}

Matrix<int> FrameMapWithProb::GetProposals(){
    Matrix<int> R;
    vector<int> N;
    int i,k,ncomp,l;
    R.SetDimension(ny,nx);
    R.Zero();
    SampleGraph();
    ncomp=sg.Get_nsets();
    for (k=0;k<ncomp;k++){
        sg.GetSetWithIndex(k,N);
        l=25*randint(10)+1;
        for (i=0;i<N.size();i++){
            LabelRegion(N[i],R,l);
        }
    }
    return R;
}




