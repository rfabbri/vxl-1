// compare pairs of observations

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

#include <dbskr/dbskr_tree_edit_pmi.h>
#include <dbskr/dbskr_tree.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

#include <vul/vul_timer.h>
#include <dbru/dbru_rcor_sptr.h>
#include <dbru/dbru_rcor.h>
#include <dbru/algo/dbru_object_matcher.h>

#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_matcher.h>

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

dbskr_tree_sptr read_esf_from_file(vcl_string fname) {
  dbsk2d_xshock_graph_fileio loader;
  dbskr_tree_sptr tree = new dbskr_tree();
  dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(fname);
  tree->acquire(sg, true, true, true);    // true, true, true was usually the default for this project but needs to be set properly
  return tree;
}

int main(int argc, char *argv[]) {
  vcl_cout << "Matching observations and shock graphs!\n";

  //: out file contains the wrong mathces if any
  vcl_string query_list, database_list, images_dir, esfs_dir, cons_dir, shgms_dir, out_file, output_dir;

  vcl_cout << "argc: " << argc << vcl_endl;
  if (argc != 11) {
    vcl_cout << "Usage: <program name> <query_list> <database_list> <images_dir> <esfs_dir> <cons_dir> <shgms_dir> <additional_string> <out file name> <output_dir> <0>\n";
    vcl_cout << "<additional string> is to be added to end of each name in database and query lists, e.g. _055-135-068-030, should be . if nothing is necessary \n";
    vcl_cout << "last argument is 0 if normal edit distance, 1 if pmi\n";
    return -1;
  }
  query_list = argv[1];
  database_list = argv[2];
  images_dir = argv[3];
  esfs_dir = argv[4];
  cons_dir = argv[5];
  shgms_dir = argv[6];
  vcl_string addition = argv[7];
  vcl_cout << "additional string read: " << addition << "\n";
  out_file = argv[8];
  output_dir = argv[9];
  int edit_type = atoi(argv[10]);
 
  vcl_ifstream fpq((query_list).c_str());

  if (!fpq.is_open()) {
    vcl_cout << "Unable to open query file!\n";
    return -1;
  }
  vcl_vector<vcl_string> query, database; vcl_string::size_type pos;
  char buffer[1000];
  while (!fpq.eof()) {
    vcl_string temp;
    fpq.getline(buffer, 1000);
    temp = buffer;
    if (temp.size() > 1) {
      vcl_cout << "temp: " << temp << " addition: " << addition << vcl_endl;
      pos = temp.find(".");
      if (pos == vcl_string::npos) {
        vcl_cout << "we did not find dot, adding addition: " << temp+addition << "\n";
        query.push_back(temp+addition);
      }
      else {
        vcl_cout << "uups we found dot, \n";
        query.push_back(temp.substr(pos+1, temp.size()-1));
      }
    }
  }
  fpq.close();
  vcl_ifstream fpd((database_list).c_str());
  if (!fpd.is_open()) {
    vcl_cout << "Unable to open database file!\n";
    return -1;
  }
  while (!fpd.eof()) {
    vcl_string temp;
    fpd.getline(buffer, 1000);
    temp = buffer;
    if (temp.size() > 1) {
      pos = temp.find(".");
      if (pos == vcl_string::npos)
        database.push_back(temp+addition);
      else 
        database.push_back(temp.substr(pos+1, temp.size()-1));
    }
  }
  fpd.close();

  unsigned int D = database.size(); unsigned int Q = query.size();
  vcl_cout << "Q: " << Q << " D: " << D << "\n";
#if 1
  vcl_cout << "printing query list: \n";
  for (unsigned int i = 0; i<Q; i++) {
    vcl_cout << query[i] << "\n";
  } vcl_cout << "printing database list: \n";
  for (unsigned int i = 0; i<D; i++) {
    vcl_cout << database[i] << "\n";
  }
#endif

  //: load images
  vcl_vector<vil_image_resource_sptr> query_images, database_images;
  for (unsigned int i = 0; i<Q; i++) {
    vcl_string image_file = images_dir + query[i] + ".png";
    vil_image_resource_sptr image_r = vil_load_image_resource( image_file.c_str() );
    query_images.push_back(image_r);
  }
  for (unsigned int i = 0; i<D; i++) {
    vcl_string image_file = images_dir + database[i] + ".png";
    vil_image_resource_sptr image_r = vil_load_image_resource( image_file.c_str() );
    database_images.push_back(image_r);
  }
  vcl_cout << "loaded images\n";

  //: load cons
  vcl_vector<vsol_polygon_2d_sptr> query_polygons, database_polygons;
  for (unsigned int i = 0; i<Q; i++) {
    vcl_string con_file = cons_dir + query[i] + ".con";
    vsol_polygon_2d_sptr poly = read_con_from_file(con_file.c_str());
    query_polygons.push_back(poly);
  }
  for (unsigned int i = 0; i<D; i++) {
    vcl_string con_file = cons_dir + database[i] + ".con";
    vsol_polygon_2d_sptr poly = read_con_from_file(con_file.c_str());
    database_polygons.push_back(poly);
  }
  vcl_cout << "loaded contours, preparing observations..\n";
  vcl_vector<dbinfo_observation_sptr> query_obs, database_obs;
  for (unsigned int i = 0; i<Q; i++) {
    dbinfo_observation_sptr obs = new dbinfo_observation(0, query_images[i], query_polygons[i], true, true, false);
    query_obs.push_back(obs);
  }
  for (unsigned int i = 0; i<D; i++) {
    dbinfo_observation_sptr obs = new dbinfo_observation(0, database_images[i], database_polygons[i], true, true, false);
    database_obs.push_back(obs);
  }
  //: load esfs and create trees
  vcl_vector<dbskr_tree_sptr> query_trees, database_trees;
  for (unsigned int i = 0; i<Q; i++) {
    vcl_string esf_file = esfs_dir + query[i] + ".esf";
    dbskr_tree_sptr tree = read_esf_from_file(esf_file.c_str());
    query_trees.push_back(tree);
  }
  for (unsigned int i = 0; i<D; i++) {
    vcl_string esf_file = esfs_dir + database[i] + ".esf";
    dbskr_tree_sptr tree = read_esf_from_file(esf_file.c_str());
    database_trees.push_back(tree);
  }
  vcl_cout << "loaded esfs\n";
  //unused int match = 0, mismatch = 0, notfound = 0;
  vbl_array_2d<double> matching_costs(Q, D, 0);
  vbl_array_2d<float> infos(Q, D, 0);
  for (unsigned int i = 0; i<Q; i++) {
    for (unsigned int j = 0; j<D; j++) {
      vul_timer t;
      dbskr_sm_cor_sptr sm_cor; double shock_matching_cost;
      if (edit_type == 0)
        sm_cor = dbru_object_matcher::compute_shock_alignment(query_trees[i],database_trees[j],shock_matching_cost,false);
      else if (edit_type == 1)
        sm_cor = dbru_object_matcher::compute_shock_alignment_pmi(query_obs[i], database_obs[j], query_trees[i],database_trees[j],shock_matching_cost,false);
      matching_costs[i][j] = shock_matching_cost;
      dbru_rcor_sptr rcor = dbru_object_matcher::generate_rcor_shock_matching(query_obs[i], database_obs[j], sm_cor, false);
      vil_image_resource_sptr out_image = rcor->get_appearance2_on_pixels1();
      infos[i][j] = dbinfo_observation_matcher::minfo(query_obs[i], database_obs[j], rcor->get_correspondences(), false);
      vil_save_image_resource(out_image, (output_dir+query[i]+"_"+database[j]+".png").c_str()); 
      vcl_cout << "cost " << shock_matching_cost << " matching time: "<< t.real()/1000.0f << " secs\n";
   }
  }

  vcl_ofstream of((out_file).c_str());
  int N = D;
  for (unsigned int i = 0; i<Q; i++) {
    of << "query: " << query[i] << " shock matching costs and top N matches\n";
    for (int k = 0; k<N; k++) {
      double mcost = 1e7;
      int mj = 0;
      for (unsigned int j = 0; j<D; j++) 
        if (matching_costs[i][j] < mcost) {
          mcost = matching_costs[i][j];
          mj = j;
        }
      of << "k: " << k << " " << database[mj] << " cost: " << mcost << "\n";
      matching_costs[i][mj] = 1e7;
    }
    
    of << "query: " << query[i] << " mutual infos and top N matches\n";
    for (int k = 0; k<N; k++) {
      float mcost = -1e7;
      int mj = 0;
      for (unsigned int j = 0; j<D; j++) 
        if (infos[i][j] > mcost) {
          mcost = infos[i][j];
          mj = j;
        }
      of << "k: " << k << " " << database[mj] << " info: " << mcost << "\n";
      infos[i][mj] = -1e7;
    }
  }
  of.close();

  return 0;
}
