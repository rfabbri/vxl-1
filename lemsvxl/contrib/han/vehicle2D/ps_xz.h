#define MODE 2
#define CRHM false
#define ERASE_FOR_NOW_5_5_2007 //
#define CENTERING_SCALE 50
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
#include <vnl/vnl_double_3x4.h>

#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_2.h>
#include <vul/vul_file.h>
#include <vcl_vector.h>
#include <vcl_string.h>
//#include <vcl_ifstream.h>
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

#include <vcl_cstring.h>
#include <vcl_string.h>
//#include <vcl_fstream.h>
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

void water_fill(vcl_vector< vnl_double_2 >p,vcl_vector< vnl_double_2 >&points0,vcl_vector< vnl_double_2 >&points1)
{

  int index;
  double max=-1.0,min=1000000000000000000.0;
  for (unsigned i=1;i<p.size(); i++) 
  {
    double a=vcl_fabs(p[i][1]-p[i-1][1]);
    if (a>max) {
      index=i;
      max=a;
    }
  }
  vcl_cout<<"separating points to 2 groups: "<<max<<" "<<index<<vcl_endl;
  vcl_vector<vgl_point_2d <double> >g0,g1;

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

  vcl_vector <vgl_point_2d<double> > p0l,p1l;
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
    
   vcl_cout<<"water level: "<<water_level<<" "<<p0.x()-p1.x()<<vcl_endl;
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
    //vcl_cout<<ll2[i]<<vcl_endl;
  }

   for (unsigned i=0;i<p1l.size();i++)
  {
    p1l[i].x()+=+add_0_psh_+ni_psh_/2.0;
    p1l[i].y()+=+add_1_psh_+nj_psh_/2.0;

    vnl_double_2 a(p1l[i].x(),p1l[i].y());
    points1.push_back(a);
    //vcl_cout<<ll2[i]<<vcl_endl;
  }



}


//get from the reading line_in
vnl_vector<double>  get_number( vcl_string line_in,
                               int ix0,int ix1, int ix2)
{

  vcl_string hhh="f1";
  vnl_double_3  vs;

  float combination_counter=0.0;

  //vcl_cout<<ix0<<" "<<ix1<<" "<<ix2<<vcl_endl;
  //vcl_cout<< "~"<<line_in<<vcl_endl;

  char * cstr;
  cstr = new char [line_in.size()+1];
  vcl_strcpy (cstr, line_in.c_str());
  vul_string_c_trim(cstr,"(error/count):"); 




  int index=0;
  char * pcha;
  pcha=vcl_strtok(cstr," (error/count)");  

  //vcl_cout<<pcha<<vcl_endl;
  while (pcha != NULL)
  {

   // vcl_cout<<index<<" "<<vul_string_atof(pcha)<<vcl_endl;
    if (index==ix0) vs[0]=(vul_string_atof(pcha));
    if (index==ix1) vs[1]=(vul_string_atof(pcha));
    if (index==ix2) vs[2]=(vul_string_atof(pcha));
   // vcl_cout<<pcha<<vcl_endl;

    pcha=vcl_strtok(NULL," ");



    index++;


  }
  // vcl_cout<< vs<<vcl_endl;
  /*}*/
  delete []cstr;
  return vs;
}



// divide number vy ','
//get from the reading line_in
vnl_vector<double>  get_number_comma( vcl_string line_in,
                               int ix0,int ix1, int ix2)
{

  vcl_string hhh="f1";
  vnl_double_3  vs;

  float combination_counter=0.0;

  //vcl_cout<<ix0<<" "<<ix1<<" "<<ix2<<vcl_endl;
  //vcl_cout<< "~"<<line_in<<vcl_endl;

  char * cstr;
  cstr = new char [line_in.size()+1];
  vcl_strcpy (cstr, line_in.c_str());
  vul_string_c_trim(cstr,">");  /// insert '>' 5-7-07




  int index=0;
  char * pcha;
  pcha=vcl_strtok(cstr,",");  /// insert ',' 5-7-07

  //vcl_cout<<pcha<<vcl_endl;
  while (pcha != NULL)
  {

   // vcl_cout<<index<<" "<<vul_string_atof(pcha)<<vcl_endl;
    if (index==ix0) vs[0]=(vul_string_atof(pcha));
    if (index==ix1) vs[1]=(vul_string_atof(pcha));
    if (index==ix2) vs[2]=(vul_string_atof(pcha));
  //  vcl_cout<<pcha<<vcl_endl;

    pcha=vcl_strtok(NULL,",");



    index++;


  }
  // vcl_cout<< vs<<vcl_endl;
  /*}*/
  delete []cstr;
  return vs;
}


