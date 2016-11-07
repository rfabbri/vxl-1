// compare pairs of observations

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

#include <georegister/dbrl_feature_sptr.h>
#include <georegister/dbrl_feature_point_sptr.h>
#include <georegister/dbrl_feature_point.h>

#include <georegister/dbrl_rpm_affine.h>
#include <vul/vul_arg.h>
#include <vbl/vbl_array_2d.h>

#include <vul/vul_timer.h>

vcl_vector<dbrl_feature_sptr> read_con_from_file(vcl_string fname) {
  double x, y;
  char buffer[2000];
  int nPoints;

  vcl_vector<dbrl_feature_sptr> inp;
  inp.clear();

  vcl_ifstream fp(fname.c_str());
  if (!fp) {
    vcl_cout<<" Unable to Open "<< fname <<vcl_endl;
    return inp;
  }
  //2)Read in file header.
  fp.getline(buffer,2000); //CONTOUR
  fp.getline(buffer,2000); //OPEN/CLOSE flag (not important, we assume close)
  fp >> nPoints;
#if 0
  vcl_cout << "Number of Points from Contour: " << nPoints << vcl_endl;
#endif     
  for (int i=0;i<nPoints;i++) {
    fp >> x >> y;
    //dbrl_feature_point_sptr vs = ;
    inp.push_back(new dbrl_feature_point(x, y));
  }
  fp.close();
  
  return inp;
}



int main(int argc, char *argv[]) {
  vcl_cout << "Matching Edge Maps!\n";

  vul_arg<vcl_string> databaselist("-i","list of input files","");
  vul_arg<vcl_string> consdir("-ic","dir of con files","");
  vul_arg<vcl_string> outfile("-o","Output file","");


  vul_arg<float> Tinit("-startT","T start",20);
  vul_arg<float> Tend("-endT","T end",0.3);
  vul_arg<float> annealrate("-anneal","Rate of Annealing",0.93);
  vul_arg<float> Mconvg("-mconvg","Rate of convergence for RPM",0.1);
  vul_arg<float> outlierval("-outlier","Value of Outlier",0.005);
  vul_arg<float> lambda("-lambda","lamda",20);


    

    vul_arg_parse(argc,argv);


  vcl_ifstream fpd(databaselist().c_str());
  if (!fpd.is_open()) {
    vcl_cout << "Unable to open database file!\n";
    return -1;
  }

  vcl_vector<vcl_string> database;
  char buffer[1000];
  while (!fpd.eof()) {
    vcl_string temp;
    fpd.getline(buffer, 1000);
    temp = buffer;
    if (temp.size() > 1) {
      vcl_cout << "temp: " << temp << vcl_endl;
      database.push_back(temp);
    }
  }
  fpd.close();
  
  unsigned int D = database.size(); 
  vcl_cout << " D: " << D << "\n";
#if 1
  vcl_cout << "printing database list: \n";
  for (unsigned int i = 0; i<D; i++) {
    vcl_cout << database[i] << "\n";
  }
#endif

  //vcl_cout << "out_file: " << out_file << vcl_endl;
  //vcl_string dump_file = "dump_results_" + out_file;
  //vcl_cout << "dump_file: " << dump_file << vcl_endl;

  //: load cons
  vcl_vector<vcl_vector<dbrl_feature_sptr> > database_points;
  for (unsigned int i = 0; i<D; i++) {
    vcl_string con_file = consdir() + database[i];
    vcl_vector<dbrl_feature_sptr> inp = read_con_from_file(con_file.c_str());
    database_points.push_back(inp);
  }

  vcl_cout<<" Read Con files\n";
//  vcl_cout << " created trees\n";
  vbl_array_2d<double> matching_costs(D, D, 100000);

  //unsigned int i_start = 0, j_start = 1;
  //vcl_ifstream if3;

  //if3.open(dump_file.c_str());
  //if (if3) {
  //  int dummy; bool tag = false;
  //  if3 >> dummy;  // read first i
  //  for (unsigned int i = 0; i<D; i++) {
  //    i_start = i;
  //    for (unsigned int j = i+1; j < D; j++) {
  //      j_start = j;
  //      if (if3.eof()) { tag = true; break; }  
  //      if3 >> dummy;   // read j
  //      if3 >> matching_costs[i][j];
  //      matching_costs[j][i] = matching_costs[i][j];

  //      if3 >> dummy;  // try reading i again, if not then it'll break 
  //    }
  //    if (tag) break;
  //  }
  //  if3.close();
  //}
  //
  

  for (unsigned int i = 0; i<D; i++) {
      for (unsigned int j=i+1 ; j<D; j++) {
          vul_timer t;
            dbrl_rpm_affine_params params(lambda(),Mconvg(),Tinit(),Tend(),outlierval(),annealrate());
          dbrl_rpm_affine affinerpm(params,database_points[i],database_points[j]);
          dbrl_match_set_sptr outset=affinerpm.rpm();
          matching_costs[i][j]=matching_costs[j][i]=affinerpm.energy();
          vcl_ofstream of(outfile().c_str());
          vcl_cout<<i<<" --> "<<j<<" : "<<matching_costs[i][j]<<"\n";
          //of << "\n shock costs: \n" << D << " " << D << "\n";
          for (unsigned i = 0; i<D; i++) {
              for (unsigned j = 0; j<D; j++)
                  if(i==j)
                      of<<0<<" ";
                  else
                      of << matching_costs[i][j] << " ";

              of << "\n";
          }
            of.close();

      }
  }


  
  return 0;
}
