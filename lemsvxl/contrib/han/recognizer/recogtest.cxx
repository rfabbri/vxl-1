// 
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
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_reg_exp.h>
#include <vul/vul_string.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_numeric_traits.h>


#define CERR false
//

#define  SKIP  in>>hhh;



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




int read_result_txt_file_vgl_point_3d(std::string res_txt, bool flag, double cut_bottom=0.5) {//,float rev,std::vector <vgl_point_3d<double> > & ptl,bool rotation=false){
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

      // //     centering(ll2);


      std::vector< vnl_double_2 >p0,p1;
      ////      water_fill(ll2_copy,p0,p1);

      for (unsigned i=0;i<ll2.size();i++)
        std::cout<<ll2[i]<<std::endl;

      std::cout<<ll2.size()<<" ------vgl_point_3d---------=="<<std::endl;


      ////      ps_write(filename,  ll2,vgl_counter);

      for (unsigned i=0;i<p0.size();i++)
        std::cout<<p0[i]<<" "<<p1[i]<<std::endl;

      ////      ps_histogram_write( filename, p0, p1);

      ll2.clear();
      lll.clear();

      vgl_counter=0;
      ps_counter++;


      marker++;

    }
    else {

      /*if (lll.size()!=0&& lll.size()>f1_counter) { 
      std::cout<<lll.size()<<std::endl;

      for (unsigned i=0;i<lll.size();i++)
      {

      vnl_double_2 l;
      if (flag==false) //xz
      { l[0]=lll[i][0];l[1]=lll[i][2];}

      if (flag==true)  //yz
      { l[0]=lll[i][1];l[1]=lll[i][2];}

      ll2.push_back(l);


      }

      std::string filename="ps_f1_and_r1";
      if (flag==true) filename+="_yz_";
      if (flag==false) filename+="_xz_";


      char buffer [33];
      filename+=itoa(ps_counter,buffer,10);
      centering(ll2);


      std::cout<<ll2.size()<<" ---------------=="<<std::endl;
      ps_write(filename,  ll2,f1_counter);
      ll2.clear();
      lll.clear();

      f1_counter=0;
      ps_counter++;
      }
      */

      continue;
    }
    /* in.getline(stch,256);

    ss=stch;



    std::string  epos("e+"),eneg("e-"),ind("#");

    //get rid of the lines containing exponential part
    int    found=ss.find(epos);
    if (found!=std::string::npos) {
    std::cout<<ss<<std::endl;
    continue;
    }

    found =ss.find(eneg);
    if (found!=std::string::npos) continue;

    found =ss.find(ind);
    if (found!=std::string::npos) continue;
    //std::cout<<stch<<std::endl;

    if (ss.size()==0) continue;
    //if (i>38) {

    if (hhh=="f1") f1_counter++;
    ddd=get_number(stch,3,4,5);
    //std::cout<<ddd.size()<<" "<<ddd[0]<<" "<<ddd[1]<<" "<<ddd[2]<<std::endl;

    //std::cout<<ddd[0]<<" "<<ddd[1]<<" "<<ddd[2]<<std::endl;

    std::cout<<lll.size()<<std::endl;
    lll.push_back(ddd);
    //}
    */
  }
  exit(1);


}


int read_result_txt_file(std::string res_txt,std::vector <std::vector <vnl_vector <double> > >&PL) {//,float rev,std::vector <vgl_point_3d<double> > & ptl,bool rotation=false){
  bool show=true;
  show=false;
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
  std::vector <vnl_vector <double> >P;
  std::vector <vnl_double_2> ll2;

  ddd.set_size(3);

  int ps_counter=0;
  double class_recognized;
  int class_real; 
  char stch[256];
    std::string ss;
    std::string d;
  //for (unsigned i=0;i<50;i++) {
  while (!in.eof()) {
    in>>hhh;
  
    //char temp[256];
    if (hhh=="id:") { 
      int directory_id;
      P.clear();
      in>> directory_id; 
      if (1) std::cout<<directory_id<<std::endl;
      if (show) std::cout<<"---------------"<<std::endl;
      if (show) std::cout<<hhh<<std::endl;

      in >>d;in>>d;in>>d;
      if (show) std::cout<<d<<std::endl;
      if (d=="Not") continue;
      SKIP;SKIP;SKIP;SKIP;SKIP;SKIP;SKIP;SKIP;SKIP;

      in.getline(stch,256);
      //if (show) std::cout<<stch<<std::endl;
      vul_string_c_trim(stch,">");
      ddd=get_number_comma(stch,0,1,2);
      if (show) std::cout<<ddd<<std::endl;

      in>>d;
      in.getline(stch,256);
      //vul_string_c_trim(stch,"<vgl_point_3d");
      vul_string_c_trim(stch,">"); 
      ddd=get_number_comma(stch,0,1,2);
      if (show) std::cout<<ddd<<std::endl;

      in>>d;
      in.getline(stch,256);
      //vul_string_c_trim(stch,"<vgl_point_3d");
      vul_string_c_trim(stch,">"); 
      vnl_vector <double> ddd_solve_z_height_from_bb_file=get_number_comma(stch,0,1,2);
      if (show) std::cout<<ddd_solve_z_height_from_bb_file<<std::endl;

      std::string  Not("Not");
      in.getline(stch,256); //size: 456
      std::string ss=stch;
      int found=ss.find(Not);
      if (found!=std::string::npos) {
        if (show) std::cout<<found<<std::endl;
        continue;
      }
      in.getline(stch,256); //size: 123
      ss=stch;
      found=ss.find(Not);
      if (found!=std::string::npos) {
        if (show) std::cout<<found<<std::endl;
        continue;
      }
      in.getline(stch,256); //recognizer
      ss=stch;
      found=ss.find(Not);
      if (found!=std::string::npos) {
        if (show) std::cout<<found<<std::endl;
        continue;
      }
      double height;
      in>>height;
      for (unsigned i=0;i<10;i++) {
        in>>ddd[0]>>ddd[1]>>ddd[2];
        P.push_back(ddd);
        if (show) std::cout<<ddd<<std::endl;
      }
      //in>>d;
      in>>ddd[0]>>ddd[1]>>ddd[2]; if (show) std::cout<<ddd<<std::endl;
      P.push_back(ddd);
      in>>ddd[0]>>ddd[1]; ddd[2]=height;

      P.push_back(ddd);
      P.push_back(ddd_solve_z_height_from_bb_file);
      in>>hhh;//"====="

      //if (show) std::cout<<hhh<<std::endl;
      in>>class_recognized;
      in>>d>>d;
      
      in>>d>>class_real;
      if (show) std::cout<<d<<" "<<class_recognized<<std::endl;
      ddd[0]=directory_id; ddd[1]=class_real; ddd[2]=class_recognized;
      P.push_back(ddd);

     PL.push_back(P);
    }

  }
  //exit(1);


}