int centering(vcl_vector <vnl_double_2> &ll2,int ni=1024,int nj=768) 
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

    vcl_cout<<ll2[i]<<vcl_endl;
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

int read_result_txt_file_vgl_point_3d(vcl_string res_txt, bool flag, vcl_vector<vnl_double_2>& p0a,vcl_vector<vnl_double_2>& p1a,double cut_bottom=0.5) {//,float rev,vcl_vector <vgl_point_3d<double> > & ptl,bool rotation=false){
  //vcl_ifstream in(res_txt.c_str());
  vcl_ifstream in(res_txt.c_str());

  if(in.fail()){
    vcl_cerr << "Failed to read file "<<res_txt << vcl_endl;
    exit(-4);
    return -1;
  }

  // in
  vcl_string hhh="";
  vcl_string dummy="";
  vcl_string st="";

  float combination_counter=0.0;
  double x,y,z;z=0.0;
  double epi_x,epi_y;
  vcl_vector <double> v,w;
  double sum_z=0.0;double sum_z_norm=0.0;



  vnl_vector <double> ddd;
  vcl_vector <vnl_vector <double> >lll;
  vcl_vector <vnl_double_2> ll2;
  int vgl_counter=0;

  int ps_counter=0;
  int marker=-1;
  //for (unsigned i=0;i<50;i++) {
  while (!in.eof()) {
    
    in>>hhh;
    char stch[256];
    vcl_string ss;
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
        vcl_string  epos("e+"),eneg("e-"),ind("#");

        //get rid of the lines containing exponential part
        int    found=ss.find(epos);
        if (found!=vcl_string::npos) {
          vcl_cout<<"*e+ skip"<<ss<<vcl_endl;
          in>>hhh;
          continue;
        }
        found =ss.find(eneg);
        if (found!=vcl_string::npos){
          vcl_cout<<"*e- skip"<<ss<<vcl_endl;
          in>>hhh;
          continue;
        }
        found =ss.find(ind);
        if (found!=vcl_string::npos){
          vcl_cout<<"*ind skip"<<ss<<vcl_endl;
          in>>hhh;
          continue;
        }
        //vcl_cout<<stch<<vcl_endl;

        if (ss.size()==0) continue;
        //if (i>38) {

       // if (hhh=="f1") f1_counter++;
        ddd=get_number_comma(stch,0,1,2);
        //vcl_cout<<ddd.size()<<" "<<ddd[0]<<" "<<ddd[1]<<" "<<ddd[2]<<vcl_endl;

        vcl_cout<<ddd[0]<<" "<<ddd[1]<<" "<<ddd[2]<<vcl_endl;

        //vcl_cout<<lll.size()<<vcl_endl;
        lll.push_back(ddd);
        in>>hhh;
      }


      vcl_cout<<lll.size()<<vcl_endl;

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

      vcl_string filename="ps_vgl";
     
      if (flag==false) filename+="_xz_";
      if (flag==true) filename+="_yz_";

      char buffer [33];
      filename+=itoa(ps_counter,buffer,10);
      //filename+="_";
     
      //vcl_cout<<cut_bottom<<" cut_at_"<<buffer<<vcl_endl;
      sprintf(buffer,"_cut_at_%4.1f",cut_bottom);
      filename+=buffer;
      filename+="_";
      //filename+=".ps";

      vcl_cout<<cut_bottom<<" "<<buffer<<vcl_endl;

      vcl_vector <vnl_double_2> ll2_copy;
      for (unsigned i=0;i<ll2.size();i++)
        ll2_copy.push_back(ll2[i]);

      centering(ll2);


      vcl_vector< vnl_double_2 >p0,p1;

      //
      p0a.clear(); p1a.clear();
      water_fill(ll2_copy,p0a,p1a);

      for (unsigned i=0;i<ll2.size();i++)
        vcl_cout<<ll2[i]<<vcl_endl;

      vcl_cout<<ll2.size()<<" ------vgl_point_3d---------=="<<vcl_endl;

     
      
/////      ps_write(filename,  ll2,vgl_counter);

      for (unsigned i=0;i<p0a.size();i++)
        vcl_cout<<p0a[i]<<" : "<<p1a[i]<<vcl_endl;

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

