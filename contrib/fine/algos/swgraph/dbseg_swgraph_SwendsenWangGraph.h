#ifndef _SWENDSENWANGGRAPH_H
#define _SWENDSENWANGGRAPH_H

#pragma warning(disable:4786)
#include<vector>
#include "matrix.h"
#include "swendsenwang.h"
#include "unionfind.h"
using namespace std;

template<class Tp> class graph_fast;
template<class Tp> class weighted_edge;
template<class Tp> class Point;
template<class Tp> class Raster_Color;
class histogram_sparse;

typedef graph_fast<double> graphsw;

class SwendsenWang_Graph:public SwendsenWangAbstract{
protected:
    graphsw            *log_orig; // pointer to the current graph 

    vector<graphsw *> all_log_orig; // all input graphs, with log edge weights
    vector<vector<weighted_edge<double> *> *> all_qe;//the weights for each proposal
    int                ntypes;        // the number or graphs (usually 1)
public:
    vector<weighted_edge<double> *> *qe;    // the current graph as list of edges with weights
    vector<weighted_edge<double> > sg_e;    // the sampled graph as list of edges with weights
    UnionFind_Matrix        sg;                // the sampled graph as a union-find data structure
    int                curr_type;        //the current type of proposal

    SwendsenWang_Graph();
    SwendsenWang_Graph(int ntypes);
    ~SwendsenWang_Graph();

    double            logProdWeights(vector<int> &N,int l); // the log of the SW cut towards label l
    virtual int     SampleComponentRep(double &prob);
    virtual int     SampleComponent(vector<int> &N);    // samples a connected component
    virtual void    SampleGraph();                        // samples all edges in the graph
    virtual int     SampleNewLabel(vector<int> &N,double &prob);// chooses new label for reassignement, by sampling
    virtual double     pNewLabel(vector<int> &N, int l);    // prob of a label for reassignment

            void    AddEdge(int i, int j, double d);
            void    Init(int type, histogram_sparse &e);
    virtual void    BuildGraph();
    graphsw  *Get_graph();
    weighted_edge<double> *Get_qe(int k);
            void    clear_qe();
            void    Set_curr_type();
            void    Set_curr_type(int n);

    double            DoGibbs();
    bool            CheckNodesSameLabel();
    virtual void    GetNodesLabeledWith(int l, vector<int> &N);
            void    GetNeighborsLabeledWith(int i,int l,vector<int> &L);
            void    GetNeighborLabels(int i, vector<int> &labels);
            void    GetNeighborLabels(int i, set<int> &labels);
            void    GetNeighborLabels(vector<int> &N, set<int> &labels);
            void    GetNeighborLabels1(vector<int> &N, set<int> &labels);
            void    GetNeighborLabels(vector<int> &N, vector<int> &labels);

    virtual int        GetNewLabel()=0;
    virtual int        GetNodeCount(int l)=0;
    virtual void    GetUsedLabels(vector<int> &L)=0;
    virtual int        GetLabel(int i)=0;
    virtual double    GetEdgeProb(int i, int j)=0;
    virtual int        node_size()=0;
    virtual void    SetLabel(vector<int> &N, int l)=0;
};
class SwendsenWang:public SwendsenWang_Graph{
public:
    UnionFind_Matrix *components;

    SwendsenWang(){
        components=new UnionFind_Matrix();
    }
    SwendsenWang(int ntypes);

            void    Update_current_partition();
            void    Update_current_partition_log_orig();
            double    GetSumPowAreaComponents(int l,double power, int &ncomp);

    virtual int        Get_size(int i){return 1;}
    virtual double    logprob(int l1,int l2)=0;    //the log prob of obj l1, l2, together
    virtual double    logprob()=0;
    virtual void    SetLabel(vector<int> &N, int l)=0;
};
#endif

