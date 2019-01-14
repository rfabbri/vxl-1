#define MODE 2
#define CRHM false
#define ERASE_FOR_NOW_5_5_2007 //
#define CENTERING_SCALE 50
#include <testlib/testlib_test.h>
#include <vgl/vgl_convex.h>
#include <iostream>
#include <vnl/vnl_matrix.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_rotation_matrix.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_double_3x4.h>

#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_2.h>
#include <vul/vul_file.h>
#include <vector>
#include <string>
//#include <std::ifstream.h>
#include <iostream>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <iosfwd>
#include <vgl/algo/vgl_line_2d_regression.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>

#include <cstring>
#include <string>
//#include <fstream>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_reg_exp.h>
#include <vul/vul_string.h>

#include <vnl/vnl_numeric_traits.h>

#include <vul/vul_psfile.h>


#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>

double add_0_psh_,add_1_psh_;
int ni_psh_,nj_psh_;
double bottom_;
///////////

void water_fill(std::vector< vnl_double_2 >p,std::vector< vnl_double_2 >&points0,std::vector< vnl_double_2 >&points1)
{

  int index;
  double max=-1.0,min=1000000000000000000.0;
  for (unsigned i=1;i<p.size(); i++) 
  {
    double a=std::fabs(p[i][1]-p[i-1][1]);
    if (a>max) {
      index=i;
      max=a;
    }
  }
  std::cout<<"separating points to 2 groups: "<<max<<" "<<index<<std::endl;
  std::vector<vgl_point_2d <double> >g0,g1;

  double max_z0=-10000000000.0,max_z1=-1000000000.0;
  for (unsigned i=0;i<index;i++)
  {
    vgl_point_2d <double> a(p[i][0],p[i][1]);
    g0.push_back(a);
    if (p[i][1]>max_z0) {
        max_z0=p[i][1]; 
      }
    
  }
  for (unsigned i=index;i<p.size();i++)
  {
    vgl_point_2d <double> a(p[i][0],p[i][1]);
    g1.push_back(a);
    if (p[i][1]>max_z1) {
        max_z1=p[i][1]; 
      }
  }

  /////////////// separation done.

  std::vector <vgl_point_2d<double> > p0l,p1l;
  for (double water_level=bottom_;water_level<15.0&&water_level<max_z0&&water_level<max_z1; water_level+=.1) {
    vgl_line_2d<double> l;

   
    int index0=-1,index1=-1;
    double min_d0=10000000.0;
    double min_d1=10000000.0;
    l.set(0,1,-water_level); //q.set(0,1); p = vgl_closest_point(l,q);
    //vgl_distance(vgl_line_2d<T> const& l,vgl_point_2d<T> const& p);
    for (unsigned i=0;i<g0.size();i++)
    {
      double d0=vgl_distance( l,g0[i]);
      if (d0<min_d0) {
        min_d0=d0; index0=i;
      }
    }
    for (unsigned i=0;i<g1.size();i++)
    {
      double d1=vgl_distance( l,g1[i]);
      if (d1<min_d1) {
        min_d1=d1; index1=i;
      }
    }
   vgl_point_2d <double> p0 = vgl_closest_point(l,g0[index0]);
   vgl_point_2d <double> p1 = vgl_closest_point(l,g1[index1]);
    
   std::cout<<"water level: "<<water_level<<" "<<p0.x()-p1.x()<<std::endl;
   p0l.push_back(p0);
   p1l.push_back(p1);
  }

  //scale and shift it for printing



  for (unsigned i=0;i<p0l.size();i++)
  {
    p0l[i].x()-=add_0_psh_;
    p0l[i].y()-=add_1_psh_;
    p0l[i].x()*=CENTERING_SCALE;
    p0l[i].y()*=CENTERING_SCALE;
  }

 
  for (unsigned i=0;i<p1l.size();i++)
  {
    p1l[i].x()-=add_0_psh_;
    p1l[i].y()-=add_1_psh_;
    p1l[i].x()*=CENTERING_SCALE;
    p1l[i].y()*=CENTERING_SCALE;
  }


  for (unsigned i=0;i<p0l.size();i++)
  {
    p0l[i].x()+=+add_0_psh_+ni_psh_/2.0;
    p0l[i].y()+=+add_1_psh_+nj_psh_/2.0;

    vnl_double_2 a(p0l[i].x(),p0l[i].y());
    points0.push_back(a);
    //std::cout<<ll2[i]<<std::endl;
  }

   for (unsigned i=0;i<p1l.size();i++)
  {
    p1l[i].x()+=+add_0_psh_+ni_psh_/2.0;
    p1l[i].y()+=+add_1_psh_+nj_psh_/2.0;

    vnl_double_2 a(p1l[i].x(),p1l[i].y());
    points1.push_back(a);
    //std::cout<<ll2[i]<<std::endl;
  }



}


