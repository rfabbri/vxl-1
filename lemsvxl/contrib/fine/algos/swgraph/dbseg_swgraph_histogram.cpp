#include "histogram.h"
#include "functions.h"
#include "imgfunctions.h"

double    histogram_abstract::Get_freq (int bin){
    double d=Get_nsamples();
    if (d>0)
        return Get_nsamples(bin)/d;
    else
        return 0;
}
void histogram_abstract::operator+=( histogram_abstract *a){
    int i,n=Get_nbins(),ni;
    for (i=0;i<n;i++){
        ni=a->Get_nsamples(i);
        Add_sample(i,ni);
    }
}
void histogram_abstract::operator-=( histogram_abstract *a){
    int i,n=Get_nbins(),ni;
    for (i=0;i<n;i++){
        ni=a->Get_nsamples(i);
        Subtract_sample(i,ni);
    }
}
double histogram_abstract::entropy(){
    double d,sum=0.;
    int i,n=Get_nbins();

    for (i=0;i<n;i++){
        d=Get_freq(i);
        if (d>0.00000001){
            sum += d*log(d);
        }
    }
    return -sum;
}

double histogram_abstract::KL(histogram_abstract *h){
    //assume they have the same bins
    double p,q,sum=0.;
    int i,n=Get_nbins();

    for (i=0;i<n;i++){
        p=Get_freq(i);
        q=h->Get_freq(i);
        if (p>0.000000001){
            if (q>0.00000001)
                sum += p*log(p/q);
            else
                sum+=p*log(p/0.00000001);
        }
    }
    return sum;
}
double histogram_abstract::DistAB(histogram_abstract *h){
    //assume they have the same bins
    double p,q,sum=0.;
    int i,n=Get_nbins();

    for (i=0;i<n;i++){
        p=Get_freq(i);
        q=h->Get_freq(i);
        if ((p>0.0)&&(q>0)){
            sum += sqrt(p*q);
        }
    }
    return sum;
}


//
// histogram_abstract_1d
//
double    histogram_abstract_1d::prob(double x){
    int i=Get_bin(x);
    return Get_freq(i);
}

void histogram_abstract_1d::Add(double x,int n){
    int i=Get_bin(x);
    Add_sample(i,n);
}
void histogram_abstract_1d::Subtract(double x,int n){
    int i=Get_bin(x);
    Subtract_sample(i,n);
}

void histogram_abstract_1d::Add( histogram_abstract_1d *a, double bias){
    int i,bi=bias/d,n=Get_nbins(),ni;
    for (i=0;i<n;i++){
        ni=a->Get_nsamples(i+bi);
        Add_sample(i,ni);
    }
}
void histogram_abstract_1d::Subtract( histogram_abstract_1d *a, double bias){
    int i,bi=bias/d,n=Get_nbins(),ni;
    for (i=0;i<n;i++){
        ni=a->Get_nsamples(i+bi);
        Subtract_sample(i,ni);
    }
}

Matrixi histogram_abstract_1d::Get_hist(){
    Matrixi M;
    int i,n=Get_nbins();
    M.SetDimension(1,n);
    for (i=0;i<n;i++){
        M(i)=Get_nsamples(i);
    }
    return M;
}
//
// histogram_abstract_2d
//

