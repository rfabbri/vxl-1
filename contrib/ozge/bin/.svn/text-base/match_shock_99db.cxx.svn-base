// compare pairs of observations

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

#include <dbskr/dbskr_tree_edit_combined.h>
#include <dbskr/dbskr_tree.h>
//#include <dbskr/dbskr_utilities.h>
#include <dbskr/dbskr_sm_cor.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/algo/dbsk2d_compute_shocks.h>
#include <dbru/algo/dbru_object_matcher.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

#include <vul/vul_timer.h>
#include <vul/vul_file.h>

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
/*
dbskr_tree_sptr read_esf_from_file(vcl_string fname, double sampling_ds, bool elastic_splice) {
  dbsk2d_xshock_graph_fileio loader;
  dbskr_tree_sptr tree = new dbskr_tree(sampling_ds);
  dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(fname);
  tree->acquire(sg, elastic_splice);    
  return tree;
}*/

void usage_evaluate() {
  vcl_cout << "Usage: <program name> <database_list> <shock pruning threshold> <shock curve sampling ds (5.0: coarse, 1.0: finest)> <outfile name> <0 if normal splice 1 if elastic splice cost> <0 if construct_circular_ends, 1 if no circular completions> <0 if original edit distance, 1 if edit distance with combined costs> \n";
}

