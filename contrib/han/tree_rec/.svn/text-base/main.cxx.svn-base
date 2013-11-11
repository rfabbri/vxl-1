// Some tests for vgl_convex
// Ian Scott, Feb 2004.
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

vnl_double_3x3 R(0.0);

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
int find_near_point(vcl_vector <vgl_point_3d<double> > pts,vgl_point_3d <double> pt,
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
read_wrl_file(vcl_vector <vgl_point_3d<double> > & ptl,char* argv,bool rotation=false){
  vcl_ifstream in(argv);
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
      //vcl_cout<<p3d<<vcl_endl;
    }
  }
  vcl_cout<<"size: "<<ptl.size()<<vcl_endl;

  if (!rotation) return 1;

  vcl_vector<vnl_double_3> pts_z;
  for (unsigned i=0;i<ptl.size();i++) {
    vnl_double_3 p(ptl[i].x(),ptl[i].y(),ptl[i].z());

    pts_z.push_back(R*p);
   // vcl_cout<<R*p<<vcl_endl;
  }

 // vcl_cout<<"------------------------------------------------------"<<vcl_endl;
  ptl.clear();

  for (unsigned i=0;i<pts_z.size();i++) {
    vgl_point_3d <double> p(pts_z[i](0),pts_z[i](1),pts_z[i](2));

    ptl.push_back(p);
    //vcl_cout<<p<<vcl_endl;
  }
 // vcl_cout<<"------------------------------------------------------"<<vcl_endl;
  in.close();

return 1;
}

/////////////////////////////////////////////////////////////////////
pts_f_and_r_from_pts(vcl_vector <vgl_point_3d<double> > &pts_f,
                     vcl_vector <vgl_point_3d<double> > &pts_r,
                     vcl_vector <vgl_point_3d<double> > const &pts,
                     char choice)
{
  vcl_cout<<choice<<vcl_endl;
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
      vcl_cout<<"pts dividing error\n"<<vcl_endl;
      exit (-2);
    }
    if (pre_flag_r!=flag_r) 
    {vcl_cout<<"***** switched"<<vcl_endl;
    vcl_cerr<<"***** switched"<<vcl_endl;}

    pre_flag_r=flag_r;
    //vcl_cout<<(pts[i].y()-max_y)*(pts[i].y()-max_y)<<" "<<(pts[i].y()-min_y)*(pts[i].y()-min_y)<<vcl_endl;
  }


  vcl_cout<<pts_f.size()<<"+"<<pts_r.size()<<"="<<pts.size()<<vcl_endl;
  //exit(1);
}

/////////////////////////////////////////////////////////////////////
read_bb_box_file(vcl_vector <vgl_point_3d<double> > & ptl,char* argv){
  vcl_ifstream in(argv);
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
    vcl_cout<<hhh<<vcl_endl;
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
  vcl_cout<<"size: "<<ptl.size()<<vcl_endl;
  in.close();
  //exit(0);
  return 1;
}

///////////////////////////////////////////////////////////
//
//    recognizer
//
///////////////////////////////////////////////////////////

