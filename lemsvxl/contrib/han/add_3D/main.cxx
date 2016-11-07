// Some tests for vgl_convex
// Ian Scott, Feb 2004.
#include <testlib/testlib_test.h>
#include <vgl/vgl_convex.h>
#include <vcl_iostream.h>
#include <vnl/vnl_matrix.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_rotation_matrix.h>
#include <vnl/vnl_inverse.h>


#include <vnl/vnl_double_3.h>
#include <vul/vul_file.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vcl_iosfwd.h>
#include <vgl/algo/vgl_line_2d_regression.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>
vnl_double_3x3 R(0.0);
double scale_=1.0;
float mean_x1_,mean_x2;
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
  
}

////////////////////////////////////////////////////////////////////
read_wrl_file(float rev,vcl_vector <vgl_point_3d<double> > & ptl,char* argv,bool rotation=false){
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
      //vcl_cout<<x<<" "<<rev*x<<vcl_endl;
      vgl_point_3d<double> p3d(rev*x,y,z);
      ptl.push_back(p3d);
      //vcl_cout<<p3d<<vcl_endl;
    }
  }
  vcl_cout<<"size: "<<ptl.size()<<vcl_endl;

  return 1;
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



void write_vrml_2(vcl_ofstream& out,//const vcl_string& filename,
                vcl_vector<vgl_point_3d<double> > pts_3d)
{

//  vcl_ofstream out(filename.c_str());
//    if(!out.is_open()){
//      std::cerr<<"Cannot open the input file.\n";
//      return;
//    }
    
    
    
    out <<      "#VRML V2.0 utf8\n";
    out <<      "Background { \n";
    out <<      "    skyColor [ 1 1 1 ]\n";
    out <<      "    groundColor [ 1 1 1 ]\n";
    out <<      "}\n";
    out <<      "PointLight {\n";
    out <<      "    on FALSE\n";
    out <<      "    intensity 1 \n";
    out <<      "ambientIntensity 0 \n";
    out <<      "color 1 1 1 \n";
    out <<      "location 0 0 0 \n";
    out <<      "attenuation 1 0 0 \n";
    out <<      "radius 100  \n";
    out <<      "}\n";
    out <<      "Shape {\n";
    out <<      "   #make the points white\n";
    out <<      "    appearance Appearance {\n";
    out <<      "       material Material { emissiveColor 1 0 0 }\n";
    out <<      "   } \n";
    out <<      "   geometry PointSet {\n";
    out <<      "      coord Coordinate{\n";
    out <<      "         point[\n";
    out <<      "         ]\n";
    out <<      "      }\n";
    out <<      "      color Color { color [ 1 1 1 ] }\n";
    out <<      "   }\n";
    out <<      "}\n";
    
    
    float xx,yy,zz;
   float red=0,green=1,blue=1;
        
    double radius=0.2;
    blue=1.0;

  for (unsigned i=0;i <pts_3d.size();i++) {
    //point_3d_mean_pts[i].set(scale_*point_3d_mean_pts[i].x(),scale_*point_3d_mean_pts[i].y(),scale_*point_3d_mean_pts[i].z());
    //vcl_cout<<point_3d_mean_pts[i]<<vcl_endl;






   

     out<<"Transform {  \n";
          out<<"translation "<<pts_3d[i].x()<<" "<<pts_3d[i].y()<<" "<<pts_3d[i].z()<< "\n";  
          out<<"Shape { \n";
          out<<"appearance Appearance { \n";
          //out<<" material Material { diffuseColor "<<r*(j+1)/15 <<" "<<g*(13-j)/13 <<" "<<b*(12+j)/24 <<"}\n";

          out<<" material Material { diffuseColor "<<red <<" "<<green <<" "<<blue <<"}\n";


          out<<" }   \n";
          out<<"  geometry  Sphere{  \n"; 
          //out<<" radius "<< radius*(j+5)/7 << "}  \n"; 
          out<<" radius "<< radius*scale_ << "}  \n"; 
          out<<" } \n"; 
          out<<"} \n"; 

  }


  vcl_cout<<"#### write_vrml_2 end ####"<<vcl_endl;
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


//
//find nearest point
//
float fnp(vcl_vector <vgl_point_3d<double> > pts,vgl_point_3d <double> pt,float shift,
                             bool y_flag=false)
{

  float min_t=10000000;
  int min_index=-1;
  //if (start_i=-1||end_i==-1) {start_i=0; end_i=pts.size();}
  for (int i=0;i<pts.size();i++) {

    double tx=pts[i].x()-pt.x()-shift;
    double ty=pts[i].y()-pt.y();
    double tz=pts[i].z()-pt.z();

    
    double t=tx*tx+ty*ty+tz*tz;
    if (!y_flag) t=tx*tx+tz*tz;
    if (t<min_t) {
      min_t=t;
      min_index=i;
    }
  }


  return min_t;
}


float shift_error(vcl_vector <vgl_point_3d<double> > &pts_fix,
                     vcl_vector <vgl_point_3d<double> > &pts_move,
                     //vcl_vector <vgl_point_3d<double> > const &pts,
                     float shift)
{
    float error=0;
    for (unsigned i=0;i<pts_move.size();i++) {
        //float del_x=pts_move[fns(pts_fix,pts_move[i])].x()-pts_fix[i].x();
        //float del_z=pts_move[fns(pts_fix,pts_move[i])].z()-pts_fix[i].z();
        //float del_error=del_x*del_x+del_y*del_y;
        float del_error=fnp(pts_fix,pts_move[i],shift);
        error+=del_error;
    }

    return error;
}

float mean_shift(vcl_vector <vgl_point_3d<double> > &pts_fix,
                 vcl_vector <vgl_point_3d<double> > &pts_move)
{
    float mean_x1=0.0,mean_x2=0.0;
    for (unsigned i=0;i<pts_fix.size();i++) {
       mean_x1+=pts_fix[i].x();
       
    }
    for (unsigned i=0;i<pts_move.size();i++) {
        mean_x2+=pts_move[i].x();
    }
    float shift=mean_x1/(float)pts_fix.size()-mean_x2/(float)pts_move.size();
   

    vcl_cout<<shift<<" "<<pts_fix.size()<<" "<<pts_move.size()<<vcl_endl; 
    return shift;
}
 


void rot_pts(vcl_vector <vgl_point_3d<double> > &ptl,float rev=1.0) 
{

    vnl_double_3x3 rot=R;
    vcl_cout<<rot<<vcl_endl;
    //vnl_double_3x3 inv_rot = rot.transpose();
    vnl_double_3x3 inv_rot = vnl_inverse(rot);
    vcl_vector<vnl_double_3> pts_z;
    for (unsigned i=0;i<ptl.size();i++) {
      vnl_double_3 p(ptl[i].x(),ptl[i].y(),ptl[i].z());

      vcl_cout<<ptl[i]<<vcl_endl;
      pts_z.push_back(inv_rot*p);
      vcl_cout<<rot*p<<vcl_endl;
      ptl[i].set(rev*pts_z[i][0],pts_z[i][1],pts_z[i][2]);
    }
}
/////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{

  
  if(argc!=4){
    //vcl_cout << "Usage: "<< argv[0] << " <wrl_file>"<< vcl_endl;
   // vcl_cout << "   or: "<< argv[0] << " <wrl_file> <front_wrl_file> <rear_wrl_file>"<< vcl_endl;
    //exit(0);
  }

  //=========================== load the files ==========================

  /////////////////////////////////////////
  /////////////////////////////////////////
  vcl_vector<vgl_point_3d<double> >ptl0;
  vcl_vector<vgl_point_3d<double> >pts_f0;
  vcl_vector<vgl_point_3d<double> >pts_r0;
  vcl_vector<vgl_point_3d<double> >ptl1;
  vcl_vector<vgl_point_3d<double> >pts_f1;
  vcl_vector<vgl_point_3d<double> >pts_r1;
  vcl_vector<vgl_point_3d<double> >ptl2;
  vcl_vector<vgl_point_3d<double> >pts_f2;
  vcl_vector<vgl_point_3d<double> >pts_r2;
 
  read_wrl_file(1.0,ptl0,  "C:/CBB_/A3900-3999/3903/sel/correct_1_10-31-2006-03PM/del-AEC-all-1.wrl");
  read_wrl_file(1.0,pts_f0,"C:/CBB_/A3900-3999/3903/sel/correct_1_10-31-2006-03PM/delf-AEC-1.wrl");
  read_wrl_file(1.0,pts_r0,"C:/CBB_/A3900-3999/3903/sel/correct_1_10-31-2006-03PM/delr-AEC-1.wrl");
  vcl_vector <vgl_point_3d<double> >  ppp;


  
  /*read_wrl_file(-1.0,ptl0,  "C:/CBB_/A3800/3863/sel/correct_1_12-05-2006-09PM/del-AEC-all-1.wrl");
  read_wrl_file(-1.0,pts_f0,"C:/CBB_/A3800/3863/sel/correct_1_12-05-2006-09PM/delf-AEC-1.wrl");
  read_wrl_file(-1.0,pts_r0,"C:/CBB_/A3800/3863/sel/correct_1_12-05-2006-09PM/delr-AEC-1.wrl");
  read_bb_box_file(ppp,"C:/CBB_/A3800/3863/sel/correct_1_12-05-2006-09PM/bbox_cam_my.txt");
  */

  read_bb_box_file(ppp,"C:/CBB_/A3900-3999/3903/sel/correct_1_10-31-2006-03PM/bbox_cam_my.txt");
  rot_pts(ptl0);
  rot_pts(pts_f0);
  rot_pts(pts_r0);

  read_wrl_file(-1.0,ptl1,  "C:/CBB_/A3800/3863/sel/correct_1_12-05-2006-09PM/del-AEC-all-1.wrl");
  read_wrl_file(-1.0,pts_f1,"C:/CBB_/A3800/3863/sel/correct_1_12-05-2006-09PM/delf-AEC-1.wrl");
  read_wrl_file(-1.0,pts_r1,"C:/CBB_/A3800/3863/sel/correct_1_12-05-2006-09PM/delr-AEC-1.wrl");
  read_bb_box_file(ppp,"C:/CBB_/A3800/3863/sel/correct_1_12-05-2006-09PM/bbox_cam_my.txt");
  rot_pts(ptl1,-1.0);
  rot_pts(pts_f1,-1.0);
  rot_pts(pts_r1,-1.0);


  float mean_sh=mean_shift(ptl0,ptl1);

  float min_sh=0.0,min_err=100000000000000.0;
  for (float sh=mean_sh-10.0;sh<mean_sh+10.0;sh+=0.1) {

      float err=shift_error( ptl0,ptl1,sh);

      if (err<min_err) {
          min_err=err;
          min_sh=sh;
      }
      vcl_cout<<sh<<": "<<err<<vcl_endl;
  }
                     
  

  for (unsigned i=0;i<ptl0.size();i++) {
      ptl2.push_back(ptl0[i]);
  }
  for (unsigned i=0;i<ptl1.size();i++) {
      vgl_point_3d <double> a(ptl1[i].x()+min_sh,ptl1[i].y(),ptl1[i].z());
      ptl2.push_back(a);
  }


  vcl_string vrml_file ="a.wrl";
      //vcl_string vrml_filef=out_file_dir+"//"+vr_dir+"//"+"delf-AEC-"+mode+".wrl";
      //vcl_string vrml_filer=out_file_dir+"//"+vr_dir+"//"+"delr-AEC-"+mode+".wrl";
      //vcl_string vrml_file3=out_file_dir+"//"+vr_dir+"//"+"del-AEC-all-"+mode+".wrl";

      vcl_ofstream vrml(vrml_file.c_str());
  write_vrml_2(vrml,ptl2);



  //vcl_vector <vgl_point_3d<double> >  pt07;
 // read_bb_box_file(pt07,"bbox_cam_my.txt");
  
 
  /////////////////////////////////////////////////////




 //  if (argc!=1) {
//    pts_f_and_r_from_pts(pts_f,pts_r,ptl,*argv[1]);

//  }

  exit(1);


}

