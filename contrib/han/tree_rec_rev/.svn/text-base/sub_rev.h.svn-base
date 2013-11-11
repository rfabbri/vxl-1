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

vnl_double_3x3 R(0.0);

bool table(int ref_num , int recognized_id, int &real_class_id) {
  vcl_cout<<ref_num<<" "<<recognized_id<<vcl_endl;
  int class_id;
  switch (ref_num) {
case 3786:    { class_id=2; break; }    // small
case 3788:    { class_id=0; break; }    //
case  3790:    { class_id=0; break; }    // check WC
case  3794:    { class_id=2; break; }    // small
case 3795:    { class_id=0; break; }    //
case 3796:    { class_id=1; break; }    //
case 3797:    { class_id=1; break; }    //
              ////////case 3798/del/rename:    { class_id=2; break; }    // ERROR
case 3798:    { class_id=2; break; }    // 
case 3799:    { class_id=2; break; }    // big
case 3801:    { class_id=2; break; }    //


case 3815:    { class_id=1; break; }    //
case 3817:    { class_id=1; break; }    //
case 3820:    { class_id=0; break; }    //
case 3823:    { class_id=0; break; }    //
case 3824:    { class_id=1; break; }    //

case 3827:    { class_id=0; break; }    //

case 3889:    { class_id=0; break; }    // tough
case 3890:    { class_id=1; break; }    // tough
case 3893:    { class_id=1; break; }    // tough
case 3894:    { class_id=0; break; }    // tough

case 3902:    { class_id=1; break; }    //

              //case 3902:    { class_id=1; break; }    //
case 3903:    { class_id=1; break; }    //
              //#case 3905:    { class_id=0; break; }    //
case 3910:    { class_id=0; break; }    //
case 3911:    { class_id=2; break; }    //


case 3913:    { class_id=0; break; }    //
case 3914:    { class_id=0; break; }    //
case 3916:    { class_id=0; break; }    //
case 3917:    { class_id=1; break; }    //
case 3918:    { class_id=0; break; }    //
case 3920:    { class_id=0; break; }    //
case 3923:    { class_id=1; break; }    //


case 4011:    { class_id=0; break; }    //
case 4013:    { class_id=0; break; }    //
case 4021:    { class_id=0; break; }    //
case 4026:    { class_id=1; break; }    //
case 4030:    { class_id=1; break; }    //

case 4031:    { class_id=0; break; }    //
case 4035:    { class_id=0; break; }    //
case 4038:    { class_id=1; break; }    // small
case 4041:    { class_id=0; break; }    //
case 4043:    { class_id=1; break; }    //

case 4046:    { class_id=0; break; }    // police
case 4048:    { class_id=2; break; }    // big van
case 4051:    { class_id=2; break; }    // big van
case 4053:    { class_id=0; break; }    //
case 4058:    { class_id=1; break; }    //

case 4059:    { class_id=0; break; }    //
case 4060:    { class_id=2; break; }    // big van
case 4062:    { class_id=0; break; }    // surfboard
case 4063:    { class_id=1; break; }    // BS =>big suv
case 4064:    { class_id=1; break; }    // jeep!!

case 4067:    { class_id=1; break; }    //
case 4068:    { class_id=1; break; }    //
case 4071:    { class_id=0; break; }    //
case 4073:    { class_id=0; break; }    //
case 4078:    { class_id=0; break; }    //

case 4079:    { class_id=1; break; }    //
case 4080:    { class_id=1; break; }    //
case 4081:    { class_id=2; break; }    //
case 4096:    { class_id=2; break; }    //



case 4198:    { class_id=0; break; }   //  car   @ 
case 4199:    { class_id=1; break; }    // big 1    @
case 4197:    { class_id=1; break; }    // small 1  @
case 4196:    { class_id=0; break; }    //          @
case 4194:    { class_id=0; break; }    // car 205c1   @

case 4193:    { class_id=0; break; }    // 204c1 @
case 4186:    { class_id=0; break; }    // 197c1 @
case 4184:    { class_id=1; break; }    // 195c1 @
case 4183:    { class_id=1; break; }    //  194c1 @
              ////////////////case 4183/sel2:    { class_id=1; break;}    //   194c1 @

case 4182:    { class_id=1; break; }    // small suv  193c1 @
case 4181:    { class_id=0; break; }    // car  !!! same date different camera!!! @
case 4174:    { class_id=0; break; }    // car
case 4173:    { class_id=1; break; }    // suv
case 4172:    { class_id=0; break; }    // car

case 4171:    { class_id=1; break; }    // suv
case 4170:    { class_id=0; break; }    // car
case 4141:    { class_id=1; break; }    // suv
case 4133:    { class_id=2; break; }// bad con but try it, too manay frames. over 100
case 4130:    { class_id=2; break; }    // mini-van // chosen before..

case 4128:    { class_id=2; break; }    // big van tough con
case 4126:    { class_id=0; break; }    // car tough
case 4124:    { class_id=0; break; }    // car nice
case 4118:    { class_id=0; break; }    // police car or taxi
case 4117:    { class_id=1; break; }    // pickup
              //4117:    class_id= pick up

case 4112:    { class_id=1; break; }    // suv tough
case 4110:    { class_id=0; break; }    // car
case 4107:    { class_id=1; break; }    // suv  //chosen before @
case 4106:    { class_id=0; break; }    // car 112c1  @
              //#cd #C:/CBB_/case 4104:    { class_id=0; break; }    // car 110c1

case 4103:    { class_id=0; break; }    // car 109c1
              //#cd #C:/CBB_/case 4102:    { class_id=0; break; }    // car 108c1
              //#case 4101:    { class_id=0; break; }    // car 107c1
case 4100:    { class_id=1; break; }    // suv 106c1


case 4200:    { class_id=0; break; }    //
case 4202:    { class_id=2; break; }    // mini van tough ??
case 4203:    { class_id=1; break; }    // very tough ??
case 4205:    { class_id=0; break; }    //  wagon
case 4206:    { class_id=1; break; }    //

case 4207:    { class_id=0; break; }    //
case 4209:    { class_id=0; break; }    //
case 4210:    { class_id=1; break; }    //  tough check farthest con..
case 4213:    { class_id=2; break; }    // big van
case 4214:    { class_id=1; break; }    //  tough strange just to check  101 frames.

case 4216:    { class_id=1; break; }    //
case 4222:    { class_id=2; break; }    //
case 4223:    { class_id=2; break; }    // tough rear engraved
case 4224:    { class_id=1; break; }    // ??

case 4229:    { class_id=0; break; }    // police car
case 4234:    { class_id=1; break; }    // suv 250
case 4236:    { class_id=1; break; }    // suv 252c1
case 4238:    { class_id=1; break; }    // suv

case 4246:    { class_id=1; break; }    // suv
case 4240:    { class_id=2; break; }    // big van tough
case 4243:    { class_id=2; break; }    // big van  ??
defalut:  
              class_id=983;
              break;
  }

  real_class_id=class_id;

  if (class_id==983) {
    vcl_cout<<"ref id not registerd"<<vcl_endl;
    vcl_cerr<<"ref id not registerd"<<vcl_endl;
  }
  else if (class_id==recognized_id) {
    return true;
 }
 else ;

 return false;

}