int main(int argc, char* argv[])
{

  std::vector<std::vector <vnl_vector <double> > >PL;
  read_result_txt_file(argv[1],PL) ;
  if (0)
  for (unsigned i=0;i<PL.size();i++){
    for (unsigned j=0;j<PL[i].size();j++){
      std::cout<<PL[i][j]<<std::endl;
    }
    std::cout<<"========"<<std::endl;
  }

  int count[4]={0};

  std::cout.precision(4);
  for (unsigned i=0;i<PL.size();i++){
   
    //std::cout<<PL[i][13][1]<<" "<<PL[i][12][1]<<" "<<PL[i][10][1]<<" "<<PL[i][9][0]-PL[i][1][0]<<std::endl;
    if (0&&PL[i][12][1]<atof(argv[2]))
    std::cout<<PL[i][13][1]<<" "<<PL[i][12][1]<<" "<<PL[i][10][1]<<" "<<PL[i][9][0]-PL[i][1][0]<<" "<<PL[i][13][0]<<std::endl;
    
    if (PL[i][9][2]>50) std::cout<<"*"; //bad probe 9

 /*   std::cout<<PL[i][13][1]<<" "
      <<PL[i][1][0]-PL[i][0][0]<<" "<<PL[i][1][1]<<" "<<PL[i][1][2]<<"  "
      <<PL[i][9][0]-PL[i][0][0]<<" "<<PL[i][9][1]<<" "<<PL[i][9][2]<<"       x01 "
      <<PL[i][10][1]<<" x19 "<<PL[i][9][0]-PL[i][1][0]<<"  x79 "<<PL[i][7][0]-PL[i][9][0]<<" "<<PL[i][13][0]<<std::endl;
      */
      std::cout<<PL[i][13][1]<<'\t' 
      <<PL[i][1][0]-PL[i][0][0]<<'\t' <<PL[i][1][1]<<'\t' <<PL[i][1][2]<<'\t' 
      <<PL[i][9][0]-PL[i][0][0]<<'\t' <<PL[i][9][1]<<'\t' <<PL[i][9][2]<<'\t' <<"x01 "
      <<PL[i][1][0]-PL[i][0][0]<<'\t' <<" x19 "<<PL[i][9][0]-PL[i][1][0]<<'\t' <<" x79 "<<PL[i][7][0]-PL[i][9][0]<<'\t' <<PL[i][13][0]<<std::endl;

  }
/*Example of PL vec vector
========
16.6811 1.55195 2.69819 *  <- P[0]  //from bbox file p[0]         0
19.2775 -0.401562 6.03642 <- P[1]                                 1
29.792 4.8591 1.44454     <- P[2]                                 2
15.8502 0.5089 1.96782    <- P[3]                                 3  
0 0 0
0 0 0
0 0 0
29.6758 7.09716 4.40467 *  <- P[7]  //bbox file                   7
0 0 0                                                                     ************************************  
25.2064 5.81727 5.90265   <- P[9]                                 9  **P9 * ifZ is high and point exist then *
2.59639 12.9947 4.46938   <- a  b  c                              10      *    P9 exist ~~!!!!!!!!!!!!!!!!!! *
0.199804 0.659076 6.6831  <- d  e  Hoc                            11      ************************************
6.11945 6.41419 3.79674 * <- //bbox solve_z (3rd one dummy)       12
4243 2 25                 <- id  class detected_class             13
========

 (a) std::cout<<x01<<" "     ;                         // x01=std::fabs(P(1,0)-P(0,0));
 (b) std::cout<<x07<<" "     ;                         // x07=std::fabs(P(7,0)-P(0,0));
 (c) std::cout<<x79<<std::endl;                         // x79=std::fabs(P(7,0)-P(9,0));
 (d) std::cout<<x01/std::fabs(P(0,0)-P(7,0))<<" ";      ///*******
 (e) std::cout<<P(7,2)/HoC<<std::endl;     


 dont trust (a)!
*/
  
}

