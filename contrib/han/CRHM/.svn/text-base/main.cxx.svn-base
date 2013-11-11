// 4-15-2007
// CRI histogram method
// read result files. and test it.
// For the alpha and beta of combining recognizer
// Dongjin
#define MODE 2
#define CRHM true
#define ERASE_FOR_NOW_5_5_2007 //
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

#include <mbl/mbl_stats_1d.h> 

/*
//geiven type read the points...
vcl_vector<vgl_point_3d<double> > read_form(vcl_string type, vcl_ifstream in)
{

vcl_string hhh="";
vcl_string dummy="";
vcl_string st="";

float combination_counter=0.0;
double x,y,z,angle;
z=0.0;

vcl_vector <double> v,w;
vcl_vector<vgl_point_3d<double> >ptl;
in.unget();

while (!in.eof()) {

in>>hhh;
if (hhh!=type) {
return ptl;
}
//vcl_cout<<hhh<<vcl_endl;
if (hhh==type) {
//combination_counter=0.0;
in >>angle;//if (hhh=="RECON") break;

char stch[256];
in.getline(stch,256);
st=stch;
vcl_string pch=strtok(stch,"(error/count)");
vcl_cout<<pch<<vcl_endl;
}
}
}



*/


//get from the reading line_in
vcl_vector<double>  get_number( vcl_string line_in,
                               int ix0,int ix1, int ix2)
{

  vcl_string hhh="f1";
  vcl_vector <double> vs;

  float combination_counter=0.0;

  //vcl_cout<<ix0<<" "<<ix1<<" "<<ix2<<vcl_endl;
  //vcl_cout<< "~"<<line_in<<vcl_endl;

  char * cstr;
  cstr = new char [line_in.size()+1];
  vcl_strcpy (cstr, line_in.c_str());
  vul_string_c_trim(cstr,"(error/count):");



  /*  if (hhh!=type) {

  vcl_cout<<"no type"<<vcl_endl; 
  return vs;
  }
  //vcl_cout<<hhh<<vcl_endl;
  if (hhh==type) {
  */

  int index=0;
  char * pcha;
  pcha=vcl_strtok(cstr," (error/count)");

  //vcl_cout<<pcha<<vcl_endl;
  while (pcha != NULL)
  {

    //vcl_cout<<index<<" "<<vul_string_atof(pcha)<<vcl_endl;
    if (index==ix0) vs.push_back(vul_string_atof(pcha));
    if (index==ix1) vs.push_back(vul_string_atof(pcha));
    if (index==ix2) vs.push_back(vul_string_atof(pcha));
    //vcl_cout<<pcha<<vcl_endl;

    pcha=vcl_strtok(NULL," ");



    index++;


  }
  // vcl_cout<< vs<<vcl_endl;
  /*}*/
  delete []cstr;
  return vs;
}



