// compare pairs of observations

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

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

#include <vul/vul_timer.h>
#include <dbru/dbru_rcor_sptr.h>
#include <dbru/dbru_rcor.h>
#include <dbru/algo/dbru_object_matcher.h>
#include <dbcvr/dbcvr_cv_cor.h>

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

dbskr_tree_sptr read_esf_from_file(vcl_string fname, double sampling_ds, bool elastic_splice) {
  dbsk2d_xshock_graph_fileio loader;
  dbskr_tree_sptr tree = new dbskr_tree(sampling_ds);
  dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(fname);
  tree->acquire(sg, elastic_splice);    
  return tree;
}

int main_shock(int argc, char *argv[]) {
  vcl_cout << "Matching observations and shock graphs!\n";

  //: out file contains the wrong mathces if any
  vcl_string database_list, images_dir, esfs_dir, cons_dir, out_file, output_dir, output_dir_shgm;

  vcl_cout << "argc: " << argc << vcl_endl;
  //if (argc == 10) not saving shgm files
  if (argc != 12 && argc != 13) {
    vcl_cout << "Usage: <program name> <database_list> <images_dir> <esfs_dir> <cons_dir> <size> <additional_string> <shock curve sampling ds (5.0: coarse, 1.0: finest)> <output_dir> <outfile name> <0 if normal splice 1 if elastic splice cost> <0 if normalize cost, 1 if no normalization> <output_dir_shgms>\n";
    vcl_cout << "<size> is 032, 064, 128 or 256, it is to be added to create output directory names when necessary\n";
    vcl_cout << "<additional string> is to be added to end of each name in database list, e.g. 055-135-068-030\n";
    vcl_cout << "<output_dir> directory for output images, e.g. shock_matches/ additional string will be added automatically\n";
    vcl_cout << "if <output_dir_shgms> exists then saves shgm files in this directory with additional string added automatically\n";
    return -1;
  }
  database_list = argv[1];
  images_dir = argv[2];
  esfs_dir = argv[3];
  cons_dir = argv[4];
  //unused int size = atoi(argv[5]);
  vcl_string addition = argv[6];
  vcl_cout << "additional string read: " << addition << "\n";

  double sampling_ds = atof(argv[7]);
  output_dir = vcl_string(argv[8])+vcl_string(argv[5])+"-"+addition+"/";
  out_file = vcl_string(argv[9])+"_"+vcl_string(argv[5])+"_";
  
  bool elastic_splice = false;
  int option = atoi(argv[10]);
  if (option == 1) {
    vcl_cout << "USING ELASTIC SPLICE COST!!!!!!!!!\n";
    elastic_splice = true;
  }

  bool normalize_cost = false;
  option = atoi(argv[11]);
  if (option == 0) {
    vcl_cout << "NORMALIZING SHOCK COSTS!!!!!!!!!\n";
    normalize_cost = true;
  }
  
  bool save_shgm = false;
  if (argc == 13) {
    save_shgm = true;
    output_dir_shgm = vcl_string(argv[12])+vcl_string(argv[5])+"-"+addition+"/";
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
      vcl_cout << "temp: " << temp << " addition: " << addition << vcl_endl;
      database.push_back(temp+"_"+addition);
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

  vcl_string dummy = addition;
  char *pch = vcl_strtok ((char *)dummy.c_str(),"-");
  while (pch != NULL)
  {
    printf ("%s\n",pch);
    out_file = out_file + pch + "_";
    pch = strtok (NULL, "-");
  }
  out_file = out_file+"shock_"+argv[7]+".out";
  vcl_cout << "out_file: " << out_file << vcl_endl;
  vcl_string dump_file = "dump_results_" + out_file;
  vcl_cout << "dump_file: " << dump_file << vcl_endl;

  //: load images
  vcl_vector<vil_image_resource_sptr> database_images;
  for (unsigned int i = 0; i<D; i++) {
    vcl_string image_file = images_dir + database[i] + ".png";
    vil_image_resource_sptr image_r = vil_load_image_resource( image_file.c_str() );
    database_images.push_back(image_r);
  }
  vcl_cout << "loaded images\n";

  //: load cons
  vcl_vector<vsol_polygon_2d_sptr> database_polygons;
  for (unsigned int i = 0; i<D; i++) {
    vcl_string con_file = cons_dir + database[i] + ".con";
    vsol_polygon_2d_sptr poly = read_con_from_file(con_file.c_str());
    database_polygons.push_back(poly);
  }

  vcl_cout << "loaded contours, preparing observations..\n";
  vcl_vector<dbinfo_observation_sptr> database_obs;
  for (unsigned int i = 0; i<D; i++) {
    dbinfo_observation_sptr obs = new dbinfo_observation(0, database_images[i], database_polygons[i], true, true, false);
    database_obs.push_back(obs);
  }
  
  //: load esfs and create trees
  vcl_vector<dbskr_tree_sptr> database_trees;
  for (unsigned int i = 0; i<D; i++) {
    vcl_string esf_file = esfs_dir + database[i] + ".esf";
    vcl_cout << "loading esf_file: " << esf_file << vcl_endl;
    dbskr_tree_sptr tree = read_esf_from_file(esf_file.c_str(), sampling_ds, elastic_splice);
    database_trees.push_back(tree);
  }

  vcl_cout << "loaded esfs\n";
  vbl_array_2d<double> matching_costs(D, D, 100000);
  vbl_array_2d<float> infos(D, D, 100000);

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

        if3 >> infos[i][j];
        infos[j][i] = infos[i][j];

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
      of3 << i << " " << j << " " << matching_costs[i][j] << " " << infos[i][j] << "\n";
    }
  of3.close();

  for (unsigned int i = i_start; i<D; i++) {
    unsigned int j = (i == i_start ? j_start : i+1);
    for ( ; j<D; j++) {
      vul_timer t;
      dbskr_sm_cor_sptr sm_cor1, sm_cor2; double cost1, cost2, norm;
      vcl_string shgm_file_name1 = output_dir_shgm+database[i]+"_"+database[j]+".shgm";
      vcl_string shgm_file_name2 = output_dir_shgm+database[j]+"_"+database[i]+".shgm";
      vcl_ifstream shgmf1(shgm_file_name1.c_str());
      vcl_ifstream shgmf2(shgm_file_name2.c_str());

      if (normalize_cost) {
        norm = database_trees[i]->total_splice_cost()+database_trees[j]->total_splice_cost();
        vcl_cout << " norm: " << norm << " ";
      } else
        norm = 1.0f;
      
      if (shgmf1.is_open()) {
        vcl_cout << "reading " << shgm_file_name1 << vcl_endl;
        shgmf1.close();
        sm_cor1 = new dbskr_sm_cor(database_trees[i], database_trees[j]);
        cost1 = sm_cor1->read_and_construct_from_shgm(shgm_file_name1);
      } else {
        sm_cor1 = dbru_object_matcher::compute_shock_alignment(database_trees[i],database_trees[j],cost1,false);
        //: shgms contain non-normalized costs as a convention
        if (save_shgm) 
          sm_cor1->write_shgm(cost1, shgm_file_name1);
      }
      
      if (shgmf2.is_open()) {
        vcl_cout << "reading " << shgm_file_name2 << vcl_endl;
        shgmf2.close();
        sm_cor2 = new dbskr_sm_cor(database_trees[j], database_trees[i]);
        cost2 = sm_cor2->read_and_construct_from_shgm(shgm_file_name2);
      } else {
        sm_cor2 = dbru_object_matcher::compute_shock_alignment(database_trees[j],database_trees[i],cost2,false);      
        //: shgms contain non-normalized costs as a convention
        if (save_shgm) 
          sm_cor2->write_shgm(cost2, shgm_file_name2);
      }
      cost1 /= norm;
      cost2 /= norm;
      if (cost1 < cost2) {
        matching_costs[i][j] = cost1;
        matching_costs[j][i] = matching_costs[i][j];
      } else {
        matching_costs[i][j] = cost2;
        matching_costs[j][i] = matching_costs[i][j]; 
      }
              
      dbru_rcor_sptr rcor1 = dbru_object_matcher::generate_rcor_shock_matching(database_obs[i], database_obs[j], sm_cor1, false);
      dbru_rcor_sptr rcor2 = dbru_object_matcher::generate_rcor_shock_matching(database_obs[j], database_obs[i], sm_cor2, false);
      float info1 = dbinfo_observation_matcher::minfo(database_obs[i], database_obs[j], rcor1->get_correspondences(), false);
      float info2 = dbinfo_observation_matcher::minfo(database_obs[j], database_obs[i], rcor2->get_correspondences(), false);

      if (info1 > info2) {
        infos[i][j] = info1;
        infos[j][i] = info1;
        vil_image_resource_sptr out_image = rcor1->get_appearance2_on_pixels1();
        vil_save_image_resource(out_image, (output_dir+database[i]+"_"+database[j]+"-shock.png").c_str()); 
        vcl_cout << "i: " << i << " j: " << j << " cost " << cost1 << " info: " << info1 << " matching time: "<< t.real()/1000.0f << " secs\n";
      } else {
        infos[i][j] = info2;
        infos[j][i] = info2;
        vil_image_resource_sptr out_image = rcor2->get_appearance2_on_pixels1();
        vil_save_image_resource(out_image, (output_dir+database[j]+"_"+database[i]+"-shock.png").c_str()); 
        vcl_cout << "i: " << i << " j: " << j << " cost " << cost2 << " info: " << info2 << " matching time: "<< t.real()/1000.0f << " secs\n";
      }

      of3.open(dump_file.c_str(), vcl_ofstream::app);
      of3 << i << " " << j << " " << matching_costs[i][j] << " " << infos[i][j] << "\n";
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

  of << "\n infos: \n" << D << " " << D << "\n";
  for (unsigned i = 0; i<D; i++) {
    for (unsigned j = 0; j<D; j++)
      of << infos[i][j] << " ";
    of << "\n";
  }

  of.close();

  return 0;
}

int main(int argc, char *argv[]) {
  main_shock(argc, argv);
}
