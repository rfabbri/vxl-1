#pragma warning(disable:4786)
#include "SwendsenWangGraph.h"
#include "histogram.h"
#include "irmm_rastercolor.h"
#include "graph.h"
#include "functions.h"
#include "sampler.h"
//#include "shapes.h"
#include <set>
#include <algorithm>
using namespace std; 

//#define save_e

// SwendsenWang_Graph
//
SwendsenWang_Graph::SwendsenWang_Graph(){
    curr_type=0;ntypes=1;
    all_log_orig.push_back(new graphsw());
    all_qe.push_back(new vector<weighted_edge<double> *>());
    log_orig=all_log_orig[0];qe=all_qe[0];
}
SwendsenWang_Graph::SwendsenWang_Graph(int nTypes){
    curr_type=0;ntypes=nTypes;
    for (int t=0;t<ntypes;t++){
        all_log_orig.push_back(new graphsw());
        all_qe.push_back(new vector<weighted_edge<double> *>());
    }
    Set_curr_type();

}
SwendsenWang_Graph::~SwendsenWang_Graph(){
    clear_qe();
}

void SwendsenWang_Graph::Set_curr_type(){
    Set_curr_type(randint(ntypes));
}
void SwendsenWang_Graph::Set_curr_type(int n){
    curr_type=n;
    log_orig=all_log_orig[curr_type];
    qe=all_qe[curr_type];
}
weighted_edge<double> *SwendsenWang_Graph::Get_qe(int k){
    return (*qe)[k];
}
graphsw *SwendsenWang_Graph::Get_graph(){
    return log_orig;
}
void SwendsenWang_Graph::AddEdge(int i, int j, double d){
    //adds an edge (i,j) of weight d to graph "type"
    if (d>0){
        if (log_orig->GetEdge(i,j)==0){
            log_orig->InsertEdge(i,j,log(1-d));
            qe->push_back(new weighted_edge<double>(i,j,d));
        }
    }
}
void SwendsenWang_Graph::Init(int type, histogram_sparse &e){
    histogram_sparse::iterator it;
    int l0,l1,nl;
    double d;
    nl=sqrt(e.Get_nbins());
    Set_curr_type(type);
    log_orig->MakeGraph(nl);
    for (it=e.begin();it!=e.end();++it){
        l0=(*it).first/nl;
        l1=(*it).first%nl;
        d=GetEdgeProb(l0,l1);
        AddEdge(l0,l1,d);
    }
}

void SwendsenWang_Graph::BuildGraph(){
    int i,j,n=node_size();
    double d;
    log_orig->MakeGraph(n);
//    Labeler::Construct(n);
    sg.Construct(node_size());

    for (i=0;i<n;i++)
        for (j=i+1;j<n;j++){
            d=GetEdgeProb(i,j);
            if ((d>0)&&(d<1)){
                log_orig->InsertEdge(i,j,log(1-d));
                qe->push_back(new weighted_edge<double> (i,j,d));
            }
        }
}
void SwendsenWang_Graph::clear_qe(){
    for (int i=0;i<all_qe.size();i++)
        delete all_qe[i];
}

bool SwendsenWang_Graph::CheckNodesSameLabel(){
    int i,n=node_size();
    for (i=0;i<n;i++){
        if (GetLabel(i)!=GetLabel(sg.findset(i)))
            return false;
    }
    return true;
}

void SwendsenWang_Graph::GetNodesLabeledWith(int l, vector<int> &N){
    int n=node_size();
    N.clear();
    for (int i=0;i<n;i++){
        if (GetLabel(i)==l)
            N.push_back(i);
    }
}
void SwendsenWang_Graph::GetNeighborsLabeledWith(int i,int l, vector<int> &L){
    int j;
    vector<int> L1;

    L.clear();
    log_orig->GetNeighborNodes(i,L1);
    for (j=0;j<L1.size();j++){
        if (GetLabel(L1[j])==l)
            L.push_back(L1[j]);
    }
}
void SwendsenWang_Graph::GetNeighborLabels(int i,vector<int> &L){
    int j;
    vector<int> L1;

    L.clear();
    log_orig->GetNeighborNodes(i,L1);
    for (j=0;j<L1.size();j++){
            L.push_back(GetLabel(L1[j]));
    }
}
void SwendsenWang_Graph::GetNeighborLabels(int i,set<int> &L){
    int j;
    vector<int> L1;

    L.clear();
    log_orig->GetNeighborNodes(i,L1);
    for (j=0;j<L1.size();j++){
        L.insert(GetLabel(L1[j]));
    }
}
void SwendsenWang_Graph::GetNeighborLabels(vector<int> &N, set<int> &L1){
    // labels only of neighbors 
    int i,k;
    vector<int> Nb;
    set<int> AllNb;
    set<int>::iterator si;
    L1.clear();
    AllNb.clear();
    for (i=0;i<N.size();i++){
        Get_graph()->GetNeighborNodes(N[i],Nb);
        for (k=0;k<Nb.size();k++)
            AllNb.insert(Nb[k]);
    }
    for (i=0;i<N.size();i++){
        AllNb.erase(N[i]);    //erase elts of N from AllNb
    }
    for (si=AllNb.begin();si!=AllNb.end();++si){
        L1.insert(GetLabel(*si));
    }
}

