#ifndef _SW_FRAMEWORK_H
#define _SW_FRAMEWORK_H
#pragma warning(disable:4786)

//  Copyright 2005, Adrian Barbu
//    Abstract framework for Swendsen-Wang Cuts with models
//  Implements the Swendsen-Wang and the Wolff variant
//  Instantiate the Node Class and the Model Class, and the graph edge weights
//
//    The Node class should contain the following: 
//        int label 
//        int curr_sym  
//    
//    The Model class should implement the following functions:
//        void Add(Node *n)
//        void Subtract(Node *n)
//        void Add(Model *m)
//        void Subtract(Model *m)
//        double KL(Model *m)
#include<vector>
#include "SwendsenWangGraph.h"
#include "sampler.h"
#include "stl_util.h"
#include "graph.h"
#include "functions.h"

template<class Node,class Model>
class SW_Framework: public SwendsenWang, public Wolff{
public:
    vector<Model *>    model;        // the models
    Model            *mN;        // the model of the chosen connected component
    int                curr_sym;   // current symbol for growing a connected component
    double            Temp;        // the temperature

            SW_Framework(){curr_sym=0;Temp=1;}

            void    Init();        // initialize by adding all nodes to model 0
            int     SampleNewLabel(vector<int> &N,double &prob);    //finds a new label for N by sampling
            double     pNewLabel(vector<int> &N, int l);    //prob of N getting label l
            int     SampleComponent(vector<int> &N);    // sample a connected component and update mN
            int        GetNewLabel();                        // return a label that is not being used
            int        GetLabel(int i);                    // return the label of node i
    virtual void    SetLabel(int i, int l);                // set the label of i to l
    virtual void    SetLabel(vector<int> &N, int label);// set the labels of nodes N to l
            void    Update_mN(vector<int> &N);            // update the model of these nodes
            bool    Check_models();                        // verify that the models are coherent with the nodes
    virtual    int        GetUsedLabelCount();                // the number of used labels
    virtual    void    GetUsedLabels(vector<int> &labels);    // the set of labels, as a vector
            void    Set_visited(int i);                    // sets node i as visited in current step
            bool    visited(int i);                        // true if node i wss visited in current step
            double    GetSampledNeighbors(int i, vector<int> &N);// sample the neighbors of i
            Model    *Get_model(int i){return model[i];}    
            void    RandomLabels(const int nlabels);    // initialize with random labels

    virtual    int        GetNodeCount(int l)=0;                // the number of nodes with label l
    virtual    double    GetEdgeProb(int i1,int i2)=0;        // return the edge weight between nodes i1 and i2
    virtual    double    logprob(int l1, int l2)=0;            // return the local probability of labels l1 and l2
                                                        // can be the full posterior
    virtual int        node_size()=0;                        // number of pixels of node i
    virtual    bool    IsEmpty(int l)=0;                    // true if there are no nodes with label l
    virtual Node    *Get_node(int i)=0;                    // return the node i
    virtual    Node    *Make_node(int i)=0;                // construct node i
    virtual    Model    *Make_model(int i)=0;                // construct a new model with label i
};

template<class Node,class Model>
void SW_Framework<Node,Model>::Init(){
    int i,n=node_size();
    GetNewLabel();
    mN=Make_model(0);
    for (i=0;i<n;i++){
        model[0]->Add(Get_node(i));
    }
}

template<class Node,class Model>
int SW_Framework<Node,Model>::SampleComponent(vector<int> &N){
    int i=SwendsenWang_Graph::SampleComponent(N);
    Update_mN(N);
    return i;
}

template<class Node,class Model>
int SW_Framework<Node,Model>::GetNewLabel(){
    int i,n=model.size();
    for(i=0;i<n;i++){
        if (IsEmpty(i)){
            clean_end<Model>(model,i);
            return i;
        }
    }
    model.push_back(Make_model(n));
    return n;
}

template<class Node,class Model>
int SW_Framework<Node,Model>::GetLabel(int i){
    return Get_node(i)->label;
}
template<class Node,class Model>
void SW_Framework<Node,Model>::SetLabel(int i, int label){
    int l0=GetLabel(i);
    if ((l0<0)||(l0==label)) return;
    Node *n=Get_node(i);
    n->label=label;
    model[l0]->Subtract(n);
    model[label]->Add(n);
}