/////////////////////////////////////////////////////////////////////
vnl_vector <int> min_max(vcl_vector <vgl_point_3d<double> > pts) {

  int max_z_index,min_z_index,max_x_index,min_x_index;
  double max_z=-10000.0,min_z=10000.0,max_x=-100000.0,min_x=100000.0;
  for (unsigned i=0;i<pts.size();i++) {

    if (pts[i].z()>max_z) {max_z_index=i; max_z=pts[i].z();}
    if (pts[i].z()<min_z) {min_z_index=i; min_z=pts[i].z();}
    if (pts[i].x()>max_x) {max_x_index=i; max_x=pts[i].x();}
    if (pts[i].x()<min_x) {min_x_index=i; min_x=pts[i].x();}

  }
  vnl_vector <int> m(4,0);
  m(0)=max_z_index;
  m(1)=min_z_index;
  m(2)=max_x_index;
  m(3)=min_x_index;
  return m;
}

/////////////////////////////////////////////////////////////////////
int find_near_point_rev(vcl_vector <vgl_point_3d<double> > pts,vgl_point_3d <double> pt,
                             int start_i,int end_i,bool y_flag=true)
{

  double min_t=10000000;
  int min_index=-1;
  if (start_i=-1||end_i==-1) {start_i=0; end_i=pts.size();}
  for (int i=start_i;i<end_i;i++) {

    double tx=pts[i].x()-pt.x();
    double ty=pts[i].y()-pt.y();
    double tz=pts[i].z()-pt.z();

    
    double t=tx*tx+ty*ty+tz*tz;
    if (!y_flag) t=tx*tx+tz*tz;
    if (t<min_t) {
      min_t=t;
      min_index=i;
    }
  }


  return min_index;
}