int read_result_txt_file(vcl_string res_txt,vcl_vector <vnl_vector <double > >data, bool show=false) {//,float rev,vcl_vector <vgl_point_3d<double> > & ptl,bool rotation=false){

  vcl_ifstream in(res_txt.c_str());

  if(in.fail()){
    vcl_cerr << "Failed to read file "<<res_txt << vcl_endl;
    exit(-4);
    return -1;
  }

  vcl_string hhh="";
  vcl_string dummy="";
  vcl_string st="";

  float combination_counter=0.0;
  double x,y,z;z=0.0;
  double epi_x,epi_y;
  vcl_vector <double> v,w;
  double sum_z=0.0;double sum_z_norm=0.0;

  //vcl_vector<vgl_point_3d<double> >ptl;
  while (!in.eof()) {

    in>>hhh;
    if (hhh=="RECON") {
      break;
    }
    //vcl_cout<<hhh<<vcl_endl;
    if (hhh=="search:") {
      combination_counter=0.0;
      in >>epi_x;//if (hhh=="RECON") break;
      in >>epi_y;
      while(hhh!="epi") {
        //vcl_cout<<hhh<<vcl_endl;
        if (hhh=="->all") {
          in>>dummy;
          in>>st;
          st.erase(0,3);
          //    vcl_cout<<st<<vcl_endl;
        }
        if (hhh=="check:-->***") 
        {
          in>> z;
          v.push_back(z); 

          sum_z+=z;
          sum_z_norm+=z/atof(st.c_str());
          combination_counter++;
        }
        else if(hhh=="camera")
        {
          //vcl_cout<<sum_z_norm<<" "<<sum_z<<vcl_endl;
          vnl_vector_fixed<double,4> a(sum_z,sum_z_norm,epi_x,epi_y);
          data.push_back(a);
          mbl_stats_1d v_stat(v);

          if (show&&CRHM) vcl_cout<<a(0)<<"  "<<v_stat<<" epi:"<<epi_x<<" "<<epi_y<<vcl_endl;
          sum_z=0.0;
          sum_z_norm=0.0;
          v.clear();
          w.clear();
        }
        else ;

        in>>hhh;
        if (hhh=="RECON") {
          vcl_cout<<vcl_endl;
          break;
        }
      }





      //vcl_cout<<p3d<<vcl_endl;
    }
  }
  //vcl_cout<<"size: "<<ptl.size()<<vcl_endl;

  /******************
  vcl_string type="r1";
  vcl_vector <double> ddd;
  vcl_vector <vcl_vector <double> >lll;
  //for (unsigned i=0;i<50;i++) {
  while (!in.eof()) {
    in>>hhh;
    char stch[256];
    vcl_string ss;
    //char temp[256];
    if (hhh=="r1") {
      //in.unget();
      //in.unget();  
    }
    else {
      if (lll.size()!=0)    
        vcl_cout<<lll.size()<<vcl_endl;
      lll.clear();
      continue;
    }
    in.getline(stch,256);
    ss=stch;
    vcl_string  epos("e+"),eneg("e-"),ind("#");
    //get rid of the lines containing exponential part
    int    found=ss.find(epos);
    if (found!=vcl_string::npos) {
      vcl_cout<<ss<<vcl_endl;
      continue;
    }

    found =ss.find(eneg);
    if (found!=vcl_string::npos) continue;
    found =ss.find(ind);
    if (found!=vcl_string::npos) continue;
    //vcl_cout<<stch<<vcl_endl;
    if (ss.size()==0) continue;
    //if (i>38) {
    ddd=get_number(stch,3,4,5);
    //vcl_cout<<ddd.size()<<" "<<ddd[0]<<" "<<ddd[1]<<" "<<ddd[2]<<vcl_endl;
    vcl_cout<<ddd[0]<<" "<<ddd[1]<<" "<<ddd[2]<<vcl_endl;
    lll.push_back(ddd);
    //}
    
  }********************/
 // exit(1);

  
  if (data.size() <3) {
  vcl_cout<<"too short list of epi search"<<vcl_endl;
  exit(5);
  }


  vcl_vector <vcl_vector <vnl_vector <double > > >subdata_all;
  vcl_vector <vnl_vector <double > >subdata;
  for (unsigned i=0;i<data.size()-1;i++) {
  //vcl_cout<<"--"<<data[i][0]<<" "<<data[i][1]<<vcl_endl;
  if (data[i][0]==0&&data[i][1]==0) {

  float sub_max=0.0;
  unsigned sub_max_index=0;
  for (unsigned j=0;j<subdata.size();j++)
  {
  if (sub_max<subdata[j][0]) 
  {
  sub_max=subdata[j][0];
  sub_max_index=j;
  }
  }
  vcl_cout<< "**sub max** "<<subdata[sub_max_index]<<vcl_endl;
  subdata_all.push_back(subdata);
  subdata.clear();
  }
  subdata.push_back(data[i]);

  }
  //exit(1);
  data.clear();
  for (unsigned i=1;i<subdata.size();i++)
  data.push_back(subdata[i]);

  subdata_all.push_back(data);

  for (unsigned i=0;i<data.size();i++)
  data[i][0]/=combination_counter;

  for (unsigned i=0;i<subdata_all.size();i++)
  for (unsigned j=0;j<subdata_all[i].size();j++)
  {
  subdata_all[i][j][0]/=combination_counter;
  }


  vcl_cout<<"data size: "<<data.size()<<" combination counter "<<combination_counter<<vcl_endl;
  in.close();



  vcl_vector <double> data1,data0;
  for (unsigned i=0;i<data.size();i++){
  data0.push_back(data[i][0]);
  data1.push_back(data[i][1]);
  }

  mbl_stats_1d stats0(data0);
  mbl_stats_1d stats1(data1);
  if (show) for (unsigned i=0;i<data.size();i++)
  {
  if (stats0.max()==data[i][0]) vcl_cout<<"!! "<<data[i]<<vcl_endl;
  else vcl_cout<<"*  normalized:  "<<data[i]<<vcl_endl;

  }
  //vcl_cout<<stats1.mean()<<" "<<stats1.max()<<" "<<stats1.min()<<" "<<stats1.variance()<<vcl_endl;

  //vcl_cout<<stats0.mean()<<" "<<stats0.max()<<" "<<stats0.min()<<" "<<stats0.variance()<<vcl_endl;
  vcl_cout<<"------------------------------"<<vcl_endl;
  //vcl_cout<<stats0.variance()<<" "<<stats1.variance()<<" "<<stats1.max()<<" "<<stats1.min()<<vcl_endl;
  vcl_cout<<stats0.variance()<<" "<<stats0.max()<<" "<<stats0.min()<<vcl_endl;
  vcl_cout<<"=============================="<<vcl_endl<<vcl_endl;


  for (unsigned b=0;b<subdata_all.size();b++) 
  {
  vcl_vector <double> d0,d1;
  for (unsigned i=0;i<subdata_all[b].size();i++){
  if (subdata_all[b][i][0]==0&&subdata_all[b][i][1]==0) vcl_cout<<"0 0 skip "<<b<<vcl_endl;
  else {
  d0.push_back(subdata_all[b][i][0]);
  d1.push_back(subdata_all[b][i][1]);
  }

  }
  mbl_stats_1d s0(d0);
  mbl_stats_1d s1(d1);

  vcl_cout<<"^^*^^ "<<s0<<" "<<s1<<vcl_endl;

  }
  // vcl_cout<<stats1<<vcl_endl;
  //exit(1);
  return 1;
}