int histogram_abstract_2d::Get_bin_idx(int ix, int iy){
    if (!inbounds(ny(),nx(),iy,ix))
        return -1;
    return iy*nx()+ix;
}
int    histogram_abstract_2d::Get_bin(double x, double y){
    if (x<lbx||y<lby)
        return -1;
    int xb=(x-lbx)/wx;
    int yb=(y-lby)/wy;
    return Get_bin_idx(xb,yb);
}
Matrixd    histogram_abstract_2d::Get_bin_center(int i){
    Matrixd p(2,1);
    p(0)=Get_x(i);
    p(1)=Get_y(i);
    return p;
}
double histogram_abstract_2d::Get_x(int bin){
    int ix=Get_ix(bin);
    return lbx+wx*(ix+0.5);
}
double histogram_abstract_2d::Get_y(int bin){
    int iy=Get_iy(bin);
    return lby+wy*(iy+0.5);
}
void histogram_abstract_2d::Add(double x, double y){
    int i=Get_bin(x,y);
    if (i>=0)
        Add_sample(i,1);
}
void histogram_abstract_2d::Subtract(double x, double y){
    int i=Get_bin(x,y);
    if (i>=0)
        Subtract_sample(i,1);
}
void histogram_abstract_2d::Add(double x, double y,int n){
    int i=Get_bin(x,y);
    if (i>=0)
        Add_sample(i,n);
}
void histogram_abstract_2d::Subtract(double x, double y,int n){
    int i=Get_bin(x,y);
    if (i>=0)
        Subtract_sample(i,n);
}

double histogram_2d::sample(int &i){
    double sum1,d1,p;
    int n=Get_nbins();
    double ns=Get_nsamples();
    d1=randdbl(ns);            //sample
    sum1=0;
    for (i=0;i<n;i++){
        p=Get_nsamples(i);
        sum1+=p;
        if (d1<=sum1&&p>0){
            return p/ns;
        }
    }
    return 0;
}
histogram_2d&    histogram_2d::operator=(histogram_2d &a){
    if(this!=&a){
        *((Matrix<__int64> *)this)=a;
        nsamples=a.nsamples;
        lbx=a.lbx;lby=a.lby;
        wx=a.wx;wy=a.wy;
    }
    return *this;
}
histogram_2d operator+(histogram_2d &a,histogram_2d &b){
    histogram_2d c(a);
    c+=&b;
    return c;
}
histogram_2d operator-(histogram_2d &a,histogram_2d &b){
    histogram_2d c(a);
    c-=&b;
    return c;
}

double histogram_2d::sample(double &x, double &y){
    int i;
    double p;
    p=sample(i);
    x=Get_x(i);y=Get_y(i);
    return p;
}
void histogram_2d::Add_sample(int bin, int n){
    (*this)(bin)+=n;
    nsamples+=n;
}
void histogram_2d::Subtract_sample(int bin, int n){
    (*this)(bin)-=n;
    nsamples-=n;
}
void    histogram_2d::Add_sample(int binx,int biny, int n){
    (*this)(biny,binx)+=n;
    nsamples+=n;
}
void    histogram_2d::Subtract_sample(int binx,int biny, int n){
    (*this)(biny,binx)-=n;
    nsamples-=n;
}

Matrixd    histogram_2d::Get_normalized(){
    Matrixd M(ny(),nx());
    int i,n=nx()*ny();
    double ns=Get_nsamples();
    for (i=0;i<n;i++){
        M(i)=Data(i)/ns;
    }
    return M;
}

Matrixd histogram_2d::Get_mean(){
    Matrixd sump(2,1);
    sump.Zero();
    int i, n=Get_nbins();
    double f;
    for (i=0;i<n;i++){
        f=Get_freq(i);
        sump=sump+Get_bin_center(i)*f;
    }
    return sump;
}
Matrixd    histogram_2d::Get_var(){
    int i, n=Get_nbins();
    double sumxx,sumxy,sumyy,x,y;
    double f;
    Matrixd mu=Get_mean(),var(2,2),p;
    sumxx=sumxy=sumyy=0;
    for (i=0;i<n;i++){
        f=Get_freq(i);
        p=Get_bin_center(i)-mu;
        x=p(0);y=p(1);
        sumxx+=f*x*x;
        sumxy+=f*x*y;
        sumyy+=f*y*y;
    }
    var(0,0)=sumxx;
    var(0,1)=var(1,0)=sumxy;
    var(1,1)=sumyy;
    return var;
}