void SwendsenWang_Graph::GetNeighborLabels1(vector<int> &N, set<int> &L1){
    //labels of neighbors and current nodes
    int i,k;
    vector<int> Nb;
    L1.clear();
    if (N.size()>0)
        L1.insert(GetLabel(N[0]));
    for (i=0;i<N.size();i++){
        log_orig->GetNeighborNodes(N[i],Nb);
        for (k=0;k<Nb.size();k++)
            L1.insert(GetLabel(Nb[k]));
    }
}
void SwendsenWang_Graph::GetNeighborLabels(vector<int> &N, vector<int> &labels){
    set<int> L1;
    set<int>::iterator j;
    labels.clear();
    GetNeighborLabels1(N,L1);
    for (j=L1.begin();j!=L1.end();j++)
        labels.push_back(*j);
}

int    SwendsenWang_Graph::SampleComponentRep(double &prob){
    int i,ncomp=sg.Get_nsets();
    i=randint(ncomp);
    prob=1./ncomp;
    return sg.GetSetRep(i);
}

int SwendsenWang_Graph::SampleComponent(vector<int> &N){
    double prob;
    int repc;
    SampleGraph();
    repc=SampleComponentRep(prob);//one node of the connected component that will have the label changed
    sg.GetSetOf(repc,N);    //Nrelabel=cluster that will change label
    return GetLabel(repc);
}

void SwendsenWang_Graph::SampleGraph(){
    //sample the graph but only the edges btw nodes with same label
    double T=1;//GetEdgeTemp();
    int i,n=qe->size();
    double d,w;
    weighted_edge<double> *e;
#ifdef save_e
    weighted_edge<double> e1;
    sg_e.clear();
#endif
    sg.Reset();

    if (T==1){
        for (i=0;i<n;i++){
            e=Get_qe(i);
            if (GetLabel(e->i)==GetLabel(e->j)){
                d=randdbl(1);
                w=e->value;
                if (d<w){
                    sg.setunion(e->i,e->j);
#ifdef save_e
    e1.i=e->i;e1.j=e->j;e1.value=1;
    sg_e.push_back(e1);
#endif
                }
            }
        }
    }
    else{
        for (i=0;i<n;i++){
            e=Get_qe(i);
            if (GetLabel(e->i)==GetLabel(e->j)){
                d=randdbl(1);
                w=pow(e->value,T);
                if (d<w){
                    sg.setunion(e->i,e->j);
#ifdef save_e
    e1.i=e->i;e1.j=e->j;e1.value=1;
    sg_e.push_back(e1);
#endif
                }
            }
        }
    }
}

double SwendsenWang_Graph::logProdWeights(vector<int> &N,int l){
    //log prod of weights from from N to other components labeled by l, not cont repc
    double d,q1i0=0;
    dEdgeListf::iterator ei;
    int i,j,repc=sg.findset(N[0]);
    for (i=0;i<N.size();i++){
        for(ei=Get_graph()->e[N[i]].begin();ei!=Get_graph()->e[N[i]].end();++ei){
            j=(*ei).first;
            if ((GetLabel(j)==l)&&(sg.findset(j)!=repc)){
                d=(*ei).second;
                q1i0 +=d;
//                if (d>0){
//                    q1i0+=log(1-d);
//                }
            }
        }
    }
    return q1i0;
}