//get from the reading line_in
vnl_vector<double>  get_number( std::string line_in,
                               int ix0,int ix1, int ix2)
{

  std::string hhh="f1";
  vnl_double_3  vs;

  float combination_counter=0.0;

  //std::cout<<ix0<<" "<<ix1<<" "<<ix2<<std::endl;
  //std::cout<< "~"<<line_in<<std::endl;

  char * cstr;
  cstr = new char [line_in.size()+1];
  std::strcpy (cstr, line_in.c_str());
  vul_string_c_trim(cstr,"(error/count):"); 




  int index=0;
  char * pcha;
  pcha=std::strtok(cstr," (error/count)");  

  //std::cout<<pcha<<std::endl;
  while (pcha != NULL)
  {

   // std::cout<<index<<" "<<vul_string_atof(pcha)<<std::endl;
    if (index==ix0) vs[0]=(vul_string_atof(pcha));
    if (index==ix1) vs[1]=(vul_string_atof(pcha));
    if (index==ix2) vs[2]=(vul_string_atof(pcha));
   // std::cout<<pcha<<std::endl;

    pcha=std::strtok(NULL," ");



    index++;


  }
  // std::cout<< vs<<std::endl;
  /*}*/
  delete []cstr;
  return vs;
}



// divide number vy ','
//get from the reading line_in
vnl_vector<double>  get_number_comma( std::string line_in,
                               int ix0,int ix1, int ix2)
{

  std::string hhh="f1";
  vnl_double_3  vs;

  float combination_counter=0.0;

  //std::cout<<ix0<<" "<<ix1<<" "<<ix2<<std::endl;
  //std::cout<< "~"<<line_in<<std::endl;

  char * cstr;
  cstr = new char [line_in.size()+1];
  std::strcpy (cstr, line_in.c_str());
  vul_string_c_trim(cstr,">");  /// insert '>' 5-7-07




  int index=0;
  char * pcha;
  pcha=std::strtok(cstr,",");  /// insert ',' 5-7-07

  //std::cout<<pcha<<std::endl;
  while (pcha != NULL)
  {

   // std::cout<<index<<" "<<vul_string_atof(pcha)<<std::endl;
    if (index==ix0) vs[0]=(vul_string_atof(pcha));
    if (index==ix1) vs[1]=(vul_string_atof(pcha));
    if (index==ix2) vs[2]=(vul_string_atof(pcha));
  //  std::cout<<pcha<<std::endl;

    pcha=std::strtok(NULL,",");



    index++;


  }
  // std::cout<< vs<<std::endl;
  /*}*/
  delete []cstr;
  return vs;
}


int centering(std::vector <vnl_double_2> &ll2,int ni=1024,int nj=768) 
{
  //double min_0=vnl_numeric::max
  double min_0 =  vnl_numeric_traits<double>::maxval;
  double min_1= min_0;
  double max_0 =  -vnl_numeric_traits<double>::maxval;
  double max_1 =  -vnl_numeric_traits<double>::maxval;
  double add_0=0.0,add_1=0.0;
  for (unsigned i=0;i<ll2.size();i++)
  {
    if (ll2[i][0]>max_0) 
    {
      max_0=ll2[i][0];
    }
    if (ll2[i][0]<min_0) 
    {
      min_0=ll2[i][0];
    }
    if (ll2[i][1]>max_1) 
    {
      max_1=ll2[i][1];
    }
    if (ll2[i][0]<min_1) 
    {
      min_1=ll2[i][1];
    }
    add_0+=ll2[i][0];
    add_1+=ll2[i][1];
  }
  add_0/=ll2.size();
  add_1/=ll2.size();

 
  double shift_0=(min_0+max_0)/2.0;
  double shift_1=(min_1+max_1)/2.0;

 
 
  for (unsigned i=0;i<ll2.size();i++)
  {
    ll2[i][0]-=add_0;
    ll2[i][1]-=add_1;
  }

 
  for (unsigned i=0;i<ll2.size();i++)
  {
    ll2[i][0]*=CENTERING_SCALE;
    ll2[i][1]*=CENTERING_SCALE;
  }

  if (0)
  for (unsigned i=0;i<ll2.size();i++)
  {
    ll2[i][0]+=ni/2.0;
    ll2[i][1]+=nj/2.0;
  }

   //method 2 center of graivity
  if (1)
  for (unsigned i=0;i<ll2.size();i++)
  {
    ll2[i][0]=+add_0+ll2[i][0]+ni/2.0;
    ll2[i][1]=+add_1+ll2[i][1]+nj/2.0;

    std::cout<<ll2[i]<<std::endl;
  }

  add_0_psh_=add_0; add_1_psh_=add_1; ni_psh_=ni; nj_psh_=nj;
  return 1;

}




////////////////////////////////////////////
///////////////////////////////////////////
//
// For <vgl_point_3d> points.
// 
//
///////////////////////////////////////////
///////////////////////////////////////////



/// both f1 and r1 together

