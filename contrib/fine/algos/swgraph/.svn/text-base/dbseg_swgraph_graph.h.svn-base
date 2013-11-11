#pragma warning(disable:4786)
#ifndef _GRAPH_H
#define _GRAPH_H

#include <vector>
#include <map>
#include <set>
#include "Matrix.h"
#include "unionfind.h"
#include "stl_util.h"
using namespace std;

class graphm;

template<class Tp>
class weighted_edge:public edge{
public:
    Tp value;
    weighted_edge (){i=0;j=0;value=0;}
    weighted_edge (int i, int j, Tp d):edge(i,j){value=d;}
    bool    operator<(const weighted_edge &e){return (value<e.value);}
    weighted_edge    &operator=(const weighted_edge &e){ 
        if(this!=&e){
            i=e.i;j=e.j;value=e.value;
        }
        return *this;
    }
};

typedef        map<int, double, less<int> > dEdgeList;
typedef        vector<pair<int, double> > dEdgeListf;
typedef        vector<dEdgeList>    dAdjacencyList;

template<class Tp>
class graph{
public:
    typedef  map<int, Tp, less<int> > EdgeList;
    typedef  vector<edge>    edgeList;
    typedef  vector<weighted_edge<Tp> >    wedgeList;
    
    vector<EdgeList> e;    //adjacency list with the edges

    graph()    {
        e.clear();
    };
    graph & operator=( graph &g);
    bool  operator==( graph &g);
    bool  operator!=( graph &g);
    
    int        size(){return e.size();};
    virtual void MakeGraph(int nv);                //allocates memory for a graph with nv vertices
    void    ClearEdges();
    void    DeleteNodeEdges(int i);
    void    DeleteEdge(int i, int j);
    void    DeleteDirectedEdge(int i, int j);
    int        GetDegree(int i);
    Tp        GetEdge(int i, int j);
    void    InsertDirectedEdge(int i, int j, Tp w);
    void    InsertEdge(int i, int j, Tp w);
    void    SetEdge(int i, int j, Tp w);
    void    GetUndirectedEdgeList(edgeList &L);
    void    GetUndirectedWEdgeList(wedgeList &L);
    void    SetDirectedEdge(int i, int j, Tp w);
    void    KeepBest(int k);
    virtual void    GetNeighborNodes(int i,vector<int> &L);
};

template<class Tp>
class graph_fast{
public:

    typedef  vector<pair<int, Tp> > EdgeList;
    
    EdgeList *e;    //adjacency list with the edges
    int n;

    graph_fast(){};
    
    int        size(){return n;};
    virtual void MakeGraph(int nv);                //allocates memory for a graph with nv vertices
    int        GetDegree(int i);
    void    InsertDirectedEdge(int i, int j, Tp w);
    void    InsertEdge(int i, int j, Tp w);
    Tp        GetEdge(int i, int j);
    virtual void    GetNeighborNodes(int i,vector<int> &L);
    /*
    void    ClearEdges();
    void    SetEdge(int i, int j, Tp w);
    void    GetUndirectedEdgeList(edgeList &L);
    void    GetUndirectedWEdgeList(wedgeList &L);
    void    SetDirectedEdge(int i, int j, Tp w);
    void    DeleteNodeEdges(int i);
    void    DeleteEdge(int i, int j);
    void    DeleteDirectedEdge(int i, int j);
    */
};

template<class Tp>
class graphuf:public UnionFind,public graph<Tp>{
//private:
public:
    ;        //union-find data structure