//
// histogram_abstract_3d
//

__int64    histogram_abstract_3d::Get_bin(double x, double y, double z){
    int xb=(x-lbx)/wx;
    int yb=(y-lby)/wy;
    int zb=(z-lbz)/wz;
    return Get_bin_idx(xb,yb,zb);
}
Matrixd    histogram_abstract_3d::Get_bin_center(__int64 i){
    Matrixd p(3,1);
    int ix=Get_ix(i);
    int iy=Get_iy(i);
    int iz=Get_iz(i);
    p(0)=lbx+wx*(ix+0.5);
    p(1)=lby+wy*(iy+0.5);
    p(2)=lbz+wz*(iz+0.5);
    return p;
}
void histogram_abstract_3d::Add(double x, double y, double z){
    int i=Get_bin(x,y,z);
    Add_sample(i,1);
}
void histogram_abstract_3d::Subtract(double x, double y, double z){
    int i=Get_bin(x,y,z);
    Subtract_sample(i,1);
}
void histogram_abstract_3d::Add(double x, double y, double z,int n){
    int i=Get_bin(x,y,z);
    Add_sample(i,n);
}
void histogram_abstract_3d::Subtract(double x, double y, double z,int n){
    int i=Get_bin(x,y,z);
    Subtract_sample(i,n);
}
void histogram_abstract_3d::Add(Matrixd &m){
    int i=Get_bin(m(0),m(1),m(2));
    Add_sample(i,1);
}
void histogram_abstract_3d::Subtract(Matrixd &m){
    int i=Get_bin(m(0),m(1),m(2));
    Subtract_sample(i,1);
}


//
// histogram
//
double    histogram::Get_mean(){
    return sumb/Get_nsamples();
}
double    histogram::Get_var(){
    int n=Get_nsamples();
    return (sumbb-sumb*sumb/n)/n;
}
int histogram::Get_nsamples(int i){
    if (i<0){
        if (oob){
            return Data(3);
        }
        else
            return Data(1);
    }
    else{
        int nb=Get_nbins();
        if (i>=nb){
            if (oob){
                return Data(nb+2);
            }
            else{
                return Data(2);
            }
        }
        else
            return Data(i+3);
    }
}

void histogram::Add_sample(int i, int n){
    if (i<0){
        if (oob){
            (*this)(3)+=n;
            (*this)(0)+=n;
        }
        else
            (*this)(1)+=n;
    }
    else{
        int nb=Get_nbins();
        if (i>=nb){
            if (oob){
                (*this)(nb+2)+=n;
                (*this)(0)+=n;
            }
            else
                (*this)(2)+=n;
        }
        else{
            (*this)(i+3)+=n;
            (*this)(0)+=n;
        }
    }
    double x=Get_bin_center(i);
    sumb+=x*n;
    sumbb+=x*x*n;
}
void histogram::Subtract_sample(int i, int n){
    if (i<0)
        if (oob){
            (*this)(3)-=n;
            (*this)(0)-=n;
        }
        else
            (*this)(1)-=n;
    else{
        int nb=Get_nbins();
        if (i>= nb)
            if (oob){
                (*this)(nb+2)-=n;
                (*this)(0)-=n;
            }
            else
                (*this)(2)-=n;
        else{
            (*this)(i+3)-=n;
            (*this)(0)-=n;
        }
    }
    double x=Get_bin_center(i);
    sumb-=x*n;
    sumbb-=x*x*n;
}



//
// histogram_sparse
//

void histogram_sparse::Zero(){
    nsamples=0;
    sumnlogn=0;
    clear();
}

double histogram_sparse::Get_nsamples(int bin){
    map<int,__int64>::iterator mi;
    double d;
    if (bin<0)
        return 0;
    mi=find(bin);
    if (mi!=end()){
        d=(*mi).second;
        return d;
    }
    else{
        return 0;
    }
}