int read_result_txt_file_vgl_point_3d(std::string res_txt, bool flag, std::vector<vnl_double_2>& p0a,std::vector<vnl_double_2>& p1a,double cut_bottom=0.5) {//,float rev,std::vector <vgl_point_3d<double> > & ptl,bool rotation=false){
  //std::ifstream in(res_txt.c_str());
  std::ifstream in(res_txt.c_str());

  if(in.fail()){
    std::cerr << "Failed to read file "<<res_txt << std::endl;
    exit(-4);
    return -1;
  }

  // in
  std::string hhh="";
  std::string dummy="";
  std::string st="";

  float combination_counter=0.0;
  double x,y,z;z=0.0;
  double epi_x,epi_y;
  std::vector <double> v,w;
  double sum_z=0.0;double sum_z_norm=0.0;



  vnl_vector <double> ddd;
  std::vector <vnl_vector <double> >lll;
  std::vector <vnl_double_2> ll2;
  int vgl_counter=0;

  int ps_counter=0;
  int marker=-1;
  //for (unsigned i=0;i<50;i++) {
  while (!in.eof()) {
    
    in>>hhh;
    char stch[256];
    std::string ss;
    //char temp[256];
    
    if (hhh=="front") {  // when marker is 3 or 6, write ps file.
      //in.unget();
      //in.unget();  
      in.getline(stch,256);
      in.getline(stch,256);
      in.getline(stch,256);
      in.getline(stch,256);

      in>>hhh; 

      if (hhh!="<vgl_point_3d") continue;
      while (hhh=="<vgl_point_3d") {

        in.getline(stch,256);
        ss=stch;
        std::string  epos("e+"),eneg("e-"),ind("#");

        //get rid of the lines containing exponential part
        int    found=ss.find(epos);
        if (found!=std::string::npos) {
          std::cout<<"*e+ skip"<<ss<<std::endl;
          in>>hhh;
          continue;
        }
        found =ss.find(eneg);
        if (found!=std::string::npos){
          std::cout<<"*e- skip"<<ss<<std::endl;
          in>>hhh;
          continue;
        }
        found =ss.find(ind);
        if (found!=std::string::npos){
          std::cout<<"*ind skip"<<ss<<std::endl;
          in>>hhh;
          continue;
        }
        //std::cout<<stch<<std::endl;

        if (ss.size()==0) continue;
        //if (i>38) {

       // if (hhh=="f1") f1_counter++;
        ddd=get_number_comma(stch,0,1,2);
        //std::cout<<ddd.size()<<" "<<ddd[0]<<" "<<ddd[1]<<" "<<ddd[2]<<std::endl;

        std::cout<<ddd[0]<<" "<<ddd[1]<<" "<<ddd[2]<<std::endl;

        //std::cout<<lll.size()<<std::endl;
        lll.push_back(ddd);
        in>>hhh;
      }


      std::cout<<lll.size()<<std::endl;

      for (unsigned i=0;i<lll.size();i++)
      {

        if (lll[i][2]<cut_bottom) continue;
        vnl_double_2 l;
        if (flag==false) //xz
        { l[0]=lll[i][0];l[1]=lll[i][2];}

        if (flag==true)  //yz
        { l[0]=lll[i][1];l[1]=lll[i][2];}

        ll2.push_back(l);


      }

      std::string filename="ps_vgl";
     
      if (flag==false) filename+="_xz_";
      if (flag==true) filename+="_yz_";

      char buffer [33];
      filename+=itoa(ps_counter,buffer,10);
      //filename+="_";
     
      //std::cout<<cut_bottom<<" cut_at_"<<buffer<<std::endl;
      sprintf(buffer,"_cut_at_%4.1f",cut_bottom);
      filename+=buffer;
      filename+="_";
      //filename+=".ps";

      std::cout<<cut_bottom<<" "<<buffer<<std::endl;

      std::vector <vnl_double_2> ll2_copy;
      for (unsigned i=0;i<ll2.size();i++)
        ll2_copy.push_back(ll2[i]);

      centering(ll2);


      std::vector< vnl_double_2 >p0,p1;

      //
      p0a.clear(); p1a.clear();
      water_fill(ll2_copy,p0a,p1a);

      for (unsigned i=0;i<ll2.size();i++)
        std::cout<<ll2[i]<<std::endl;

      std::cout<<ll2.size()<<" ------vgl_point_3d---------=="<<std::endl;

     
      
/////      ps_write(filename,  ll2,vgl_counter);

      for (unsigned i=0;i<p0a.size();i++)
        std::cout<<p0a[i]<<" : "<<p1a[i]<<std::endl;

     // ps_histogram_write( filename, p0, p1);

      //** 5-11-2007 **//
      if (ps_counter==0) return 1;
      
        
      ll2.clear();
      lll.clear();

      vgl_counter=0;
      ps_counter++;


      marker++;

    }
    else {

 

      continue;
    }
  
  }
  exit(1);


}

