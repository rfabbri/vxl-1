
#include <testlib/testlib_test.h>
#include <vgl/vgl_convex.h>
#include <iostream>
#include <vnl/vnl_matrix.h>
#include "tube.h"
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_reg_exp.h>
#include <cstring>
#include <string>
#include "sub.h"
#include "recog.h"

#define CERR false
//
float main3_rev(int argc, std::vector <std::string> argv, vnl_vector <float>V)
{
  ///##comented out 10-2-2006##/// std::cout << "Test dtube\n";

  vgl_point_3d <double> pts0(1,0,0);

  ///##comented out 10-2-2006##/// std::cout << "Inputs\n";
  // Simple triangle
  std::vector<vgl_point_3d<double> > pts;
  pts.push_back( vgl_point_3d<double>(0.0, 0, 0.0) );
  pts.push_back( vgl_point_3d<double>(3.0, 0, .74) );
  pts.push_back( vgl_point_3d<double>(3.0, 0, .75) );
  pts.push_back( vgl_point_3d<double>(3.0, 0, 0.0) );
  pts.push_back( vgl_point_3d<double>(2.0, 0, 1.0) );
  pts.push_back( vgl_point_3d<double>(0.0, 0, 5.0) );

  //for (unsigned i=0; i < pts.size(); ++i)
  //  ///##comented out 10-2-2006##/// std::cout << '(' << pts[i].x() <<','<<pts[i].z()<<") ";
 // ///##comented out 10-2-2006##/// std::cout << std::endl;

  vgl_point_3d <double> e12(1,0,3);
  vgl_point_3d <double> e22(4,0,3);
  vgl_point_3d <double> e32(1,0,3);
  vgl_point_3d <double> e42(1,0,3);
  //double radius= 1.5; double length=3.0;
  double radius= .2; double length=9.0;

  vgl_vector_3d<double> t1(-1,0,0.4);
  vgl_vector_3d<double> t2(1,.5,-1);
  vgl_vector_3d<double> t3(0,0,3);
  vgl_vector_3d<double> t4(0,0,3);

  //tube at(pts0,e12,e22,e32,e42,radius,length,1,1,0,0);
   tube aT(pts0,t1,t2,t3,t4,radius,length,1,0,0,0);
  aT.set_length(9.0);

 // aT.ranger(pts);
  /*vgl_polygon<double> poly=vgl_convex_hull(pts);
  ///##comented out 10-2-2006##/// std::cout << "Outputs\n";
  poly.print(///##comented out 10-2-2006##/// std::cout);
  ///##comented out 10-2-2006##/// std::cout << std::endl;

  TEST("inside", poly.contains( pts[0] ), true );
  TEST("inside", poly.contains( pts[1] ), true );
  TEST("inside", poly.contains( pts[3] ), true );
  TEST("inside", poly.contains( pts[4] ), true );*/



  //=========================== load the files ==========================

  /////////////////////////////////////////
  /////////////////////////////////////////
  std::vector<vgl_point_3d<double> >ptl;
  std::vector<vgl_point_3d<double> >pts_f;
  std::vector<vgl_point_3d<double> >pts_r;
  //read_wrl_file(ptl,"C:/CBB/A4000-4099/4073/sel/del.wrl");
  //read_wrl_file(pts_f,"C:/CBB/A4000-4099/4073/sel/delf.wrl");
  //read_wrl_file(pts_r,"C:/CBB/A4000-4099/4073/sel/delr.wrl");
  std::vector <vgl_point_3d<double> >  pt07;
  //read_bb_box_file(pt07,"bbox_cam_my.txt");
  
  //std::cout<<pt07[0];
  //*//read_wrl_file(ptl,"del.wrl",true);
 
  read_wrl_file(ptl,argv[0],true);
   read_bb_box_file(pt07,argv[3]);
  if (argc==1) {
    //*//read_wrl_file(pts_f,"delf.wrl",true);
    //*//read_wrl_file(pts_r,"delr.wrl",true);
    read_wrl_file(pts_f,argv[1],true);
    read_wrl_file(pts_r,argv[2],true);
  }

  /////////////////////////////////////////////////////



  vnl_matrix  <double> P(10,3,0.0);
  ///////////////////////////////
  /////   probe 0 & 7  //////////
  ///////////////////////////////

   if (argc!=1) {
    //pts_f_and_r_from_pts(pts_f,pts_r,ptl,*argv[1]);
     pts_f_and_r_from_pts(pts_f,pts_r,ptl, 'y');

  }

 
  if (argc==1) {
    P(0,0)=pt07[0].x();P(0,1)=pt07[0].y();P(0,2)=pt07[0].z();

    P(7,0)=pt07[1].x();P(7,1)=pt07[1].y();P(7,2)=pt07[1].z();
  }

  ///////////////////////////////
  /////    probe 0     //////////
  ///////////////////////////////
  vnl_vector <int> min_max_r=min_max(pts_r);

  vgl_point_3d <double> max_r_z( pts_r[min_max_r(0)].x(),pts_r[min_max_r(0)].y(),pts_r[min_max_r(0)].z());
  vgl_point_3d <double> min_r_z( pts_r[min_max_r(1)].x(),pts_r[min_max_r(1)].y(),pts_r[min_max_r(1)].z());
  vgl_point_3d <double> max_r_x( pts_r[min_max_r(2)].x(),pts_r[min_max_r(2)].y(),pts_r[min_max_r(2)].z());
  vgl_point_3d <double> min_r_x( pts_r[min_max_r(3)].x(),pts_r[min_max_r(3)].y(),pts_r[min_max_r(3)].z());

  t1.set(0,0,1); // 
  int max=-1;
  vgl_point_3d <double> max_point0;
  for (double  xi=min_r_x.x();xi<max_r_x.x();xi+=.1) // search just above
  {
    vgl_point_3d <double> s0(xi,0,1.0);//location of p0 probe.,
    tube p0(s0,t1,t2,t3,t4,radius=.2,length,1,0,0,0);
    p0.set_length(2.0);
   
    
    vnl_vector <int> ans=p0.search(pts_r,s0);
    
    ///##comented out 10-2-2006##/// std::cout<<ans<<" "<<s0<<std::endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point0.set(s0.x(),s0.y(),s0.z());
    }

  }
  
  int p0_index= find_near_point_rev( pts_r,max_point0,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
  ///##comented out 10-2-2006##/// std::cout<<"p0:->from bbox"<<P(0,0)<<" "<<P(0,1)<<" "<<P(0,2)<<" "<<pts_r[p0_index]<<std::endl<<"-------------------------------"<<std::endl;
  if (argc!=1) {
  P(0,0)=pts_r[p0_index].x();
  P(0,1)=pts_r[p0_index].y();
  P(0,2)=pts_r[p0_index].z();
  }

  ///////////////////////////////
  /////    probe 1     //////////
  ///////////////////////////////
  /*vnl_vector <int> min_max_p1=min_max(pts_r);

  vgl_point_3d <double> max_r_z( pts_r[min_max_p1(0)].x(),pts_r[min_max_p1(0)].y(),pts_r[min_max_p1(0)].z());
  vgl_point_3d <double> min_r_z( pts_r[min_max_p1(1)].x(),pts_r[min_max_p1(1)].y(),pts_r[min_max_p1(1)].z());
  vgl_point_3d <double> max_r_x( pts_r[min_max_p1(2)].x(),pts_r[min_max_p1(2)].y(),pts_r[min_max_p1(2)].z());
  vgl_point_3d <double> min_r_x( pts_r[min_max_p1(3)].x(),pts_r[min_max_p1(3)].y(),pts_r[min_max_p1(3)].z());
*/
  t1.set(-.8,0,-.2); // 
   t1.set(-.1,0,-.9);
   max=-1;
  vgl_point_3d <double> max_point;
  for (double  xi=min_r_x.x();xi<max_r_x.x();xi+=.1) // search just above
  {
    vgl_point_3d <double> s0(xi,0,max_r_z.z());
    tube p1(s0,t1,t2,t3,t4,radius=.5,length,1,0,0,0);
    p1.set_length(.5);
   
    
    vnl_vector <int> ans=p1.search(pts_r,s0);
    
    ///##comented out 10-2-2006##/// std::cout<<ans<<" "<<s0<<std::endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point.set(s0.x(),s0.y(),s0.z());
    }

  }
  
  int p1_index= find_near_point_rev( pts_r,max_point,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
  ///##comented out 10-2-2006##/// std::cout<<"p1:"<<pts_r[p1_index]<<std::endl<<"-------------------------------"<<std::endl;
  P(1,0)=pts_r[p1_index].x();
  P(1,1)=pts_r[p1_index].y();
  P(1,2)=pts_r[p1_index].z();
  /*t1.set(1,0,0.4);
  t2.set(0,0,-1);
  //tube aT(pts0,t1,t2,t3,t4,radius,length,1,0,0,0);
  for (unsigned i=0;i<pts_r.size()/3;i++) // search just above
  {
    tube p1(pts_r[i],t1,t2,t3,t4,radius,length,1,1,0,0);
    p1.set_length(1.0);
    //p1.ranger(pts_r);
    //p1.search(pts_r,pts_r[i]);
    vnl_vector <int> ans=p1.search(pts_r,pts_r[i]);
    ///##comented out 10-2-2006##/// std::cout<<pts_r[i]<<ans<<std::endl;
  }*/

  //exit(1);
  ///////////////////////////////
  /////    probe 2     //////////
  ///////////////////////////////




  ///////////////////////////////
  /////    probe 3     //////////
  ///////////////////////////////

  t1.set(1,0,-.2); // 
  max=-1;
  vgl_point_3d <double> max_point_p3;
  for (double  zi=min_r_z.z()+.5;zi<min_r_z.z()+1.5;zi+=.1) // search just above
  {
    vgl_point_3d <double> s0(min_r_x.x(),0,zi);
   // tube p1(s0,t1,t2,t3,t4,radius,length,1,0,0,0);
     tube p3(s0,t1,t2,t3,t4,radius=.2,length,1,0,0,0);
    p3.set_length(3.0);
   
    
    vnl_vector <int> ans=p3.search(pts_r,s0);
    
    ///##comented out 10-2-2006##/// std::cout<<ans<<" "<<s0<<std::endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point_p3.set(s0.x(),s0.y(),s0.z());
    }

  }
  
  int p3_index= find_near_point_rev( pts_r,max_point_p3,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
  ///##comented out 10-2-2006##/// std::cout<<"p3:"<<pts_r[p3_index]<<std::endl<<"-------------------------------"<<std::endl;
  P(3,0)=pts_r[p3_index].x();
  P(3,1)=pts_r[p3_index].y();
  P(3,2)=pts_r[p3_index].z();

  ///////////////////////////////
  /////    probe 9     //////////
  ///////////////////////////////
  vnl_vector <int> min_max_p9=min_max(pts_f);

 vgl_point_3d <double> max_f_z( pts_f[min_max_p9(0)].x(),pts_f[min_max_p9(0)].y(),pts_f[min_max_p9(0)].z());
 vgl_point_3d <double> min_f_z( pts_f[min_max_p9(1)].x(),pts_f[min_max_p9(1)].y(),pts_f[min_max_p9(1)].z());
 vgl_point_3d <double> max_f_x( pts_f[min_max_p9(2)].x(),pts_f[min_max_p9(2)].y(),pts_f[min_max_p9(2)].z());
 vgl_point_3d <double> min_f_x( pts_f[min_max_p9(3)].x(),pts_f[min_max_p9(3)].y(),pts_f[min_max_p9(3)].z());

  t1.set(1,0,-.8); // 
   max=-1;
   double x_p9_angle;
  //vgl_point_3d <double> max_point;
  for (double  xi=min_f_x.x();xi<max_f_x.x();xi+=.1) // search just above
  {
    vgl_point_3d <double> s0(xi,0,max_f_z.z());
    tube p9(s0,t1,t2,t3,t4,radius,length,1,0,0,0);
    p9.set_length(2.0);
   
    
    vnl_vector <int> ans=p9.search(pts_f,s0);
    
    ///##comented out 10-2-2006##/// std::cout<<ans<<" "<<s0<<std::endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point.set(s0.x(),s0.y(),s0.z());
      x_p9_angle=s0.x();
    }

  }
  
  int p9_index= find_near_point_rev( pts_f,max_point,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가?
  ///##comented out 10-2-2006##/// std::cout<<"p9:"<<pts_f[p9_index]<<std::endl<<"-------------------------------"<<std::endl;
  P(9,0)=pts_f[p9_index].x();
  P(9,1)=pts_f[p9_index].y();
  P(9,2)=pts_f[p9_index].z();
  
  ///////////////////////////////
  /////  probe 2(nose) //////////
  ///////////////////////////////


  t1.set(-1,0,.2); // 
  t2.set(-.2,0,-1); // 
  max=-1;
  vgl_point_3d <double> max_point_p2_nose;
  //for (double  zi=min_r_z.z()+.5;zi<min_r_z.z()+1.5;zi+=.1) // search just above
  for (double  xi=max_f_x.x()-1;xi<max_f_x.x();xi+=.1) // search just front
    for (double  zi=min_f_z.z()+1.5;zi<min_f_z.z()+3.0;zi+=.1)
    {
      vgl_point_3d <double> s0(xi,0,zi);
      // tube p1(s0,t1,t2,t3,t4,radius,length,1,0,0,0);
      tube p2_nose(s0,t1,t2,t3,t4,radius=.1,length,1,0,0,0);
      p2_nose.set_length(3,1.5,1,1);


      vnl_vector <int> ans=p2_nose.search(pts_f,s0);

      ///##comented out 10-2-2006##/// std::cout<<ans<<" "<<s0<<std::endl;
      if (ans[4]>max) {
       max=ans[4];
     //if (2*ans[0]+ans[1]>max) {
     //   max=2*ans[0]+ans[1];
        max_point_p2_nose.set(s0.x(),s0.y(),s0.z());
       // ///##comented out 10-2-2006##/// std::cout<<max<<s0<<std::endl;
      }

    }

    int p2_nose_index= find_near_point_rev( pts_f,max_point_p2_nose,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
    ///##comented out 10-2-2006##/// std::cout<<"p2_nose:"<<pts_f[p2_nose_index]<<"\n======================="<<std::endl;
    P(2,0)=pts_f[p2_nose_index].x();
    P(2,1)=pts_f[p2_nose_index].y();
    P(2,2)=pts_f[p2_nose_index].z();

    if (argc!=1) {
      P(7,0)=pts_f[p2_nose_index].x();
      P(7,1)=pts_f[p2_nose_index].y();
      P(7,2)=pts_f[p2_nose_index].z();
    }
    ////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////

    double height_of_car=max_f_z.z();
    if (max_r_z.z()>height_of_car) height_of_car=max_r_z.z();

    //float result=recognizer3(P,height_of_car);
    //std::cerr<<V<<std::endl;
   // float result=recognizer4(P,height_of_car,V);
   //  float result=recognizer_RG(P,height_of_car,V);


    std::vector <int> choice;
    for (unsigned i=22;i<V.size();i++)
      choice.push_back((int) std::floor(V[i]));
    float result=tree(P,height_of_car,V,choice);


        return result;






}

//////////////////////






/////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//int main2_rev(int argc, char* argv[])

float main2_rev(int argc, std::vector <std::string> argv)
{
  ///##comented out 10-2-2006##/// std::cout << "Test dtube\n";

  vgl_point_3d <double> pts0(1,0,0);

  ///##comented out 10-2-2006##/// std::cout << "Inputs\n";
  // Simple triangle
  std::vector<vgl_point_3d<double> > pts;
  pts.push_back( vgl_point_3d<double>(0.0, 0, 0.0) );
  pts.push_back( vgl_point_3d<double>(3.0, 0, .74) );
  pts.push_back( vgl_point_3d<double>(3.0, 0, .75) );
  pts.push_back( vgl_point_3d<double>(3.0, 0, 0.0) );
  pts.push_back( vgl_point_3d<double>(2.0, 0, 1.0) );
  pts.push_back( vgl_point_3d<double>(0.0, 0, 5.0) );

  //for (unsigned i=0; i < pts.size(); ++i)
  //  ///##comented out 10-2-2006##/// std::cout << '(' << pts[i].x() <<','<<pts[i].z()<<") ";
 // ///##comented out 10-2-2006##/// std::cout << std::endl;

  vgl_point_3d <double> e12(1,0,3);
  vgl_point_3d <double> e22(4,0,3);
  vgl_point_3d <double> e32(1,0,3);
  vgl_point_3d <double> e42(1,0,3);
  //double radius= 1.5; double length=3.0;
  double radius= .2; double length=9.0;

  vgl_vector_3d<double> t1(-1,0,0.4);
  vgl_vector_3d<double> t2(1,.5,-1);
  vgl_vector_3d<double> t3(0,0,3);
  vgl_vector_3d<double> t4(0,0,3);

  //tube at(pts0,e12,e22,e32,e42,radius,length,1,1,0,0);
   tube aT(pts0,t1,t2,t3,t4,radius,length,1,0,0,0);
  aT.set_length(9.0);

 // aT.ranger(pts);
  /*vgl_polygon<double> poly=vgl_convex_hull(pts);
  ///##comented out 10-2-2006##/// std::cout << "Outputs\n";
  poly.print(///##comented out 10-2-2006##/// std::cout);
  ///##comented out 10-2-2006##/// std::cout << std::endl;

  TEST("inside", poly.contains( pts[0] ), true );
  TEST("inside", poly.contains( pts[1] ), true );
  TEST("inside", poly.contains( pts[3] ), true );
  TEST("inside", poly.contains( pts[4] ), true );*/



  //=========================== load the files ==========================

  /////////////////////////////////////////
  /////////////////////////////////////////
  std::vector<vgl_point_3d<double> >ptl;
  std::vector<vgl_point_3d<double> >pts_f;
  std::vector<vgl_point_3d<double> >pts_r;
  //read_wrl_file(ptl,"C:/CBB/A4000-4099/4073/sel/del.wrl");
  //read_wrl_file(pts_f,"C:/CBB/A4000-4099/4073/sel/delf.wrl");
  //read_wrl_file(pts_r,"C:/CBB/A4000-4099/4073/sel/delr.wrl");
  std::vector <vgl_point_3d<double> >  pt07;
  //read_bb_box_file(pt07,"bbox_cam_my.txt");
  
  //std::cout<<pt07[0];
  //*//read_wrl_file(ptl,"del.wrl",true);
 
  read_wrl_file(ptl,argv[0],true);
   read_bb_box_file(pt07,argv[3]);
  if (argc==1) {
    //*//read_wrl_file(pts_f,"delf.wrl",true);
    //*//read_wrl_file(pts_r,"delr.wrl",true);
    read_wrl_file(pts_f,argv[1],true);
    read_wrl_file(pts_r,argv[2],true);
  }

  /////////////////////////////////////////////////////



  vnl_matrix  <double> P(10,3,0.0);
  ///////////////////////////////
  /////   probe 0 & 7  //////////
  ///////////////////////////////

   if (argc!=1) {
    //pts_f_and_r_from_pts(pts_f,pts_r,ptl,*argv[1]);
     pts_f_and_r_from_pts(pts_f,pts_r,ptl, 'y');

  }

 
  if (argc==1) {
    P(0,0)=pt07[0].x();P(0,1)=pt07[0].y();P(0,2)=pt07[0].z();

    P(7,0)=pt07[1].x();P(7,1)=pt07[1].y();P(7,2)=pt07[1].z();
  }

  ///////////////////////////////
  /////    probe 0     //////////
  ///////////////////////////////
  vnl_vector <int> min_max_r=min_max(pts_r);

  vgl_point_3d <double> max_r_z( pts_r[min_max_r(0)].x(),pts_r[min_max_r(0)].y(),pts_r[min_max_r(0)].z());
  vgl_point_3d <double> min_r_z( pts_r[min_max_r(1)].x(),pts_r[min_max_r(1)].y(),pts_r[min_max_r(1)].z());
  vgl_point_3d <double> max_r_x( pts_r[min_max_r(2)].x(),pts_r[min_max_r(2)].y(),pts_r[min_max_r(2)].z());
  vgl_point_3d <double> min_r_x( pts_r[min_max_r(3)].x(),pts_r[min_max_r(3)].y(),pts_r[min_max_r(3)].z());

  t1.set(0,0,1); // 
  int max=-1;
  vgl_point_3d <double> max_point0;
  for (double  xi=min_r_x.x();xi<max_r_x.x();xi+=.1) // search just above
  {
    vgl_point_3d <double> s0(xi,0,1.0);//location of p0 probe.,
    tube p0(s0,t1,t2,t3,t4,radius=.2,length,1,0,0,0);
    p0.set_length(2.0);
   
    
    vnl_vector <int> ans=p0.search(pts_r,s0);
    
    ///##comented out 10-2-2006##/// std::cout<<ans<<" "<<s0<<std::endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point0.set(s0.x(),s0.y(),s0.z());
    }

  }
  
  int p0_index= find_near_point_rev( pts_r,max_point0,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
  ///##comented out 10-2-2006##/// std::cout<<"p0:->from bbox"<<P(0,0)<<" "<<P(0,1)<<" "<<P(0,2)<<" "<<pts_r[p0_index]<<std::endl<<"-------------------------------"<<std::endl;
  if (argc!=1) {
  P(0,0)=pts_r[p0_index].x();
  P(0,1)=pts_r[p0_index].y();
  P(0,2)=pts_r[p0_index].z();
  }

  ///////////////////////////////
  /////    probe 1     //////////
  ///////////////////////////////
  /*vnl_vector <int> min_max_p1=min_max(pts_r);

  vgl_point_3d <double> max_r_z( pts_r[min_max_p1(0)].x(),pts_r[min_max_p1(0)].y(),pts_r[min_max_p1(0)].z());
  vgl_point_3d <double> min_r_z( pts_r[min_max_p1(1)].x(),pts_r[min_max_p1(1)].y(),pts_r[min_max_p1(1)].z());
  vgl_point_3d <double> max_r_x( pts_r[min_max_p1(2)].x(),pts_r[min_max_p1(2)].y(),pts_r[min_max_p1(2)].z());
  vgl_point_3d <double> min_r_x( pts_r[min_max_p1(3)].x(),pts_r[min_max_p1(3)].y(),pts_r[min_max_p1(3)].z());
*/
  t1.set(-.8,0,-.2); // 
   t1.set(-.1,0,-.9);
   max=-1;
  vgl_point_3d <double> max_point;
  for (double  xi=min_r_x.x();xi<max_r_x.x();xi+=.1) // search just above
  {
    vgl_point_3d <double> s0(xi,0,max_r_z.z());
    tube p1(s0,t1,t2,t3,t4,radius=.5,length,1,0,0,0);
    p1.set_length(.5);
   
    
    vnl_vector <int> ans=p1.search(pts_r,s0);
    
    ///##comented out 10-2-2006##/// std::cout<<ans<<" "<<s0<<std::endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point.set(s0.x(),s0.y(),s0.z());
    }

  }
  
  int p1_index= find_near_point_rev( pts_r,max_point,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
  ///##comented out 10-2-2006##/// std::cout<<"p1:"<<pts_r[p1_index]<<std::endl<<"-------------------------------"<<std::endl;
  P(1,0)=pts_r[p1_index].x();
  P(1,1)=pts_r[p1_index].y();
  P(1,2)=pts_r[p1_index].z();
  /*t1.set(1,0,0.4);
  t2.set(0,0,-1);
  //tube aT(pts0,t1,t2,t3,t4,radius,length,1,0,0,0);
  for (unsigned i=0;i<pts_r.size()/3;i++) // search just above
  {
    tube p1(pts_r[i],t1,t2,t3,t4,radius,length,1,1,0,0);
    p1.set_length(1.0);
    //p1.ranger(pts_r);
    //p1.search(pts_r,pts_r[i]);
    vnl_vector <int> ans=p1.search(pts_r,pts_r[i]);
    ///##comented out 10-2-2006##/// std::cout<<pts_r[i]<<ans<<std::endl;
  }*/

  //exit(1);
  ///////////////////////////////
  /////    probe 2     //////////
  ///////////////////////////////




  ///////////////////////////////
  /////    probe 3     //////////
  ///////////////////////////////

  t1.set(1,0,-.2); // 
  max=-1;
  vgl_point_3d <double> max_point_p3;
  for (double  zi=min_r_z.z()+.5;zi<min_r_z.z()+1.5;zi+=.1) // search just above
  {
    vgl_point_3d <double> s0(min_r_x.x(),0,zi);
   // tube p1(s0,t1,t2,t3,t4,radius,length,1,0,0,0);
     tube p3(s0,t1,t2,t3,t4,radius=.2,length,1,0,0,0);
    p3.set_length(3.0);
   
    
    vnl_vector <int> ans=p3.search(pts_r,s0);
    
    ///##comented out 10-2-2006##/// std::cout<<ans<<" "<<s0<<std::endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point_p3.set(s0.x(),s0.y(),s0.z());
    }

  }
  
  int p3_index= find_near_point_rev( pts_r,max_point_p3,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
  ///##comented out 10-2-2006##/// std::cout<<"p3:"<<pts_r[p3_index]<<std::endl<<"-------------------------------"<<std::endl;
  P(3,0)=pts_r[p3_index].x();
  P(3,1)=pts_r[p3_index].y();
  P(3,2)=pts_r[p3_index].z();

  ///////////////////////////////
  /////    probe 9     //////////
  ///////////////////////////////
  vnl_vector <int> min_max_p9=min_max(pts_f);

 vgl_point_3d <double> max_f_z( pts_f[min_max_p9(0)].x(),pts_f[min_max_p9(0)].y(),pts_f[min_max_p9(0)].z());
 vgl_point_3d <double> min_f_z( pts_f[min_max_p9(1)].x(),pts_f[min_max_p9(1)].y(),pts_f[min_max_p9(1)].z());
 vgl_point_3d <double> max_f_x( pts_f[min_max_p9(2)].x(),pts_f[min_max_p9(2)].y(),pts_f[min_max_p9(2)].z());
 vgl_point_3d <double> min_f_x( pts_f[min_max_p9(3)].x(),pts_f[min_max_p9(3)].y(),pts_f[min_max_p9(3)].z());

  t1.set(1,0,-.8); // 
   max=-1;
   double x_p9_angle;
  //vgl_point_3d <double> max_point;
  for (double  xi=min_f_x.x();xi<max_f_x.x();xi+=.1) // search just above
  {
    vgl_point_3d <double> s0(xi,0,max_f_z.z());
    tube p9(s0,t1,t2,t3,t4,radius,length,1,0,0,0);
    p9.set_length(2.0);
   
    
    vnl_vector <int> ans=p9.search(pts_f,s0);
    
    ///##comented out 10-2-2006##/// std::cout<<ans<<" "<<s0<<std::endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point.set(s0.x(),s0.y(),s0.z());
      x_p9_angle=s0.x();
    }

  }
  
  int p9_index= find_near_point_rev( pts_f,max_point,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가?
  ///##comented out 10-2-2006##/// std::cout<<"p9:"<<pts_f[p9_index]<<std::endl<<"-------------------------------"<<std::endl;
  P(9,0)=pts_f[p9_index].x();
  P(9,1)=pts_f[p9_index].y();
  P(9,2)=pts_f[p9_index].z();
  
  ///////////////////////////////
  /////  probe 2(nose) //////////
  ///////////////////////////////


  t1.set(-1,0,.2); // 
  t2.set(-.2,0,-1); // 
  max=-1;
  vgl_point_3d <double> max_point_p2_nose;
  //for (double  zi=min_r_z.z()+.5;zi<min_r_z.z()+1.5;zi+=.1) // search just above
  for (double  xi=max_f_x.x()-1;xi<max_f_x.x();xi+=.1) // search just front
    for (double  zi=min_f_z.z()+1.5;zi<min_f_z.z()+3.0;zi+=.1)
    {
      vgl_point_3d <double> s0(xi,0,zi);
      // tube p1(s0,t1,t2,t3,t4,radius,length,1,0,0,0);
      tube p2_nose(s0,t1,t2,t3,t4,radius=.1,length,1,0,0,0);
      p2_nose.set_length(3,1.5,1,1);


      vnl_vector <int> ans=p2_nose.search(pts_f,s0);

      ///##comented out 10-2-2006##/// std::cout<<ans<<" "<<s0<<std::endl;
      if (ans[4]>max) {
       max=ans[4];
     //if (2*ans[0]+ans[1]>max) {
     //   max=2*ans[0]+ans[1];
        max_point_p2_nose.set(s0.x(),s0.y(),s0.z());
       // ///##comented out 10-2-2006##/// std::cout<<max<<s0<<std::endl;
      }

    }

    int p2_nose_index= find_near_point_rev( pts_f,max_point_p2_nose,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
    ///##comented out 10-2-2006##/// std::cout<<"p2_nose:"<<pts_f[p2_nose_index]<<"\n======================="<<std::endl;
    P(2,0)=pts_f[p2_nose_index].x();
    P(2,1)=pts_f[p2_nose_index].y();
    P(2,2)=pts_f[p2_nose_index].z();

    if (argc!=1) {
      P(7,0)=pts_f[p2_nose_index].x();
      P(7,1)=pts_f[p2_nose_index].y();
      P(7,2)=pts_f[p2_nose_index].z();
    }
    ////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////

    double height_of_car=max_f_z.z();
    if (max_r_z.z()>height_of_car) height_of_car=max_r_z.z();

    float result=recognizer3(P,height_of_car);
    


        return result;



    ///##comented out 10-2-2006##/// std::cout<<"************************************\n"<<result<<"        "<<vul_file::get_cwd()<<
    ///##comented out 10-2-2006##///  "\n************************************"<<std::endl;
    ///##comented out 10-2-2006##/// std::cout<<P<<std::endl;
    /*
    t1.set(1,0,-1);
    t2.set(0,0,-1);
    //tube aT(pts0,t1,t2,t3,t4,radius,length,1,0,0,0);
    for (unsigned i=0;i<20;i++) // search just 10 points..
    {
    //for (double t1z=0;t1z<1;t1z+=.1) {
    if (std::fabs(pts_r[i].z()-pts_r[0].z()) <.5)
    {



    double r=.3;
    tube p1(pts_r[i],t1,t2,t3,t4,r,length,1,0,0,0);

    //p1.set_length(t1_l,4.0,4.0, 4.0);
    p1.set_length(3.0,4.0,4.0, 4.0);

    vnl_vector <int> ans=p1.search(pts_r,pts_r[i]);
    ///##comented out 10-2-2006##/// std::cout<<ans<<std::endl;
    }

    }

    exit(1);*/





    ///////////////////////////////
    /////    probe 9  angle  //////////
    ///////////////////////////////

    x_p9_angle;
    //  vnl_vector <int> min_max_p9=min_max(pts_f);

    // vgl_point_3d <double> max_f_z( pts_f[min_max_p9(0)].x(),pts_f[min_max_p9(0)].y(),pts_f[min_max_p9(0)].z());
    // vgl_point_3d <double> min_f_z( pts_f[min_max_p9(1)].x(),pts_f[min_max_p9(1)].y(),pts_f[min_max_p9(1)].z());
    // vgl_point_3d <double> max_f_x( pts_f[min_max_p9(2)].x(),pts_f[min_max_p9(2)].y(),pts_f[min_max_p9(2)].z());
    // vgl_point_3d <double> min_f_x( pts_f[min_max_p9(3)].x(),pts_f[min_max_p9(3)].y(),pts_f[min_max_p9(3)].z());

    // t1.set(1,0,-.8); // 
    max=-1;
    //vgl_point_3d <double> max_point;


    /*x= -54.49910;   z=  32.24610;  
    x= -73.28730;   z=  48.27782;  


    x= -46.07720;   z=  35.90390; 
    x= -71.21410;   z=  54.47188;*/

    //73.28730-54.49910= //suv xdiff  18.7882  zdiff  16.03172  /// van xdiff 25.1369 zdif f18.56798




    vgl_point_3d <double> ep7(P(7,0),P(7,1),P(7,2));
    vgl_point_3d <double> ep9(P(9,0),P(9,1),P(9,2));


    for (double  kz=max_f_z.z()-3.0;kz<max_f_z.z()-.5;kz+=.5) // search just above 
      for (double kx=x_p9_angle+1.0;kx<x_p9_angle+3.0;kx+=.4)
      {
        // vgl_point_3d <double> s0(xi,0,max_f_z.z());
        vgl_point_3d <double> s0(kx,0,kz);
        tube p9_angle(s0,ep9,ep7,ep9,ep7,.1,1,1,0,0);
        // p9.set_length(2.0);

    
    vnl_vector <int> ans=p9_angle.search(pts_f,s0);
    
    ///##comented out 10-2-2006##/// std::cout<<ans<<" "<<s0<<std::endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point.set(s0.x(),s0.y(),s0.z());
    }

  }
  
  int p9_angle_index= find_near_point_rev( pts_f,max_point,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가?
  ///##comented out 10-2-2006##/// std::cout<<"p9 angle:"<<pts_f[p9_angle_index]<<std::endl;

  vgl_vector_3d <double>a1=ep7-pts_f[p9_angle_index];
  vgl_vector_3d <double>a2=ep9-pts_f[p9_angle_index];

  double dp=dot_product(a1,a2);
  ///##comented out 10-2-2006##/// std::cout<<"dot_product(a1,a2)  "<<a1.length()<<" "<<a2.length()<<" "<<acos(dp/(a1.length()*a2.length()))*180/vnl_math::pi<<std::endl;
  ///##comented out 10-2-2006##/// std::cout <<"a1(hood)" <<a1<<std::endl;

  /////##comented out 10-2-2006##/// std::cout<<"angle :" <<
  //P(9,0)=pts_f[p9_index].x();
  //P(9,1)=pts_f[p9_index].y();
  //P(9,2)=pts_f[p9_index].z();

   //std::cerr<<" vcl_cerr_test"<<std::endl;

   //std::cerr<<vul_file::dirname(argv[0]);
  if (CERR) std::cerr<<vul_file::get_cwd()<<"----------------   "<<result<<std::endl;
   //std::string input_file_path=".";
  // std::string aaa=vul_file::strip_directory(input_file);
   //   std::string nums1=vul_file::strip_extension(aaa);

  return 7;
  ///////////////////////////////
  /////    probe 7-8-9     //////////
  ///////////////////////////////


  t1.set(1,0,-.2);
  t2.set(0,0,-1);
  //tube aT(pts0,t1,t2,t3,t4,radius,length,1,0,0,0);

  //temp: input needed from calibration




  vnl_vector <int> max_linear(5,0);
  vnl_vector <int> max_square(5,0);
  vgl_point_3d<double>  max_lin_point;
  vgl_point_3d<double>  max_square_point;
  int max_lin_index=-1,max_square_index=-1;

  //vgl_point_3d <double> p9_from_calibartion(15.7944, 9.16384, 2.38852);//p9fC point
  //vgl_point_3d <double> p9(pts_f[p9_index].x(),pts_f[p9_index].y(),pts_f[p9_index].z());
  //unsigned int p789_index=find_near_point_rev(pts_f,p9_from_calibartion,1,pts_f.size());

  vgl_point_3d <double> p7(P(7,0),P(7,1),P(7,2));
  unsigned int p7_index=find_near_point_rev(pts_f,p7,1,pts_f.size());
  ///##comented out 10-2-2006##/// std::cout<<p7_index<<std::endl;
  for (unsigned i=0;i<2;i++) // search just 10 points..
  {

    //for (unsigned j=p789_index;j<p789_index+1 ;j++) { // near tha p9fC points..
    for (unsigned j=p7_index;j<p7_index+1 ;j++) { // near tha p9fC points..
      for (unsigned k=i+1;k<j;k++) {

       // ///##comented out 10-2-2006##/// std::cout<<pts_f[i]<<pts_f[k]<<pts_f[j]<<std::endl;
        t1.set(pts_f[i].x()-pts_f[k].x(),pts_f[i].y()-pts_f[k].y(),pts_f[i].z()-pts_f[k].z());
        t2.set(pts_f[j].x()-pts_f[k].x(),pts_f[j].y()-pts_f[k].y(),pts_f[j].z()-pts_f[k].z());


        tube p789(pts_f[k],t1,t2,t3,t4,.1,-1,1,1,0,0);
        
        vnl_vector <int> ans=p789.search(pts_f,pts_f[k]);
        ///##comented out 10-2-2006##/// std::cout<<ans<<std::endl;
        if (ans[4]>max_linear[4]) {
          max_linear=ans;
          max_lin_index=k;
          max_lin_point=pts_f[k];
        }

        if (ans[0]*ans[0]+ans[1]*ans[1]+ans[2]*ans[2]+ans[3]*ans[3]>
          max_square[0]*max_square[0]+max_square[1]*max_square[1]+
          max_square[2]*max_square[2]+max_square[3]*max_square[3])
        {
          max_square=ans;
          max_square_index=k;
          max_square_point=pts_f[k];
        }
      }
    }

  }
  ///##comented out 10-2-2006##/// std::cout<<"max_lin "<<max_linear<<" "<<max_lin_point<<std::endl;
  ///##comented out 10-2-2006##/// std::cout<<"max_sqr "<<max_square<<" "<<max_square_point<<std::endl;



}

//////////////////////
//////////////////////
//////////////////////
//////////////////////
//////////////////////
//////////////////////
//////////////////////
//////////////////////

std::string get_recent(std::vector <std::string> flist[],int i) {
  int N=flist[i].size();
  if (N==0) return "NULL";
  std::string to_return =flist[i][N-1];
  return to_return;
}

std::vector <std::string>  get_from_range( std::vector <std::string> flist[], int index, int month_start, int month_end, int day_start, int day_end, int time_start, int time_end) 
//void get_from_range(std::vector <std::string> flist[], int index) 
{

  std::vector <std::string> to_return;
  // std::vector <vul_reg_exp> rlist;
  std::string vr_dir,mode;
  int word_count;
  // for (unsigned i=0;i<4;i++) {
  if (index==0) { vr_dir="contour_0_"; mode="0"; word_count=9;}
  else if (index==1) {vr_dir="correct_1_";mode="1";word_count=9;}
  else if (index==2) {vr_dir="messy_2_";mode="2";word_count=7;}
  else if (index==3) {vr_dir="all_3_";mode="3";word_count=5;}
  else ;
  //vul_reg_exp r0(vr_dir.c_str());
  //rlist.push_back(r0);
  // }

  std::cout<<vr_dir<<std::endl;

  //std::vector <std::string> to_return;
  for (unsigned i=0;i<flist[index].size();i++) 
  {
    std::string along=flist[index][i];
    //a=vul_file::basename(along);
    std::string sub=along.substr(word_count+3);
    int month=atoi(sub.substr(0,2).c_str());
    int day=atoi(sub.substr(3,2).c_str());
    int year=atoi(sub.substr(6,4).c_str());
    int time=atoi(sub.substr(11,2).c_str());

   //// std::cout<<month<<" "<<day<<" "<<year<<" "<<time<<std::endl;

    if (day>=day_start&&day<=day_end && month>=month_start&&month<=month_end && time>=time_start&&time<=time_end)
    {
       to_return.push_back(flist[index][i]);

    }
  }
  return to_return;
 //  return rlist;
}


int main(int argc, char* argv[])
{
        std::cout<<"--------------------------------------------"<<std::endl;
        if (CERR) std::cerr<<"--------------------------------------------"<<std::endl;
        std::cout<<vul_file::get_cwd()<<std::endl;
        if (CERR) std::cerr<<vul_file::get_cwd();//<<std::endl;
 std::vector <std::string>  flist[4];
  //vul_file_iterator fn=vul_file::dirname(filename)+"/*.con";
  //vul_file_iterator fn=vul_file::dirname(vul_file::get_cwd());
  //vul_file_iterator fn=input_file_path+"/*.con";
  bool recon_flag=0;
  std::string vr_dir;
  std::string mode;

  int METHODS_NUM=2;
  std::vector <vul_reg_exp> rlist;
  for (unsigned i=0;i<METHODS_NUM;i++) {
    if (i==0) { vr_dir="contour_0"; mode="0";}
    else if (i==1) {vr_dir="correct_1";mode="1";}
    else if (i==2) {vr_dir="messy_2";mode="2";}
    else if (i==3) {vr_dir="all_3";mode="3";}
    else ;
    vul_reg_exp r0(vr_dir.c_str());
    rlist.push_back(r0);
  }
  
  //std::string input_file_path="C:/CBB/A4000-4099/4011/sel.";
  std::string input_file_path=".";
  vul_file_iterator fn=input_file_path+"/*";//nframes_=5;
  for ( ; fn; ++fn) 
  {
    std::string input_file = fn();//
    if (vul_file::is_directory(input_file)) {

      for (unsigned j=0;j<METHODS_NUM;j++)
        if (rlist[j].find(input_file.c_str()) ){
          flist[j].push_back(input_file);
         //***// std::cout<<j<<": "<<input_file<<std::endl;
        }
    }
  }
  //***//std::cout<<vul_file::get_cwd()<<"----------------   "<<std::endl;
 

  /*int day_in=atoi(argv[1]);
  int day_out=atoi(argv[2]);
  int time_in =atoi(argv[3]);
  int time_out =atoi(argv[4]);
*/
  for (unsigned i=1;i<METHODS_NUM;i++) {
    std::string where;
    where=get_recent(flist,i);
    std::cout<< where<<std::endl;
    //std::vector <std::string> FL=get_from_range(flist,1, 10,11,9,17, 0,12);
   /* std::vector <std::string> FL=get_from_range(flist,1, 10,10,day_in,day_out, time_in,time_out);
    std::cout<<"================="<<std::endl;
    for (unsigned j=0;j<FL.size();j++)
      std::cout<<FL[j]<<std::endl;
*/
    

   std::vector <std::string>  which_to_read;

    if (i==0) 
    {
      std::cout<<"***case 0***"<<std::endl;
      std::string File =where+"/del-AEC-0.wrl";
      std::string Filef=where+"/delf-AEC-0.wrl";
      std::string Filer=where+"/delr-AEC-0.wrl";

      which_to_read.push_back(File);
      which_to_read.push_back(Filef);
      which_to_read.push_back(Filer);
      
    }
    if (i==1)
    {
      std::cout<<"***case 1***"<<std::endl;
      std::string File =where+"/del-AEC-1.wrl";
      std::string Filef=where+"/delf-AEC-1.wrl";
      std::string Filer=where+"/delr-AEC-1.wrl";

      which_to_read.push_back(File);
      which_to_read.push_back(Filef);
      which_to_read.push_back(Filer);
    }
    which_to_read.push_back(where+"/bbox_cam_my.txt");

          std::string reference=vul_file::get_cwd();
         std::cout<<reference<<"***"<<std::endl;
         // std::cout<<"afafdsa"<<std::endl;
         int SZ=reference.size();
   std::cout<<SZ<<std::endl;
    int ref_id=atoi(reference.substr(SZ-8,SZ-4).c_str());  
    std::cout<<"directory id: "<<ref_id<<std::endl;
        
        ///float result=main2_rev(i,which_to_read);
    vnl_vector <float> V(argc,0);
    
    
    for (unsigned j=1;j<argc;j++) {
    //for (unsigned j=1;j<21;j++) { //for tree.. from 21th element it means order of tree
      
      if (j<22) {
      V[j]=atof(argv[j])/100;
      //V[j]=(atof(argv[j])-32767*.5)/(4*32767);
      V[j]=(atof(argv[j])-32767*.5)/(2*32767);

      }
      else {

        V[j]=atof(argv[j]);



      }
      if (CERR) std::cerr<<V[j]<<" ";
    }



    if (CERR) std::cerr<<std::endl;
    float result=main3_rev(i,which_to_read,V);
        std::cout<<result<<std::endl;
        //vul_reg_exp r(vul_file::get_cwd());

        int class_id=970;
  if (result==999) 
  {
                std::cout<<"999"<<std::endl;
                std::cerr<<"                   No Decision         "<<result<<std::endl;
        }
        if (result<10) class_id=0;
        else if (result<20) class_id=1;
        else if (result<30) class_id=2;
        else class_id=454;

  int real_class_id;
  bool check=table(ref_id,class_id,real_class_id);
  std::cout<<"real_class_id "<<real_class_id<<std::endl;
        if (check) 
        {
                std::cout<<"OOOOO "<<real_class_id<<" "<<result<<std::endl;
                if (CERR) std::cerr<<"                       OOOOO    "<<real_class_id<<" "<<result<<std::endl;
        }
        else {
                std::cout<<"XXXXX "<<real_class_id<<" "<<result<<std::endl;
                if (CERR) std::cerr<<"                         XXXXX     "<<real_class_id<<" "<<result<<std::endl;
        }

  }

}

