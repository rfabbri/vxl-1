#include "tube.h"
#include <vcl_cmath.h> // for vcl_sqrt()
void tube::init()
{
//  

}


vnl_vector<int> tube::search(vcl_vector<vgl_point_3d<double> > pts, vgl_point_3d <double> p0) 
{
  //on the xz plane....
  e11_.set(p0.x(),0.0,p0.z());
  e12_.set(p0.x()+t1_.x(),0.0,p0.z()+t1_.z());    //t1_ ~t4_ are vectors for 4 tube arms..
  
  e21_.set(e11_.x(),e11_.y(),e11_.z());
  e22_.set(p0.x()+t2_.x(),0.0,p0.z()+t2_.z());

  e31_.set(e11_.x(),e11_.y(),e11_.z());
  e32_.set(p0.x()+t3_.x(),0.0,p0.z()+t3_.z());

  e41_.set(e11_.x(),e11_.y(),e11_.z());
  e42_.set(p0.x()+t4_.x(),0.0,p0.z()+t4_.z());

  //e11_.x()=p0.x(); e11_.y()=p0.z();
  //e12_.x()=p0.x()-t1_.x(); e12_.y()=p0.z()-t1_.z();

  double px[4],py[4];
  double t1x=e11_.x()-e12_.x();  
  double t1y=e11_.y()-e12_.y();
  t1x=t1_.x(); t1y=t1_.z(); //same as definition...
  
  double a1=radius1_*length1_/(2.0*sqrt(t1x*t1x+t1y*t1y));

  px[0]=e11_.x()+( a1/2.0)*t1y; py[0]= e11_.z()+(-a1/2.0)*t1x;
  px[1]=e11_.x()+(-a1/2.0)*t1y; py[1]= e11_.z()+( a1/2.0)*t1x;

  px[3]=e12_.x()+( a1/2.0)*t1y; py[3]= e12_.z()+(-a1/2.0)*t1x;
  px[2]=e12_.x()+(-a1/2.0)*t1y; py[2]= e12_.z()+( a1/2.0)*t1x;

  for (unsigned j=0;j<4;j++)
  ////vcl_cout<<px[j]<<" "<<py[j]<<vcl_endl;
  ////vcl_cout<<vcl_endl;
  pol1_=vgl_polygon<double> ( px,py, 4);
///////////////////////////////////////////////////

  double t2x=e21_.x()-e22_.x();
  double t2y=e21_.y()-e22_.y();
  t2x=t2_.x(); t2y=t2_.z();
  
  double a2=radius2_*length2_/(2.0*sqrt(t2x*t2x+t2y*t2y));

  px[0]=e21_.x()+( a2/2.0)*t2y; py[0]= e21_.z()+(-a2/2.0)*t2x;
  px[1]=e21_.x()+(-a2/2.0)*t2y; py[1]= e21_.z()+( a2/2.0)*t2x;

  px[3]=e22_.x()+( a2/2.0)*t2y; py[3]= e22_.z()+(-a2/2.0)*t2x;
  px[2]=e22_.x()+(-a2/2.0)*t2y; py[2]= e22_.z()+( a2/2.0)*t2x;

  pol2_=vgl_polygon<double> ( px,py, 4);
  for (unsigned j=0;j<4;j++)
  ////vcl_cout<<px[j]<<" "<<py[j]<<vcl_endl;
  ////vcl_cout<<vcl_endl;
////////////////////////////////////////////////////

  double t3x=e31_.x()-e32_.x();
  double t3y=e31_.y()-e32_.y();
  t3x=t3_.x(); t3y=t3_.z();
  
  double a3=radius3_*length3_/(2.0*sqrt(t3x*t3x+t3y*t3y));

  px[0]=e31_.x()+( a3/2.0)*t3y; py[0]= e31_.z()+(-a3/2.0)*t3x;
  px[1]=e31_.x()+(-a3/2.0)*t3y; py[1]= e31_.z()+( a3/2.0)*t3x;

  px[3]=e32_.x()+( a3/2.0)*t3y; py[3]= e32_.z()+(-a3/2.0)*t3x;
  px[2]=e32_.x()+(-a3/2.0)*t3y; py[2]= e32_.z()+( a3/2.0)*t3x;

  pol3_=vgl_polygon<double> ( px,py, 4);
  for (unsigned j=0;j<4;j++)
  ////vcl_cout<<px[j]<<" "<<py[j]<<vcl_endl;
  ////vcl_cout<<vcl_endl;
/////////////////////////////////////////////////////

  double t4x=e41_.x()-e42_.x();
  double t4y=e41_.y()-e42_.y();
  t4x=t4_.x(); t4y=t4_.z();
  
  double a4=radius4_*length4_/(2.0*sqrt(t4x*t4x+t4y*t4y));

  px[0]=e41_.x()+( a4/2.0)*t4y; py[0]= e41_.z()+(-a4/2.0)*t4x;
  px[1]=e41_.x()+(-a4/2.0)*t4y; py[1]= e41_.z()+( a4/2.0)*t4x;

  px[3]=e42_.x()+( a4/2.0)*t4y; py[3]= e42_.z()+(-a4/2.0)*t4x;
  px[2]=e42_.x()+(-a4/2.0)*t4y; py[2]= e42_.z()+( a4/2.0)*t4x;

  pol4_=vgl_polygon<double> ( px,py, 4);
 for (unsigned j=0;j<4;j++)
  ////vcl_cout<<px[j]<<" "<<py[j]<<vcl_endl;
  ////vcl_cout<<vcl_endl;
  ////////////////////////////////////////////////////
  
  int count1=0; int count2=0; int count3=0; int count4=0;

  for (unsigned i=0;i<pts.size();i++) {
    ////vcl_cout<<pts[i].x()<<" "<<pts[i].z()<<vcl_endl;
    if (use1_ && pol1_.contains(pts[i].x(),pts[i].z()) )
    count1++;
    if (use2_ && pol2_.contains(pts[i].x(),pts[i].z()) )
    count2++;
    if (use3_ && pol3_.contains(pts[i].x(),pts[i].z()) )
    count3++;
    if (use4_ && pol4_.contains(pts[i].x(),pts[i].z()) )
    count4++;
  }


  
 vnl_vector <int> ret(5,0);
 ret(0)=count1;
 ret(1)=count2;
 ret(2)=count3;
 ret(3)=count4;
 ret(4)=count1+count2+count3+count4;
  

  return ret;

}