// For drawing XZ contour image in matlab.
int read_result_txt_file_for_XZdrawing(vcl_string res_txt,vcl_vector <vnl_vector <double > >data, bool show=false) {//,float rev,vcl_vector <vgl_point_3d<double> > & ptl,bool rotation=false){
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

  //vcl_vector<vgl_point_3d<double> >ptl;
  while (!in.eof()) {

    // if (hhh=="RECON") break;
    in>>hhh;
    if (hhh=="RECON") {
      break;
    }
    //vcl_cout<<hhh<<vcl_endl;
    if (hhh=="search:") {
      combination_counter=0.0;
      in >>epi_x;//if (hhh=="RECON") break;
      in >>epi_y;
      while(hhh!="epi") {
        //vcl_cout<<hhh<<vcl_endl;
        if (hhh=="->all") {
          in>>dummy;
          in>>st;
          st.erase(0,3);
          //    vcl_cout<<st<<vcl_endl;
        }
        if (hhh=="check:-->***") 
        {
          in>> z;
          v.push_back(z); 

          sum_z+=z;
          sum_z_norm+=z/atof(st.c_str());
          combination_counter++;
        }
        else if(hhh=="camera")
        {
          //vcl_cout<<sum_z_norm<<" "<<sum_z<<vcl_endl;
          vnl_vector_fixed<double,4> a(sum_z,sum_z_norm,epi_x,epi_y);
          data.push_back(a);
          mbl_stats_1d v_stat(v);

          if (show) vcl_cout<<a<<"  "<<v_stat<<" "<<epi_x<<" "<<epi_y<<vcl_endl;
          sum_z=0.0;
          sum_z_norm=0.0;
          v.clear();
          w.clear();
        }
        else ;

        in>>hhh;
        if (hhh=="RECON") {
          vcl_cout<<vcl_endl;
          break;
        }
      }





      //vcl_cout<<p3d<<vcl_endl;
    }
  }
  //vcl_cout<<"size: "<<ptl.size()<<vcl_endl;

  if (data.size() <3) {
    vcl_cout<<"too short list of epi search"<<vcl_endl;
    exit(5);
  }


  vcl_vector <vcl_vector <vnl_vector <double > > >subdata_all;
  vcl_vector <vnl_vector <double > >subdata;
  for (unsigned i=0;i<data.size()-1;i++) {
    //vcl_cout<<"--"<<data[i][0]<<" "<<data[i][1]<<vcl_endl;
    if (data[i][0]==0&&data[i][1]==0) {

      float sub_max=0.0;
      unsigned sub_max_index=0;
      for (unsigned j=0;j<subdata.size();j++)
      {
        if (sub_max<subdata[j][0]) 
        {
          sub_max=subdata[j][0];
          sub_max_index=j;
        }
      }
      vcl_cout<< "**sub max** "<<subdata[sub_max_index]<<vcl_endl;
      subdata_all.push_back(subdata);
      subdata.clear();
    }
    subdata.push_back(data[i]);

  }
  //exit(1);
  /**  data.clear();
  for (unsigned i=1;i<subdata.size();i++)
  data.push_back(subdata[i]);

  subdata_all.push_back(data);

  for (unsigned i=0;i<data.size();i++)
  data[i][0]/=combination_counter;

  for (unsigned i=0;i<subdata_all.size();i++)
  for (unsigned j=0;j<subdata_all[i].size();j++)
  {
  subdata_all[i][j][0]/=combination_counter;
  }


  vcl_cout<<"data size: "<<data.size()<<" combination counter "<<combination_counter<<vcl_endl;
  in.close();



  vcl_vector <double> data1,data0;
  for (unsigned i=0;i<data.size();i++){
  data0.push_back(data[i][0]);
  data1.push_back(data[i][1]);
  }

  mbl_stats_1d stats0(data0);
  mbl_stats_1d stats1(data1);
  if (show) for (unsigned i=0;i<data.size();i++)
  {
  if (stats0.max()==data[i][0]) vcl_cout<<"!! "<<data[i]<<vcl_endl;
  else vcl_cout<<"*  "<<data[i]<<vcl_endl;

  }
  //vcl_cout<<stats1.mean()<<" "<<stats1.max()<<" "<<stats1.min()<<" "<<stats1.variance()<<vcl_endl;

  //vcl_cout<<stats0.mean()<<" "<<stats0.max()<<" "<<stats0.min()<<" "<<stats0.variance()<<vcl_endl;
  vcl_cout<<"------------------------------"<<vcl_endl;
  //vcl_cout<<stats0.variance()<<" "<<stats1.variance()<<" "<<stats1.max()<<" "<<stats1.min()<<vcl_endl;
  vcl_cout<<stats0.variance()<<" "<<stats0.max()<<" "<<stats0.min()<<vcl_endl;
  vcl_cout<<"=============================="<<vcl_endl<<vcl_endl;


  for (unsigned b=0;b<subdata_all.size();b++) 
  {
  vcl_vector <double> d0,d1;
  for (unsigned i=0;i<subdata_all[b].size();i++){
  if (subdata_all[b][i][0]==0&&subdata_all[b][i][1]==0) vcl_cout<<"0 0 skip "<<b<<vcl_endl;
  else {
  d0.push_back(subdata_all[b][i][0]);
  d1.push_back(subdata_all[b][i][1]);
  }

  }
  mbl_stats_1d s0(d0);
  mbl_stats_1d s1(d1);

  vcl_cout<<"^^*^^ "<<s0<<" "<<s1<<vcl_endl;

  }
  // vcl_cout<<stats1<<vcl_endl;
  //exit(1);
  return 1;**/
}

