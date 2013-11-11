#pragma warning(disable:4786)
#include "unionfind.h"
#include "functions.h"
#include "histogram.h"
#include <set>
#include <deque>
#include <algorithm>
#include <cmath>

//
// UnionFind functions
//

UnionFind &UnionFind::operator=(const UnionFind &g){
    if(this!=&g){
        int n=g.P.size();
        P.reserve(n);
        nsets=g.nsets;
        for (int i=0;i<n;i++)
            P.push_back(g.P[i]);
    }
    return *this;
}
void UnionFind::Construct(int N){
    P.assign(N,-1);
    nsets=N;
}

double    UnionFind::GetSumPowSizeSets(double power){
    double sum;
    int i,n=P.size();
    vector<double> L;
    sum=0;
    L.assign(n,0);
    for (i=0;i<n;i++){
        L[findset(i)]+=Get_size(i);
    }
    for (i=0;i<n;i++){
        if (L[i]>0){
            sum+=pow(L[i],power);
        }
    }
    return sum;
}
double    UnionFind::GetSumPownEltsSets(double power){
    double sum;
    int i,n=P.size();
    vector<int> L;
    sum=0;
    L.assign(n,0);
    for (i=0;i<n;i++){
        L[findset(i)]++;
    }
    for (i=0;i<n;i++){
        if (L[i]>0){
            sum+=pow(L[i],power);
        }
    }
    return sum;
}
void UnionFind::GetCompIndices(vector<int> &idx){
    //find the index of the component of each element
    int nComponent;
    nComponent=0;
    int node,nodep;
    node=getFirstNode();
    while (node!=eos){
        nodep=Get_P(node);
        if (nodep==eos){    
            idx.push_back(nComponent);
            nComponent++;
        }
        else{
            idx.push_back(idx[nodep]);
        }
        node=getNextNode();
    }
}

//
// UnionFind_Matrix functions
//

void UnionFind_Matrix::Construct(int N){
    SetDimension(1,N);init_value(-1);
    nsets=N;
}

void UnionFind_Matrix::Construct(int nx,int ny){
    SetDimension(ny,nx);init_value(-1);
    nsets=ny*nx;
}
int UnionFind_Matrix::getFirstNode(){
    nCurPos=0;
    FindFirstValid();
    return nCurPos;
}
int UnionFind_Matrix::getNextNode(){
    nCurPos++;
    FindFirstValid();
    if (nCurPos>=size())
        return eos;
    return nCurPos;
}
void UnionFind_Matrix::FindFirstValid(){
    int n=size();
    while(!valid(nCurPos)&&nCurPos<n)
        nCurPos++;
}
void UnionFind_Matrix::Set(vector<vector<int> *> L){
    //set the union-find to have the sets *L[l]
    int l, n=L.size(),i,i0,nl;
    vector<int> v;
    Reset();
    for (l=0;l<n;l++){
        v=*L[l];
        nl=v.size();
        if (nl>0){
            i0=v[0];
            for (i=1;i<nl;i++){
                setunion(i0,v[i]);
            }
        }
    }
}
void UnionFind_Matrix::Write_components(Matrix<int> &M){
    //for each i writes the index of the cc containing it (begining with 1)
    int i,j, k,n=nx()*ny();
    k=1;
    for (i=0;i<n;i++){
        j=Get_P(i);
        if (j==-1){
            M(i)=k;
            k++;
        }
        else{
            M(i)=M(j);
        }
    }
}