struct p_list_set 
{
   int probe_number;
   vcl_vector <vnl_matrix <float>  >p_list;
   int class_id;
   float scale;
};

/////////////////////////////////////////////////////////////////////
void save_points(vcl_string save_filename,vcl_vector <struct p_list_set> p_list_set_)
{
  
  vcl_ofstream out(save_filename.c_str());

  if(!out.is_open()){
    std::cerr<<"Cannot open the write selected probes file.\n";
    exit(2);

  }

  out<<p_list_set_.size()<<"\n";
  for (unsigned i=0;i<p_list_set_.size();i++) {
    out<<i<<" "<<p_list_set_[i].p_list.size()<<vcl_endl;
    for( unsigned j=0;j<p_list_set_[i].p_list.size();j++){
      out<<p_list_set_[i].p_list[j]<< vcl_endl ;
    }
  }
  out<<"from 3d detection"<<vcl_endl;
  out.close();
  /*
  out<<"Scale:"<<Scale<<" Box_Add: "<<Box_Add<<"\n "
    <<" cube_inc: "<<cube_inc_<<" d_tube_: "<<d_tube_<<"\n"
    <<" d3_rho_: "<<d3_rho_<<" s_thresh_1: "<<s_thresh_1<<vcl_endl;
  out<<"shift:"<<shift_x_arg<<" "<<shift_y_arg<<" "<<shift_z_arg<<vcl_endl;
  out<<"theta:"<<theta_x<<" "<<theta_y<<" "<<theta_z<<vcl_endl;
  out<<"cut:"<<cut<<" p0 absoulut?: "<<p0_abs_flag<<vcl_endl;
  out<<"epsilon_small_:"<<epsilon_small_<<" distorsion: "<<right_back_shift<<" right up: "<<right_up<<vcl_endl;

  out<<"\n"<<"right spread: "<<right_spread<<" left spread: "<<left_spread<<
    " up_spread: "<<up_spread<<" back_spread: "<<back_spread<<vcl_endl;
  for (unsigned i=0;i<fore_name.size();i++) {
    out<<fore_name[i]<<vcl_endl;
  }
*/

}

////////////////////////////////////////////////////////////////////
read_wrl_file(vcl_vector <vgl_point_3d<double> > & ptl,vcl_string argv,bool rotation=false){
  vcl_ifstream in(argv.c_str());
  //in >> cam_matrix;
  if(in.fail()){
    vcl_cerr << "Failed to read file "<<argv << vcl_endl;
    exit(-4);
    return -1;
  }
  
  // in
  vcl_string hhh="";
  double x,y,z;
  //vcl_vector<vgl_point_3d<double> >ptl;
  while (!in.eof()) {

    in>>hhh;
    if (hhh=="translation") {
      in>>x>>y>>z;
      vgl_point_3d<double> p3d(x,y,z);
      ptl.push_back(p3d);
      /////##comented out 10-2-2006##/// vcl_cout<<p3d<<vcl_endl;
    }
  }
 vcl_cout<<"size: "<<ptl.size()<<vcl_endl;
 if (ptl.size()<20) {
   vcl_cout<<"Not enough points for recognition"<<vcl_endl;
   ////  vcl_cerr<<"Not enough points for recognition"<<vcl_endl;
   exit(555);
 }

  if (!rotation) return 1;

  vcl_vector<vnl_double_3> pts_z;
  for (unsigned i=0;i<ptl.size();i++) {
    vnl_double_3 p(ptl[i].x(),ptl[i].y(),ptl[i].z());

    pts_z.push_back(R*p);
   // ///##comented out 10-2-2006##/// vcl_cout<<R*p<<vcl_endl;
  }

 // ///##comented out 10-2-2006##/// vcl_cout<<"------------------------------------------------------"<<vcl_endl;
  ptl.clear();

  for (unsigned i=0;i<pts_z.size();i++) {
    vgl_point_3d <double> p(pts_z[i](0),pts_z[i](1),pts_z[i](2));

    ptl.push_back(p);
    /////##comented out 10-2-2006##/// vcl_cout<<p<<vcl_endl;
  }
 // ///##comented out 10-2-2006##/// vcl_cout<<"------------------------------------------------------"<<vcl_endl;
  in.close();

return 1;
}

