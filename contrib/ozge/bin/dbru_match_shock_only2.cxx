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
  vcl_cout << "Matching observations and shock graphs reading pairs from a list!\n";

  //: out file contains the wrong mathces if any
  vcl_string database_list, esfs_dir, out_file, output_dir_shgm;

  vcl_cout << "argc: " << argc << vcl_endl;
  //if (argc == 10) not saving shgm files
  if (argc != 6 && argc != 7) {
    vcl_cout << "Usage: <program name> <pairs list> <esfs_dir> <shock curve sampling ds> <output_file> <output_dir_shgms>\n";
    vcl_cout << "shock curve sampling ds: 5.0 coarse, 1.0: finest, typically 1.0 is used to get best recognition\n";
    vcl_cout << "<matrix name> just give initial portion of the name: shock sampling ds and .out will be added automatically\n";
    vcl_cout << "if <output_dir_shgms> exists then saves shgm files in this directory (put \\ character at the end of directory name) \n";
    return -1;
  }
  database_list = argv[1];
  esfs_dir = argv[2];
  double sampling_ds = atof(argv[3]);
  out_file = vcl_string(argv[4])+"_"+vcl_string(argv[3])+"_pair_distances.out";
  
  bool save_shgm = false;
  if (argc == 6) {
    save_shgm = true;
    output_dir_shgm = vcl_string(argv[5]);
  }

  vcl_vector<vcl_pair<vcl_string, vcl_string> > pairs_list; 
  vcl_map<vcl_string, int> item_ids;
  vcl_map<vcl_string, int>::iterator iter;
  vcl_ifstream fpd((database_list).c_str());
  if (!fpd.is_open()) {
    vcl_cout << "Unable to open database file!\n";
    return -1;
  }

  char buffer[1000];
  int cnt = 0;
  while (!fpd.eof()) {
    vcl_string temp;
    fpd.getline(buffer, 1000);
    temp = buffer;
    if (temp.size() > 1) {
      
      vcl_cout << "pair: " << temp << vcl_endl;
      vcl_string::size_type pos = temp.find(" ", 0);
      vcl_string item1 = temp.substr(0, pos);
      vcl_string item2 = temp.substr(pos+1, temp.length());
      vcl_cout << "item1: " << item1 << " item2: " << item2 << vcl_endl;
      
      iter = item_ids.find(item1);
      if (iter == item_ids.end()) {
        item_ids[item1] = cnt;
        cnt++;
      }
      iter = item_ids.find(item2);
      if (iter == item_ids.end()) {
        item_ids[item2] = cnt;
        cnt++;
      }
      pairs_list.push_back(vcl_pair<vcl_string, vcl_string> (item1, item2));
    }
  }
  fpd.close();
  
  unsigned int D = pairs_list.size(); 
  vcl_cout << " pairs size: " << D << "\n";
#if 1

  vcl_cout << "printing items and ids: \n";
  for (iter = item_ids.begin(); iter != item_ids.end(); iter++) {
    vcl_cout << iter->first << " id: " << iter->second << "\n";
  }

  vcl_cout << "printing pairs list: \n";
  for (unsigned int i = 0; i<D; i++) {
    vcl_cout << pairs_list[i].first << " " << pairs_list[i].second << "\n";
  }
#endif

  vcl_cout << "out_file: " << out_file << vcl_endl;
 
  //: load esfs and create trees
  vcl_vector<dbskr_tree_sptr> database_trees;
  for (iter = item_ids.begin(); iter != item_ids.end(); iter++) {
    vcl_string esf_file = esfs_dir + "crop" + iter->first + ".esf";
    vcl_cout << "reading esf: " << esf_file << vcl_endl;
    dbskr_tree_sptr tree = read_esf_from_file(esf_file.c_str(), sampling_ds);
    database_trees.push_back(tree);
  }

  vcl_cout << "loaded esfs\n";

  unsigned int i_start = 0; // in pairs_list
  vcl_ifstream if3;
  vcl_map<vcl_pair<vcl_string, vcl_string>, float> pair_dists;

  if3.open(out_file.c_str());
  if (if3) {
    vcl_string item1, item2; float dist; bool tag = false;
    if3 >> item1;  // read first item
    while (!if3.eof()) {
      if3 >> item2;   // read j
      if (if3.eof()) break;
      if3 >> dist;
      vcl_pair<vcl_string, vcl_string> items(item1, item2);
      pair_dists[items] = dist;
      if3 >> item2;  // try reading item1 again, if not then it'll break 
    }
    if3.close();
  }
  
  vcl_ofstream of3;
  of3.open(out_file.c_str());
  
  vcl_map<vcl_pair<vcl_string, vcl_string>, float>::iterator iter2;
  for (iter2 = pair_dists.begin(); iter2 != pair_dists.end(); iter2++)
    of3 << (iter2->first).first << " " << (iter2->first).second << " " << iter2->second << "\n";
  of3.close();

  for (unsigned int is = 0; is<D; is++) {
    vul_timer t; double matching_cost;
    vcl_cout << pairs_list[is].first << " " << pairs_list[is].second << " ";
    iter2 = pair_dists.find(pairs_list[is]);
    if (iter2 == pair_dists.end()) {  // do the matching
      vcl_string namei = pairs_list[is].first;
      vcl_string namej = pairs_list[is].second;
      unsigned i = item_ids[pairs_list[is].first];
      unsigned j = item_ids[pairs_list[is].second];

      dbskr_sm_cor_sptr sm_cor1, sm_cor2; double cost1, cost2;
      sm_cor1 = dbru_object_matcher::compute_shock_alignment(database_trees[i],database_trees[j],cost1,false);
      sm_cor2 = dbru_object_matcher::compute_shock_alignment(database_trees[j],database_trees[i],cost2,false);      
      
      if (cost1 < cost2) {
        matching_cost = cost1;
        if (save_shgm) {
          vcl_string file_name = output_dir_shgm+namei+"_"+namej+".shgm";
          sm_cor1->write_shgm(cost1, file_name);
        }
      } else {
        matching_cost = cost2;
        if (save_shgm) {
          vcl_string file_name = output_dir_shgm+namej+"_"+namei+".shgm";
          sm_cor2->write_shgm(cost2, file_name);
        }
      }
      of3.open(out_file.c_str(), vcl_ofstream::app);
      of3 << namei << " " << namej << " " << matching_cost << "\n";
      vcl_cout << matching_cost << " " << t.real()/1000.0f << " secs\n";
      of3.close();
    } else {  // if exists then I read from the file so should already be written back
      matching_cost = iter2->second;
      vcl_cout << matching_cost << " " << t.real()/1000.0f << " secs\n";
    }
  }

  return 0;
}

int main(int argc, char *argv[]) {
  main_shock(argc, argv);
}