int recognizer(vnl_matrix <double> P,double HoC,char* argv)
{
  vnl_vector <double> v(3,0);
  vcl_vector <vnl_vector <double> > C0,C1,C2;

  vcl_vector <vcl_vector <vnl_vector <double> > >C;
  double x,y,z;

  x=-179.21471; y= -25.48460;  z= 28.59190;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x=12.97000;  cube_y= 6.39340;  cube_z= 7.46310;             break;
  x=-132.89429; y= -22.26056;  z= 42.49110;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 7.18900;  cube_y= 4.76700;  cube_z= 4.86460;             break;
  x=-102.15789; y= -23.64121;  z= 42.24343;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 6.18280;  cube_y= 4.44620;  cube_z= 3.01100;     break;
  x=-98.18877;  y= -30.71624;  z= 27.09789;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 5.98590;  cube_y= 4.40370;  cube_z= 3.62240;             break;
  x=-81.45436;  y= -22.11407;  z= 41.59693;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 8.65970;  cube_y= 2.83270;  cube_z= 3.51150;             break;
  x=-57.59297;  y= -28.70307;  z= 26.55219;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x=10.84180;  cube_y= 4.76220;  cube_z= 4.84040;             break;
  x=-12.12567;  y= -27.03229;  z= 18.72117;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 4.24410;  cube_y= 3.92240;  cube_z= 2.59930;       break;
  x=-12.23820;  y=  26.67023;  z= 18.98727;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 4.93660;  cube_y= 3.81930;  cube_z= 1.78740;             break;
  x=-57.12216;  y=  28.55730;  z= 26.44823;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 9.86200;  cube_y= 4.98380;  cube_z= 2.76390;             break;
  x=-81.82614;  y=  22.49641;  z= 41.58564;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 7.21800;  cube_y= 3.10390;  cube_z= 2.85120;             break;

  x= -172.91833; y=  -30.13112;  z=  30.19587;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.38700 ;  cube_y=   7.46940;  cube_z=    3.44540;            break;
  x= -162.98083; y=  -23.80763;  z=  50.85197;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=11.55100;  cube_y=    8.99240;  cube_z=    5.95980;            break;
  x= -95.42235;  y=  -26.12093;  z=  48.01413;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=12.22960 ;  cube_y=   8.72150;  cube_z=    2.70570;            break;
  x= -94.42485;  y=  -30.89255;  z=  31.54360;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.12040;  cube_y=    6.78130;  cube_z=    2.97720;            break;
  x= -73.21947;  y=  -24.48868;  z=  48.17738;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.70360;  cube_y=    7.92530;  cube_z=    3.49310;            break;
  x= -54.77978;  y=  -28.78197;  z=  32.33453;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x= 9.52330;  cube_y=    7.59310;  cube_z=    1.74170;            break;
  x= -9.01974;   y=  -27.10253;  z=  24.14972;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x= 8.19839;  cube_y=    7.95370;  cube_z=   11.26400;            break;
  x= -8.92432;   y=   26.29308;  z=  24.41268;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x= 7.36289;  cube_y=    7.28910;  cube_z=   11.13400;            break;
  x= -54.49910;  y=   28.81713;  z=  32.24610;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.78870 ;  cube_y=   8.53330;  cube_z=    1.81310;   break;
  x= -73.28730 ; y=   24.19770;  z=  48.27782;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.53240 ;  cube_y=   7.51110;  cube_z=    4.48150;            break;

  x= -182.70140; y=  -33.00098;  z=  36.03196;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 11.32500;  cube_y=  2.66220;  cube_z=  5.30500;               break;
  x= -172.25520; y=  -25.58108;  z=  57.31650;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 10.41800;  cube_y=  8.78890;  cube_z=  6.26010;               break;
  x= -93.23690;  y=  -28.57264;  z=  54.92082;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 18.29850;  cube_y=  8.09550;  cube_z=  4.93300;               break;
  x= -92.03020;  y=  -35.13788;  z=  35.35490;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 15.75600;  cube_y=  5.29950;  cube_z=  4.74010;               break;
  x= -71.11060;  y=  -25.93910;  z=  54.39806;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 17.44500;  cube_y=  8.24900;  cube_z=  4.54850;               break;
  x= -45.97132;  y=  -31.52034;  z=  35.75554;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 14.54310;  cube_y=  5.06220;  cube_z=  2.10600;               break;
  x= -9.98616;   y=  -25.91536;  z=  23.66608;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 6.10600;   cube_y=  9.14360;  cube_z= 15.62190;               break;
  x= -9.95002;   y=   25.62242;  z=  23.88524;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 6.10850;   cube_y=  9.14360;  cube_z= 15.62690;               break;
  x= -46.07720;  y=   31.31362;  z=  35.90390;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 14.54140;  cube_y=  5.06220;  cube_z=  3.15410;               break;
  x= -71.21410;  y=   25.79662;  z=  54.47188;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 17.44350;  cube_y=  8.24900;  cube_z=  4.91610;               break;

  C.push_back(C0);C.push_back(C1);C.push_back(C2);
 
  /*vcl_cout<<"x01 distance :" ;
  for (unsigned i=0;i<3;i++) 
  {

    vcl_cout<<i<<":" ;
    vcl_cout<<C[i][1][0]-C[i][0][0]<<" ";
  }*/

  double R_HoC=0.0;
  vcl_cout<<"HoC :" ;
  for (unsigned i=0;i<3;i++) 
  {

    vcl_cout<<i<<":" ;
    vcl_cout<<C[i][1][2]/12.0<<" ";
  }

  vcl_cout<<vcl_endl;
  vcl_cout<<"LEN :" ;
  for (unsigned i=0;i<3;i++) 
  {

    vcl_cout<<i<<":" ;
    vcl_cout<<vcl_fabs(C[i][0][0]-C[i][7][0])/12.0<<" ";
  }

  vcl_cout<<vcl_endl;

  double CR_x01_07[3];
  vcl_cout<<"x01/x07 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    vcl_cout<<i<<":" ;
    CR_x01_07[i]=(C[i][1][0]-C[i][0][0])/(C[i][7][0]-C[i][0][0]);
    vcl_cout<<CR_x01_07[i]<<" ";
  }
  vcl_cout<<vcl_endl;

  double CR_x19_07[3];
  vcl_cout<<"x19/x07 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    vcl_cout<<i<<":" ;
    CR_x19_07[i]=(C[i][9][0]-C[i][1][0])/(C[i][7][0]-C[i][0][0]);
    vcl_cout<<CR_x19_07[i]<<" ";
  }
  vcl_cout<<vcl_endl;

  double CR_x79_07[3];
  vcl_cout<<"x79/x07 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    vcl_cout<<i<<":" ;
    CR_x79_07[i]=vcl_abs(C[i][9][0]-C[i][7][0])/(C[i][7][0]-C[i][0][0]);
    vcl_cout<<CR_x79_07[i]<<" ";
  }
  vcl_cout<<vcl_endl;


  double CR_z37_39[3];
  vcl_cout<<"z37/z39 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    vcl_cout<<i<<":" ;
    CR_z37_39[i]=vcl_abs(C[i][7][2])/(C[i][9][2]);
    vcl_cout<<CR_z37_39[i]<<" ";
  }
  vcl_cout<<vcl_endl;

  double CR_x07_z39[3];
  vcl_cout<<"z39/x07 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    vcl_cout<<i<<":" ;
    CR_x07_z39[i]=vcl_abs((C[i][9][2])/((C[i][7][0]-C[i][0][0])));
    vcl_cout<<CR_x07_z39[i]<<" ";
  }
  vcl_cout<<vcl_endl;


  vcl_cout<<"-------------\n"<<"input:"<<" "<<" \n--------------"<<vcl_endl;
  double x01=vcl_fabs(P(1,0)-P(0,0));
  double x07=vcl_fabs(P(7,0)-P(0,0));
  double x79=vcl_fabs(P(7,0)-P(9,0));
  double real_length=vcl_fabs(P(0,0)-P(7,0));
  vcl_cout<<"HoC:"<<HoC-0.9<<vcl_endl;
  vcl_cout<<"length: "<<real_length<<vcl_endl;

  vcl_cout<<"x01/x07 ratio  : "<<x01<<"  ratio "<<x01/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  vcl_cout<<"x19/x07 ratio  : "<<vcl_fabs(P(1,0)-P(9,0))/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  vcl_cout<<"x79/x07 ratio  : "<<x79/x07<<vcl_endl ;
  vcl_cout<<"z3             : "<<P(3,2)<<vcl_endl;
  /////////bug///vcl_cout<<"z7/z9   ratio  : "<<P(3,2)/HoC<<vcl_endl;
  vcl_cout<<"z7/z9   ratio  : "<<P(7,2)/HoC<<vcl_endl;


  vcl_cout<<"z9/x07  ratio  : "<<(HoC-.9)/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;




  if (x01/vcl_fabs(P(0,0)-P(7,0))>.20 && HoC-.9 < 4.5) return 00;
  if (x01/vcl_fabs(P(0,0)-P(7,0))>.16 && HoC-.9 < 3.5) return 01; // big offset 0 and 1
  if (HoC-.9 < 3.0) return 02;                                    // small height
  if ((HoC-.9)/vcl_fabs(P(0,0)-P(7,0)) < .2) return 03;           //height/length
  //if (real_length<12) return 02;

  if (HoC-.9>5.5) return 23;  // if too high, van..
 // if (P(3,2)>2.0) return 10;  // rear botom high suv

 // if (P(3,2)<1.0) return 20;  //  rear botom low mini van
  if (P(2,2)>2.5 && P(3,2)>2.5) return 11; 
  if (P(2,2)>3.5) return 12;////danger????

  if (P(3,2)<2.0&&P(2,2)<2.5) return 21;


  //if (x01/vcl_fabs(P(0,0)-P(7,0))>.3 && HoC-.9 < 4.5) return 05;
 // if ((P(7,0)-.9)/(HoC-.9)>.5) return 19;
 // if ((P(7,0)-.9)/(HoC-.9)<.4) return 29;
  if (P(7,2)/HoC>.5) return 19;
  if (P(7,2)/HoC<.4) return 29;
  if (P(3,2)>2.0) return 10;  // rear botom high suv

  if (P(3,2)<1.0) return 20;  //  rear botom low mini van





  double error0,error1,error2,error3,error4,error5;
  //double error0
  return (99);
}
/////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  vcl_cout << "Test dtube\n";

  vgl_point_3d <double> pts0(1,0,0);

  vcl_cout << "Inputs\n";
  // Simple triangle
  vcl_vector<vgl_point_3d<double> > pts;
  pts.push_back( vgl_point_3d<double>(0.0, 0, 0.0) );
  pts.push_back( vgl_point_3d<double>(3.0, 0, .74) );
  pts.push_back( vgl_point_3d<double>(3.0, 0, .75) );
  pts.push_back( vgl_point_3d<double>(3.0, 0, 0.0) );
  pts.push_back( vgl_point_3d<double>(2.0, 0, 1.0) );
  pts.push_back( vgl_point_3d<double>(0.0, 0, 5.0) );

  //for (unsigned i=0; i < pts.size(); ++i)
  //  vcl_cout << '(' << pts[i].x() <<','<<pts[i].z()<<") ";
 // vcl_cout << vcl_endl;

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
  vcl_cout << "Outputs\n";
  poly.print(vcl_cout);
  vcl_cout << vcl_endl;

  TEST("inside", poly.contains( pts[0] ), true );
  TEST("inside", poly.contains( pts[1] ), true );
  TEST("inside", poly.contains( pts[3] ), true );
  TEST("inside", poly.contains( pts[4] ), true );*/


  
  if(argc!=4){
    //vcl_cout << "Usage: "<< argv[0] << " <wrl_file>"<< vcl_endl;
   // vcl_cout << "   or: "<< argv[0] << " <wrl_file> <front_wrl_file> <rear_wrl_file>"<< vcl_endl;
    //exit(0);
  }

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
  read_bb_box_file(pt07,"bbox_cam_my.txt");
  
  read_wrl_file(ptl,"del.wrl",true);

  if (argc==1) {
    read_wrl_file(pts_f,"delf.wrl",true);
    read_wrl_file(pts_r,"delr.wrl",true);
  }

  /////////////////////////////////////////////////////



  vnl_matrix  <double> P(10,3,0.0);
  ///////////////////////////////
  /////   probe 0 & 7  //////////
  ///////////////////////////////

   if (argc!=1) {
    pts_f_and_r_from_pts(pts_f,pts_r,ptl,*argv[1]);

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
    
    vcl_cout<<ans<<" "<<s0<<vcl_endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point0.set(s0.x(),s0.y(),s0.z());
    }

  }
  
  int p0_index= find_near_point( pts_r,max_point0,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
  vcl_cout<<"p0:->from bbox"<<P(0,0)<<" "<<P(0,1)<<" "<<P(0,2)<<" "<<pts_r[p0_index]<<vcl_endl<<"-------------------------------"<<vcl_endl;
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
    
    vcl_cout<<ans<<" "<<s0<<vcl_endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point.set(s0.x(),s0.y(),s0.z());
    }

  }
  
  int p1_index= find_near_point( pts_r,max_point,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
  vcl_cout<<"p1:"<<pts_r[p1_index]<<vcl_endl<<"-------------------------------"<<vcl_endl;
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
    vcl_cout<<pts_r[i]<<ans<<vcl_endl;
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
    
    vcl_cout<<ans<<" "<<s0<<vcl_endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point_p3.set(s0.x(),s0.y(),s0.z());
    }

  }
  
  int p3_index= find_near_point( pts_r,max_point_p3,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
  vcl_cout<<"p3:"<<pts_r[p3_index]<<vcl_endl<<"-------------------------------"<<vcl_endl;
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
    
    vcl_cout<<ans<<" "<<s0<<vcl_endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point.set(s0.x(),s0.y(),s0.z());
      x_p9_angle=s0.x();
    }

  }
  
  int p9_index= find_near_point( pts_f,max_point,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가?
  vcl_cout<<"p9:"<<pts_f[p9_index]<<vcl_endl<<"-------------------------------"<<vcl_endl;
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

      vcl_cout<<ans<<" "<<s0<<vcl_endl;
      if (ans[4]>max) {
       max=ans[4];
     //if (2*ans[0]+ans[1]>max) {
     //   max=2*ans[0]+ans[1];
        max_point_p2_nose.set(s0.x(),s0.y(),s0.z());
       // vcl_cout<<max<<s0<<vcl_endl;
      }

    }

    int p2_nose_index= find_near_point( pts_f,max_point_p2_nose,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가? no
    vcl_cout<<"p2_nose:"<<pts_f[p2_nose_index]<<"\n======================="<<vcl_endl;
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

    int result=recognizer(P,height_of_car,argv[0]);
    vcl_cout<<"************************************\n"<<result<<"        "<<vul_file::get_cwd()<<
      "\n************************************"<<vcl_endl;
    vcl_cout<<P<<vcl_endl;
    /*
    t1.set(1,0,-1);
    t2.set(0,0,-1);
    //tube aT(pts0,t1,t2,t3,t4,radius,length,1,0,0,0);
    for (unsigned i=0;i<20;i++) // search just 10 points..
    {
    //for (double t1z=0;t1z<1;t1z+=.1) {
    if (vcl_fabs(pts_r[i].z()-pts_r[0].z()) <.5)
    {



    double r=.3;
    tube p1(pts_r[i],t1,t2,t3,t4,r,length,1,0,0,0);

    //p1.set_length(t1_l,4.0,4.0, 4.0);
    p1.set_length(3.0,4.0,4.0, 4.0);

    vnl_vector <int> ans=p1.search(pts_r,pts_r[i]);
    vcl_cout<<ans<<vcl_endl;
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
    
    vcl_cout<<ans<<" "<<s0<<vcl_endl;
    if (ans[4]>max) {
      max=ans[4];
      max_point.set(s0.x(),s0.y(),s0.z());
    }

  }
  
  int p9_angle_index= find_near_point( pts_f,max_point,-1,-1,false);//  한평면에서 최소거리의 점은 그 평면이 아니어도 유지되는가?
  vcl_cout<<"p9 angle:"<<pts_f[p9_angle_index]<<vcl_endl;

  vgl_vector_3d <double>a1=ep7-pts_f[p9_angle_index];
  vgl_vector_3d <double>a2=ep9-pts_f[p9_angle_index];

  double dp=dot_product(a1,a2);
  vcl_cout<<"dot_product(a1,a2)  "<<a1.length()<<" "<<a2.length()<<" "<<acos(dp/(a1.length()*a2.length()))*180/vnl_math::pi<<vcl_endl;
  vcl_cout <<"a1(hood)" <<a1<<vcl_endl;

  //vcl_cout<<"angle :" <<
  //P(9,0)=pts_f[p9_index].x();
  //P(9,1)=pts_f[p9_index].y();
  //P(9,2)=pts_f[p9_index].z();

   //vcl_cerr<<" vcl_cerr_test"<<vcl_endl;

   //vcl_cerr<<vul_file::dirname(argv[0]);
  vcl_cerr<<vul_file::get_cwd()<<"----------------   "<<result<<vcl_endl;
   //vcl_string input_file_path=".";
  // vcl_string aaa=vul_file::strip_directory(input_file);
   //   vcl_string nums1=vul_file::strip_extension(aaa);

  exit(1);
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
  //unsigned int p789_index=find_near_point(pts_f,p9_from_calibartion,1,pts_f.size());

  vgl_point_3d <double> p7(P(7,0),P(7,1),P(7,2));
  unsigned int p7_index=find_near_point(pts_f,p7,1,pts_f.size());
  vcl_cout<<p7_index<<vcl_endl;
  for (unsigned i=0;i<2;i++) // search just 10 points..
  {

    //for (unsigned j=p789_index;j<p789_index+1 ;j++) { // near tha p9fC points..
    for (unsigned j=p7_index;j<p7_index+1 ;j++) { // near tha p9fC points..
      for (unsigned k=i+1;k<j;k++) {

       // vcl_cout<<pts_f[i]<<pts_f[k]<<pts_f[j]<<vcl_endl;
        t1.set(pts_f[i].x()-pts_f[k].x(),pts_f[i].y()-pts_f[k].y(),pts_f[i].z()-pts_f[k].z());
        t2.set(pts_f[j].x()-pts_f[k].x(),pts_f[j].y()-pts_f[k].y(),pts_f[j].z()-pts_f[k].z());


        tube p789(pts_f[k],t1,t2,t3,t4,.1,-1,1,1,0,0);
        
        vnl_vector <int> ans=p789.search(pts_f,pts_f[k]);
        vcl_cout<<ans<<vcl_endl;
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
  vcl_cout<<"max_lin "<<max_linear<<" "<<max_lin_point<<vcl_endl;
  vcl_cout<<"max_sqr "<<max_square<<" "<<max_square_point<<vcl_endl;



}

