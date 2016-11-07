//:
// \file
#include <vcl_cmath.h>
#include <vcl_cstdlib.h> // for std::abs(int)
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_math.h> // for pi
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>
#include <dbctrk/dbctrk_algs.h>
#include <dbctrk/dbctrk_tracker_curve.h>

#include <vnl/algo/vnl_gaussian_kernel_1d.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_det.h>
#include <vnl/vnl_inverse.h>

// smooth a curve by convolving x(s),y(s) with a gaussian filter
void dbctrk_algs::
smooth_curve(vcl_vector<vgl_point_2d<double> > &curve,double sigma)
{
  vnl_gaussian_kernel_1d gauss_1d(sigma);
  curve.insert(curve.begin(),curve[0]);
  curve.insert(curve.begin(),curve[0]);
  curve.insert(curve.begin(),curve[0]);

  curve.insert(curve.end(),curve[curve.size()-1]);
  curve.insert(curve.end(),curve[curve.size()-1]);
  curve.insert(curve.end(),curve[curve.size()-1]);
  double sum=gauss_1d[0];
  for (int i=1;i<gauss_1d.width();i++)
    sum+=2*gauss_1d[i];

  for (unsigned int i=3; i+3<curve.size(); ++i)
  {
    double x=curve[i-3].x()*gauss_1d[3] + curve[i-2].x()*gauss_1d[2]+
             curve[i-1].x()*gauss_1d[1] + curve[i  ].x()*gauss_1d[0]+
             curve[i+1].x()*gauss_1d[1] + curve[i+2].x()*gauss_1d[2]+
             curve[i+3].x()*gauss_1d[3];
    double y=curve[i-3].y()*gauss_1d[3] + curve[i-2].y()*gauss_1d[2]+
             curve[i-1].y()*gauss_1d[1] + curve[i  ].y()*gauss_1d[0]+
             curve[i+1].y()*gauss_1d[1] + curve[i+2].y()*gauss_1d[2]+
             curve[i+3].y()*gauss_1d[3];
    x/=sum;
    y/=sum;
    curve[i].set(x,y);
  }

  curve.erase(curve.begin());
  curve.erase(curve.begin());
  curve.erase(curve.begin());

  curve.erase(curve.end()-1);
  curve.erase(curve.end()-1);
  curve.erase(curve.end()-1);

}

vdgl_digital_curve_sptr  dbctrk_algs::
create_digital_curves(vcl_vector<vgl_point_2d<double> > & curve)
{
  vdgl_edgel_chain_sptr vec;
  vec= new vdgl_edgel_chain;
  for (unsigned int j=0; j<curve.size(); ++j)
  {
   vdgl_edgel el(curve[j].x(),curve[j].y(), 0,0 );
   vec->add_edgel(el);
  }
  vdgl_interpolator_sptr interp= new vdgl_interpolator_linear(vec);
  vdgl_digital_curve_sptr dc = new vdgl_digital_curve(interp);
  return dc;
}

vdgl_digital_curve_sptr  dbctrk_algs::
create_digital_curves(Curve * c)
{
  vdgl_edgel_chain_sptr vec;
  vec= new vdgl_edgel_chain;
  for (int j=0; j<c->numPoints(); ++j)
  {
   vdgl_edgel el(c->x(j),c->y(j), 0,0 );
   vec->add_edgel(el);
  }
  vdgl_interpolator_sptr interp= new vdgl_interpolator_linear(vec);
  vdgl_digital_curve_sptr dc = new vdgl_digital_curve(interp);
  return dc;
}
double  dbctrk_algs::compute_transformed_euclidean_distance
                      (dbctrk_tracker_curve_sptr c1,
                       dbctrk_tracker_curve_sptr c2,
                       vnl_matrix<double> R,
                       vnl_matrix<double> T,
                       double /*s*/,vcl_map<int,int> alignment)

