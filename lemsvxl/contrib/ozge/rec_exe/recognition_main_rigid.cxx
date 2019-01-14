
#include<cstdio>
#include<vul/vul_timer.h>
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
#include <dbru/dbru_rcor.h>
#include <dbru/dbru_osl.h>
#include <dbru/pro/dbru_osl_storage.h>
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
//std::vector<vgl_point_2d<double> > meanpts;

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
  std::cout << osl->size() << " objects and observations are read from binary input file\n";
  return reinterpret_cast<unsigned int>(osl);
}

dbru_osl_sptr load_osl_old_using_storage(const char *osl_file_name) 
{
  vsl_b_ifstream obfile(osl_file_name);
  dbru_osl_storage_sptr osl_storage = dbru_osl_storage_new();
  unsigned size;
  vsl_b_read(obfile, size);
  for (int i = 0; i<size; i++) {
    dbru_object_sptr obj = new dbru_object();
    obj->b_read(obfile);
    osl_storage->osl()->add_object(obj);
  }
  obfile.close();
  std::cout << osl_storage->osl()->n_objects() << " objects and observations are read from binary input file\n";
  return osl_storage->osl();
}

//: read osl using storage binary reader (new OSL version after Aug 03, 2006)
dbru_osl_sptr load_osl_new_version(const char *osl_file_name) 
{
  vsl_b_ifstream obfile(osl_file_name);
  dbru_osl_storage_sptr osl_str = dbru_osl_storage_new();
  osl_str->b_read(obfile);
  obfile.close();
  std::cout << osl_str->get_osl_size() << " objects and observations are read from binary input file\n";
  return osl_str->osl();
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


int main_same_osl(int argc, char *argv[]) {
  std::cout << "run rigid alignment on selected frames as database\n";

  std::string in_osl, out_file;
  int n = 5;  // use every n frames in the database
  std::cout << "argc: " << argc << std::endl;
  if (argc != 8) {
    std::cout << "This program assumes that query and database instances are in the same osl!!!!\n";
    std::cout << "Usage: <program name> <input_osl_name> <dx> <dr> <ds> <out_file> <query pairs list> <db pairs list>\n";
    return -1;
  }

  in_osl = argv[1];
  float dx = float(atof(argv[2]));
  float dr = float(atof(argv[3]));
  float ds = float(atof(argv[4]));  // restricted curve matching ratio for closed curve matching

  out_file = std::string(argv[5])+"_"+std::string(argv[2])+"_"+std::string(argv[3])+"_"+std::string(argv[4]);

  //int k = atoi(argv[4]);
  int k = 3; // fixed for now
  std::string query_list = argv[6];
  std::string db_list = argv[7];

  //LOAD DATABASE PREVIOUSLY CREATED
  vbl_array_1d<dbru_object_sptr> *database = 
    reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (load_osl(in_osl.c_str()));

  std::cout << database->size() << " objects and observations are read from binary input file\n";
  //count the number of polygons
  
  int total_poly_cnt = 0;
  std::vector<int> used_poly_cnts;
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

  std::ifstream fp(query_list.c_str());
  if (!fp) {
    std::cout << "Problems in opening selected frames query list file!\n";
    return 0;
  }
                  // obj_id, poly_id
  std::vector<std::pair<int, int> > query_pairs;
  int query_size = 0;
  fp >> query_size;
  for (int i = 0; i<query_size; i++) {  // 1 line for each object, first cnt then frame nos
    std::pair<int, int> obj_pair;
    fp >> obj_pair.first;
    fp >> obj_pair.second;
    query_pairs.push_back(obj_pair);
  }
  fp.close();

  std::ifstream fp2(db_list.c_str());
  if (!fp2) {
    std::cout << "Problems in opening selected frames database list file!\n";
    return 0;
  }
                  // obj_id, poly_id
  std::vector<std::pair<int, int> > database_pairs;
  int database_size = 0;
  fp2 >> database_size;
  for (int i = 0; i<database_size; i++) {  // 1 line for each object, first cnt then frame nos
    std::pair<int, int> obj_pair;
    fp2 >> obj_pair.first;
    fp2 >> obj_pair.second;
    database_pairs.push_back(obj_pair);
  }
  fp2.close();

  std::map<std::string, int> category_id_map;
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

  std::ofstream of1((out_file+".out").c_str());
  of1 << "db size: " << database_size << std::endl;
  
  std::string dump_file = "dump_"+out_file+".out";
  std::map<std::pair<std::pair<unsigned int, unsigned int> , std::pair<unsigned int, unsigned int> >, double > matched_instances;
  
  std::ifstream if3(dump_file.c_str());
  if (if3) {
    unsigned int i, pi, j, pj; double info;
    if3 >> i;  // read first i
    while (!if3.eof()) {
      if3 >> pi;   // read j
      if (if3.eof()) break;
      if3 >> j;
      if (if3.eof()) break;
      if3 >> pj;
      if (if3.eof()) break;
      if3 >> info;
      if (if3.eof()) break;
      std::pair<unsigned int, unsigned int> q_pair(i, pi);
      std::pair<unsigned int, unsigned int> d_pair(j, pj);
      std::pair<std::pair<unsigned int, unsigned int> , std::pair<unsigned int, unsigned int> > key(q_pair, d_pair);
      matched_instances[key] = info;
      std::pair<std::pair<unsigned int, unsigned int> , std::pair<unsigned int, unsigned int> > key2(d_pair, q_pair);
      matched_instances[key2] = info;
      
      if3 >> i;  // try reading i again, if not then it'll break 
    }
    if3.close();
  }

  std::ofstream of3;
  of3.open(dump_file.c_str());
  // first dump the currently known elements:
  std::map<std::pair<std::pair<unsigned int, unsigned int> , std::pair<unsigned int, unsigned int> >, double >::iterator iter;
  for (iter = matched_instances.begin(); iter != matched_instances.end(); iter++) {
    std::pair<std::pair<unsigned int, unsigned int> , std::pair<unsigned int, unsigned int> > key = iter->first;
    double info = iter->second;
    of3 << key.first.first << " " << key.first.second << " " << key.second.first << " " << key.second.second;
    of3 << " " << info << "\n";
  }

  vbl_array_2d<double> info_costs(query_size, database_size, 1000);
  int info_corrects = 0;
  
  //: run the experiment on all the objects
  int results_collected = 0;
  for (unsigned q = 0; q <query_pairs.size(); q++) {
    unsigned int i0 = query_pairs[q].first;
    unsigned int p0 = query_pairs[q].second;
    dbru_object_sptr obj0 = (*database)[i0];
    if ((obj0->polygons_[p0])->size() <= 0) continue;
  
    int best_info_match_obj_id = -1;
    int best_info_match_poly_id = -1;  

    float best_info = -100000000;

    dbru_label_sptr input_label = obj0->labels_[p0];
    vil_image_resource_sptr best_imgr;
    
    if (obj0->n_observations() <= 0) {
      std::cout << "Observations of this object are not created, exiting!\n";
      return 0;
    }
    dbinfo_observation_sptr obs0 = obj0->get_observation(p0);
      
    std::pair<unsigned int, unsigned int> q_pair(i0, p0);

    for (unsigned d = 0; d <database_pairs.size(); d++) {
      unsigned int i = database_pairs[d].first;
      unsigned int pi = database_pairs[d].second;
      dbru_object_sptr obji = (*database)[i];
      if (i == i0) 
        continue;
        
      if ((obji->polygons_[pi])->size() <= 0) continue;
      dbru_label_sptr object_label = obji->labels_[pi];
      if (!check_labels(input_label, object_label)) continue;

      double info;
      vil_image_resource_sptr output_imgr;

      std::pair<unsigned int, unsigned int> d_pair(i, pi);
      std::pair<std::pair<unsigned int, unsigned int> , std::pair<unsigned int, unsigned int> > key(q_pair, d_pair);
      std::pair<std::pair<unsigned int, unsigned int> , std::pair<unsigned int, unsigned int> > key2(d_pair, q_pair);
    
      iter = matched_instances.find(key);   // searching for either key or key2 is enough
      if (iter == matched_instances.end()) {  // redo the matching, its not saved!!
        
        if (obji->n_observations() <= 0) {
          std::cout << "Observations of this object are not created, exiting!\n";
          return 0;
        }
        dbinfo_observation_sptr obsi = obji->get_observation(pi);
        std::cout << "matching: " << i0 << " " << p0 << " (" << obj0->category_ << ") to " << i << " " << pi << " (" << obji->category_<< ")\n";
        
        vul_timer t;
        vil_image_resource_sptr out_image1, out_image2, dummy;
        float info1 = dbru_object_matcher::minfo_rigid_alignment(obs0, obsi, dx, dr, ds, dummy, dummy, out_image1, false);
        float info2 = dbru_object_matcher::minfo_rigid_alignment(obsi, obs0, dx, dr, ds, dummy, dummy, out_image2, false);
      
        if (info1 > info2) {
          info = info1; output_imgr = out_image1;
        } else {
          info = info2; output_imgr = out_image2;
        }

        std::cout << " info " << info << " time: "<< t.real()/1000.0f << " ";
        matched_instances[key] = info;
        matched_instances[key2] = info;
      } else {
        info = iter->second;
      }
      of3 << i0 << " " << p0 << " " << i << " " << pi << " " << info << " " << "\n";
      
      info_costs[q][d] = info;
      if (best_info < info) {
        best_info = float(info);
        best_info_match_obj_id = i;
        best_info_match_poly_id = pi;
        best_imgr = output_imgr;
      }
   }
      
   if (best_info_match_obj_id < 0) {
     std::cout << "obj " << i0 << " poly " << p0 << " has no comparable instance in the db, skipping it\n";
     continue;
   }

   of1 << (*database)[i0]->category_ << "_obj" << i0 << "_poly " << p0 << "\n";   
   of1 << "info best match:\t\t" << (*database)[best_info_match_obj_id]->category_ << "_obj" << best_info_match_obj_id << "_poly" << best_info_match_poly_id << "\n";
   if (category_id_map[(*database)[i0]->category_] == category_id_map[(*database)[best_info_match_obj_id]->category_])
    info_corrects++;

   results_collected++;
   
   // output the best match's image
   char buffer[1000];
   std::sprintf(buffer, "./outrigidimages/obj%d-obj%d-poly%d-poly%d",i0, best_info_match_obj_id, p0, best_info_match_poly_id);
   std::string filename = buffer;
   filename = filename+"-rigid.png";
   if (best_imgr)
      vil_save_image_resource(best_imgr, filename.c_str());
  }

  of1 << "total number of polygons: " << total_poly_cnt << " " << " results collected from " << results_collected << " polygons (should be equal to query size which is " << query_pairs.size() << " )\n";
  of1 << "algorithm\t\tnumber of corrects\t\tpercentage\n";
  of1 << "rigid info \t\t" << info_corrects << "\t\t" << (((double)info_corrects/results_collected)*100.0f) << "\n";
  of1.close();

  of3.close();

  std::ofstream of((out_file+"matrix.out").c_str());
  of << "query size: " << query_pairs.size() << " names (each row is a query):\n";
  for (unsigned q = 0; q<query_pairs.size(); q++) {
    int i = query_pairs[q].first;
    int j = query_pairs[q].second;
    dbru_object_sptr obji = (*database)[i];
    char buffer[1000];
    std::sprintf(buffer, "%s_obj%d_poly%d ",obji->category_.c_str(), i, j);
    std::string filename = buffer;
    of << filename << "\n";
  }

  of << "database size: " << database_pairs.size() << " names:\n";
  for (unsigned d = 0; d<database_pairs.size(); d++) {
    int i = database_pairs[d].first;
    int j = database_pairs[d].second;
    dbru_object_sptr obji = (*database)[i];
    char buffer[1000];
    std::sprintf(buffer, "%s_obj%d_poly%d ",obji->category_.c_str(), i, j);
    std::string filename = buffer;
    of << filename << "\n";
  }

  of << "\n rigid infos: \n";
  of << info_costs.rows() << " " << info_costs.cols() << "\n";
  
  for (int i = 0; i<info_costs.rows(); i++) {
    for (int j = 0; j<info_costs.cols(); j++) {
      of << info_costs[i][j] << " ";
    }
    of << std::endl;
  }

  of.close();
  
  return 0;
}



int main(int argc, char *argv[]) {
  std::cout << "run rigid alignment on selected frames as database\n";

  std::string db_osl_name, query_osl_name, out_file;
  int n = 5;  // use every n frames in the database
  std::cout << "argc: " << argc << std::endl;
  if (argc != 11) {
    std::cout << "This program assumes that query and database instances are in different osl!!!!\n";
    std::cout << "Usage: <program name> <query_osl_name> <0: if old version, 1: if new storage version>\n";
    std::cout << " <db_osl_name> <0: if old version, 1: if new storage version> <dx> <dr> <ds> <out_file> <query pairs list> <db pairs list>\n";
    return -1;
  }

  query_osl_name = argv[1];
  int version = atoi(argv[2]);
  bool query_old = version == 0 ? true : false;
  
  db_osl_name = argv[3];
  version = atoi(argv[4]);
  bool db_old = version == 0 ? true : false;
  
  float dx = float(atof(argv[5]));
  float dr = float(atof(argv[6]));
  float ds = float(atof(argv[7]));  // restricted curve matching ratio for closed curve matching

  out_file = std::string(argv[8])+"_"+std::string(argv[5])+"_"+std::string(argv[6])+"_"+std::string(argv[7]);

  //int k = atoi(argv[4]);
  //int k = 3; // fixed for now
  std::string query_list = argv[9];
  std::string db_list = argv[10];

  //LOAD DATABASE PREVIOUSLY CREATED
  dbru_osl_sptr db_osl, query_osl;
  if (db_old)
    db_osl = load_osl_old_using_storage(db_osl_name.c_str());
  else
    db_osl = load_osl_new_version(db_osl_name.c_str());
  if (query_old)
    query_osl = load_osl_old_using_storage(query_osl_name.c_str());
  else
    query_osl = load_osl_new_version(query_osl_name.c_str());

  std::cout << "Database osl contains " << db_osl->n_objects() << " objects, read from: " << db_osl_name << "\n";
  std::cout << "Query osl contains " << query_osl->n_objects() << " objects, read from: " << query_osl_name << "\n";
  
  std::ifstream fp(query_list.c_str());
  if (!fp) {
    std::cout << "Problems in opening selected frames query list file!\n";
    return 0;
  }
                  // obj_id, poly_id
  std::vector<std::pair<int, int> > query_pairs;
  int query_size = 0;
  fp >> query_size;
  for (int i = 0; i<query_size; i++) {  // 1 line for each object, first cnt then frame nos
    std::pair<int, int> obj_pair;
    fp >> obj_pair.first;
    fp >> obj_pair.second;
    query_pairs.push_back(obj_pair);
  }
  fp.close();

  std::ifstream fp2(db_list.c_str());
  if (!fp2) {
    std::cout << "Problems in opening selected frames database list file!\n";
    return 0;
  }
                  // obj_id, poly_id
  std::vector<std::pair<int, int> > database_pairs;
  int database_size = 0;
  fp2 >> database_size;
  for (int i = 0; i<database_size; i++) {  // 1 line for each object, first cnt then frame nos
    std::pair<int, int> obj_pair;
    fp2 >> obj_pair.first;
    fp2 >> obj_pair.second;
    database_pairs.push_back(obj_pair);
  }
  fp2.close();

  std::map<std::string, int> category_id_map;
  int k = 3; //fixed for now
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

  std::ofstream of1((out_file+".out").c_str());
  of1 << "db size: " << database_size << std::endl;
  
  std::string dump_file = "dump_"+out_file+".out";
  std::map<std::pair<std::pair<unsigned int, unsigned int> , std::pair<unsigned int, unsigned int> >, double > matched_instances;
  
  std::ifstream if3(dump_file.c_str());
  if (if3) {
    unsigned int i, pi, j, pj; double info;
    if3 >> i;  // read first i
    while (!if3.eof()) {
      if3 >> pi;   // read j
      if (if3.eof()) break;
      if3 >> j;
      if (if3.eof()) break;
      if3 >> pj;
      if (if3.eof()) break;
      if3 >> info;
      if (if3.eof()) break;
      std::pair<unsigned int, unsigned int> q_pair(i, pi);
      std::pair<unsigned int, unsigned int> d_pair(j, pj);
      std::pair<std::pair<unsigned int, unsigned int> , std::pair<unsigned int, unsigned int> > key(q_pair, d_pair);
      matched_instances[key] = info;
      std::pair<std::pair<unsigned int, unsigned int> , std::pair<unsigned int, unsigned int> > key2(d_pair, q_pair);
      matched_instances[key2] = info;
      
      if3 >> i;  // try reading i again, if not then it'll break 
    }
    if3.close();
  }

  std::ofstream of3;
  of3.open(dump_file.c_str());
  // first dump the currently known elements:
  std::map<std::pair<std::pair<unsigned int, unsigned int> , std::pair<unsigned int, unsigned int> >, double >::iterator iter;
  for (iter = matched_instances.begin(); iter != matched_instances.end(); iter++) {
    std::pair<std::pair<unsigned int, unsigned int> , std::pair<unsigned int, unsigned int> > key = iter->first;
    double info = iter->second;
    of3 << key.first.first << " " << key.first.second << " " << key.second.first << " " << key.second.second;
    of3 << " " << info << "\n";
  }

  vbl_array_2d<double> info_costs(query_size, database_size, 1000);
  int info_corrects = 0;
  
  //: run the experiment on all the objects
  int results_collected = 0;
  for (unsigned q = 0; q <query_pairs.size(); q++) {
    unsigned int i0 = query_pairs[q].first;
    unsigned int p0 = query_pairs[q].second;
    dbru_object_sptr obj0 = query_osl->get_object(i0);
    if ((obj0->polygons_[p0])->size() <= 0) continue;
  
    int best_info_match_obj_id = -1;
    int best_info_match_poly_id = -1;  

    float best_info = -100000000;

    dbru_label_sptr input_label = obj0->labels_[p0];
    vil_image_resource_sptr best_imgr;
    
    if (obj0->n_observations() <= 0) {
      std::cout << "Observations of this object are not created, exiting!\n";
      return 0;
    }
    dbinfo_observation_sptr obs0 = obj0->get_observation(p0);
      
    std::pair<unsigned int, unsigned int> q_pair(i0, p0);

    for (unsigned d = 0; d <database_pairs.size(); d++) {
      unsigned int i = database_pairs[d].first;
      unsigned int pi = database_pairs[d].second;
      dbru_object_sptr obji = db_osl->get_object(i);
        
      if ((obji->polygons_[pi])->size() <= 0) continue;
      dbru_label_sptr object_label = obji->labels_[pi];
      if (!check_labels(input_label, object_label)) continue;

      double info;
      vil_image_resource_sptr output_imgr;

      std::pair<unsigned int, unsigned int> d_pair(i, pi);
      std::pair<std::pair<unsigned int, unsigned int> , std::pair<unsigned int, unsigned int> > key(q_pair, d_pair);
      std::pair<std::pair<unsigned int, unsigned int> , std::pair<unsigned int, unsigned int> > key2(d_pair, q_pair);
    
      iter = matched_instances.find(key);   // searching for either key or key2 is enough
      if (iter == matched_instances.end()) {  // redo the matching, its not saved!!
        
        if (obji->n_observations() <= 0) {
          std::cout << "Observations of this object are not created, exiting!\n";
          return 0;
        }
        dbinfo_observation_sptr obsi = obji->get_observation(pi);
        std::cout << "matching: " << i0 << " " << p0 << " (" << obj0->category_ << ") to " << i << " " << pi << " (" << obji->category_<< ")\n";
        
        vul_timer t;
        vil_image_resource_sptr out_image1, out_image2, dummy;
        float info1 = dbru_object_matcher::minfo_rigid_alignment(obs0, obsi, dx, dr, ds, dummy, dummy, out_image1, false);
        float info2 = dbru_object_matcher::minfo_rigid_alignment(obsi, obs0, dx, dr, ds, dummy, dummy, out_image2, false);
      
        if (info1 > info2) {
          info = info1; output_imgr = out_image1;
        } else {
          info = info2; output_imgr = out_image2;
        }

        std::cout << " info " << info << " time: "<< t.real()/1000.0f << " ";
        matched_instances[key] = info;
        matched_instances[key2] = info;
      } else {
        info = iter->second;
      }
      of3 << i0 << " " << p0 << " " << i << " " << pi << " " << info << " " << "\n";
      
      info_costs[q][d] = info;
      if (best_info < info) {
        best_info = float(info);
        best_info_match_obj_id = i;
        best_info_match_poly_id = pi;
        best_imgr = output_imgr;
      }
   }
      
   if (best_info_match_obj_id < 0) {
     std::cout << "obj " << i0 << " poly " << p0 << " has no comparable instance in the db, skipping it\n";
     continue;
   }

   of1 << query_osl->get_object(i0)->category_ << "_obj" << i0 << "_poly " << p0 << "\n";   
   of1 << "info best match:\t\t" << db_osl->get_object(best_info_match_obj_id)->category_ << "_obj" << best_info_match_obj_id << "_poly" << best_info_match_poly_id << "\n";
   if (category_id_map[(query_osl->get_object(i0))->category_] == category_id_map[(db_osl->get_object(best_info_match_obj_id))->category_])
    info_corrects++;

   results_collected++;
   
   // output the best match's image
   char buffer[1000];
   std::sprintf(buffer, "./outrigidimages/obj%d-obj%d-poly%d-poly%d",i0, best_info_match_obj_id, p0, best_info_match_poly_id);
   std::string filename = buffer;
   filename = filename+"-rigid.png";
   if (best_imgr)
      vil_save_image_resource(best_imgr, filename.c_str());
  }

  of1 << "results collected from " << results_collected << " polygons (should be equal to query size which is " << query_pairs.size() << " )\n";
  of1 << "algorithm\t\tnumber of corrects\t\tpercentage\n";
  of1 << "rigid info \t\t" << info_corrects << "\t\t" << (((double)info_corrects/results_collected)*100.0f) << "\n";
  of1.close();

  of3.close();

  std::ofstream of((out_file+"matrix.out").c_str());
  of << "query size: " << query_pairs.size() << " names (each row is a query):\n";
  for (unsigned q = 0; q<query_pairs.size(); q++) {
    int i = query_pairs[q].first;
    int j = query_pairs[q].second;
    dbru_object_sptr obji = query_osl->get_object(i);
    char buffer[1000];
    std::sprintf(buffer, "%s_obj%d_poly%d ",obji->category_.c_str(), i, j);
    std::string filename = buffer;
    of << filename << "\n";
  }

  of << "database size: " << database_pairs.size() << " names:\n";
  for (unsigned d = 0; d<database_pairs.size(); d++) {
    int i = database_pairs[d].first;
    int j = database_pairs[d].second;
    dbru_object_sptr obji = db_osl->get_object(i);
    char buffer[1000];
    std::sprintf(buffer, "%s_obj%d_poly%d ",obji->category_.c_str(), i, j);
    std::string filename = buffer;
    of << filename << "\n";
  }

  of << "\n rigid infos: \n";
  of << info_costs.rows() << " " << info_costs.cols() << "\n";
  
  for (int i = 0; i<info_costs.rows(); i++) {
    for (int j = 0; j<info_costs.cols(); j++) {
      of << info_costs[i][j] << " ";
    }
    of << std::endl;
  }

  of.close();
  
  return 0;
}