template<class Node,class Model>
int SW_Framework<Node,Model>::GetUsedLabelCount(){
    int l,sum=0,nm=model.size();
    for (l=0;l<nm;l++){
        if (!IsEmpty(l))
            sum++;
    }
    return sum;
}
template<class Node,class Model>
void SW_Framework<Node,Model>::GetUsedLabels(vector<int> &labels){
    int l,n=model.size();
    labels.clear();
    for (l=0;l<n;l++){
        if (!IsEmpty(l))
            labels.push_back(l);
    }
}
template<class Node,class Model>
void SW_Framework<Node,Model>::Update_mN(vector<int> &N){
    int i,n;
    n=N.size();
    mN->Zero();
    for (i=0;i<n;i++){
        mN->Add(Get_node(N[i]));
    }
}

template<class Node,class Model>
void SW_Framework<Node,Model>::SetLabel(vector<int> &N, int label){
    int n=N.size();
    if (n==0)
        return;
    int l0=GetLabel(N[0]);
    if ((l0<0)||(l0==label)) return;
    for (int i=0;i<n;i++){
        Get_node(N[i])->label=label;
    }
    model[l0]->Subtract(mN);
    model[label]->Add(mN);

//    model[l0]->Update();
//    model[label]->Update();
}
template<class Node,class Model>
void SW_Framework<Node,Model>::Set_visited(int i){
    Get_node(i)->curr_sym=curr_sym;
}
template<class Node,class Model>
bool SW_Framework<Node,Model>::visited(int i){
    return (Get_node(i)->curr_sym==curr_sym);
}

template<class Node,class Model>
double SW_Framework<Node,Model>::GetSampledNeighbors(int i, vector<int> &N){
    int j,n=log_orig->e[i].size();
    pair<int, double> *e;
    double v,prod=1;
    for (j=0;j<n;j++){
        e=&log_orig->e[i][j];
        v=e->second;
        if (randdbl(1)<exp(v)){
            prod*=v;
            N.push_back(e->first);
        }
        else
            prod*=(1-v);
    }
    return log(prod);
}

template<class Node,class Model>
int SW_Framework<Node,Model>::SampleNewLabel(vector<int> &N,double &prob){
    //finds a new label for N by sampling
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
//            s.Set_d(i,0.01+exp(-mN->SymKL1(model[l])/2));
            s(i)=0.01+exp(-mN->KL(model[l]));
//            s.Set_d(i,0.5);
            if (posslabels.find(l)!=posslabels.end())
                s(i)*=30;
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

template<class Node,class Model>
double     SW_Framework<Node,Model>::pNewLabel(vector<int> &N, int l1){    //prob of N getting label l
    vector<int>    alllabels;
    set<int>    posslabels;
    int i,i1,nCls,l,l0=GetLabel(N[0]),n=N.size();
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
    i1=nCls;
    for (i=0;i<nCls;i++){
        l=alllabels[i];
        if (l==l1)
            i1=i;
        if (allobj &&(l==l0))
            s(i)=np;
        else{
//            s.Set_d(i,0.01+exp(-mN->SymKL1(model[l])/2));
            s(i)=0.01+exp(-mN->KL(model[l]));
//            s.Set_d(i,0.5);
            if (posslabels.find(l)!=posslabels.end())
                s(i)*=30;
        }
    }
    if (!allobj)
        s(nCls)=np;
    s.Normalize();
    return s.prob(i1);
}
template<class Node,class Model>
bool SW_Framework<Node,Model>::Check_models(){
    double n=node_size(),nm=model.size();
    vector<Model *> m;
    int i;
    for (i=0;i<nm;i++){
        m.push_back(Make_model(i));
    }
    for (i=0;i<n;i++){
        m[GetLabel(i)]->Add(Get_node(i));
    }
    for (i=0;i<nm;i++){
        if (!((*m[i])==(*model[i])))
            return false;
        delete m[i];
    }
    return true;
}
//#define NLABELS 5
template<class Node,class Model>
void SW_Framework<Node,Model>::RandomLabels(const int NLABELS){
    vector<int> N[NLABELS];
    int i;
    for (i=0;i<NLABELS;i++)
        N[i].clear();
    for (i=0;i<size();i++){
        N[randint(NLABELS)].push_back(i);
    }
    for (i=1;i<NLABELS;i++){
        Update_mN(N[i]);
        SetLabel(N[i],GetNewLabel());
    }
}

#endif