double histogram_sparse::sample(int &i){
    int s=randint(Get_nsamples()),n, sum=0;
    map<int,__int64>::iterator mit,me=end();
    for (mit=begin();mit!=me;++mit){
        if((*mit).first>=0){
            n=(*mit).second;
            if (n>0){
                sum+=n;
                if (s<=sum){
                    i=(*mit).first;
                    return ((double)n)/Get_nsamples();
                }
            }
        }
    }
    return 0;
}

double histogram_sparse::compute_entropy(){
    double sum1=0,d,n=size();
    map<int,__int64>::iterator mit;
    for (mit=begin();mit!=end();++mit){
        if((*mit).first>=0){
            d=((double)(*mit).second)/nsamples;
            if (d>0)
                sum1+=d*log(d);
        }
    }
    return -sum1;
}
#define MULT 1.
double histogram_sparse::entropy(){
    double d=-sumnlogn/(MULT*nsamples)+log(nsamples);
    if (d>=0)
        return d;
    else
        return 0;
}

void histogram_sparse::operator+=(histogram_sparse *a){
    map<int,__int64>::iterator mit;
    if (a->Get_nsamples()==0)
        return;
    for (mit=a->begin();mit!=a->end();++mit){
        Add_sample((*mit).first,(*mit).second);
    }
}
void histogram_sparse::operator-=(histogram_sparse *a){
    map<int,__int64>::iterator mit;
    if (this==a){
        Zero();
        return;
    }
    for (mit=a->begin();mit!=a->end();++mit){
        Subtract_sample((*mit).first,(*mit).second);
    }
}
bool histogram_sparse::operator==(histogram_sparse *a){
    map<int,__int64>::iterator mit,mita;
    if (size()!=a->size())
        return false;
    mit=begin();
    for (mita=a->begin();mit!=end();++mit){
        if (mita==a->end())
            return false;
        if ((*mit).first!=(*mita).first)
            return false;
        if ((*mit).second!=(*mita).second)
            return false;
        ++mita;
    }
    return true;
}

void histogram_sparse::Add_sample(int bin,int n){
    map<int,__int64>::iterator mit;
    if (bin<0)
        return;
    mit=find(bin);
    if (mit==end()){
        insert(map<int,__int64>::value_type(bin,n));
        sumnlogn+=n*log(n)*MULT;
    }
    else{
        sumnlogn-=(*mit).second*log((*mit).second)*MULT;
        (*mit).second+=n;
        sumnlogn+=(*mit).second*log((*mit).second)*MULT;
    }
    nsamples+=n;
}
void histogram_sparse::Subtract_sample(int bin,int n){
    map<int,__int64>::iterator mit;
    if (bin<0)
        return;
    mit=find(bin);
    if (mit==end()){
        insert(map<int,__int64>::value_type(bin,-n));
    }
    else{
        sumnlogn-=(*mit).second*log((*mit).second)*MULT;
        (*mit).second-=n;
        if ((*mit).second==0)
            erase(mit);
        else
            sumnlogn+=(*mit).second*log((*mit).second)*MULT;
    }
    nsamples-=n;
}

void histogram_sparse::Read(char *filename){
    Matrix<__int64> M;

    dlmread(filename,M);
    Load(M);
}
void histogram_sparse::Write(char *filename){
    Matrix<__int64> M;
    Upload(M);
    print(filename,M);
}
void histogram_sparse::Load (Matrix<__int64> &M){
    int i,idx;
    __int64 d;
    Zero();
    nsamples=M(0,0);
    for (i=1;i<M.cols();i++){
        idx=(int) M(0,i);
        d=M(1,i);
        insert(map<int,__int64>::value_type(idx,d));
    }
}
void histogram_sparse::Upload (Matrix<__int64> &M){
    map<int,__int64>::iterator mit;
    M.SetDimension(2,size()+1);
    int i,idx;
    __int64 p;
    M(0,0)=nsamples;
    M(1,0)=0;
    i=1;
    for (mit=begin();mit!=end();++mit){
        idx=(*mit).first;
        p=(*mit).second;
        M(0,i)=idx;
        M(1,i)=p;
        i++;
    }
}