int main_evaluate(int argc, char *argv[]) {

  vcl_cout << "Evaluating matched observations\n";

  vcl_string database_list, out_file;

  vcl_cout << "argc: " << argc << vcl_endl;
  //if (argc == 10) not saving shgm files
  if (argc != 8) {
    usage_evaluate();   
    return -1;
  }
  database_list = argv[1];

  double pruning_thres = atof(argv[2]);
  vcl_string pruning_str = argv[2];
  double sampling_ds = atof(argv[3]);
  vcl_string sampling_str = argv[3];
  
  bool elastic_splice_cost = false;
  int option = atoi(argv[5]);
  if (option == 1) {
    vcl_cout << "USING ELASTIC SPLICE COST!!!!!!!!!\n";
    elastic_splice_cost = true;
  }

  bool construct_circular_ends = true;
  option = atoi(argv[6]);
  if (option == 1) {
    vcl_cout << "Matching with NO Circular COMPLETIONS!!!!!!!!!\n";
    construct_circular_ends = false;
  }

  bool original_edit = true;
  option = atoi(argv[7]);
  if (option == 1) {
    vcl_cout << "Using edit distance with COMBINED costs!!!!!!!!!\n";
    original_edit = false;
  }

  vcl_string elastic_str, circular_str, edit_str;
  if (elastic_splice_cost)
    elastic_str = "elastic";
  else
    elastic_str = "analytic";
  if (construct_circular_ends) 
    circular_str = "circular";
  else
    circular_str = "noncircular";
  if (original_edit) 
    edit_str = "original-" + pruning_str + "-" + sampling_str;
  else
    edit_str = "combined-" + pruning_str + "-" + sampling_str;

  out_file = vcl_string(argv[4]) + "_" + elastic_str + "_" + circular_str + "_" + edit_str + ".out";

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

  vcl_cout << "out_file: " << out_file << vcl_endl;
  if (!vul_file::exists(out_file.c_str())) {
    vcl_cout << " out file does not exist!!\n";
    return -1;
  }
 
  vbl_array_2d<double> matching_costs(D, D, 100000);
  vbl_array_2d<double> matching_costs_norm(D, D, 100000);

  vcl_ifstream ifs((out_file).c_str());
  char buf[1000];
  ifs.getline(buf, 1000);
  vcl_cout << " read: " << buf << vcl_endl;
  ifs.getline(buf, 1000);
  vcl_cout << " read: " << buf << vcl_endl;
  int Df;
  ifs >> Df;
  ifs >> Df;
  if (Df != D) {
    vcl_cout << "out file contains " << Df << "input database file size: " << D << " mismatch!! Quiting\n";
    return -1;
  }

  for (unsigned i = 0; i<D; i++) {
    for (unsigned j = 0; j<D; j++) {
      ifs >> matching_costs[i][j];
    }
  }

  ifs.getline(buf, 1000);
  ifs.getline(buf, 1000);
  ifs.getline(buf, 1000);
  
  ifs >> Df;
  ifs >> Df;
  if (Df != D) {
    vcl_cout << "out file contains " << Df << "input database file size: " << D << " mismatch!! Quiting\n";
    return -1;
  }

  for (unsigned i = 0; i<D; i++) {
    for (unsigned j = 0; j<D; j++) {
      ifs >> matching_costs_norm[i][j];
    }
  }

  vcl_cout << "matching_costs 0 1: " << matching_costs[0][1] << " [D-2][D-1]: " << matching_costs[D-2][D-1] << vcl_endl;
  vcl_cout << "matching_costs norm 0 1: " << matching_costs_norm[0][1] << " [D-2][D-1]: " << matching_costs_norm[D-2][D-1] << vcl_endl;
  ifs.close();

  vcl_cout << "there are 9 classes with 11 instances each, it is assumed that the input database file was ordered!!\n";
  // find top 15 as in the Sebastian et al. PAMI 06 paper

  vcl_string out_html = out_file + "-top-15.html";

  vcl_ofstream tf(out_html.c_str(), vcl_ios::app);
  
  if (!tf) {
    vcl_cout << "Unable to open output html file " << out_html << " for write " << vcl_endl;
    return false;
  }

  vcl_string img_dir = ".\\pngs\\";
  tf << "<TABLE BORDER=\"1\">\n";
  tf << "<caption align=\"top\">" << out_file << "</caption>\n";
  tf << "<TR> <TH>   ";  // leave the first cell of first row empty
  // write top n to the first row
  for (unsigned i = 0; int(i) < 15; i++) 
    tf << "<TH> Match " << i+1 << " ";
  tf << "</TH> </TR>\n";

  vcl_vector<unsigned> correct(15, 0);
  vcl_vector<unsigned> wrong(15, 0);
  
  bool print = false;
  for (unsigned i = 0; i<D; i++) {
    
    if (print)
      vcl_cout << i << ": " << database[i] << " ";
    int class_id = (i-i%11)/11;
    if (print)
      vcl_cout << "class id: " << class_id << "\n";

    vcl_string name = database[i];
    name = name.substr(0, name.length()-4) + ".png";
    vcl_string image_file = img_dir + name;
    tf << "<TR> <TD> <img src=\"" << image_file << "\"> ";
    tf << database[i] << " " << class_id << " </TD> "; 

    
    for (unsigned m = 0; m < 15; m++) {
      unsigned min_j = 0;
      double min = matching_costs[i][0];
      for (unsigned j = 0; j < D; j ++) {
        if (matching_costs[i][j] < min) {
          min = matching_costs[i][j];
          min_j = j;
        }
      }
      if (print)
        vcl_cout << database[min_j] << " (val: " << min << ") ";
      int class_id_j = (min_j - min_j%11)/11;
      if (class_id_j == class_id) {
        correct[m]++;
      } else {
        wrong[m]++;
      }
      matching_costs[i][min_j] = 100000;

      vcl_string name2 = database[min_j];
      name2 = name2.substr(0, name2.length()-4) + ".png";
      vcl_string image_file2 = img_dir + name2;
      tf << "<TD> <img src=\"" << image_file2 << "\"> ";
      tf << database[min_j] << " " << min << " " << class_id_j << " </TD> ";
    }

    tf << "</TR>\n";

    if (print)
      vcl_cout << "\n";
  }

  tf << "</TABLE>\n";

  for (unsigned m = 0; m < 15; m++) {
    vcl_cout << "correct: " << correct[m] << " wrong: " << wrong[m] << " total: " << correct[m] + wrong[m] << vcl_endl;
    unsigned sum_c = 0, sum_w = 0;
    for (unsigned mm = 0; mm <= m; mm++) {
      sum_c += correct[mm];
      sum_w += wrong[mm];
    }
    sum_c /= (m+1);
    sum_w /= (m+1);

    vcl_cout << "correct in top " << m+1 << ": " << sum_c << "% wrong in top " << m+1 << ": " << sum_w << "%" << vcl_endl;
  }
  vcl_cout << "--------- normalized ------------\n";

  tf << "<TABLE BORDER=\"1\">\n";
  tf << "<caption align=\"top\"> normalized costs </caption>\n";
  tf << "<TR> <TH>   ";  // leave the first cell of first row empty
  // write top n to the first row
  for (unsigned i = 0; int(i) < 15; i++) 
    tf << "<TH> Match " << i+1 << " ";
  tf << "</TH> </TR>\n";

  correct.clear();
  correct.resize(15, 0);
  wrong.clear();
  wrong.resize(15, 0);
  for (unsigned i = 0; i<D; i++) {
    if (print)
      vcl_cout << i << ": " << database[i] << " ";
    int class_id = (i-i%11)/11;
    if (print)
      vcl_cout << "class id: " << class_id << "\n";

    vcl_string name = database[i];
    name = name.substr(0, name.length()-4) + ".png";
    vcl_string image_file = img_dir + name;
    tf << "<TR> <TD> <img src=\"" << image_file << "\"> ";
    tf << database[i] << " " << class_id << " </TD> "; 

    for (unsigned m = 0; m < 15; m++) {
      unsigned min_j = 0;
      double min = matching_costs_norm[i][0];
      for (unsigned j = 0; j < D; j ++) {
        if (matching_costs_norm[i][j] < min) {
          min = matching_costs_norm[i][j];
          min_j = j;
        }
      }
      if (print)
        vcl_cout << database[min_j] << " (val: " << min << ") ";
      int class_id_j = (min_j - min_j%11)/11;
      if (class_id_j == class_id) {
        correct[m]++;
      } else {
        wrong[m]++;
      }
      matching_costs_norm[i][min_j] = 100000;

      vcl_string name2 = database[min_j];
      name2 = name2.substr(0, name2.length()-4) + ".png";
      vcl_string image_file2 = img_dir + name2;
      tf << "<TD> <img src=\"" << image_file2 << "\"> ";
      tf << database[min_j] << " " << min << " " << class_id_j << " </TD> ";
    }

    tf << "</TR>\n";

    if (print)
      vcl_cout << "\n";
  }

  tf << "</TABLE>\n";
  tf.close();

  for (unsigned m = 0; m < 15; m++) {
    vcl_cout << "correct: " << correct[m] << " wrong: " << wrong[m] << " total: " << correct[m] + wrong[m] << vcl_endl;
    unsigned sum_c = 0, sum_w = 0;
    for (unsigned mm = 0; mm <= m; mm++) {
      sum_c += correct[mm];
      sum_w += wrong[mm];
    }
    sum_c /= (m+1);
    sum_w /= (m+1);

    vcl_cout << "correct in top " << m+1 << ": " << sum_c << "% wrong in top " << m+1 << ": " << sum_w << "%" << vcl_endl;
  }

  return 0;

}

