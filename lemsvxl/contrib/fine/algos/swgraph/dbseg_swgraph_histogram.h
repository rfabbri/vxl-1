#pragma warning(disable:4786)
#ifndef _HISTOGRAM_H
#define _HISTOGRAM_H

#include "Matrix.h"  
#include "point.h"  
#include <set>  
#include <map>  
using namespace std;

class histogram_abstract{    
public:
    virtual double    KL(histogram_abstract *h);
    double    DistAB(histogram_abstract *h);
    double    Get_freq (int bin);
    virtual double    entropy();
    virtual void operator+=(histogram_abstract *a);
    virtual void operator-=(histogram_abstract *a);

    virtual void    Add_sample(int bin, int n)=0;
    virtual void    Subtract_sample(int bin, int n)=0;
    virtual int        Get_nbins()=0;
    virtual int        Get_nsamples()=0;
    virtual int     Get_nsamples(int bin)=0;
    virtual void    Zero()=0;
};
class histogram_abstract_1d:public histogram_abstract{    
public:
    double        lb;                    // lower bound of bins
    double        d;                    // bin width

    histogram_abstract_1d(){
        lb=0;d=1;
    }
    void Add(double x,int n=1);
    void Subtract(double x,int n=1);
    void Add( histogram_abstract_1d *a, double bias);
    void Subtract( histogram_abstract_1d *a, double bias);

    double    prob(double x);
    int        Get_bin(double x){return (x-lb)/d;}
    double    Get_bin_center(int i){return lb+d*(i+0.5);}
    Matrixi Get_hist();

    void    Set_bounds(double lowb, double uppb){lb=lowb;d=(uppb-lowb)/Get_nbins();}
};

class histogram_abstract_2d:public histogram_abstract{    
public:
    double        lbx,lby;                // lower bound of bins
    double        wx,wy;                    // bin width

    histogram_abstract_2d(){
        lbx=lby=0;wx=wy=1;
    }
    void Add(double x, double y);
    void Subtract(double x,double y);
    void Add(double x, double y,int n);
    void Subtract(double x,double y,int n);

    int        Get_bin(double x, double y);
    Matrixd    Get_bin_center(int i);
    double    Get_x(int bin);
    double    Get_y(int bin);
    int        Get_ix(int bin){return bin%nx();}
    int        Get_iy(int bin){return bin/nx();}
    int        Get_bin_idx(int ix, int iy);

    void    Set_bounds(double x0, double y0, double x1, double y1){lbx=x0;lby=y0,wx=(x1-x0)/nx();wy=(y1-y0)/ny();}
    virtual int nx()=0;
    virtual int ny()=0;
};
class histogram_abstract_3d:public histogram_abstract{    
public:
    double        lbx,lby,lbz;                // lower bound of bins
    double        wx,wy,wz;                    // bin width

    histogram_abstract_3d(){
        lbx=lby=lbz=0;wx=wy=wz=1;
    }
    void Add(double x, double y, double z);
    void Subtract(double x,double y, double z);
    void Add(Matrixd &m);
    void Subtract(Matrixd &m);
    void Add(double x, double y, double z, int n);
    void Subtract(double x, double y, double z, int n);

    __int64    Get_bin(double x, double y, double z);
    Matrixd    Get_bin_center(__int64 i);
    int        Get_ix(__int64 bin){return bin/(ny()*nz());}
    int        Get_iy(__int64 bin){return (bin/nz())%ny();}
    int        Get_iz(__int64 bin){return bin%nz();}
    __int64    Get_bin_idx(int ix, int iy, int iz){return (__int64)ix*ny()*nz()+ (__int64)iy*nz()+(__int64)iz;}

    void    Set_bounds(double x0, double y0, double z0, double x1, double y1, double z1){
        lbx=x0;lby=y0,lbz=z0;wx=(x1-x0)/nx();wy=(y1-y0)/ny();wz=(z1-z0)/nz();}

    virtual int nx()=0;
    virtual int ny()=0;
    virtual int nz()=0;
};

class histogram:public histogram_abstract_1d, public Matrix<int>{    //data[0]=count,data[1]=lb_count,data[2]=ub_count
public:
    bool        oob;                //incorporate out of bounds into endbins
    double        sumb,sumbb;

    histogram(){
        oob=false;
    }
    histogram( int n){
        SetDimension(1,n+3);Zero();
    }
    histogram( int n, double lowb, double uppb){
        Construct(n,lowb,uppb);
    }

