#pragma warning(disable:4786)
#ifndef _UNIONFIND_H
#define _UNIONFIND_H

#include "Matrix.h"
#include <vector>
#include <map>
#include <set>
using namespace std;

template<class Tp> class rectangle;

class edge{
public:
    int i;
    int j;
    edge(){i=0;j=0;}
    edge(int ii, int jj){i=ii;j=jj;}
    edge(const edge &e){i=e.i;j=e.j;}
    edge    &operator=(const edge &e){ 
        if(this!=&e){
            i=e.i;
            j=e.j;
        }
        return *this;
    }
};
template<class Tp>
class UnionFind_Basic{
public:
    int nsets;
    Tp  eos;            //represents the fact that it has no predecesor

    UnionFind_Basic(Tp eos_char){
        nsets=0;
        eos=eos_char;
    }
    bool    is_rep(Tp i){return (Get_P(i)==eos);} //true if i is the rep of its set
    void    makeset(Tp i);    //makes a set containing only i
    Tp        findset(Tp i);    //find the label of set containing i
    void    setunion(Tp i,Tp j);    //updates P to reflect the union of the sets specified by i and j
    bool    IsSameSet(Tp i, Tp j);    //returns true if the two elts are in the same component
    int        Get_nsets();

    virtual Tp Get_P(Tp node)=0;
    virtual void Set_P(Tp node, Tp value)=0;
};

template<class Tp>
class UnionFind_Abstract:public UnionFind_Basic<Tp>{
public:

    UnionFind_Abstract(Tp eos):UnionFind_Basic<Tp>(eos){
    }
    int        GetSetnElts(Tp i);
    void    GetSetOf(Tp i, vector<Tp> &L);
    int        GetSetIndex(Tp i);            //returns the number k so that i is in the k-th set
    void    GetSetWithIndex(int k, vector<Tp> &L);
    Tp        GetSetRep(int nComponentNumber);
    Tp        SampleSet(vector<Tp> &N, double &prob);
    void    Reset();

    virtual    Tp     getFirstNode()=0;
    virtual Tp   getNextNode()=0;
    virtual int     size()=0;
};

template<class Tp>
class UnionFind_vector:public UnionFind_Abstract<Tp>{
public:
    vector<pair<Tp,Tp> > P;
    int        nCurPos;
    UnionFind_vector():UnionFind_Abstract<Tp>(0){nCurPos=0;}
    void    clear(){P.clear();nsets=0;nCurPos=0;}
    void    addNode(Tp node){pair<Tp,Tp> p(node,eos);P.push_back(p);nsets++;}
    void    Construct(int N);
    Tp        Get_P(Tp i);
    void    Set_P(Tp i, Tp j);
    void    Set(Tp value){P[nCurPos]=value;}
    int        size(){return P.size();}
    virtual    Tp     getFirstNode();
    virtual Tp   getNextNode();
};

template<class Tp>
class UnionFind_map:public UnionFind_Abstract<Tp>{
public:
    map<Tp,Tp> P;        //union-find data structure
    map<Tp,Tp>::iterator nCurPos;

    UnionFind_map(Tp eos):UnionFind_Abstract<Tp>(eos){
        P.clear();
    }

    Tp        Get_P(Tp i){return P[i];}
    void    Set_P(Tp i, Tp j){P[i]=j;}
    Tp        Get(){return (*nCurPos).second;}
    Tp        Get_pos(){return (*nCurPos).first;}
    void    Set(Tp value){(*nCurPos).second=value;}
    void    Begin(){nCurPos=P.node=getFirstNode();}
    void    Next(){    if (nCurPos!=P.end())    ++nCurPos;}
    bool    valid(){return (nCurPos!=P.end());}
    int        size(){return P.size();}
    void    insert(Tp i);
    void    erase();
    void    erase(Tp i);
    void    Reset();
};

class UnionFind:public UnionFind_Abstract<int>{
public:
    vector<int> P;        //union-find data structure
    int            nCurPos;

    UnionFind():UnionFind_Abstract<int>(-1){
        P.clear();
    }
    void    clear(){P.clear();nsets=0;}
    void    Construct(int N);
    UnionFind & operator=(const UnionFind &g);
    int        Get_P(int i){return P[i];}
    void    Set_P(int i, int j){P[i]=j;}
    int        Get_pos(){return nCurPos;}
    int     getFirstNode(){nCurPos=0;return nCurPos;}
    int     getNextNode(){nCurPos++;if (nCurPos<P.size()) return nCurPos;else return eos;}
    bool    valid(){return (nCurPos<P.size());}
    int        size(){return P.size();}
    void    push_node(){P.push_back(eos);nsets++;}
    void    GetCompIndices(vector<int> &idx);

    double    GetSumPowSizeSets(double power);//sum of powers of size
    double    GetSumPownEltsSets(double power);//sum of count 
    virtual double Get_size(int i){return 1;}
};
class UnionFind_Matrix:public UnionFind_Abstract<int>, public Matrix<int>{
public:
    int        nCurPos;
    
    UnionFind_Matrix():UnionFind_Abstract<int>(-1){}
    void    Construct(int N);
    void    Construct(int nx, int ny);
    int        Get_P(int i){return Data(i);}
    void    Set_P(int i, int j){(*this)(i)=j;}
    int        Get_pos(){return nCurPos;}
//    int        getCurrentNode(){return Data(nCurPos);}
//    void    getCurrentNode(int value){(*this)(nCurPos)=value;}
    int     getFirstNode();
    int     getNextNode();
    void    FindFirstValid();
    bool    valid(){return valid(nCurPos);}
    int        size(){return Matrix<int>::size();}
    virtual bool valid (int i){return i<size();}
    void    Set(vector<vector<int> *> L);
    void    Write_components(Matrix<int> &M);
};