int SwendsenWang_Graph::SampleNewLabel(vector<int> &N, double &prob){
    vector<int>    alllabels;
    set<int>    posslabels;
    int i,nCls,l,l0=GetLabel(N[0]),n=N.size();
    bool allobj=(GetNodeCount(l0)==n);
    double np=1;//*(n-.9)/n;
    msampler s;
    GetUsedLabels(alllabels);
    GetNeighborLabels(N,posslabels);
    nCls=alllabels.size();
    if (allobj)
        s.SetDimension(nCls,1);
    else
        s.SetDimension(nCls+1,1);
    for (i=0;i<nCls;i++){
        l=alllabels[i];
        if (allobj &&(l==l0))
            s(i)=np;
        else{
            if (posslabels.find(l)!=posslabels.end())
                s(i)=1;
            else
                s(i)=0.1;
        }
        
    }
    if (!allobj)
        s(nCls)=np;
    s.Normalize();
    prob=s.sample(i);
    if (i<nCls)
        return alllabels[i];    
    else
        return GetNewLabel();
}

double SwendsenWang_Graph::pNewLabel(vector<int> &N,int l1){
    vector<int>    alllabels;
    set<int>    posslabels;
    int i,i1,nCls,l,l0=GetLabel(N[0]),n=N.size();
    bool allobj=(GetNodeCount(l0)==n);
    double np=1;
    msampler s;
    GetUsedLabels(alllabels);
    GetNeighborLabels(N,posslabels);
    nCls=alllabels.size();
    if (allobj)
        s.SetDimension(nCls,1);
    else
        s.SetDimension(nCls+1,1);
    i1=nCls;
    for (i=0;i<nCls;i++){
        l=alllabels[i];
        if (l==l1)
            i1=i;
        if (allobj &&(l==l0))
            s(i)=np;
        else{
            if (posslabels.find(l)!=posslabels.end())
                s(i)=1;
            else
                s(i)=0.1;
        }
    }
    if (!allobj)
        s(nCls)=np;
    s.Normalize();
    return s.prob(i1);
}
double SwendsenWang_Graph::DoGibbs(){
    vector<int> Nrelabel,UsedL;
    int i,repc;
    int oldl,newl;
    bool allobjselected;
    double d;
    double Temp=GetTemp();
    msampler s;

    step++;
    SampleGraph();
    repc=SampleComponentRep(d);//one node of the connected component that will have the label changed
    oldl=GetLabel(repc);
    sg.GetSetOf(repc,Nrelabel);    //Nrelabel=cluster that will change label

    allobjselected=(GetNodeCount(oldl)==Nrelabel.size());
    GetUsedLabels(UsedL);
    if (!allobjselected){
        s.SetDimension(UsedL.size()+1,1);
    }
    else{
        s.SetDimension(UsedL.size (),1);
    }
    for (i=0;i<UsedL.size ();i++){
        newl=UsedL[i];
        SetLabel(Nrelabel,newl);
        d=logprob()+logProdWeights(Nrelabel,newl);;
        s(i)=d;
    }
    if (!allobjselected){
        newl=GetNewLabel();
        SetLabel(Nrelabel,newl);
        d=logprob()+logProdWeights(Nrelabel,newl);;
        s(UsedL.size())=d;
    }
    s.Normalize();
    d=s.sample(i);
    if (i==UsedL.size())
        SetLabel(Nrelabel,newl);
    else
        SetLabel(Nrelabel,UsedL[i]);
    return d;
}


void SwendsenWang::Update_current_partition(){ 
    //update the connected components of the current partition
    int i, n=qe->size();
    edge *e;
    components->Construct(node_size());
    for (i=0;i<n;i++){
        e=Get_qe(i);
        if (GetLabel(e->i)==GetLabel(e->j)){
            components->setunion(e->i,e->j);
        }
    }
}
void SwendsenWang::Update_current_partition_log_orig(){ 
    //update the connected components of the current partition
    dEdgeListf::iterator ei,ee;
    int i,j,li,n=node_size();
    components->Reset();
    for (i=0;i<n;i++){
        li=GetLabel(i);
        ee=log_orig->e[i].end();
        for (ei=log_orig->e[i].begin();ei!=ee;++ei){
            j=(*ei).first;
            if (li==GetLabel(j)){
                components->setunion(i,j);
            }
        }
    }
}
double SwendsenWang::GetSumPowAreaComponents(int l,double power, int &ncomp){
    double sum;
    int i,n=node_size();
    vector<double> L;
    sum=0;ncomp=0;
    L.assign(n,0);
    for (i=0;i<n;i++){
        L[components->findset(i)]+=Get_size(i);
    }
    for (i=0;i<n;i++){
        if ((L[i]>0)&&(GetLabel(i)==l)){
            sum+=pow(L[i],power);
            ncomp++;
        }
    }
    return sum;
}