double histogram_sparse::KL(histogram_abstract *h){
    //assume they have the same bins
    double p,q,sum=0.;
    int i;
    map<int,__int64>::iterator mit;

    for (mit=begin();mit!=end();++mit){
        i=(*mit).first;
        p=((double)(*mit).second)/nsamples;
        if (i>=0&&p>0.0){
            q=h->Get_freq(i);
            if (q>0.0001)
                sum += p*log(p/q);
            else
                sum+=p*log(p/0.0001);
        }
    }
    return sum;
}
bool histogram_sparse::less(histogram_abstract *h){
    histogram_sparse::iterator it;
    int n, bin,nh;
    for (it=begin();it!=end();++it){
        n=(*it).second;
        bin=(*it).first;
        nh=h->Get_nsamples(bin);
        if (n>nh){
            n=n;
            return false;
        }
    }
    return true;
}

void graph_hist::Add_edge(int i, int j, int n){
    e[i]->Add(j,n);
    e[j]->Add(i,n);
}
void graph_hist::Subtract_edge(int i, int j, int n){
    e[i]->Add(j,-n);
    e[j]->Add(i,-n);
}
void graph_hist::Check_e(int i){
    int n=e.size();
    while(i>=n){
        e.push_back(new vector_sparse<int>());
        n++;
    }
}

void graph_hist::GetNeighborNodes(int i,vector<int> &L){
    vector_sparse<int>::iterator ei,ee=e[i]->end();
    L.clear();
    for (ei=e[i]->begin();ei!=ee;++ei){
        L.push_back((*ei).first);    
    }
}

void graph_hist_set::Add_edge(int i, int j, int ii, int ij){
    Pointi p(ii, ij),q(ij,ii);
    e[i]->Insert(j,p);
    e[j]->Insert(i,q);
}
void graph_hist_set::Subtract_edge(int i, int j, int ii, int ij){
    Pointi p(ii, ij),q(ij,ii);
    e[i]->Extract(j,p);
    e[j]->Extract(i,q);
}
void graph_hist_set::Check_e(int i){
    int n=e.size();
    while(i>=n){
        e.push_back(new vector_sparse_set<Pointi>());
        n++;
    }
}
set<Pointi> * graph_hist_set::Get_set(int i, int j){
    return (*e[i])(j);
}


graph_hist_edge_iterator graph_hist::e_begin(){
    graph_hist_edge_iterator gi;
    int n=e.size();
    gi.parent=this;
    gi.it=0;
    while(gi.it<n&&e[gi.it]->empty()){
        gi.it++;
    }
    if (gi.it<n)
        gi.jt=e[gi.it]->begin();
    return gi;
}
graph_hist_edge_iterator graph_hist::e_end(){
    graph_hist_edge_iterator gi;
    gi.parent=this;
    gi.it=e.size();
    return gi;
}

graph_hist_edge_iterator & graph_hist_edge_iterator::operator=(const graph_hist_edge_iterator &i){
    if(this!=&i){
        parent=i.parent;
        it=i.it;
        jt=i.jt;
    }
    return *this;
}
bool graph_hist_edge_iterator::operator!=(const graph_hist_edge_iterator &i){
    if (it!=i.it)
        return true;
    else{
        if (it==parent->e.size())
            return false;
        return (jt!=i.jt);
    }
}
void graph_hist_edge_iterator::operator++(){
    int n=parent->e.size();
    if (it>=n)
        return;
    ++jt;
    if (jt==parent->e[it]->end()){
        do{    
            it++;
        } while(it<n&&parent->e[it]->empty());
        if (it<n)
            jt=parent->e[it]->begin();
    }
}