    graphuf()    :graph<Tp>(){
        P.clear();
    }
    graphuf & operator=(const graphuf &g);
    virtual int    ConnectedComponents();    //run this before querying if two elts are in the same component
                                    //updates the union-find data structure to reflect the components
    void    MakeGraph(int nv);                //allocates memory for a graph with nv vertices
    bool    SameComponent(int i, int j){return SameSet(i,j);}    //returns true if the two elts are in the same component
    int        SampleConnectedComponent(vector<int> &pick){return SampleSet(pick);}
    int        GetNoComponents(){return Get_nsets();}
    int        GetComponentnElts(int i){return GetSetnElts(i);}
    void    GetComponentWithNo(int k, vector<int> &L){GetSetWithIndex(k,L);}
    void    GetComponentOf(int i, vector<int> &L){GetSetOf(i,L);}
    void    GetComponentEdges(int i, edgeList &L);
    int        GetCompNo(int i){return GetSetIndex(i);}
    int        GetCompRep(int k){return GetSetRep(k);}
    void    Get_reps(vector<int> &L);    //finds representatives of all components
};

class graphm{
    //graph with data structure as a matrix
public:
    int n;        //the size of the graph;
    Matrix<double> e;

    graphm(){n=0;};
    ~graphm(){};
    graphm & operator=(const graphm &g);
    graphm & operator=( graph<double> &g);
    void MakeGraph(int nv);                //allocates memory for a graph with nv vertices
    int GetEdgeCount();
};

typedef vector<int> List;

class dirgraph{
public:
    int n;
    vector<List> Adj;
    List rank;                //after TopSort rank[i]=object of rank i in top order

private:
    int time;
    List d,f,col,p,rez;

public:
    dirgraph(){n=0;Adj.clear();};
    void    MakeGraph(int nv);                //allocates memory for a graph with nv vertices
    void    ResizeGraph(int nv);
    bool    IsAcyclic();
    bool    TopSort();
    int        DFS_Visit(int u);
    int        rankindex(int u);
    void    InEdges(int u, List &l);
    bool    Edge(int u, int v);
    void    printorder(char *text);
    void    Save(char *filename);
    void    Load(char *filename);
};

class BreadthFirstSearch{
public:
    graph<double> *g;
    BreadthFirstSearch(graph<double> *pgraph){g=pgraph;}
    void DoSearch(int s, vector<int> &d, vector<int> &pred);
    void GetPath(vector<int> &pred, int d, vector<int> &path);
    void GetLongestPath(vector<int> &path);
};

template<class Tp>
void CheckVectorSize(vector<Tp> &v,int label){
    while (label>=v.size ()){
        Tp tmp;
        v.push_back(tmp);
    }
}

void SampleGraph(graphm &gm,graph<double> &g);                    //samples the graph with weights=probabilities
void ThreshGraph(graph<double> &g,graph<double> &g1,double thresh);
void findMinimumSpanningTree(graph<double> &g, graphuf<double> &g1);        //finds MST for graph g, stores it in g1
void MakeProbGraph(graph<double> &g, graph<double> &g1, double Temp);    //makes a weighted graph into a prob. graph by exponentiating
void MakeProbGraph(graphm &g, graph<double> &g1, double Temp);    //makes a weighted graph into a prob. graph by exponentiating
void MakeProbGraph(graphm &g, graphm &g1, double Temp);    //makes a weighted graph into a prob. graph by exponentiating
void MakeProbGraph(graph<double> &g, graphm &g1, double Temp);    //makes a weighted graph into a prob. graph by exponentiating
void print(char *filename, vector<weighted_edge<double> > &m);


//
// graph functions
// 
template<class Tp>
void graph<Tp>::MakeGraph(int nv){
    EdgeList e1;
    e.clear();
    e1.clear();
    e.assign(nv,e1);
};

template<class Tp>
graph<Tp> &graph<Tp>::operator=( graph<Tp> &g){
    EdgeList::iterator ei;
    EdgeList el;
    AdjancencyList::iterator ai;
    if(this!=&g){
        e.clear();
        for (ai=g.e.begin();ai!=g.e.end();++ai){
            el.clear();
            for (ei=(*ai).begin();ei!=(*ai).end();++ei)
                el.insert(*ei);
            e.push_back(el);
        }
    }
    return *this;
}