//
// UnionFind_Basic functions
//

template<class Tp>
void UnionFind_Basic<Tp>::makeset(Tp i){    
    //makes a set containing only i
    Set_P(i,eos);
}


template<class Tp>
Tp UnionFind_Basic<Tp>::findset(Tp i){    
    //find the label of set containing i
    Tp j=i;
    while(Get_P(j)!=eos){
       j=Get_P(j);
    }
    return j;
}

template<class Tp>
void UnionFind_Basic<Tp>::setunion(Tp x,Tp y){    
    //updates P to relect the union of the sets specified by x and y
    Tp i=findset(x);
    Tp j=findset(y);
    if (i>j){
        Set_P(i,j);
        nsets--;
    }
    if (j>i){
        Set_P(j,i);
        nsets--;
    }
}

template<class Tp>
bool UnionFind_Basic<Tp>::IsSameSet(Tp i, Tp j){    
    //returns true if the two elts are in the same set
    return findset(i)==findset(j);
}

template<class Tp>
int UnionFind_Basic<Tp>::Get_nsets(){        
    //returns the number of sets
    return nsets;
}

//
// UnionFind_Abstract functions
//

template<class Tp>
void UnionFind_Abstract<Tp>::Reset(){
    Tp node=getFirstNode();
    while(node!=eos){
        Set_P(node,eos);
        node=getNextNode();
    }
    nsets=size();
}

template<class Tp>
int UnionFind_Abstract<Tp>::GetSetIndex(Tp i){
    //returns the number k so that i is in the k-th set

    int k;
    Tp ir=findset(i);
    k=0;
    node=getFirstNode();
    while (node!=eos&&(Get_pos()!=ir)){
        if (Get()==eos){    
            k++;
        }
        node=getNextNode();
    }
    return k;
}

template<class Tp>
Tp UnionFind_Abstract<Tp>::GetSetRep(int nComponentNumber){
    //find the repr of the k-th set
    int nComponentIdx;
    Tp node;
    nComponentIdx=0;
    node=getFirstNode();
    while (node!=eos){
        if (Get_P(node)==eos){    
            if (nComponentIdx==nComponentNumber)
                break;
            nComponentIdx++;
        }
        node=getNextNode();
    }
    return node;
}

template<class Tp>
void UnionFind_Abstract<Tp>::GetSetOf(Tp i, vector<Tp> &L){
    //find the elements of the set containing i and stores them in L
    //first element is the rep

    Tp rep,node;
    rep=findset(i);
    L.clear();
    L.reserve(size());
    L.push_back(rep);
    node=getFirstNode();
    while (node!=eos){
        if ((node!=rep)&&(findset(node)==rep))
            L.push_back(node);
        node=getNextNode();
    }
}

template<class Tp>
void UnionFind_Abstract<Tp>::GetSetWithIndex(int k, vector<Tp> &L){
    GetSetOf(GetSetRep(k),L);
}

template<class Tp>
int UnionFind_Abstract<Tp>::GetSetnElts(Tp i){
    //returns the number of elts of the set containing i
    int ne=0;
    Tp j,rep=findset(i);
    node=getFirstNode();
    while (node!=eos){
        j=Get_pos();
        if (findset(j)==rep)
            ne++;
        node=getNextNode();
    }
    return ne;
}

template<class Tp>
Tp    UnionFind_Abstract<Tp>::SampleSet(vector<Tp> &N,double &prob){
    //get a random set in N
    int i;
    Tp repc;
    i=randint(Get_nsets());
    prob=1./Get_nsets();
    repc=GetSetRep(i);
    GetSetOf(repc,N);    
    return repc;
}

template<class Tp>
void UnionFind_map<Tp>::insert(Tp i){
    P.insert(map<Tp,Tp>::value_type(i,eos));
}

template<class Tp>
void UnionFind_map<Tp>::erase(){
    nCurPos=P.erase(nCurPos);
//    --nCurPos;
}
template<class Tp>
void UnionFind_map<Tp>::erase(Tp i){
    P.erase(i);
}
template<class Tp>
void UnionFind_map<Tp>::Reset(){
    map<Tp,Tp>::iterator i,me=P.end();
    for (i=P.begin();i!=me;++i){
        (*i).second=eos;
    }
}

template<class Tp>
void UnionFind_vector<Tp>::Construct(int N){
    P.assign(N,eos);
    nsets=N;
}
template<class Tp>
Tp    UnionFind_vector<Tp>::Get_P(Tp node){
    int i,n=P.size();
    for (i=0;i<n;i++){
        if (P[i].first==node)
            return P[i].second;
    }
    return eos;
}
template<class Tp>
void UnionFind_vector<Tp>::Set_P(Tp node, Tp value){
    int i,n=P.size();
    for (i=0;i<n;i++){
        if (P[i].first==node){
            P[i].second=value;
            return;
        }
    }
}

template<class Tp>
Tp UnionFind_vector<Tp>::getFirstNode(){
    nCurPos=0;
    if (nCurPos<P.size())
        return P[nCurPos].first;
    else
        return eos;
}

template<class Tp>
Tp UnionFind_vector<Tp>::getNextNode(){
    nCurPos++;
    if (nCurPos<P.size())
        return P[nCurPos].first;
    else
        return eos;
}

#endif

