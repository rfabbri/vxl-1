#include "dbrl_support.h"
#include <vnl/vnl_math.h>
#include <vcl_cmath.h>
#include <dbgl/algo/dbgl_eulerspiral.h>

double dbrl_support::support(int i,int j,double angledeg)
    {
    assert(img_.ni()>=0 && img_.nj()>=0);
    assert(i>=0 && j>=0 && i<static_cast<int>(img_.ni()) && j<static_cast<int>(img_.nj()));
    double cocircularitysupport=0.0;
    //double smoothnessupport    =0.0;
    //double proximitysupport    =0.0;

    double anglerad=2*angledeg*vnl_math::pi/180.0;

    for(int m=i-radius_;m<=i+radius_;m++)
        {
        if(m>=0 && m<static_cast<int>(img_.ni()) && m!=i)
            {
            for(int n=j-radius_;n<=j+radius_;n++)
                {
                if(n>=0 && n<static_cast<int>(img_.nj()) && n!=j)
                    {
                    if(img_(m,n)!=255)
                        {

                        double lambdap=compute_lambdap(i,j,m,n,anglerad);
                        double lambda0=compute_lambda0(i,j,m,n);
                        cocircularitysupport+=cocircularity(lambdap,lambda0)*smoothness(lambdap,lambda0)*proximity(i,j,m,n);
                        /*smoothnessupport    +=;
                        proximitysupport    +=;*/
                        }
                    }
                }
            }
        }
    return cocircularitysupport;//*smoothnessupport*proximitysupport;
    }


double dbrl_support::compute_lambdap(int i,int j,int i0,int j0,double angle)
    {
        double lambdap=vcl_acos((-(double)(j0-j)*vcl_sin(angle)+((double)(i0-i))*vcl_cos(angle))/vcl_sqrt((double)(j-j0)*(double)(j-j0)+(double)(i-i0)*(double)(i-i0)));
        return lambdap;
    }
double dbrl_support::compute_lambda0(int i,int j,int i0,int j0)
    {
        double angle=2*(double)img_(i0,j0)*vnl_math::pi/180.0;
        double lambda0= vcl_acos((-(double)(j-j0)*vcl_sin(angle)+(double)(i-i0)*vcl_cos(angle))/vcl_sqrt(((double)(j-j0))*((double)(j-j0))+((double)(i-i0))*((double)(i-i0))));
        return lambda0;
    }

double dbrl_support::cocircularity(double lambdap,double lambda0)
    {
    double del=vcl_fabs(lambdap+lambda0-vnl_math::pi);
    double c=(1-del*del/(vnl_math::pi*vnl_math::pi))*vcl_exp(-del*del/(2*(vnl_math::pi/4)*(vnl_math::pi/4)));
    return c;
    }
double dbrl_support::smoothness(double lambdap,double lambda0)
    {
    double csmooth=(1-lambdap*(vnl_math::pi-lambdap)/(vnl_math::pi*vnl_math::pi))*(1-lambda0*(vnl_math::pi-lambda0)/(vnl_math::pi*vnl_math::pi));
    return csmooth;
    }
double dbrl_support::proximity(int i,int j, int i0,int j0)
    {
        double d2=((double)(j-j0))*((double)(j-j0))+((double)(i-i0))*((double)(i-i0));
        return vcl_exp(-d2/((double)radius_*radius_));
    }



double compute_support(double x0,double y0,double theta0,double x1,double y1,double theta1,double stdrad_)
    {
        //: compute lambda1 and lambda0
        double a=0.75;
        double angle0=theta0;//*vnl_math::pi/180.0;
        double lambda0=vcl_acos(((y1-y0)*vcl_sin(angle0)+(x1-x0)*vcl_cos(angle0))/vcl_sqrt((y0-y1)*(y0-y1)+(x0-x1)*(x0-x1)));

        double angle1=theta1;//*vnl_math::pi/180.0;
        double lambda1=vcl_acos(((y0-y1)*vcl_sin(angle1)+(x0-x1)*vcl_cos(angle1))/vcl_sqrt((y0-y1)*(y0-y1)+(x0-x1)*(x0-x1)));

        double del=vcl_fabs(lambda1+lambda0-vnl_math::pi);
        double c=(1-del*del/(vnl_math::pi*vnl_math::pi));//*vcl_exp(-del*del/(2*(vnl_math::pi/8)*(vnl_math::pi/8)));

        //double csmooth=(1-lambda1*(vnl_math::pi-lambda1)/(vnl_math::pi*vnl_math::pi))*(1-lambda0*(vnl_math::pi-lambda0)/(vnl_math::pi*vnl_math::pi));

        double csmooth=vcl_fabs(1-lambda1/(vnl_math::pi/2))*vcl_fabs(1-lambda0/(vnl_math::pi/2));
        double dist=(y0-y1)*(y0-y1)+(x0-x1)*(x0-x1);
        double d2=vcl_exp(-(dist)/(2*(stdrad_*stdrad_)));

        //: to exclude very close edges 
        //if(dist<a*a)
        //   return 0;
        return c*csmooth*d2;
    }