{
  if(!c1.ptr() && !c2.ptr())
    return -1;
  vcl_vector<double> x1,y1,x2,y2,x1t,y1t;
  double xcen1=0,xcen2=0,ycen1=0,ycen2=0;
    double H[2]={0,0};
  vcl_map<int,int>::iterator iter1;
  vcl_cout<<"\n"<<c1->desc->curve_->numPoints()<<"\t"<<c2->desc->curve_->numPoints();
  for(iter1 = alignment.begin(); iter1!=alignment.end(); ++iter1)
  {
     x1.push_back(c1->desc->curve_->point((*iter1).first).x());
     y1.push_back(c1->desc->curve_->point((*iter1).first).y());
     x2.push_back(c2->desc->curve_->point((*iter1).second).x());
      y2.push_back(c2->desc->curve_->point((*iter1).second).y());
  }
#if 0
//this variable is not defined.  And x1.size would be unsigned int.  PLEASE FIX!  -MM
  int t=x1.size();
#endif
  for(unsigned j=0;j<x1.size()-1;j++)
  {
      xcen1+=x1[j];
      ycen1+=y1[j];
      xcen2+=x2[j];
      ycen2+=y2[j];
  }
  xcen1/=alignment.size();
  ycen1/=alignment.size();
  xcen2/=alignment.size();
  ycen2/=alignment.size();

  for(unsigned i=0;i<x1.size();i++)
  {
  x1[i]-=xcen1;
  y1[i]-=ycen1;
  //x2[i]-=xcen2;
  //y2[i]-=ycen2;
  }
  double dist=0;
  double X2[2]={0,0};
  double X1cen[2]={xcen1,ycen1};
  for (unsigned i=0;i<x1.size();i++)
  {
    H[0]=x1[i];
    H[1]=y1[i];
  X2[0]=x2[i];
  X2[1]=y2[i];

  vnl_matrix<double> X (H, 2, 1);
  vnl_matrix<double> X2t (X2, 2, 1);
   
  vnl_matrix<double> X1center(X1cen,2,1);
  vnl_matrix<double> Xt=R*X+T+X1center-X2t;
    vcl_cout<<"\n"<<X2t(0,0)<<"\t"<<X2t(1,0)<<"\t"<<X(0,0)+X1center(0,0)<<"\t"<<X(1,0)+X1center(0,0);
    dist+=vcl_sqrt(Xt(0,0)*Xt(0,0)+Xt(1,0)*Xt(1,0));
  }
  dist/=alignment.size();
  return dist;

  }


bool dbctrk_algs::compute_transformation(vcl_vector<vgl_point_2d<double> > curve,
                              vcl_vector<vgl_point_2d<double> > & transformed_curve,
                              vnl_matrix<double> R,vnl_matrix<double> T,double scale)
{
  if(curve.size()<=0)
    return false;

  transformed_curve.clear();

  double xcen=0,ycen=0;

  for(unsigned i=0;i<curve.size();i++)
  {
    xcen+=curve[i].x();
    ycen+=curve[i].y();
  }
  xcen=xcen/curve.size();
  ycen=ycen/curve.size();

  for(unsigned i=0;i<curve.size();i++)
  {  
    vnl_double_2 X (curve[i].x()-xcen,curve[i].y()-ycen);
    vnl_double_2 Xt=R*X;

    vgl_point_2d<double> point(Xt[0]+xcen+T(0,0),Xt[1]+ycen+T(1,0));
    transformed_curve.push_back(point);
     }
return true;  
}


bool dbctrk_algs::compute_transformation_next(vcl_vector<vgl_point_2d<double> > curve,
                              vcl_vector<vgl_point_2d<double> > & transformed_curve,
                              vnl_double_2x2 R,vnl_matrix<double> T,double scale)
{
  if(curve.size()<=0)
    return false;

  transformed_curve.clear();
  vnl_double_2x2 Rp;

  if(vnl_det<double>(R)==0)
    return false;
  else
  {
    Rp=vnl_inverse(R);
  }
  double xcen=0,ycen=0;

  for(unsigned i=0;i<curve.size();i++)
  {
    xcen+=curve[i].x();
    ycen+=curve[i].y();
  }
  xcen=xcen/curve.size();
  ycen=ycen/curve.size();

  for(unsigned i=0;i<curve.size();i++)
  {  
    vnl_double_2 X (curve[i].x()-xcen,curve[i].y()-ycen);
    vnl_double_2 Xt=Rp*X;

    vgl_point_2d<double> point(Xt[0]+xcen-T(0,0),Xt[1]+ycen-T(1,0));
    transformed_curve.push_back(point);
     }
  return true;          
}
