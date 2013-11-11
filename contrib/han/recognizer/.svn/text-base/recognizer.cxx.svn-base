// 
#include <testlib/testlib_test.h>
#include <vgl/vgl_convex.h>
#include <vcl_iostream.h>
#include <vnl/vnl_matrix.h>
#include "tube.h"
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_reg_exp.h>
#include <vcl_cstring.h>
#include <vcl_string.h>
#include "subsidary_routine.h"
#include "recogognizers_tree_type.h"

#define CERR false
//
float main3(int argc, vcl_vector <vcl_string> argv, vnl_vector <float>V)
{

  vgl_point_3d <double> pts0(1,0,0);
  double radius= .2; double length=9.0;
  vgl_vector_3d<double> t1(-1,0,0.4);
  vgl_vector_3d<double> t2(1,.5,-1);
  vgl_vector_3d<double> t3(0,0,3);
  vgl_vector_3d<double> t4(0,0,3);



  //=========================== load the files ==========================

  /////////////////////////////////////////
  /////////////////////////////////////////
  vcl_vector<vgl_point_3d<double> >ptl;
  vcl_vector<vgl_point_3d<double> >pts_f;
  vcl_vector<vgl_point_3d<double> >pts_r;
  //read_wrl_file(ptl,"C:/CBB/A4000-4099/4073/sel/del.wrl");
  //read_wrl_file(pts_f,"C:/CBB/A4000-4099/4073/sel/delf.wrl");
  //read_wrl_file(pts_r,"C:/CBB/A4000-4099/4073/sel/delr.wrl");
  vcl_vector <vgl_point_3d<double> >  pt07;
  //read_bb_box_file(pt07,"bbox_cam_my.txt");

  //vcl_cout<<pt07[0];
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

    //******************************************************************
    vgl_point_3d <double> s0(xi,0,1.0);//location of p0 probe.,**********  z is 1 feet high !! (right way?)
    //******************************************************************
    tube p0(s0,t1,t2,t3,t4,radius=.2,length,1,0,0,0);
    p0.set_length(2.0);  // so 1 to 3 feet..!!!!!!!!!!!!!!!


    vnl_vector <int> ans=p0.search(pts_r,s0);

    ///##comented out 10-2-2006##/// vcl_cout<<ans<<" "<<s0<<vcl_endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point0.set(s0.x(),s0.y(),s0.z());
    }

  }

  int p0_index= find_near_point( pts_r,max_point0,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
  ///##comented out 10-2-2006##/// vcl_cout<<"p0:->from bbox"<<P(0,0)<<" "<<P(0,1)<<" "<<P(0,2)<<" "<<pts_r[p0_index]<<vcl_endl<<"-------------------------------"<<vcl_endl;
  if (argc!=1) {
    P(0,0)=pts_r[p0_index].x();
    P(0,1)=pts_r[p0_index].y();
    P(0,2)=pts_r[p0_index].z();
  }

  ///////////////////////////////
  /////    probe 1     //////////
  ///////////////////////////////
 
  t1.set(-.8,0,-.2); // 
  t1.set(-.1,0,-.9);
  max=-1;
  vgl_point_3d <double> max_point;
  for (double  xi=min_r_x.x();xi<max_r_x.x();xi+=.1) // search just above
  {
    vgl_point_3d <double> s0(xi,0,max_r_z.z());
    tube p1(s0,t1,t2,t3,t4,radius=.5,length,1,0,0,0);
    p1.set_length(.5); //isn't this too short? 0.5 


    vnl_vector <int> ans=p1.search(pts_r,s0);

    if (1) vcl_cout<<xi<<'\t'<<ans<<" "<<s0<<vcl_endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point.set(s0.x(),s0.y(),s0.z());
    }

  }

  int p1_index= find_near_point( pts_r,max_point,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
  ///##comented out 10-2-2006##/// vcl_cout<<"p1:"<<pts_r[p1_index]<<vcl_endl<<"-------------------------------"<<vcl_endl;
  P(1,0)=pts_r[p1_index].x();
  P(1,1)=pts_r[p1_index].y();
  P(1,2)=pts_r[p1_index].z();



   //5-26-07// start
  //pt07[2][1]  height of vehicle..
  // 
  P(1,0)=max_point.x();P(1,1)=max_point.y();P(1,2)=max_point.z();
  if (vcl_fabs(pt07[2].y()-P(1,2))> 1.5) {  //pt07[2].y()  has solve_z height from bb_box file.
    vcl_cerr<<vcl_fabs(pt07[2].y()-P(1,2))<<vcl_endl;
    P(1,2)+=100;
  }//5-26-07// end





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

    ///##comented out 10-2-2006##/// vcl_cout<<ans<<" "<<s0<<vcl_endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point_p3.set(s0.x(),s0.y(),s0.z());
    }

  }

  int p3_index= find_near_point( pts_r,max_point_p3,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
  ///##comented out 10-2-2006##/// vcl_cout<<"p3:"<<pts_r[p3_index]<<vcl_endl<<"-------------------------------"<<vcl_endl;
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
    vgl_point_3d <double> s0(xi,0,max_f_z.z()); //at the top..
    tube p9(s0,t1,t2,t3,t4,radius,length,1,0,0,0);
    p9.set_length(2.0);


    vnl_vector <int> ans=p9.search(pts_f,s0);  //search method does not search.. it just counts the number..

    ///##comented out 10-2-2006##/// vcl_cout<<ans<<" "<<s0<<vcl_endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point.set(s0.x(),s0.y(),s0.z());
      x_p9_angle=s0.x();
    }

  }

  int p9_index= find_near_point( pts_f,max_point,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가?
 

  ///##comented out 10-2-2006##/// vcl_cout<<"p9:"<<pts_f[p9_index]<<vcl_endl<<"-------------------------------"<<vcl_endl;
  P(9,0)=pts_f[p9_index].x();
  P(9,1)=pts_f[p9_index].y();
  P(9,2)=pts_f[p9_index].z();



  //5-26-07//
  //pt07[2][1]  height of vehicle..
  //  
  P(9,0)=max_point.x();P(9,1)=max_point.y();P(9,2)=max_point.z();
  if (vcl_fabs(pt07[2].y()-P(9,2))> 1.5) {  //pt07[2].y()  has solve_z height from bb_box file.
    vcl_cerr<<vcl_fabs(pt07[2].y()-P(9,2))<<vcl_endl;
    P(9,2)+=100;
  }
  //5-26-07// end

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

      ///##comented out 10-2-2006##/// vcl_cout<<ans<<" "<<s0<<vcl_endl;
      if (ans[4]>max) {
        max=ans[4];
        //if (2*ans[0]+ans[1]>max) {
        //   max=2*ans[0]+ans[1];
        max_point_p2_nose.set(s0.x(),s0.y(),s0.z());
        // ///##comented out 10-2-2006##/// vcl_cout<<max<<s0<<vcl_endl;
      }

    }

    int p2_nose_index= find_near_point( pts_f,max_point_p2_nose,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
    ///##comented out 10-2-2006##/// vcl_cout<<"p2_nose:"<<pts_f[p2_nose_index]<<"\n======================="<<vcl_endl;
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
    //vcl_cerr<<V<<vcl_endl;
    // float result=recognizer4(P,height_of_car,V);
    //  float result=recognizer_RG(P,height_of_car,V);


    vcl_vector <int> choice;
    for (unsigned i=22;i<V.size();i++)
      choice.push_back((int) vcl_floor(V[i]));
    float result=tree(P,height_of_car,V,choice);


    return result;

}