void usage_match() {
  vcl_cout << "Usage: <program name> <database_list> <cons_dir> <shock pruning threshold> <shock curve sampling ds (5.0: coarse, 1.0: finest)> <outfile name> <0 if normal splice 1 if elastic splice cost> <0 if construct_circular_ends, 1 if no circular completions> <0 if original edit distance, 1 if edit distance with combined costs> <output dir to save binary match files>\n";
  //vcl_cout << "if <output dir binary hgms> exists then saves shgm files in this directory with additional string added automatically\n";
}

int main_match(int argc, char *argv[]) {
  vcl_cout << "Matching observations and shock graphs!\n";

  //: out file contains the wrong mathces if any
  vcl_string database_list, cons_dir, out_file, output_dir_shgm;

  vcl_cout << "argc: " << argc << vcl_endl;
  //if (argc == 10) not saving shgm files
  if (argc != 10) {
    usage_match();
    return -1;   
  }
  database_list = argv[1];
  cons_dir = argv[2];

  double pruning_thres = atof(argv[3]);
  vcl_string pruning_str = argv[3];
  double sampling_ds = atof(argv[4]);
  vcl_string sampling_str = argv[4];
  
  
  bool elastic_splice_cost = false;
  int option = atoi(argv[6]);
  if (option == 1) {
    vcl_cout << "USING ELASTIC SPLICE COST!!!!!!!!!\n";
    elastic_splice_cost = true;
  }

  bool construct_circular_ends = true;
  option = atoi(argv[7]);
  if (option == 1) {
    vcl_cout << "Matching with NO Circular COMPLETIONS!!!!!!!!!\n";
    construct_circular_ends = false;
  }

  bool original_edit = true;
  option = atoi(argv[8]);
  if (option == 1) {
    vcl_cout << "Using edit distance with COMBINED costs!!!!!!!!!\n";
    original_edit = false;
  }

  vcl_string elastic_str, circular_str, edit_str;
  if (elastic_splice_cost)
    elastic_str = "elastic";
  else
    elastic_str = "analytic";
  if (construct_circular_ends) 
    circular_str = "circular";
  else
    circular_str = "noncircular";
  if (original_edit) 
    edit_str = "original-" + pruning_str + "-" + sampling_str;
  else
    edit_str = "combined-" + pruning_str + "-" + sampling_str;

  out_file = vcl_string(argv[5]) + "_" + elastic_str + "_" + circular_str + "_" + edit_str + ".out";

  bool save_shgm = false;
  if (argc == 10) {
    save_shgm = true;
    output_dir_shgm = vcl_string(argv[9])+"/";
  } else {
    output_dir_shgm = "";
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

  vcl_cout << "out_file: " << out_file << vcl_endl;
  vcl_string dump_file = "dump_results_" + out_file;
  vcl_cout << "dump_file: " << dump_file << vcl_endl;

  //: load cons
  vcl_vector<vsol_polygon_2d_sptr> database_polygons;
  for (unsigned int i = 0; i<D; i++) {
    vcl_string con_file = cons_dir + database[i];
    vsol_polygon_2d_sptr poly = read_con_from_file(con_file.c_str());
    vsol_polygon_2d_sptr new_poly = fit_lines_to_contour(poly, 0.05f);
    database_polygons.push_back(new_poly);
  }

  //: create trees
  vcl_vector<dbskr_tree_sptr> database_trees;
  vcl_cout << "creating trees \n";
  for (unsigned int i = 0; i<D; i++) {
    vcl_cout << "extracting shocks from: " << database[i] << vcl_endl;
    dbsk2d_shock_graph_sptr sg = dbsk2d_compute_xshocks(database_polygons[i], float(pruning_thres), 1.0f);
    dbskr_tree_sptr tree = new dbskr_tree(sampling_ds);
    tree->acquire(sg, elastic_splice_cost, construct_circular_ends, !original_edit);
    database_trees.push_back(tree);
    //vcl_cout << ".";
  }
  vcl_cout << " created trees\n";
  vbl_array_2d<double> matching_costs(D, D, 100000);
  vbl_array_2d<double> matching_costs_norm(D, D, 100000);

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
        if3 >> matching_costs_norm[i][j];
        matching_costs[j][i] = matching_costs[i][j];
        matching_costs_norm[j][i] = matching_costs_norm[i][j];

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
      of3 << i << " " << j << " " << matching_costs[i][j] << " " << matching_costs_norm[i][j] << "\n";
    }
  of3.close();

  for (unsigned int i = i_start; i<D; i++) {
    unsigned int j = (i == i_start ? j_start : i+1);
    for ( ; j<D; j++) {
      vul_timer t;
      dbskr_sm_cor_sptr sm_cor1, sm_cor2;
      float val, norm_val;
      vcl_string sm_file_name1, sm_file_name2;
      sm_file_name1 = output_dir_shgm+database[i]+"_"+database[j]+"-"+elastic_str + "-" + circular_str + "-" + edit_str+ ".bin";
      sm_file_name2 = output_dir_shgm+database[j]+"_"+database[i]+"-"+elastic_str + "-" + circular_str + "-" + edit_str+ ".bin";
      
      
      if (vul_file::exists(sm_file_name1)) {
        sm_cor1 = new dbskr_sm_cor(database_trees[i], database_trees[j]);
        vsl_b_ifstream bf(sm_file_name1);
        sm_cor1->b_read(bf);  // reads normalized cost as well which is valid if computed
        bf.close();
      } else {
        vcl_cout << "matching " << database[i] << " (nodes: " << database_trees[i]->node_size() << ") and " << database[j] << " (nodes: " << database_trees[j]->node_size() << ")..." << vcl_endl;
        if (original_edit) {
          //instantiate the edit distance algorithm
          dbskr_tree_edit edit(database_trees[i], database_trees[j], construct_circular_ends);

          edit.save_path(true);
          if (!edit.edit()) {
            vcl_cout << "Problems in editing trees\n";
            return false;
          }
          val = edit.final_cost();
          norm_val = val/(database_trees[i]->total_splice_cost()+database_trees[j]->total_splice_cost());
          sm_cor1 = edit.get_correspondence();
          sm_cor1->set_final_cost(val);
          sm_cor1->set_final_norm_cost(norm_val);
          sm_cor1->contstruct_circular_ends_ = construct_circular_ends;
        } else {
          //instantiate the edit distance algorithm
          dbskr_tree_edit_combined edit(database_trees[i], database_trees[j], construct_circular_ends);

          edit.save_path(true);
          if (!edit.edit()) {
            vcl_cout << "Problems in editing trees\n";
            return false;
          }
          val = edit.final_cost();
          norm_val = val/(database_trees[i]->total_splice_cost()+database_trees[j]->total_splice_cost());
          sm_cor1 = edit.get_correspondence();
          sm_cor1->set_final_cost(val);
          sm_cor1->set_final_norm_cost(norm_val);
          sm_cor1->contstruct_circular_ends_ = construct_circular_ends;
        }
        vcl_cout << "final cost: " << val << " final norm cost: " << norm_val << vcl_endl;
        vsl_b_ofstream bf(sm_file_name1.c_str());
        sm_cor1->b_write(bf);
        bf.close();
        
      }

      if (vul_file::exists(sm_file_name2)) {
        sm_cor2 = new dbskr_sm_cor(database_trees[j], database_trees[i]);
        vsl_b_ifstream bf(sm_file_name2);
        sm_cor2->b_read(bf);
        bf.close();
      } else {
        vcl_cout << "matching " << database[j] << " (nodes: " << database_trees[j]->node_size() << ") and " << database[i] << " (nodes: " << database_trees[i]->node_size() << ")..." << vcl_endl;
        if (original_edit) {
          //instantiate the edit distance algorithm
          dbskr_tree_edit edit(database_trees[j], database_trees[i], construct_circular_ends);

          edit.save_path(true);
          if (!edit.edit()) {
            vcl_cout << "Problems in editing trees\n";
            return false;
          }
          val = edit.final_cost();
          norm_val = val/(database_trees[j]->total_splice_cost()+database_trees[i]->total_splice_cost());
          sm_cor2 = edit.get_correspondence();
          sm_cor2->set_final_cost(val);
          sm_cor2->set_final_norm_cost(norm_val);
          sm_cor2->contstruct_circular_ends_ = construct_circular_ends;
        } else {
          //instantiate the edit distance algorithm
          dbskr_tree_edit_combined edit(database_trees[j], database_trees[i], construct_circular_ends);

          edit.save_path(true);
          if (!edit.edit()) {
            vcl_cout << "Problems in editing trees\n";
            return false;
          }
          val = edit.final_cost();
          norm_val = val/(database_trees[j]->total_splice_cost()+database_trees[i]->total_splice_cost());
          sm_cor2 = edit.get_correspondence();
          sm_cor2->set_final_cost(val);
          sm_cor2->set_final_norm_cost(norm_val);
          sm_cor2->contstruct_circular_ends_ = construct_circular_ends;
        }
        vcl_cout << "final cost: " << val << " final norm cost: " << norm_val << vcl_endl;
        vsl_b_ofstream bf(sm_file_name2.c_str());
        sm_cor2->b_write(bf);
        bf.close();
        
      }

      if (sm_cor1->final_cost() < sm_cor2->final_cost()) {
        matching_costs[i][j] = sm_cor1->final_cost();
        matching_costs[j][i] = matching_costs[i][j];
      } else {
        matching_costs[i][j] = sm_cor2->final_cost();
        matching_costs[j][i] = matching_costs[i][j]; 
      }

      if (sm_cor1->final_norm_cost() < sm_cor2->final_norm_cost()) {
        matching_costs_norm[i][j] = sm_cor1->final_norm_cost();
        matching_costs_norm[j][i] = matching_costs_norm[i][j];
      } else {
        matching_costs_norm[i][j] = sm_cor2->final_norm_cost();
        matching_costs_norm[j][i] = matching_costs_norm[i][j]; 
      }

      of3.open(dump_file.c_str(), vcl_ofstream::app);
      of3 << i << " " << j << " " << matching_costs[i][j] << " " << matching_costs_norm[i][j] << "\n";
      of3.close();
   }
  }

  vcl_ofstream of((out_file).c_str());
  of << "\n shock costs: \n" << D << " " << D << "\n";
  for (unsigned i = 0; i<D; i++) {
    for (unsigned j = 0; j<D; j++)
      of << matching_costs[i][j] << " ";
    of << "\n";
  }

  of << "\n norm shock costs: \n" << D << " " << D << "\n";
  for (unsigned i = 0; i<D; i++) {
    for (unsigned j = 0; j<D; j++)
      of << matching_costs_norm[i][j] << " ";
    of << "\n";
  }

  of.close();
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc == 10) {
    main_match(argc, argv); 
  } else if (argc == 8) {
    main_evaluate(argc, argv);
  } else {
    usage_match();
    vcl_cout << vcl_endl;
    usage_evaluate();
  }
}