int main(int argc, char* argv[])
{


  vul_reg_exp r1("res");
  vul_reg_exp r2("txt");
  vcl_vector <vnl_vector <double> > data;


  vul_file_iterator fn="*.txt";

  bool show=true;
  if (argv[1]&&r1.find(argv[1])) {


    read_result_txt_file(argv[1],data,show);
    exit(1);
  }
  //vcl_cout<<show<<vcl_endl;
  for ( ; fn; ++fn) 
  {
    vcl_string input_file = fn();

    if (r1.find(input_file.c_str())&&r2.find(input_file.c_str()) )
    {
      vcl_ifstream fp(input_file.c_str());
      if (!fp) {
        vcl_cout<<" Unable to Open "<< input_file<<vcl_endl;
        return false;
      }
      read_result_txt_file(input_file,data,show);
      //vcl_cout<<"reading "<<input_file<<" Done!"<<vcl_endl;
    }
  }

  exit(1);
  if(argc!=2){
    vcl_cout << "Usage: "<< argv[0] << " <res_text_file>"<< vcl_endl;
    //vcl_cout << "   or: "<< argv[0] << " <wrl_file> <front_wrl_file> <rear_wrl_file>"<< vcl_endl;
    exit(0);
  }

  //read_result_txt_file(argv[1]);
  vcl_string in0="C:/CBB_/A3900-3999/3903/sel/res_194c0.txt";

  vcl_string in1="C:/CBB_/A3800/3863/sel/res_193c0_7relase.txt";
  // read_result_txt_file(in0);
  //// read_result_txt_file(argv[1]);
  return 0;

}
