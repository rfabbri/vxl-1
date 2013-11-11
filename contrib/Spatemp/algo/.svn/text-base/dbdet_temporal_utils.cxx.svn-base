#include "dbdet_temporal_utils.h"

#include <vgl/vgl_vector_2d.h>
#include <vcl_cmath.h>
#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/sel/dbdet_curvelet.h>
#include <dbgl/algo/dbgl_closest_point.h>

double 
dbdet_temporal_utils::perp_distance(vgl_vector_2d<double> p, vgl_vector_2d<double> n)
{
    return vcl_fabs(p.x()*n.y()-p.y()*n.x());
}
bool 
dbdet_temporal_utils::compute_beta(dbdet_edgel* e, dbdet_curvelet * c, double & beta)
{
    double x1=e->pt.x();
    double y1=e->pt.y();
    double theta1=e->tangent;

    double n1x=-vcl_sin(theta1);
    double n1y=vcl_cos(theta1);


    double x2=c->ref_edgel->pt.x();
    double y2=c->ref_edgel->pt.y();
    double k2;
    double theta2;

    double s2max=c->spos;
    double s2min=c->sneg;

    if(dbdet_CC_curve_model * ccmodel2=dynamic_cast<dbdet_CC_curve_model *> (c->curve_model))
    {
        k2=ccmodel2->k;
        theta2=ccmodel2->theta;
    }
    else if(dbdet_CC_curve_model_3d * ccmodel2=dynamic_cast<dbdet_CC_curve_model_3d *> (c->curve_model))
    {
        k2=ccmodel2->k;
        theta2=ccmodel2->theta;
    }
    else
        return false;

    //: TODO get a reasonable value.
    //if(vcl_fabs(k1-k2)>vcl_fabs(k1))
    //    return false;

    double theta_t=vcl_fmod(vcl_fabs(theta1-theta2),(2*vnl_math::pi));
    double lb=1;
    //if(theta_t>lb)
    //    return false;

    double n2x=-vcl_sin(theta2);
    double n2y=vcl_cos(theta2);

    double t2x=vcl_cos(theta2);
    double t2y=vcl_sin(theta2);

    if(vcl_fabs(k2)>0.001 )
    {
        double s1= (-(n1y*t2x-n1x*t2y)+vcl_sqrt((n1y*t2x-n1x*t2y)*(n1y*t2x-n1x*t2y)+2*k2*(n1y*n2x-n1x*n2y)*(n1y*(x1-x2)-n1x*(y1-y2))))/(k2*(n1y*n2x-n1x*n2y));
        double s2= (-(n1y*t2x-n1x*t2y)-vcl_sqrt((n1y*t2x-n1x*t2y)*(n1y*t2x-n1x*t2y)+2*k2*(n1y*n2x-n1x*n2y)*(n1y*(x1-x2)-n1x*(y1-y2))))/(k2*(n1y*n2x-n1x*n2y));

        if(s1>-s2min && s1<s2max)
        {
            if(vcl_fabs(n1x)>0.5)
                beta=(x2-x1 +t2x*s1+0.5*k2*n2x*s1*s1)/n1x;
            else    
                beta=(y2-y1 +t2y*s1+0.5*k2*n2y*s1*s1)/n1y;
            return true;
        }
        else if(s2>-s2min && s2<s2max)
        {
            if(vcl_fabs(n1x)>0.5)
                beta=(x2-x1 +t2x*s2+0.5*k2*n2x*s2*s2)/n1x;
            else    
                beta=(y2-y1 +t2y*s2+0.5*k2*n2y*s2*s2)/n1y;
            return true;
        }
        else
            return false;
    }
    else
    {
        double s=(n1y*(x1-x2)-n1x*(y1-y2))/(n1y*t2x-n1x*t2y);
        if(s>-s2min && s<s2max)
        {
            if(vcl_fabs(n1x)>0.5)
                beta=(x2-x1+t2x*s)/n1x;
            else    
                beta=(y2-y1+t2y*s)/n1y;
            return true;
        }
        else
            return false;
    }


}

bool 
dbdet_temporal_utils::compute_backward_beta(dbdet_curvelet * c,dbdet_edgel* e,  double & beta, vgl_vector_2d<double> & p1)
{
    double x2=c->ref_edgel->pt.x();
    double y2=c->ref_edgel->pt.y();
    
    double theta2=c->ref_edgel->tangent;
    double k2; 

    double s2max=c->spos;
    double s2min=c->sneg;

    double x1=e->pt.x();

    if(dbdet_CC_curve_model * ccmodel2=dynamic_cast<dbdet_CC_curve_model *> (c->curve_model))
        k2=ccmodel2->k;
    else if(dbdet_CC_curve_model_3d * ccmodel2=dynamic_cast<dbdet_CC_curve_model_3d *> (c->curve_model))
        k2=ccmodel2->k;
    else 
        return false;

    vgl_vector_2d<double> t0(vcl_cos(theta2),vcl_sin(theta2));
    vgl_vector_2d<double> n0(-vcl_sin(theta2),vcl_cos(theta2));

    double arclength=-1e5;
    double dist=dbgl_closest_point::point_to_circle(e->pt,c->ref_edgel->pt,t0,k2,arclength);


    if(arclength<s2max && arclength>-s2min)
    {
        p1=c->ref_edgel->pt+t0*arclength+0.5*k2*arclength*arclength*n0 - e->pt;

        if(p1.x()*n0.x()+p1.y()*n0.y()>0)
            beta=-dist;
        else
            beta=dist;
    }
    else
        return false;

    return true;
}



bool 
dbdet_temporal_utils::cartesian2spherical(double x, double y, double z, double & theta, double & phi,double & r)
{
    r=vcl_sqrt(x*x+y*y+z*z);
    phi=atan2(y,x);
    theta=asin(z/r);
    return true;
}