    void operator+=( histogram *a){
        *(Matrixi *)this=*this+*a;
    }
    void operator-=( histogram *a){
        *(Matrixi *)this=*this-*a;
    }
    void Construct( int n, double lowb, double uppb){
        SetDimension(1,n+3);Zero();
        Set_bounds(lowb,uppb);
    }
    double    Get_mean();
    double  Get_var();

    void    Add_sample(int bin, int n);
    void    Subtract_sample(int bin, int n);
    int        Get_nbins(){return size()-3;}
    int     Get_nsamples(){return Data(0);}
    int     Get_nsamples(int i);
    void    Zero(){Matrix<int>::Zero();sumb=0;sumbb=0;}
};
class histogram_2d:public histogram_abstract_2d,public Matrix<__int64>{
public:
    __int64 nsamples;

    histogram_2d(){};
    histogram_2d(int nx, int ny){
        SetDimension(ny,nx);Zero();
    }
    histogram_2d(const histogram_2d &h):Matrix<__int64>(h){
        nsamples=h.nsamples;lbx=h.lbx;lby=h.lby;wx=h.wx;wy=h.wy;
    }
    histogram_2d&    operator=(histogram_2d &a);
    friend histogram_2d operator+(histogram_2d &a,histogram_2d &b);
    friend histogram_2d operator-(histogram_2d &a,histogram_2d &b);
    void    operator+=(histogram_2d *a){*((Matrix<__int64> *)this)=*((Matrix<__int64> *)this)+*a;nsamples+=a->nsamples;}
    void    operator-=(histogram_2d *a){*((Matrix<__int64> *)this)=*((Matrix<__int64> *)this)-*a;nsamples-=a->nsamples;}
    bool    operator==(histogram_2d &a){
        if (nsamples!=a.nsamples)
            return false;
        return *((Matrix<__int64> *)this)==a;}
    void    Add_sample(int bin, int n);
    void    Subtract_sample(int bin, int n);
    void    Add_sample(int binx,int biny, int n);
    void    Subtract_sample(int binx,int biny, int n);
    void    Zero(){nsamples=0;Matrix<__int64>::Zero();}
    double    sample(int &i);
    double    sample(double &x, double &y);

    Matrixd    Get_normalized();
    Matrixd    Get_mean();
    Matrixd    Get_var();
    int        Get_nbins(){return nx()*ny();}
    int        Get_nsamples(){return nsamples;}
    int     Get_nsamples(int bin){return (*this)(bin);}
    int        nx(){return Matrix<__int64>::nx();}
    int        ny(){return Matrix<__int64>::ny();}
};

template<class Tp>
class vector_sparse:public map<int,Tp>{ // a sparse vector with values only at some indices i
public:
    vector_sparse(){
        Zero();
    }
    void    operator+=(vector_sparse *a);
    void    operator-=(vector_sparse *a);
    bool    operator==(vector_sparse *a);
    Tp        operator ()(int i);

    void    Add(int bin, Tp n);    // add value n to index i
    void    Zero();
    int        GetMax_i();            // return i which has max value
    Tp        GetMax();            // return max value
    void    Read (char *filename);
    void    Write(char *filename);
    void    Load (Matrix<Tp> &M);
    void    Upload (Matrix<Tp> &M);
};
template<class Tp>
class vector_sparse_set:public map<int,set<Tp>*>{
public:
    vector_sparse_set(){Zero();}
    void    Insert(int bin, Tp n);
    void    Extract(int bin, Tp n);
    void    Zero();
    set<Tp> *operator ()(int i);
};

class histogram_sparse:public map<int,__int64>{
public:
    int        nbins;
    __int64    nsamples;
    double  sumnlogn;

    histogram_sparse(){
        nbins=0;sumnlogn=0;
        Zero();
    }
    histogram_sparse(int nBins){
        nbins=nBins;sumnlogn=0;
        Zero();
    }
    bool    operator==(histogram_sparse *a);
    void    operator+=(histogram_sparse *a);
    void    operator-=(histogram_sparse *a);
    void    Add_sample(int bin, int n);
    void    Subtract_sample(int bin, int n);
    double    Get_nbins(){return nbins;}
    void     Set_nbins(int nb){nbins=nb;}
    double    Get_nsamples(){return nsamples;}
    double    Get_nsamples(int bin);
    double    entropy();
    double    compute_entropy();
    double    KL(histogram_abstract *h);
    void    Zero();
    bool    less(histogram_abstract *h);
    double    sample(int &i);