template<class Tp>
bool graph<Tp>::operator==( graph<Tp> &g){
    EdgeList::iterator ei,ej;
    int i,j;
    if(this!=&g){
        if (n!=g.n)
            return false;
        for (i=0;i<n;i++){
            if (e[i].size()!=g.e[i].size())
                return false;
            for (ei=g.e[i].begin();ei!=g.e[i].end();++ei){
                j=(*ei).first;
                ej=e[i].find(j);
                if (ej==e[i].end())
                    return false;
                if ((*ej).second!=(*ei).second)
                    return false;
            }
        }
    }
    return true;
}

template<class Tp>
bool graph<Tp>::operator!=( graph<Tp> &g){
    return !(*this==g);
}

template<class Tp>
void graph<Tp>::DeleteEdge(int i, int j){
    DeleteDirectedEdge(i,j);
    DeleteDirectedEdge(j,i);
}

template<class Tp>
void graph<Tp>::DeleteDirectedEdge(int i, int j){
    EdgeList::iterator ei;
    ei=e[i].find(j);
    if (ei!=e[i].end())
        e[i].erase(ei);
}

template<class Tp>
void graph<Tp>::DeleteNodeEdges(int i){
    List L;
    GetNeighborNodes(i,L);
    for (int j=0;j<L.size();j++){
        DeleteDirectedEdge(L[j],i);
    }
    e[i].clear();
}

template<class Tp>
int    graph<Tp>::GetDegree(int i){
    //the number of edges at vertex i
    return e[i].size();
}

template<class Tp>
Tp graph<Tp>::GetEdge(int i, int j){
    //returns the weight of the edge (i,j), 0 if there is no such edge
    EdgeList::iterator ei;
    ei=e[i].find(j);
    if (ei!=e[i].end()){
        return (*ei).second;
    }
    else{
        Tp tmp;
        tmp=0;
        return tmp;
    }
}

template<class Tp>
void graph<Tp>::InsertDirectedEdge(int i, int j, Tp w){
    //inserts edge without checking
    e[i].insert(EdgeList::value_type(j,w));
}

template<class Tp>
void graph<Tp>::InsertEdge(int i, int j, Tp w){
    //inserts edge without checking
    InsertDirectedEdge(i,j,w);
    InsertDirectedEdge(j,i,w);
}

template<class Tp>
void graph<Tp>::SetDirectedEdge(int i, int j, Tp w){
    //sets the weight of the edge to w if it exists, else, inserts it
    EdgeList::iterator ei;
    ei=e[i].find(j);
    if (ei!=e[i].end()){
        (*ei).second=w;
    }
    else{
        InsertDirectedEdge(i,j,w);
    }
}

template<class Tp>
void graph<Tp>::SetEdge(int i, int j, Tp w){
    SetDirectedEdge(i,j,w);
    SetDirectedEdge(j,i,w);
}

template<class Tp>
void graph<Tp>::GetNeighborNodes(int i,vector<int> &L){
    EdgeList::iterator ei;
    L.clear();
    for (ei=e[i].begin();ei!=e[i].end();++ei){
        L.push_back((*ei).first);    
    }
}

template<class Tp>
void graph<Tp>::GetUndirectedEdgeList(edgeList &L){
    edge e1;
    EdgeList::iterator ei;
    int i,n=e.size();
    L.clear();
    for (i=0;i<n;i++){
        for (ei=e[i].begin();ei!=e[i].end();++ei){
            if ((*ei).first>i){
                e1.i=i;e1.j=(*ei).first;
                L.push_back(e1);    
            }
        }
    }
}
template<class Tp>
void graph<Tp>::GetUndirectedWEdgeList(wedgeList &L){
    weighted_edge<Tp> e1;
    EdgeList::iterator ei;
//    AdjacencyList::iterator ai;
    int i,n=e.size();
    L.clear();
//    for (ai=e.begin();ai!=e.end();++ai){
    for (i=0;i<n;i++){
        for (ei=e[i].begin();ei!=e[i].end();++ei){
            if ((*ei).first>i){
                e1.i=i;e1.j=(*ei).first;e1.value=(*ei).second;
                L.push_back(e1);    
            }
        }
    }
}

