
// aerial_vehicle_segmentation.cpp : Defines the entry point for the DLL application.
#include<vcl_cstdio.h>
//#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
//#include <vidpro1/storage/vidpro1_image_storage.h>

//#include <vil/vil_image_resource.h>
//#include <bpro1/bpro1_parameters_sptr.h>
//#include <bpro1/bpro1_parameters.h>
//#include <bpro1/bpro1_process_sptr.h>
//#include <vidpro1/vidpro1_repository.h>
//#include <vidpro1/process/vidpro1_load_video_process.h>
//#include <vidpro1/vidpro1_process_manager.h>
//#include <brip/brip_vil_float_ops.h>
//#include <vsol/vsol_point_2d.h>
//#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsl/vsl_binary_io.h>

#include <dbru/dbru_object_sptr.h>
#include <dbru/dbru_object.h>
#include <dbru/dbru_label.h>
#include <dbru/algo/dbru_object_matcher.h>


#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_feature_data.h>
#include <dbinfo/dbinfo_intensity_feature.h>
#include <dbinfo/dbinfo_gradient_feature.h>
#include <dbinfo/dbinfo_observation_matcher.h>
#include <dbinfo/dbinfo_region_geometry_sptr.h>
#include <dbinfo/dbinfo_region_geometry.h>

#include <vil1/vil1_memory_image_of.h>
#include <brip/brip_vil1_float_ops.h>
#include <vil1/vil1_vil.h>
#include <vil/vil_save.h>
//vcl_vector<vgl_point_2d<double> > meanpts;

unsigned int load_osl(const char *osl_file_name) 
{
  vbl_array_1d<dbru_object_sptr> *osl = new vbl_array_1d<dbru_object_sptr>();
  vsl_b_ifstream obfile(osl_file_name);
  unsigned int size;
  vsl_b_read(obfile, size);
  for (unsigned int i = 0; i<size; i++) {
    dbru_object_sptr obj = new dbru_object();
    obj->b_read(obfile);
    osl->push_back(obj);
  }

  obfile.close();
  vcl_cout << osl->size() << " objects and observations are read from binary input file\n";
  return reinterpret_cast<unsigned int>(osl);
}

bool check_labels(dbru_label_sptr input_label, dbru_label_sptr object_label) 
{
  if (input_label->view_angle_bin_         != object_label->view_angle_bin_ ||
      input_label->shadow_angle_bin_       != object_label->shadow_angle_bin_ ||
      input_label->shadow_length_          != object_label->shadow_length_)
      return false;

  if (input_label->motion_orientation_bin_ == object_label->motion_orientation_bin_ )
       //||
      //input_label->motion_orientation_bin_ == (object_label->motion_orientation_bin_+1)%8 ||
      //input_label->motion_orientation_bin_ == (object_label->motion_orientation_bin_-1+8)%8 )
      return true;
  return false;
}