    void    Read (char *filename);
    void    Write(char *filename);
    void    Load (Matrix<__int64> &M);
    void    Upload (Matrix<__int64> &M);
};
class graph_hist;
class graph_hist_edge_iterator{
public:
    int it;
    vector_sparse<int>::iterator jt;
    graph_hist *parent;
    
    int i(){return it;}
    int j(){return (*jt).first;}
    graph_hist_edge_iterator & operator=(const graph_hist_edge_iterator &i);
    bool operator!=(const graph_hist_edge_iterator &i);
    void operator++();
};
class graph_hist{
public:
    vector<vector_sparse<int> *> e;    //adjacency list with the edges

    graph_hist(){};
    ~graph_hist(){ for (int i=0;i<e.size();i++) delete e[i];}
    
    void    Add_edge(int i, int j, int n);
    void    Subtract_edge(int i, int j, int n);
    void    GetNeighborNodes(int i, vector<int> &N);
    void    Check_e(int i);
    graph_hist_edge_iterator e_begin();
    graph_hist_edge_iterator e_end();
};
class graph_hist_set{
public:
    vector<vector_sparse_set<Point<int> > *> e;    //adjacency list with the edges

    graph_hist_set(){};
    ~graph_hist_set(){ for (int i=0;i<e.size();i++) delete e[i];}
    
    void    Add_edge(int i, int j, int ii, int ij);
    void    Subtract_edge(int i, int j, int ii, int ij);
    void    Check_e(int i);
    set<Pointi> * Get_set(int i, int j);
};
class histogram_sparse_3d:public histogram_abstract_3d,public histogram_sparse{
public:
    int        n_x,n_y,n_z;

    histogram_sparse_3d(const int nx,const  int ny,const  int nz):histogram_sparse(nx*ny*nz){
        n_x=nx;n_y=ny;n_z=nz;
    }
    void    operator+=(histogram_sparse_3d *a){*((histogram_sparse *)this)+=a;}
    void    operator-=(histogram_sparse_3d *a){*((histogram_sparse *)this)-=a;}
    void    Add_b(histogram_sparse_3d *a, Matrixi &bias);
    void    Subtract_b(histogram_sparse_3d *a, Matrixi &bias);
    void    Add_sample(int bin, int n){histogram_sparse::Add_sample(bin,n);}
    void    Subtract_sample(int bin, int n){histogram_sparse::Subtract_sample(bin,n);}
    void    Zero(){histogram_sparse::Zero();}

    int        Get_nbins(){return histogram_sparse::Get_nbins();}
    void    Set_nbins(int nx, int ny, int nz){
        histogram_sparse::Set_nbins(nx*ny*nz);
        n_x=nx;n_y=ny;n_z=nz;}
    int        Get_nsamples(){return histogram_sparse::Get_nsamples();}
    int     Get_nsamples(int bin){return histogram_sparse::Get_nsamples(bin);}
    int        nx(){return n_x;}
    int        ny(){return n_y;}
    int        nz(){return n_z;}
    Matrixd    Get_mean();
    Matrixd    Get_var();
    void    Get_eig(Matrixd &l);
    double  sample(Matrixd &x);

    double    entropy(){return histogram_sparse::entropy();}
    double    KL(histogram_abstract *h){return histogram_sparse::KL(h);}
};

void  Parse_edges(Matrix<int> &L, histogram_sparse &e);

//
// vector_sparse
//

template<class Tp>
void vector_sparse<Tp>::Zero(){
    clear();
}
template<class Tp>
void vector_sparse<Tp>::Add(int bin,Tp n){
    map<int,Tp>::iterator mit;
    if (bin==48674)
        bin=bin;
    mit=find(bin);
    if (mit==end()){
        insert(map<int,Tp>::value_type(bin,n));
    }
    else{
        (*mit).second+=n;
        if ((*mit).second==0)
            erase(mit);
    }
}
/*
template<class Tp>
void vector_sparse<Tp>::Subtract(int bin,Tp n){
    map<int,Tp>::iterator mit;
    mit=find(bin);
    if (mit==end()){
        insert(map<int,Tp>::value_type(bin,-n));
    }
    else{
        (*mit).second-=n;
        if ((*mit).second==0)
            erase(mit);
    }
}
*/
template<class Tp>
Tp vector_sparse<Tp>::operator ()(int bin){
    map<int,Tp>::iterator mi;
    mi=find(bin);
    if (mi!=end()){
        return (*mi).second;
    }
    else{
        return 0;
    }
}