void tube::ranger(vcl_vector<vgl_point_3d<double> > pts)
{

  for (unsigned i=0;i<pts.size();i++) {
    ////vcl_cout<<pts[i]<<vcl_endl;
    vnl_vector <int> ans=search(pts,pts[i]);
    ////vcl_cout <<ans << vcl_endl;
  }

}



/*
void tube::check(vcl_vector<vgl_point_3d<double> > pts)
{
  vnl_vector <int> max_linear(5,0);
  vnl_vector <int> max_square(5,0);
  vgl_point_3d<double>  max_lin_point;
  vgl_point_3d<double>  max_square_point;
  int max_lin_index=-1,max_square_index=-1;

  for (unsigned i=0;i<pts.size();i++) {
   // vcl_cout<<pts[i]<<vcl_endl;
    vnl_vector <int> ans=search(pts,pts[i]);
   // vcl_cout <<ans << vcl_endl;
    if (ans[4]>max_linear[4]) {
      max_linear=ans;
      max_lin_index=i;
      max_lin_point=pts[i];
    }

    if (ans[0]*ans[0]+ans[1]*ans[1]+ans[2]*ans[2]+ans[3]*ans[3]>
      max_square[0]*max_square[0]+max_square[1]*max_square[1]+
      max_square[2]*max_square[2]+max_square[3]*max_square[3])
    {
      max_square=ans;
      max_square_index=i;
      max_square_point=pts[i];
    }

  }

  vcl_cout<<"max_lin"<<max_linear<<" "<<max_lin_point<<vcl_endl;
  vcl_cout<<"max_sqr"<<max_square<<" "<<max_square_point<<vcl_endl;
}
*/