////////end main 3//////////////
//////////////////////


//use most recent generation of 3D contour
vcl_string get_recent(vcl_vector <vcl_string> flist[],int i) {
  int N=flist[i].size();
  if (N==0) return "NULL";
  vcl_string to_return =flist[i][N-1];
  return to_return;
}


vcl_vector <vcl_string>  get_from_range( vcl_vector <vcl_string> flist[], int index, int month_start, int month_end, int day_start, int day_end, int time_start, int time_end) 
//void get_from_range(vcl_vector <vcl_string> flist[], int index) 
{

  vcl_vector <vcl_string> to_return;
  // vcl_vector <vul_reg_exp> rlist;
  vcl_string vr_dir,mode;
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

  vcl_cout<<vr_dir<<vcl_endl;

  //vcl_vector <vcl_string> to_return;
  for (unsigned i=0;i<flist[index].size();i++) 
  {
    vcl_string along=flist[index][i];
    //a=vul_file::basename(along);
    vcl_string sub=along.substr(word_count+3);
    int month=atoi(sub.substr(0,2).c_str());
    int day=atoi(sub.substr(3,2).c_str());
    int year=atoi(sub.substr(6,4).c_str());
    int time=atoi(sub.substr(11,2).c_str());

    //// vcl_cout<<month<<" "<<day<<" "<<year<<" "<<time<<vcl_endl;

    if (day>=day_start&&day<=day_end && month>=month_start&&month<=month_end && time>=time_start&&time<=time_end
      &&
      year==2007)
    {
      to_return.push_back(flist[index][i]);

    }
  }
  return to_return;
  //  return rlist;
}


