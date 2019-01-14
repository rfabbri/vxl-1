#include <iostream>
#include <cassert>
#include <cstdlib>
#include <utility>

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_matrix.h>
#include <dt/partial_derivatives.h>
#include <vnl/vnl_random.h>

int compare(const void * x1, const void *x2)
{
    return ( ((std::pair<int,double>*)x1)->second - ((std::pair<int,double>*)x2)->second );
}
bool compute_partial_derivatives(int n,double *x, double *y, double *z,double *fx, double *fy)
{
    int N=9;
    assert(N<n);
    vnl_random randomgen;
    //fx=new double[n];
    //fy=new double[n];
    for (int i=0;i<n;i++)
    {
        std::pair<int,double> * distances=new std::pair<int,double>[n-1] ;
        for(int j=0,cnt=0;j<n;j++)
        {
            if(i!=j)
            {
                double dist=std::sqrt((x[i]-x[j])*(x[i]-x[j])+(y[i]-y[j])*(y[i]-y[j]));
                distances[cnt]=(std::make_pair(j,dist));
                cnt++;
            }
            
            
        }
        qsort(distances,n-1,sizeof(std::pair<int,double>),compare);
        double Ri=distances[N].second;
        double *Wi=new double[N];
        for(int j=0;j<N;j++)
        {
            Wi[j]=1/distances[j].second-1/Ri+1e-5;
        }
        vnl_matrix<double> A(N, 5);
        vnl_vector<double> b(N);
        for(unsigned k=0;k<A.rows();k++)
        {
            A(k,0)=Wi[k]*(x[distances[k].first]-x[i])*(x[distances[k].first]-x[i])+ randomgen.drand32(0.0,1e-4);
            A(k,1)=Wi[k]*(x[distances[k].first]-x[i])*(y[distances[k].first]-y[i])+ randomgen.drand32(0.0,1e-4);
            A(k,2)=Wi[k]*(y[distances[k].first]-y[i])*(y[distances[k].first]-y[i])+ randomgen.drand32(0.0,1e-4);
            A(k,3)=Wi[k]*(x[distances[k].first]-x[i])+ randomgen.drand32(0.0,1e-4);
            A(k,4)=Wi[k]*(y[distances[k].first]-y[i])+ randomgen.drand32(0.0,1e-4);
            b[k]=Wi[k]*(z[distances[k].first]-z[i])+ randomgen.drand32(0.0,1e-4);
        }

        vnl_svd<double> svd(A);
        vnl_vector<double> u=svd.solve(b);

        //std::cout<<A;
        ////std::cout<<b;
        ////std::cout<<svd.W();
        //std::cout<<"("<<x[i]<<","<<y[i]<<","<<z[i]<<")   ("<<u[3]<<" "<<u[4]<<")\n";
        //std::cout<<"\n rank is "<<svd.rank();

        fx[i]=u[3];
        fy[i]=u[4];
    }

    return true;
}

bool compute_normal_derivatives(int n,XYZ p[], ITRIANGLE v[], int &ntri)
{
    for(int i=0;i<ntri;i++)
    {
        
        XYZ a=p[v[i].p1];
        XYZ b=p[v[i].p2];
        XYZ c=p[v[i].p3];
        //: edge1
        double fabn;
        double fbcn;
        double fcan;

        double vabx=a.x-b.x;
        double vaby=a.y-b.y;
        
        vabx/=std::sqrt(vabx*vabx+vaby*vaby);
        vaby/=std::sqrt(vabx*vabx+vaby*vaby);

        double vacx=a.x-c.x;
        double vacy=a.y-c.y;

        if(vabx*vacy-vaby*vacx<0)
            fabn=(a.fx+b.fx)*vaby/2-(a.fy+b.fy)*vabx/2;
        else
            fabn=-(a.fx+b.fx)*vaby/2+(a.fy+b.fy)*vabx/2;

        double vbcx=b.x-c.x;
        double vbcy=b.y-c.y;

        double vbax=b.x-a.x;
        double vbay=b.y-a.y;

        vbcx/=std::sqrt(vbcx*vbcx+vbcy*vbcy);
        vbcy/=std::sqrt(vbcx*vbcx+vbcy*vbcy);

        if(vbcx*vbay-vbcy*vbax<0)
            fbcn=(b.fx+c.fx)*vbcy/2-(b.fy+c.fy)*vbcx/2;
        else
            fbcn=-(b.fx+c.fx)*vbcy/2+(b.fy+c.fy)*vbcx/2;

        double vcax=c.x-a.x;
        double vcay=c.y-a.y;

        double vcbx=c.x-b.x;
        double vcby=c.y-b.y;

        vcax/=std::sqrt(vcax*vcax+vcay*vcay);
        vcay/=std::sqrt(vcax*vcax+vcay*vcay);

        if(vcax*vcby-vcay*vcbx<0)
            fcan=(a.fx+c.fx)*vcay/2-(a.fy+c.fy)*vcax/2;
        else
            fcan=-(a.fx+c.fx)*vcay/2+(a.fy+c.fy)*vcax/2;

        //a.out();
        //b.out();
        //c.out();
        //std::cout<<" "<<fabn<<" "<<fbcn<<" "<<fcan<<"\n";

        v[i].f12n=fabn;
        v[i].f23n=fbcn;
        v[i].f31n=fcan;
    }
    return true;
}
