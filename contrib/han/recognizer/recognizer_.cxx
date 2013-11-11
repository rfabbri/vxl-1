// 
#include <testlib/testlib_test.h>
#include <vgl/vgl_convex.h>
#include <vcl_iostream.h>
#include <vnl/vnl_matrix.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_reg_exp.h>
#include <vcl_cstring.h>
#include <vcl_string.h>

struct p 
{
   //int probe_number;
   vcl_vector <vnl_double_3>   pt;
   int real_class_id;
   double HoC;
   double x01,x07,x79,x01_over_x07,z7_over_HoC;
   float recognized_class_id;
   //float scale;
};
typedef struct p p;

///////////////////
read_data_file(vcl_vector <p > & ptl,vcl_string argv){
  vcl_ifstream in(argv.c_str());
  vcl_cout<<argv<<vcl_endl;
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



  double temp;
  int counter=0,real_class_id;
  double a,b,c,d;

  double HoC;
  double x01,x07,x79,x01_over_x07,z7_over_HoC;
  while (!in.eof()) {

   

    in>>hhh;
    if (hhh=="recognizer") {
      p p1;
      in>>p1.HoC;
      //vcl_cout<<HoC;
      for (unsigned j=0;j<10;j++) {
        in >>x>>y>>z;
        vnl_double_3 xyz(x,y,z);
        p1.pt.push_back(xyz);
      //  vcl_cout<<xyz<<vcl_endl;
      }

 
      in >>p1.x01>>p1.x07>>p1.x79>>p1.x01_over_x07>>p1.z7_over_HoC;
     // vcl_cout<<p1.x01<<" "<<p1.x07<<" "<<p1.x79<<" "<<p1.x01_over_x07<<" "<<p1.z7_over_HoC<<vcl_endl;
      while (hhh!="real_class_id") {
        in>>hhh;
      }
      in>> p1.real_class_id;
      in>>hhh; 
      if (hhh=="OOOOO") vcl_cout<<1;
      else vcl_cout<<-1;
      in>>hhh;
      in>>p1.recognized_class_id; vcl_cout<<" "<<p1.real_class_id<<" "<<p1.recognized_class_id<<" ";

      in >>hhh>>hhh;
      if (hhh=="------------------------------") {
      in>>a>>b>>c>>d;
      vcl_cout<<a<<" "<<b<<" "<<c<<" "<<d<<vcl_endl;
      }
     // vcl_cout<<p1.recognized_class_id<<vcl_endl;
      counter++;

      vnl_double_3 xyz1(p1.x01,p1.x07,p1.x79);
      p1.pt.push_back(xyz1);
      vnl_double_3 xyz2(p1.HoC,p1.x01_over_x07,p1.z7_over_HoC);
      p1.pt.push_back(xyz2);

      ptl.push_back(p1);
    }

  }
  ///##comented out 10-2-2006##/// vcl_cout<<"size: "<<ptl.size()<<vcl_endl;
  in.close();
  
}
// learn_rec file
int main(int argc, char* argv[])
{

    if (argc!=4) {

        vcl_cout<<"Usage!  learn_rec <class_id> <probe_id> <read_file> "<<vcl_endl;
            exit(1);

    }
  vcl_vector <p> t;
 // read_data_file(t,"X:/CBB_/BATCH/GOOD/r-com1-all_res4.txt");
  /// read_data_file(t,"X:/CBB_/BATCH/GOOD/r-com1-all_res_TEMP.txt");
  read_data_file(t,argv[3]);
  vcl_cout<<t.size()<<vcl_endl;

  int total_counter=0,correct_counter=0;
  for (unsigned i=0;i<t.size();i++) 
  {

    int class_id;
    total_counter++;

    if (t[i].recognized_class_id<10) class_id=0;
    else if (t[i].recognized_class_id<20) class_id=1;
    else if (t[i].recognized_class_id<30) class_id=2;
    else class_id=454;
    
    if (t[i].real_class_id==class_id) correct_counter++;
    if (atoi(argv[1])==t[i].real_class_id&&argv[3]||atoi(argv[1])==3&&argv[3]) 
    {
    vcl_cout<<t[i].pt[atoi(argv[2])]<<vcl_endl;

    }
  }

  vcl_cout<<total_counter<<" "<<correct_counter<<vcl_endl;
    vcl_cerr<<total_counter<<" "<<correct_counter<<vcl_endl;

}