template<class Tp>
void vector_sparse<Tp>::operator+=(vector_sparse *a){
    map<int,Tp>::iterator mit;
    for (mit=a->begin();mit!=a->end();++mit){
        Add((*mit).first,(*mit).second);
    }
}
template<class Tp>
void vector_sparse<Tp>::operator-=(vector_sparse *a){
    map<int,Tp>::iterator mit;
    for (mit=a->begin();mit!=a->end();++mit){
        Add((*mit).first,-(*mit).second);
    }
}
template<class Tp>
bool vector_sparse<Tp>::operator==(vector_sparse *a){
    map<int,Tp>::iterator mit;
    int i;
    if (size()!=a->size()){
        Matrix<Tp> M;
        Upload(M);print("this.txt",M);
        a->Upload(M);print("a.txt",M);
        return false;
    }
    for (mit=a->begin();mit!=a->end();++mit){
        i=(*mit).first;
        if (i==48674)
            i=i;
        if ((*mit).second!=(*a)[i])
            return false;
    }
    return true;
}
template<class Tp>
void vector_sparse<Tp>::Read(char *filename){
    Matrix<Tp> M;

    dlmread(filename,',',M);
    Load(M);
}
template<class Tp>
void vector_sparse<Tp>::Write(char *filename){
    Matrix<Tp> M;
    Upload(M);
    print(filename,M);
}
template<class Tp>
void vector_sparse<Tp>::Load (Matrix<Tp> &M){
    int i,idx;
    Tp d;
    Zero();
    for (i=0;i<M.cols();i++){
        idx=(int) M(0,i);
        d=M(1,i);
        insert(map<int,Tp>::value_type(idx,d));
    }
}
template<class Tp>
void vector_sparse<Tp>::Upload (Matrix<Tp> &M){
    map<int,Tp>::iterator mit;
    M.SetDimension(2,size());
    int i,idx;
    Tp p;
    i=0;
    for (mit=begin();mit!=end();++mit){
        idx=(*mit).first;
        p=(*mit).second;
        M(0,i)=idx;
        M(1,i)=p;
        i++;
    }
}
template<class Tp>
int    vector_sparse<Tp>::GetMax_i(){
    if (empty())
        return -1;
    map<int,Tp>::iterator mit=begin();
    Tp max=(*mit).second;
    int maxi=(*mit).first;
    ++mit;
    for (;mit!=end();++mit){
        if ((*mit).second>max){
            maxi=(*mit).first;
            max=(*mit).second;
        }
    }
    return maxi;
}
template<class Tp>
Tp vector_sparse<Tp>::GetMax(){
    if (empty())
        return 0;
    map<int,Tp>::iterator mit=begin();
    Tp max=(*mit).second;
    ++mit;
    for (;mit!=end();++mit){
        if ((*mit).second>max){
            max=(*mit).second;
        }
    }
    return max;
}

//
// vector_sparse_set
//

template<class Tp>
void vector_sparse_set<Tp>::Zero(){
    map<int,set<Tp>*>::iterator mit;
    for (mit=begin();mit!=end();++mit){
        delete (*mit).second;
    }
    clear();
}
template<class Tp>
void vector_sparse_set<Tp>::Insert(int bin,Tp n){
    map<int,set<Tp>*>::iterator mit;
    mit=find(bin);
    if (mit==end()){
        set<Tp> *s=new set<Tp>();
        s->insert(n);
        insert(map<int,set<Tp>*>::value_type(bin,s));
    }
    else{
        (*mit).second->insert(n);
    }
}
template<class Tp>
void vector_sparse_set<Tp>::Extract(int bin,Tp n){
    map<int,set<Tp>*>::iterator mit;
    mit=find(bin);
    if (mit!=end()){
        (*mit).second->erase(n);
        if ((*mit).second->empty()){
            delete (*mit).second;
            erase(mit);
        }
    }
}
template<class Tp>
set<Tp>* vector_sparse_set<Tp>::operator ()(int bin){
    map<int,set<Tp>*>::iterator mi;
    mi=find(bin);
    if (mi!=end()){
        return (*mi).second;
    }
    else{
        return NULL;
    }
}


#endif

