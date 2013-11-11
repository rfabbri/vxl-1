
// aerial_vehicle_segmentation.cpp : Defines the entry point for the DLL application.
#include<vcl_cstdio.h>
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
  vcl_cout << "run curve matching on selected frames as database\n";

  vcl_string in_osl, out_file;
  int n = 5;  // use every n frames in the database
  vcl_cout << "argc: " << argc << vcl_endl;
  if (argc != 8) {
    vcl_cout << "This program assumes that query and database instances are in the same osl!!!!\n";
    vcl_cout << "Usage: <program name> <input_osl_name> <rms> <ratio for dt> <line increment (20)> <out_file> <query pairs list> <db pairs list>\n";
    return -1;
  }

  in_osl = argv[1];
  float rms = float(atof(argv[2]));
  float R = 10.0f;
  float rest_ratio = 0.25f;  // restricted curve matching ratio for closed curve matching

  float ratio = float(atof(argv[3]));
  int increment = atoi(argv[4]);
  out_file = argv[5];

  //int k = atoi(argv[4]);
  int k = 3; // fixed for now
  vcl_string query_list = argv[6];
  vcl_string db_list = argv[7];

  //LOAD DATABASE PREVIOUSLY CREATED
  vbl_array_1d<dbru_object_sptr> *database = 
    reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (load_osl(in_osl.c_str()));

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
  vcl_map<vcl_pair<vcl_pair<unsigned int, unsigned int> , vcl_pair<unsigned int, unsigned int> >, vcl_pair<double, vcl_pair<double, double> > > matched_instances;
  
  vcl_ifstream if3(dump_file.c_str());
  if (if3) {
    unsigned int i, pi, j, pj; double curve, info, info_dt;
    if3 >> i;  // read first i
    while (!if3.eof()) {
      if3 >> pi;   // read j
      if (if3.eof()) break;
      if3 >> j;
      if (if3.eof()) break;
      if3 >> pj;
      if (if3.eof()) break;
      if3 >> curve;
      if (if3.eof()) break;
      if3 >> info;
      if (if3.eof()) break;
      if3 >> info_dt;
      vcl_pair<unsigned int, unsigned int> q_pair(i, pi);
      vcl_pair<unsigned int, unsigned int> d_pair(j, pj);
      vcl_pair<vcl_pair<unsigned int, unsigned int> , vcl_pair<unsigned int, unsigned int> > key(q_pair, d_pair);
      vcl_pair<double, double> info_pair(info, info_dt);
      matched_instances[key] = vcl_pair<double, vcl_pair<double, double> > (curve, info_pair);
      vcl_pair<vcl_pair<unsigned int, unsigned int> , vcl_pair<unsigned int, unsigned int> > key2(d_pair, q_pair);
      matched_instances[key2] = vcl_pair<double, vcl_pair<double, double> > (curve, info_pair);
      
      if3 >> i;  // try reading i again, if not then it'll break 
    }
    if3.close();
  }

  vcl_ofstream of3;
  of3.open(dump_file.c_str());
  // first dump the currently known elements:
  vcl_map<vcl_pair<vcl_pair<unsigned int, unsigned int> , vcl_pair<unsigned int, unsigned int> >, vcl_pair<double, vcl_pair<double, double> > >::iterator iter;
  for (iter = matched_instances.begin(); iter != matched_instances.end(); iter++) {
    vcl_pair<vcl_pair<unsigned int, unsigned int> , vcl_pair<unsigned int, unsigned int> > key = iter->first;
    vcl_pair<double, vcl_pair<double, double> > costs = iter->second;
    of3 << key.first.first << " " << key.first.second << " " << key.second.first << " " << key.second.second;
    of3 << " " << costs.first << " " << costs.second.first << " " << costs.second.second << "\n";
  }

  vbl_array_2d<double> curve_costs(query_size, database_size, 1000);
  vbl_array_2d<double> info_costs(query_size, database_size, 1000);
  vbl_array_2d<double> info_costs_dt(query_size, database_size, 1000);

  int curve_corrects = 0;
  int info_corrects = 0;
  int info_dt_corrects = 0;
  //: run the experiment on all the objects

  int results_collected = 0;
  for (unsigned q = 0; q <query_pairs.size(); q++) {
    unsigned int i0 = query_pairs[q].first;
    unsigned int p0 = query_pairs[q].second;
    dbru_object_sptr obj0 = (*database)[i0];
    if ((obj0->polygons_[p0])->size() <= 0) continue;
  
    int best_curve_match_obj_id = -1;
    int best_info_match_obj_id = -1;
    int best_info_dt_match_obj_id = -1;
    int best_curve_match_poly_id = -1;
    int best_info_match_poly_id = -1;
    int best_info_dt_match_poly_id = -1;

    float best_curve_cost = 100000000;
    float best_info = -100000000;
    float best_info_dt = -100000000;

    dbru_label_sptr input_label = obj0->labels_[p0];
    vil_image_resource_sptr best_imgr;
    vil_image_resource_sptr best_imgr_dt;
    
    if (obj0->n_observations() <= 0) {
      vcl_cout << "Observations of this object are not created, exiting!\n";
      return 0;
    }
    dbinfo_observation_sptr obs0 = obj0->get_observation(p0);
    

  
      
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

      double curve_matching_cost, info, info_dt;
      vil_image_resource_sptr output_imgr, output_imgr_dt;

      vcl_pair<unsigned int, unsigned int> d_pair(i, pi);
      vcl_pair<vcl_pair<unsigned int, unsigned int> , vcl_pair<unsigned int, unsigned int> > key(q_pair, d_pair);
      vcl_pair<vcl_pair<unsigned int, unsigned int> , vcl_pair<unsigned int, unsigned int> > key2(d_pair, q_pair);
    
      iter = matched_instances.find(key);   // searching for either key or key2 is enough
      if (iter == matched_instances.end()) {  // redo the matching, its not saved!!
        
        if (obji->n_observations() <= 0) {
          vcl_cout << "Observations of this object are not created, exiting!\n";
          return 0;
        }
        dbinfo_observation_sptr obsi = obji->get_observation(pi);
        
        
        vcl_cout << "matching: " << i0 << " " << p0 << " (" << obj0->category_ << ") to " << i << " " << pi << " (" << obji->category_<< ")\n";
        
        vul_timer t;
        dbcvr_cv_cor_sptr cm_cor1, cm_cor2; double cost1, cost2;
        cm_cor1 = dbru_object_matcher::compute_curve_alignment(obj0->get_polygon(p0),obji->get_polygon(pi),cost1,R,rms,rest_ratio,false);
        cm_cor2 = dbru_object_matcher::compute_curve_alignment(obji->get_polygon(pi),obj0->get_polygon(p0),cost2,R,rms,rest_ratio,false);      
      
        if (cost1 < cost2) {
          curve_matching_cost = cost1;
          vcl_cout << " cost " << cost1 << " time: "<< t.real()/1000.0f << " ";
        } else {
          curve_matching_cost = cost2;
          vcl_cout << " cost " << cost2 << " time: "<< t.real()/1000.0f << " "; 
        }

        t.mark();
        dbru_rcor_sptr rcor1 = dbru_object_matcher::generate_rcor_curve_matching_line(obs0, obsi, cm_cor1, increment, false);
        dbru_rcor_sptr rcor2 = dbru_object_matcher::generate_rcor_curve_matching_line(obsi, obs0, cm_cor2, increment, false);
        float info1 = dbinfo_observation_matcher::minfo(obs0, obsi, rcor1->get_correspondences(), false);
        float info2 = dbinfo_observation_matcher::minfo(obsi, obs0, rcor2->get_correspondences(), false);

        if (info1 > info2) {
          info = info1;
          output_imgr = rcor1->get_appearance2_on_pixels1();
          vcl_cout << " info1: " << info1 << " time: "<< t.real()/1000.0f << " ";
        } else {
          info = info2;
          output_imgr = rcor2->get_appearance2_on_pixels1();
          vcl_cout << " info2: " << info2 << " time: "<< t.real()/1000.0f << " ";
        }

        t.mark();
        dbru_rcor_sptr rcor3 = dbru_object_matcher::generate_rcor_curve_matching_dt2(obs0, obsi, cm_cor1, ratio, false);
        dbru_rcor_sptr rcor4 = dbru_object_matcher::generate_rcor_curve_matching_dt2(obsi, obs0, cm_cor2, ratio, false);
        float info3 = dbinfo_observation_matcher::minfo(obs0, obsi, rcor3->get_correspondences(), false);
        float info4 = dbinfo_observation_matcher::minfo(obsi, obs0, rcor4->get_correspondences(), false);

        if (info3 > info4) {
          info_dt = info3;
          output_imgr_dt = rcor3->get_appearance2_on_pixels1();
          vcl_cout << " info3: " << info3 << " time: "<< t.real()/1000.0f << " secs\n";
        } else {
          info_dt = info4;
          output_imgr_dt = rcor4->get_appearance2_on_pixels1();
          vcl_cout << " info4: " << info4 << " time: "<< t.real()/1000.0f << " secs\n";
        }
        
        vcl_pair<double, double> cost_pair(info, info_dt);
        matched_instances[key] = vcl_pair<double, vcl_pair<double, double> > (curve_matching_cost, cost_pair);
        matched_instances[key2] = vcl_pair<double, vcl_pair<double, double> > (curve_matching_cost, cost_pair);

      } else {
        curve_matching_cost = (iter->second).first;
        info = ((iter->second).second).first;
        info_dt = ((iter->second).second).second;
      }
      of3 << i0 << " " << p0 << " " << i << " " << pi << " " << curve_matching_cost << " " << info << " " << info_dt << "\n";

      curve_costs[q][d] = curve_matching_cost;
      info_costs[q][d] = info;
      info_costs_dt[q][d] = info_dt;

      if (best_curve_cost > curve_matching_cost) {
        best_curve_cost = float(curve_matching_cost);
        best_curve_match_obj_id = i;
        best_curve_match_poly_id = pi;
      }
      if (best_info < info) {
        best_info = float(info);
        best_info_match_obj_id = i;
        best_info_match_poly_id = pi;
        best_imgr = output_imgr;
      }
      if (best_info_dt < info_dt) {
        best_info_dt = float(info_dt);
        best_info_dt_match_obj_id = i;
        best_info_dt_match_poly_id = pi;
        best_imgr_dt = output_imgr_dt;
      }
   }
      
   if (best_curve_match_obj_id < 0 || best_info_match_obj_id < 0 || best_info_dt_match_obj_id < 0) {
     vcl_cout << "obj " << i0 << " poly " << p0 << " has no comparable instance in the db, skipping it\n";
     continue;
   }

   of1 << (*database)[i0]->category_ << "_obj" << i0 << "_poly " << p0 << "\n";
   of1 << "cm best match:\t\t" << (*database)[best_curve_match_obj_id]->category_ << "_obj" << best_curve_match_obj_id << "_poly" << best_curve_match_poly_id << "\n";
   of1 << "info best match:\t\t" << (*database)[best_info_match_obj_id]->category_ << "_obj" << best_info_match_obj_id << "_poly" << best_info_match_poly_id << "\n";
   of1 << "info_dt best match:\t\t" << (*database)[best_info_dt_match_obj_id]->category_ << "_obj" << best_info_dt_match_obj_id << "_poly" << best_info_dt_match_poly_id << "\n";
   if (category_id_map[(*database)[i0]->category_] == category_id_map[(*database)[best_curve_match_obj_id]->category_])
     curve_corrects++;
   if (category_id_map[(*database)[i0]->category_] == category_id_map[(*database)[best_info_match_obj_id]->category_])
    info_corrects++;
   if (category_id_map[(*database)[i0]->category_] == category_id_map[(*database)[best_info_dt_match_obj_id]->category_])
    info_dt_corrects++;
   results_collected++;
   
   // output the best match's image
   char buffer[1000];
   vcl_sprintf(buffer, "./outcmimages/obj%d-obj%d-poly%d-poly%d",i0, best_info_match_obj_id, p0, best_info_match_poly_id);
   vcl_string filename = buffer;
   filename = filename+"-line.png";
   if (best_imgr)
      vil_save_image_resource(best_imgr, filename.c_str());

   vcl_sprintf(buffer, "./outcmimages/obj%d-obj%d-poly%d-poly%d",i0, best_info_dt_match_obj_id, p0, best_info_dt_match_poly_id);
   filename = buffer;
   filename = filename+"-dt.png";
   if (best_imgr_dt)
      vil_save_image_resource(best_imgr_dt, filename.c_str());
  }

  of1 << "total number of polygons: " << total_poly_cnt << " " << " results collected from " << results_collected << " polygons (should be equal to query size which is " << query_pairs.size() << " )\n";
  of1 << "algorithm\t\tnumber of corrects\t\tpercentage\n";
  of1 << "curve matching\t\t" << curve_corrects << "\t\t" << (((double)curve_corrects/results_collected)*100.0f) << "\n";
  of1 << "info \t\t" << info_corrects << "\t\t" << (((double)info_corrects/results_collected)*100.0f) << "\n";
  of1 << "info dt \t\t" << info_dt_corrects << "\t\t" << (((double)info_dt_corrects/results_collected)*100.0f) << "\n";
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

  of << "\n curve costs: \n";
  of << curve_costs.rows() << " " << curve_costs.cols() << "\n";
  
  for (int i = 0; i<curve_costs.rows(); i++) {
    for (int j = 0; j<curve_costs.cols(); j++) {
      of << curve_costs[i][j] << " ";
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

  of << "\n infos dt: \n";
  of << info_costs_dt.rows() << " " << info_costs_dt.cols() << "\n";
  
  for (int i = 0; i<info_costs_dt.rows(); i++) {
    for (int j = 0; j<info_costs_dt.cols(); j++) {
      of << info_costs_dt[i][j] << " ";
    }
    of << vcl_endl;
  }

  of.close();
  
  return 0;
}
