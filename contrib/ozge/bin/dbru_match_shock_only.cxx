// create shock matching matrix only

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

#include <dbskr/dbskr_tree_edit_pmi.h>
#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_sm_cor.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

//#include <vil/vil_image_resource_sptr.h>
//#include <vil/vil_image_resource.h>
//#include <vil/vil_load.h>
//#include <vil/vil_save.h>

#include <vul/vul_timer.h>
//#include <dbru/dbru_rcor_sptr.h>
//#include <dbru/dbru_rcor.h>
#include <dbru/algo/dbru_object_matcher.h>
//#include <dbcvr/dbcvr_cv_cor.h>

//#include <dbinfo/dbinfo_observation_sptr.h>
//#include <dbinfo/dbinfo_observation.h>
//#include <dbinfo/dbinfo_observation_matcher.h>
/*
vsol_polygon_2d_sptr read_con_from_file(vcl_string fname) {
  double x, y;
  char buffer[2000];
  int nPoints;

  vcl_vector<vsol_point_2d_sptr> inp;
  inp.clear();

  vcl_ifstream fp(fname.c_str());
  if (!fp) {
    vcl_cout<<" Unable to Open "<< fname <<vcl_endl;
    return 0;
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
    vsol_point_2d_sptr vs = new vsol_point_2d(x, y);
    inp.push_back(vs);
  }
  fp.close();
  vsol_polygon_2d_sptr poly = new vsol_polygon_2d(inp);
  return poly;
}
*/
dbskr_tree_sptr read_esf_from_file(vcl_string fname, double sampling_ds) {
  dbsk2d_xshock_graph_fileio loader;
  dbskr_tree_sptr tree = new dbskr_tree(sampling_ds);
  dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(fname);
  tree->acquire(sg);    
  return tree;
}