//
// histogram_sparse_3d
//
void    histogram_sparse_3d::Add_b(histogram_sparse_3d *a, Matrixi &bias){
    map<int,__int64>::iterator mit;
    Matrixd x;
    for (mit=a->begin();mit!=a->end();++mit){
        x=a->Get_bin_center((*mit).first);
        histogram_abstract_3d::Add(x(0)-bias(0),x(1)-bias(1),x(2)-bias(2),(*mit).second);
    }
}
void    histogram_sparse_3d::Subtract_b(histogram_sparse_3d *a, Matrixi &bias){
    map<int,__int64>::iterator mit;
    Matrixd x;
    for (mit=a->begin();mit!=a->end();++mit){
        x=a->Get_bin_center((*mit).first);
        histogram_abstract_3d::Subtract(x(0)-bias(0),x(1)-bias(1),x(2)-bias(2),(*mit).second);
    }
}

double  histogram_sparse_3d::sample(Matrixd &x){
    int i;
    double p=histogram_sparse::sample(i);
    x=Get_bin_center(i);
    return p;
}

void    histogram_sparse_3d::Get_eig(Matrixd &l){
    Matrixd U,lambda,V,var=Get_var();
    SVD(var,U,lambda,V);
    l.SetDimension(3,1);
    for (int i=0;i<3;i++){
        l(i)=lambda(i,i);
    }
}

Matrixd histogram_sparse_3d::Get_mean(){
    Matrixd sump(3,1);
    sump.Zero();
    int bin;
    double f,n;
    histogram_sparse::iterator it;
    for (it=begin();it!=end();++it){
        n=(*it).second;
        bin=(*it).first;
        f=n/Get_nsamples();
        sump=sump+Get_bin_center(bin)*f;
    }
    return sump;
}
Matrixd    histogram_sparse_3d::Get_var(){
    int bin;
    double n,sumxx,sumxy,sumxz,sumyy,sumyz,sumzz,x,y,z;
    double f;
    Matrixd mu=Get_mean(),var(3,3),p;
    sumxx=sumxy=sumxz=sumyy=sumyz=sumzz=0;
    histogram_sparse::iterator it;
    for (it=begin();it!=end();++it){
        n=(*it).second;
        bin=(*it).first;
        f=n/Get_nsamples();
        p=Get_bin_center(bin)-mu;
        x=p(0);y=p(1);z=p(2);
        sumxx+=f*x*x;
        sumxy+=f*x*y;
        sumxz+=f*x*z;
        sumyy+=f*y*y;
        sumyz+=f*y*z;
        sumzz+=f*z*z;
    }
    var(0,0)=sumxx;
    var(0,1)=var(1,0)=sumxy;
    var(0,2)=var(2,0)=sumxz;
    var(1,2)=var(2,1)=sumyz;
    var(1,1)=sumyy;
    var(2,2)=sumzz;
    return var;
}

void  Parse_edges(Matrix<int> &L, histogram_sparse &e){
    // L=label map of some regions
    // output= e=edges between the diff labels;
    int nx=L.nx(),ny=L.ny();
    int x,y,l0,l1,nl=L.GetMax();

    e.nbins=nl*nl;e.Zero();
    for (y=0;y<ny;y++)
        for (x=0;x<nx-1;x++){
            l0=L(y,x)-1;
            l1=L(y,x+1)-1;
            if (l0<l1)
                e.Add_sample(l0*nl+l1,1);
            else
                if(l1<l0)
                    e.Add_sample(l1*nl+l0,1);
        }
    for (y=0;y<ny-1;y++)
        for (x=0;x<nx;x++){
            l0=L(y,x)-1;
            l1=L(y+1,x)-1;
            if (l0<l1)
                e.Add_sample(l0*nl+l1,1);
            else
                if(l1<l0)
                    e.Add_sample(l1*nl+l0,1);
        }
}