int main(int argc, char* argv[])
{
  vcl_cout<<"--------------------------------------------"<<vcl_endl;
  if (CERR) vcl_cerr<<"--------------------------------------------"<<vcl_endl;
  vcl_cout<<vul_file::get_cwd()<<vcl_endl;
  if (CERR) vcl_cerr<<vul_file::get_cwd();//<<vcl_endl;
  vcl_vector <vcl_string>  flist[4];

  bool recon_flag=0;
  vcl_string vr_dir;
  vcl_string mode;

  int METHODS_NUM=2;
  vcl_vector <vul_reg_exp> rlist;
  for (unsigned i=0;i<METHODS_NUM;i++) {
    if (i==0) { vr_dir="contour_0"; mode="0";}
    else if (i==1) {vr_dir="correct_1";mode="1";}
    else if (i==2) {vr_dir="messy_2";mode="2";}
    else if (i==3) {vr_dir="all_3";mode="3";}
    else ;
    vul_reg_exp r0(vr_dir.c_str());
    rlist.push_back(r0);
  }

  //vcl_string input_file_path="C:/CBB/A4000-4099/4011/sel.";
  vcl_string input_file_path=".";
  vul_file_iterator fn=input_file_path+"/*";//nframes_=5;

  vul_reg_exp year("2007");
  for ( ; fn; ++fn) 
  {
    vcl_string input_file = fn();//
    if (vul_file::is_directory(input_file)&&year.find(input_file)) {

      for (unsigned j=0;j<METHODS_NUM;j++)
        if (rlist[j].find(input_file.c_str()) ){
          flist[j].push_back(input_file);
          if (1) vcl_cout<<j<<": "<<input_file<<vcl_endl;
        }
    }
  }
  //***//vcl_cout<<vul_file::get_cwd()<<"----------------   "<<vcl_endl;


  /*int day_in=atoi(argv[1]);
  int day_out=atoi(argv[2]);
  int time_in =atoi(argv[3]);
  int time_out =atoi(argv[4]);
  */
  for (unsigned i=1;i<METHODS_NUM;i++) {// from case 1
    vcl_string where;
    where=get_recent(flist,i);
    vcl_cout<< where<<vcl_endl;
    //vcl_vector <vcl_string> FL=get_from_range(flist,1, 10,11,9,17, 0,12);
    /* vcl_vector <vcl_string> FL=get_from_range(flist,1, 10,10,day_in,day_out, time_in,time_out);
    vcl_cout<<"================="<<vcl_endl;
    for (unsigned j=0;j<FL.size();j++)
    vcl_cout<<FL[j]<<vcl_endl;
    */


    vcl_vector <vcl_string>  which_to_read;

    if (i==0) 
    {
      vcl_cout<<"***case 0***"<<vcl_endl;
      vcl_string File =where+"/del-AEC-0.wrl";
      vcl_string Filef=where+"/delf-AEC-0.wrl";
      vcl_string Filer=where+"/delr-AEC-0.wrl";

      which_to_read.push_back(File);
      which_to_read.push_back(Filef);
      which_to_read.push_back(Filer);

    }
    if (i==1)
    {
      vcl_cout<<"***case 1***"<<vcl_endl;
      vcl_string File =where+"/del-AEC-1.wrl";
      vcl_string Filef=where+"/delf-AEC-1.wrl";
      vcl_string Filer=where+"/delr-AEC-1.wrl";

      which_to_read.push_back(File);
      which_to_read.push_back(Filef);
      which_to_read.push_back(Filer);
    }
    which_to_read.push_back(where+"/bbox_cam_my.txt");

    vcl_string reference=vul_file::get_cwd();
    vcl_cout<<reference<<"***"<<vcl_endl;
    // vcl_cout<<"afafdsa"<<vcl_endl;
    int SZ=reference.size();
    vcl_cout<<SZ<<vcl_endl;
    int ref_id=atoi(reference.substr(SZ-8,SZ-4).c_str());  
    vcl_cout<<"directory id: "<<ref_id<<vcl_endl;

    ///float result=main2(i,which_to_read);
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
      if (CERR) vcl_cerr<<V[j]<<" ";
    }



    if (CERR) vcl_cerr<<vcl_endl;
    float result=main3(i,which_to_read,V);
    vcl_cout<<result<<vcl_endl;
    //vul_reg_exp r(vul_file::get_cwd());

    int class_id=970;
    if (result==999) 
    {
      vcl_cout<<"999"<<vcl_endl;
      vcl_cerr<<"                   No Decision         "<<result<<vcl_endl;
    }
    if (result<10) class_id=0;
    else if (result<20) class_id=1;
    else if (result<30) class_id=2;
    else class_id=454;

    int real_class_id;
    bool check=table(ref_id,class_id,real_class_id);
    vcl_cout<<"real_class_id "<<real_class_id<<vcl_endl;
    if (check) 
    {
      vcl_cout<<"OOOOO "<<real_class_id<<" "<<result<<vcl_endl;
      if (CERR) vcl_cerr<<"                       OOOOO    "<<real_class_id<<" "<<result<<vcl_endl;
    }
    else {
      vcl_cout<<"XXXXX "<<real_class_id<<" "<<result<<vcl_endl;
      if (CERR) vcl_cerr<<"                         XXXXX     "<<real_class_id<<" "<<result<<vcl_endl;
    }

  }

}