template<class Tp>
void graph<Tp>::KeepBest(int k){
    vector<weighted_edge<Tp> > L;
    if (k>=e.size())
        return;
    GetUndirectedWEdgeList(L);
    sort(L.begin(),L.end());
    L.erase(L.begin()+k,L.end());
    ClearEdges();
    for (int i=0;i<L.size();i++){
        InsertEdge(L[i].i,L[i].j,L[i].w);
    }
}
//
// graph_fast functions
// 
template<class Tp>
void graph_fast<Tp>::MakeGraph(int nv){
    e=new EdgeList[nv];
    n=nv;
};

template<class Tp>
Tp graph_fast<Tp>::GetEdge(int i, int j){
    //returns the weight of the edge (i,j), 0 if there is no such edge
    EdgeList::iterator ei=find_key(e[i],j);
    if (ei!=e[i].end()){
        return (*ei).second;
    }
    Tp tmp;
    tmp=0;
    return tmp;
}

template<class Tp>
int    graph_fast<Tp>::GetDegree(int i){
    //the number of edges at vertex i
    return e[i].size();
}


template<class Tp>
void graph_fast<Tp>::InsertDirectedEdge(int i, int j, Tp w){
    //inserts edge without checking
    pair<int, Tp> ee;
    ee.first=j;ee.second=w;
    e[i].push_back(ee);
}

template<class Tp>
void graph_fast<Tp>::InsertEdge(int i, int j, Tp w){
    //inserts edge without checking
    InsertDirectedEdge(i,j,w);
    InsertDirectedEdge(j,i,w);
}
template<class Tp>
void graph_fast<Tp>::GetNeighborNodes(int i,vector<int> &L){
    EdgeList::iterator ei;
    L.clear();
    for (ei=e[i].begin();ei!=e[i].end();++ei){
        L.push_back((*ei).first);    
    }
}

//
// graphuf functions
//


template<class Tp>
void graphuf<Tp>::MakeGraph(int nv){
    graph<Tp>::MakeGraph(nv);
    UnionFind::Construct(nv);
};

template<class Tp>
graphuf<Tp> &graphuf<Tp>::operator=(const graphuf<Tp> &g){
    if(this!=&g){
        *(graph *)this=*(graph *)&g;
        *(UnionFind *)this=*(UnionFind *)&g;
    }
    return *this;
}

template<class Tp>
int graphuf<Tp>::ConnectedComponents(){        
    //run this before querying if two elts are in the same component
    //updates the union-find data structure to reflect the components
    //returns the number of components
    int i,j,n=e.size();
    vector<weighted_edge<Tp> >::iterator ei;
    vector<weighted_edge<Tp> > e;
    UnionFind::Construct(n);
    GetUndirectedWEdgeList(e);
    for(ei=e.begin();ei!=e.end();++ei){
        i=(*ei).i;j=(*ei).j;
        if (findset(i)!=findset(j)){
            setunion(i,j);
        }
    }
    return nsets;
}

template<class Tp>
void graphuf<Tp>::Get_reps(vector<int> &L){
    // finds the representatives of the components 
    int i,n=e.size();
    L.clear();
    for (i=0;i<n;i++){
        if (P[i]==-1){
            L.push_back(i);
        }
    }
}

template<class Tp>
void graphuf<Tp>::GetComponentEdges(int j, edgeList &L){
    //returns the edges of component with representative j
    edge<Tp> e1;
    EdgeList::iterator ei;
    vector<int> N;
    int i,n;
    GetComponentOf(j,N);
    n=N.size();
    L.clear();
    for (i=0;i<n;i++){
        for (ei=e[N[i]].begin();ei!=e[N[i]].end();++ei){
            if ((*ei).first>N[i]){
                e1.i=N[i];e1.j=(*ei).first;e1.value=(*ei).second;
                L.push_back(e1);    
            }
        }
    }
}


#endif