int main_shock(int argc, char *argv[]) {
  vcl_cout << "Matching observations and shock graphs!\n";

  //: out file contains the wrong mathces if any
  vcl_string database_list, esfs_dir, out_file, output_dir_shgm;

  vcl_cout << "argc: " << argc << vcl_endl;
  //if (argc == 10) not saving shgm files
  if (argc != 6 && argc != 7) {
    vcl_cout << "Usage: <program name> <database_list> <esfs_dir> <shock curve sampling ds> <matrix name> <output_dir_shgms>\n";
    vcl_cout << "shock curve sampling ds: 5.0 coarse, 1.0: finest, typically 1.0 is used to get best recognition\n";
    vcl_cout << "<matrix name> just give initial portion of the name: shock sampling ds and .out will be added automatically\n";
    vcl_cout << "if <output_dir_shgms> exists then saves shgm files in this directory (put \\ character at the end of directory name) \n";
    return -1;
  }
  database_list = argv[1];
  esfs_dir = argv[2];
  double sampling_ds = atof(argv[3]);
  out_file = vcl_string(argv[4])+"_"+vcl_string(argv[3])+"_shock_matrix.out";
  
  bool save_shgm = false;
  if (argc == 6) {
    save_shgm = true;
    output_dir_shgm = vcl_string(argv[5]);
  }

  vcl_vector<vcl_string> database; 
  vcl_ifstream fpd((database_list).c_str());
  if (!fpd.is_open()) {
    vcl_cout << "Unable to open database file!\n";
    return -1;
  }

  char buffer[1000];
  while (!fpd.eof()) {
    vcl_string temp;
    fpd.getline(buffer, 1000);
    temp = buffer;
    if (temp.size() > 1) {
      database.push_back(temp);
      vcl_cout << "database item: " << temp << vcl_endl;
    }
  }
  fpd.close();
  
  unsigned int D = database.size(); 
  vcl_cout << " database size: " << D << "\n";
#if 1
  vcl_cout << "printing database list: \n";
  for (unsigned int i = 0; i<D; i++) {
    vcl_cout << database[i] << "\n";
  }
#endif

  vcl_cout << "out_file: " << out_file << vcl_endl;
  vcl_string dump_file = "dump_results_" + out_file;
  vcl_cout << "dump_file: " << dump_file << vcl_endl;

  //: load esfs and create trees
  vcl_vector<dbskr_tree_sptr> database_trees;
  for (unsigned int i = 0; i<D; i++) {
    vcl_string esf_file = esfs_dir + database[i];
    dbskr_tree_sptr tree = read_esf_from_file(esf_file.c_str(), sampling_ds);
    database_trees.push_back(tree);
  }

  vcl_cout << "loaded esfs\n";
  vbl_array_2d<double> matching_costs(D, D, 0.0);

  unsigned int i_start = 0, j_start = 1;
  vcl_ifstream if3;

  if3.open(dump_file.c_str());
  if (if3) {
    int dummy; bool tag = false;
    if3 >> dummy;  // read first i
    for (unsigned int i = 0; i<D; i++) {
      i_start = i;
      for (unsigned int j = i+1; j < D; j++) {
        j_start = j;
        if (if3.eof()) { tag = true; break; }  
        if3 >> dummy;   // read j
        if3 >> matching_costs[i][j];
        matching_costs[j][i] = matching_costs[i][j];
        if3 >> dummy;  // try reading i again, if not then it'll break 
      }
      if (tag) break;
    }
    if3.close();
  }
  
  vcl_cout << "i_start: " << i_start << " j_start: " << j_start << vcl_endl;
  vcl_ofstream of3;

  of3.open(dump_file.c_str());
  
  for (unsigned int i = 0; i<=i_start; i++)
    for (unsigned int j = i+1; j < (i == i_start?j_start:D); j++) {
      of3 << i << " " << j << " " << matching_costs[i][j] << "\n";
    }
  of3.close();

  for (unsigned int i = i_start; i<D; i++) {
    unsigned int j = (i == i_start ? j_start : i+1);
    for ( ; j<D; j++) {
      vul_timer t;
      if (i == j) {
        //matching_costs[i][j] = 0.0f;
        //of3.open(dump_file.c_str(), vcl_ofstream::app);
        //of3 << i << " " << j << " " << matching_costs[i][j] << "\n";
        //vcl_cout << i << " " << j << " " << matching_costs[i][j] << " " << t.real() << " secs\n";
        //of3.close();
        continue;
      }
      
      dbskr_sm_cor_sptr sm_cor1, sm_cor2; double cost1, cost2;
      sm_cor1 = dbru_object_matcher::compute_shock_alignment(database_trees[i],database_trees[j],cost1,false);
      sm_cor2 = dbru_object_matcher::compute_shock_alignment(database_trees[j],database_trees[i],cost2,false);      
      
      if (cost1 < cost2) {
        matching_costs[i][j] = cost1;
        matching_costs[j][i] = cost1;
        if (save_shgm) {
          vcl_string namei = database[i].substr(0, database[i].length()-4);
          vcl_string namej = database[j].substr(0, database[j].length()-4);
          vcl_string file_name = output_dir_shgm+namei+"_"+namej+".shgm";
          sm_cor1->write_shgm(cost1, file_name);
        }
      } else {
        matching_costs[i][j] = cost2;
        matching_costs[j][i] = cost2;
        if (save_shgm) {
          vcl_string namei = database[i].substr(0, database[i].length()-4);
          vcl_string namej = database[j].substr(0, database[j].length()-4);
          vcl_string file_name = output_dir_shgm+namej+"_"+namei+".shgm";
          sm_cor2->write_shgm(cost2, file_name);
        }
      }
      of3.open(dump_file.c_str(), vcl_ofstream::app);
      of3 << i << " " << j << " " << matching_costs[i][j] << "\n";
      vcl_cout << i << " " << j << " " << matching_costs[i][j] << " " << t.real()/1000.0f << " secs\n";
      of3.close();
   }
  }

  vcl_ofstream of((out_file).c_str());
  if (!of.is_open()) {
    vcl_cout << "output file: " << out_file << " could not be opened! Matrix could not be saved!\n";
    return 0;
  } else {
    vcl_cout << "opened output file! Outputting the matrix\n";
  }
  for (unsigned i = 0; i<D; i++) {
    for (unsigned j = 0; j<D; j++)
      of << matching_costs[i][j] << " ";
    of << "\n";
  }
  of.close();
  return 0;
}

int main(int argc, char *argv[]) {
  main_shock(argc, argv);
}