/////////////////////////////////////////////////////////////////////
pts_f_and_r_from_pts(vcl_vector <vgl_point_3d<double> > &pts_f,
                     vcl_vector <vgl_point_3d<double> > &pts_r,
                     vcl_vector <vgl_point_3d<double> > const &pts,
                     char choice)
{
  ///##comented out 10-2-2006##/// vcl_cout<<choice<<vcl_endl;
  //exit(1);
  float max_y=-100000,min_y=100000;
  float max_x=-100000,min_x=100000;
  for (unsigned i=0;i<pts.size();i++) 
  {

    if (max_y<pts[i].y()) max_y=pts[i].y();
    if (min_y>pts[i].y()) min_y=pts[i].y();
    if (max_x<pts[i].x()) max_x=pts[i].x();
    if (min_x>pts[i].x()) min_x=pts[i].x();
  }

  int BIN(100);
  bool flag_r=0,flag_f=0,pre_flag_r=0,pre_flag_f=0;
  double diff_max,diff_min;

  for (unsigned i=0;i<pts.size();i++) 
  {

    if (choice=='y') {
      diff_max=(pts[i].y()-max_y)*(pts[i].y()-max_y);
      diff_min=(pts[i].y()-min_y)*(pts[i].y()-min_y);
    } 
    else if (choice=='x') 
    {
      diff_max=(pts[i].x()-max_x)*(pts[i].x()-max_x);
      diff_min=(pts[i].x()-min_x)*(pts[i].x()-min_x);
    }

    if (diff_max>diff_min) {
      pts_r.push_back(pts[i]);
      flag_r=true; flag_f=false;
    }
    else if (diff_max<diff_min) {
      pts_f.push_back(pts[i]);
      flag_r=false; flag_f=true;
    }
    else {
      ///##comented out 10-2-2006##/// vcl_cout<<"pts dividing error\n"<<vcl_endl;
      exit (-2);
    }
    if (pre_flag_r!=flag_r) 
    {///##comented out 10-2-2006##/// vcl_cout<<"***** switched"<<vcl_endl;
   ///##xxxxxxxxxxxxxx comented out 10-31-2006##/// vcl_cerr<<"***** switched"<<vcl_endl;
        }

    pre_flag_r=flag_r;
    /////##comented out 10-2-2006##/// vcl_cout<<(pts[i].y()-max_y)*(pts[i].y()-max_y)<<" "<<(pts[i].y()-min_y)*(pts[i].y()-min_y)<<vcl_endl;
  }


  ///##comented out 10-2-2006##/// vcl_cout<<pts_f.size()<<"+"<<pts_r.size()<<"="<<pts.size()<<vcl_endl;
  //exit(1);
}

/////////////////////////////////////////////////////////////////////
read_bb_box_file(vcl_vector <vgl_point_3d<double> > & ptl,vcl_string argv){
  vcl_ifstream in(argv.c_str());
  //in >> cam_matrix;
  if(in.fail()){
    vcl_cerr << "Failed to read file "<<argv << vcl_endl;
    return -1;
  }

  // in
  vcl_string hhh="";
  double x,y,z;
  double dummy;
  //vcl_vector<vgl_point_3d<double> >ptl;

  for (unsigned j=0;j<3;j++) {
    in >>hhh;
    ///##comented out 10-2-2006##/// vcl_cout<<hhh<<vcl_endl;
  }

  double temp;
  vnl_double_4x4 RT(0.0);
  for (unsigned i=0;i<4;i++) {
    for (unsigned j=0;j<4;j++) {
      in >> temp;
      RT[i][j]=temp;
    }
  }



  R[0][0]=RT[0][0];R[0][1]=RT[0][1];R[0][2]=RT[0][2];
  R[1][0]=RT[1][0];R[1][1]=RT[1][1];R[1][2]=RT[1][2];
  R[2][0]=RT[2][0];R[2][1]=RT[2][1];R[2][2]=RT[2][2];

  R.normalize_columns();
   vcl_cout<<R<<vcl_endl;

  while (!in.eof()) {

    in>>hhh;
    if (hhh=="Camera") {

      for (unsigned j=0;j<13;j++) in >>dummy;

    }
    else {
      x=atof(hhh.c_str());
      in>>y>>z;
      vgl_point_3d<double> p3d(x,y,z);
      ptl.push_back(p3d);
       vcl_cout<<p3d<<vcl_endl; 
      //exit(1);
    }
  }
  ///##comented out 10-2-2006##/// vcl_cout<<"size: "<<ptl.size()<<vcl_endl;
  in.close();
  //exit(0);
  return 1;
}