double compute_euler_spiral(double x0,double y0,double theta0,double x1,double y1,double theta1)
    {
    //    /*vgl_point_2d<double> p0(x0,y0);
    //    vgl_point_2d<double> p1(x1,y1);*/

    //    //compute scaling distance
    //    vgl_vector_2d< double > v(x1-x0, y1-y0);
    //    double d = v.length();
  
    //    // psi is the angle of line from start point to end point
    //    double psi = vcl_atan2(v.y(), v.x());
    //    if (psi < 0)
    //        psi += vnl_math::pi * 2;

    //    double k0, gamma, len, k0_max_error, gamma_max_error, len_max_error;

    //    theta0*=(vnl_math::pi/180);
    //    theta1*=(vnl_math::pi/180);
    //    dbgl_eulerspiral_lookup_table::instance()->look_up(
    //        theta0-psi, theta1-psi, &k0, &gamma, &len, 
    //        &k0_max_error, &gamma_max_error, &len_max_error );

    //    k0/=d; gamma/=(d*d);

    //    double e1=gamma*gamma*len;


    //    double e1=gamma*gamma*len*len +
    //    theta0+=(vnl_math::pi);
    //    
    //    dbgl_eulerspiral_lookup_table::instance()->look_up(
    //        theta0-psi, theta1-psi, &k0, &gamma, &len, 
    //        &k0_max_error, &gamma_max_error, &len_max_error );

    //    k0/=d; gamma/=(d*d);

    //    double e2=gamma*gamma*len;

    //    if(e1<e2 )
    //        if(e1>100 || e1<0)
    //            return 100;
    //        else
    //            return e1;
    //    if(e2<e1 )
    //        if(e2>100 || e2<0)
    //            return 100;
    //        else
    //            return e2;

        theta0*=(vnl_math::pi/180);
        theta1*=(vnl_math::pi/180);


        dbgl_eulerspiral e1(vgl_point_2d<double>(x0,y0),theta0,
            vgl_point_2d<double>(x1,y1),theta1);

        double len1=e1.length();
        double energy1;
        if(len1<20 && len1>0)
        {
            double kavg1=(e1.curvature_at(1.0)+e1.curvature_at(0))/2;
            double gamma1=e1.gamma();
            energy1=(gamma1*gamma1*len1*len1+kavg1*kavg1*len1*len1+1)*len1;
            vcl_cout<<e1.curvature_at(1.0)<<"\t"<<e1.curvature_at(0.0)<<"\t"<<energy1<<"\n";
        }
        else
            energy1=10000;

        dbgl_eulerspiral e2(vgl_point_2d<double>(x0,y0),theta0+vnl_math::pi,
            vgl_point_2d<double>(x1,y1),theta1);

        double len2=e2.length();
        double energy2;
        if(len2<20 && len2>0)
        {
            double kavg2=(e2.curvature_at(1)+e2.curvature_at(0))/2;
            double gamma2=e2.gamma();
            energy2=(gamma2*gamma2*len2*len2+kavg2*kavg2*len2*len2+1)*len2;
            vcl_cout<<e2.curvature_at(1.0)<<"\t"<<e2.curvature_at(0.0)<<"\t"<<energy2<<"\n";
        }
        else
        {
            energy2=10000;
        }


        if(energy1<energy2)
        {
            vcl_cout<<"En1= "<<vcl_exp(-energy1/60)<<"\n";
            return vcl_exp(-energy1/100);
        }
        else{
            vcl_cout<<"En2= "<<vcl_exp(-energy2/60)<<"\n";
            return vcl_exp(-energy2/100);
        }
        
    }