int main(int argc, char *argv[]) {
  vcl_cout << "run shock matching on selected frames as database\n";

  vcl_string out_osl, out_file;
  int n = 5;  // use every n frames in the database
  vcl_cout << "argc: " << argc << vcl_endl;
  if (argc != 7) {
    vcl_cout << "This program assumes that query and database instances are in the same osl!!!!\n";
    vcl_cout << "Usage: <program name> <input_osl_name> <out_file> <double p_threshold> <k (number of classes)> <query pairs list> <db pairs list>\n";
    return -1;
  }

  out_osl = argv[1];
  out_file = argv[2];
  double p_threshold = atof(argv[3]);
  int k = atoi(argv[4]);
  
  vcl_string query_list = argv[5];
  vcl_string db_list = argv[6];

  //LOAD DATABASE PREVIOUSLY CREATED
  vbl_array_1d<dbru_object_sptr> *database = 
    reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (load_osl(out_osl.c_str()));

  vcl_cout << database->size() << " objects and observations are read from binary input file\n";
  //count the number of polygons
  
  int total_poly_cnt = 0;
  vcl_vector<int> used_poly_cnts;
  for (unsigned i = 0; i<database->size(); i++) {
  //for (unsigned i = 0; i<2; i++) {
    dbru_object_sptr obji = (*database)[i];
    int cnt = 0;
    for (unsigned j = 0; j<obji->polygon_cnt_; j+=n)
    //total_poly_cnt += obji->polygon_cnt_;
      cnt++;
    used_poly_cnts.push_back(cnt);
    total_poly_cnt += cnt;
  }

  vcl_ifstream fp(query_list.c_str());
  if (!fp) {
    vcl_cout << "Problems in opening selected frames query list file!\n";
    return 0;
  }
                  // obj_id, poly_id
  vcl_vector<vcl_pair<int, int> > query_pairs;
  int query_size = 0;
  fp >> query_size;
  for (int i = 0; i<query_size; i++) {  // 1 line for each object, first cnt then frame nos
    vcl_pair<int, int> obj_pair;
    fp >> obj_pair.first;
    fp >> obj_pair.second;
    query_pairs.push_back(obj_pair);
  }
  fp.close();

  vcl_ifstream fp2(db_list.c_str());
  if (!fp2) {
    vcl_cout << "Problems in opening selected frames database list file!\n";
    return 0;
  }
                  // obj_id, poly_id
  vcl_vector<vcl_pair<int, int> > database_pairs;
  int database_size = 0;
  fp2 >> database_size;
  for (int i = 0; i<database_size; i++) {  // 1 line for each object, first cnt then frame nos
    vcl_pair<int, int> obj_pair;
    fp2 >> obj_pair.first;
    fp2 >> obj_pair.second;
    database_pairs.push_back(obj_pair);
  }
  fp2.close();

  vcl_map<vcl_string, int> category_id_map;
  if (k == 4) {
    category_id_map["minivan"] = 0;
    category_id_map["suv"] = 1;
    category_id_map["car"] = 2;
    category_id_map["pick-up truck"] = 3;
    category_id_map["van"] = 4;
  } else if (k == 3) {
    category_id_map["minivan"] = 0;
    category_id_map["suv"] = 0;
    category_id_map["utility"] = 0;
    category_id_map["car"] = 1;
    category_id_map["pick-up truck"] = 2;
    category_id_map["van"] = 3;
  }

  vcl_ofstream of1((out_file+".out").c_str());
  of1 << "db size: " << database_size << vcl_endl;
  
  vcl_string dump_file = "dump_"+out_file+".out";
  vcl_map<vcl_pair<vcl_pair<unsigned int, unsigned int> , vcl_pair<unsigned int, unsigned int> >, vcl_pair<double, double> > matched_instances;
  
  vcl_ifstream if3(dump_file.c_str());
  if (if3) {
    unsigned int i, pi, j, pj; double shock, info;
    if3 >> i;  // read first i
    while (!if3.eof()) {
      if3 >> pi;   // read j
      if (if3.eof()) break;
      if3 >> j;
      if (if3.eof()) break;
      if3 >> pj;
      if (if3.eof()) break;
      if3 >> shock;
      if (if3.eof()) break;
      if3 >> info;
      vcl_pair<unsigned int, unsigned int> q_pair(i, pi);
      vcl_pair<unsigned int, unsigned int> d_pair(j, pj);
      vcl_pair<vcl_pair<unsigned int, unsigned int> , vcl_pair<unsigned int, unsigned int> > key(q_pair, d_pair);
      matched_instances[key] = vcl_pair<double, double> (shock, info);
      vcl_pair<vcl_pair<unsigned int, unsigned int> , vcl_pair<unsigned int, unsigned int> > key2(d_pair, q_pair);
      matched_instances[key2] = vcl_pair<double, double> (shock, info);
      
      if3 >> i;  // try reading i again, if not then it'll break 
    }
    if3.close();
  }

  vcl_ofstream of3;
  of3.open(dump_file.c_str());
  // first dump the currently known elements:
  vcl_map<vcl_pair<vcl_pair<unsigned int, unsigned int> , vcl_pair<unsigned int, unsigned int> >, vcl_pair<double, double> >::iterator iter;
  for (iter = matched_instances.begin(); iter != matched_instances.end(); iter++) {
    vcl_pair<vcl_pair<unsigned int, unsigned int> , vcl_pair<unsigned int, unsigned int> > key = iter->first;
    vcl_pair<double, double> costs = iter->second;
    of3 << key.first.first << " " << key.first.second << " " << key.second.first << " " << key.second.second;
    of3 << " " << costs.first << " " << costs.second << "\n";
  }

  vbl_array_2d<double> shock_costs(query_size, database_size, 1000);
  vbl_array_2d<double> info_costs(query_size, database_size, 1000);

  int shock_corrects = 0;
  int info_corrects = 0;
  //: run the experiment on all the objects

  int results_collected = 0;
  //: keep the dbskr_tree_sptrs as they are created in an array for each object used
  //  not to repeat costly shock curve constructions each time they are reused
  vbl_array_1d< vbl_array_1d<dbskr_tree_sptr> * > database_objects_trees(database->size(), 0);
  
  for (unsigned q = 0; q <query_pairs.size(); q++) {
    unsigned int i0 = query_pairs[q].first;
    unsigned int p0 = query_pairs[q].second;
    dbru_object_sptr obj0 = (*database)[i0];
    if ((obj0->polygons_[p0])->size() <= 0) continue;

    vbl_array_1d<dbskr_tree_sptr> *obj0_trees = database_objects_trees[i0];
    if (obj0_trees == 0) {
      obj0_trees = new vbl_array_1d<dbskr_tree_sptr>();
      database_objects_trees[i0] = obj0_trees;
    }
      
    int best_shock_match_obj_id = -1;
    int best_info_match_obj_id = -1;
    int best_shock_match_poly_id = -1;
    int best_info_match_poly_id = -1;
    float best_shock_cost = 100000000;
    float best_info = -100000000;
    dbru_label_sptr input_label = obj0->labels_[p0];
    vil_image_resource_sptr best_imgr;
      
    vcl_pair<unsigned int, unsigned int> q_pair(i0, p0);

    for (unsigned d = 0; d <database_pairs.size(); d++) {
      unsigned int i = database_pairs[d].first;
      unsigned int pi = database_pairs[d].second;
      dbru_object_sptr obji = (*database)[i];
      if (i == i0) 
        continue;
        
      if ((obji->polygons_[pi])->size() <= 0) continue;
      dbru_label_sptr object_label = obji->labels_[pi];
      if (!check_labels(input_label, object_label)) continue;

      vbl_array_1d<dbskr_tree_sptr> *obji_trees = database_objects_trees[i];
      if (obji_trees == 0) {
        obji_trees = new vbl_array_1d<dbskr_tree_sptr>();
        database_objects_trees[i] = obji_trees;
      }

      double shock_matching_cost, info;
      vil_image_resource_sptr output_imgr;
      vcl_pair<unsigned int, unsigned int> d_pair(i, pi);
      vcl_pair<vcl_pair<unsigned int, unsigned int> , vcl_pair<unsigned int, unsigned int> > key(q_pair, d_pair);
      vcl_pair<vcl_pair<unsigned int, unsigned int> , vcl_pair<unsigned int, unsigned int> > key2(d_pair, q_pair);
    
      iter = matched_instances.find(key);   // searching for either key or key2 is enough
      if (iter == matched_instances.end()) {  // redo the matching, its not saved!!
        vcl_cout << "matching: " << i0 << " " << p0 << " (" << obj0->category_ << ") to " << i << " " << pi << " (" << obji->category_<< ")\n";
        //: this method makes a symettric matching, i.e. matches both ways and selects the best
        output_imgr = dbru_object_matcher::minfo_shock_matching(obj0, obji, p0, pi, obj0_trees, obji_trees,
                                          shock_matching_cost,
                                          info,
                                          p_threshold,
                                          false);  // verbose
        
        //vcl_map<vcl_pair<vcl_pair<int, int> , vcl_pair<int, int> >, vcl_pair<double, double> > matched_instances;
        matched_instances[key] = vcl_pair<double, double> (shock_matching_cost, info);
        matched_instances[key2] = vcl_pair<double, double> (shock_matching_cost, info);

      } else {
        shock_matching_cost = (iter->second).first;
        info = (iter->second).second;
      }
      of3 << i0 << " " << p0 << " " << i << " " << pi << " " << shock_matching_cost << " " << info << "\n"; 

      shock_costs[q][d] = shock_matching_cost;
      info_costs[q][d] = info;
      if (best_shock_cost > shock_matching_cost) {
        best_shock_cost = float(shock_matching_cost);
        best_shock_match_obj_id = i;
        best_shock_match_poly_id = pi;
      }
      if (best_info < info) {
        best_info = float(info);
        best_info_match_obj_id = i;
        best_info_match_poly_id = pi;
        best_imgr = output_imgr;
      }
   }
      
   if (best_shock_match_obj_id < 0 || best_info_match_obj_id < 0) {
     vcl_cout << "obj " << i0 << " poly " << p0 << " has no comparable instance in the db, skipping it\n";
     continue;
   }

   of1 << (*database)[i0]->category_ << "_obj" << i0 << "_poly " << p0 << "\n";
   of1 << "sm best match:\t\t" << (*database)[best_shock_match_obj_id]->category_ << "_obj" << best_shock_match_obj_id << "_poly" << best_shock_match_poly_id << "\n";
   of1 << "info best match:\t\t" << (*database)[best_info_match_obj_id]->category_ << "_obj" << best_info_match_obj_id << "_poly" << best_info_match_poly_id << "\n";
   if (category_id_map[(*database)[i0]->category_] == category_id_map[(*database)[best_shock_match_obj_id]->category_])
     shock_corrects++;
   if (category_id_map[(*database)[i0]->category_] == category_id_map[(*database)[best_info_match_obj_id]->category_])
    info_corrects++;
   results_collected++;
   // output the best match's image
   char buffer[1000];
   vcl_sprintf(buffer, "./outsmimages/obj%d-obj%d-poly%d-poly%d",i0, best_info_match_obj_id, p0, best_info_match_poly_id);
   vcl_string filename = buffer;
   filename = filename+"-shock.png";
   
   if (best_imgr)
      vil_save_image_resource(best_imgr, filename.c_str());
  }

  of1 << "total number of polygons: " << total_poly_cnt << " " << " results collected from " << results_collected << " polygons (should be equal to query size which is " << query_pairs.size() << " )\n";
  of1 << "algorithm\tnumber of corrects\tpercentage\n";
  of1 << "shock matching\t" << shock_corrects << "\t" << (((double)shock_corrects/results_collected)*100.0f) << "\n";
  of1 << "info \t" << info_corrects << "\t" << (((double)info_corrects/results_collected)*100.0f) << "\n";
  of1.close();

  of3.close();

  vcl_ofstream of((out_file+"matrix.out").c_str());
  of << "query size: " << query_pairs.size() << " names (each row is a query):\n";
  for (unsigned q = 0; q<query_pairs.size(); q++) {
    int i = query_pairs[q].first;
    int j = query_pairs[q].second;
    dbru_object_sptr obji = (*database)[i];
    char buffer[1000];
    vcl_sprintf(buffer, "%s_obj%d_poly%d ",obji->category_.c_str(), i, j);
    vcl_string filename = buffer;
    of << filename << "\n";
  }

  of << "database size: " << database_pairs.size() << " names:\n";
  for (unsigned d = 0; d<database_pairs.size(); d++) {
    int i = database_pairs[d].first;
    int j = database_pairs[d].second;
    dbru_object_sptr obji = (*database)[i];
    char buffer[1000];
    vcl_sprintf(buffer, "%s_obj%d_poly%d ",obji->category_.c_str(), i, j);
    vcl_string filename = buffer;
    of << filename << "\n";
  }

  of << "\n shock costs: \n";
  of << shock_costs.rows() << " " << shock_costs.cols() << "\n";
  
  for (int i = 0; i<shock_costs.rows(); i++) {
    for (int j = 0; j<shock_costs.cols(); j++) {
      of << shock_costs[i][j] << " ";
    }
    of << vcl_endl;
  }

  of << "\n infos: \n";
  of << info_costs.rows() << " " << info_costs.cols() << "\n";
  
  for (int i = 0; i<info_costs.rows(); i++) {
    for (int j = 0; j<info_costs.cols(); j++) {
      of << info_costs[i][j] << " ";
    }
    of << vcl_endl;
  }

  of.close();
  
  return 0;
}
