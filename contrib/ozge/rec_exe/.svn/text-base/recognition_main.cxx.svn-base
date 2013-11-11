
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
#include <vidl1/vidl1_movie_sptr.h>
#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_io.h>

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

//: db_list_filename is the xml file that stores each object, and its polygons
//  video_list_file_name is <id, directory> pair for each video which have objects in the database
int createdb(int handle, const char *db_object_list_filename, const char *video_list_file_name, const char *osl_file_name) 
{ 
  //vcl_ofstream ofile("temp.txt");
  //vcl_string home("/projects/vorl1/");
  //vcl_string home("d:/lockheed_videos/");
  vcl_ifstream fp(video_list_file_name);
  if (!fp) {
    vcl_cout << "Problems in opening video directory list file!\n";
    return 0;
  }

  vcl_map<int, vidl1_movie_sptr> video_directories;
  int id = -1;
  fp >> id;
  while (!fp.eof()) {
    vcl_string dir;
    fp >> dir;
    vidl1_movie_sptr my_movie = vidl1_io::load_movie(dir.c_str());
    
    if (!my_movie) {
      vcl_cout << "problems in loading video ";
      vcl_cout << " with video file name: " << dir << vcl_endl;
      return 0;
    }
    video_directories[id] = my_movie;
    fp >> id;
  }
  fp.close();
  
  vcl_cout << "loaded " << video_directories.size() << " video dirs from the file\n";
  
  vbl_array_1d<dbru_object_sptr> *database_objects = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (handle);  
  database_objects->clear();
  vcl_cout << "initial database size should be zero: " << database_objects->size() << vcl_endl;
  vcl_ifstream dbfp(db_object_list_filename);
  if (!dbfp) {
    vcl_cout << "Problems in opening db object list file!\n";
    return 0;
  }

  vcl_cout << "reading database objects...\n";
  
  char buffer[1000]; 
  dbfp.getline(buffer, 1000);  // comment 
  vcl_string dummy;
  dbfp >> dummy;   // <contour_segmentation   
  dbfp >> dummy; // object_cnt="23">
  unsigned int size;
  sscanf(dummy.c_str(), "object_cnt=\"%d\">", &size); 

  for (unsigned i = 0; i<size; i++) {
    vcl_cout << "reading database object: " << i << "...\n";
    dbru_object_sptr obj = new dbru_object();
    if (!obj->read_xml(dbfp)) { 
      vcl_cout << "problems in reading database object number: " << i << vcl_endl;
      return 0;
    }

    //: load video file for this database object
    vidl1_movie_sptr my_movie = video_directories[obj->video_id_];
    
    vcl_cout << "read: " << obj << "extracting observations assuming 1 polygon per frame" << vcl_endl;
    
    for (int j = obj->start_frame_; j<=obj->end_frame_; j++) {
      vidl1_frame_sptr frame = my_movie->get_frame(j);
      vil_image_resource_sptr imgr = frame->get_resource();
      vsol_polygon_2d_sptr poly = obj->get_polygon(j-obj->start_frame_);
      int s = poly->size();
      if (s > 0) {
      dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, poly);
      dbinfo_feature_base_sptr intf = new dbinfo_intensity_feature();
      dbinfo_feature_base_sptr gradf = new dbinfo_gradient_feature();
      vcl_vector<dbinfo_feature_base_sptr> features;
      features.push_back(intf);   features.push_back(gradf);
      obs->set_features(features);
      obs->scan(0, imgr);
      obj->add_observation(obs);
#if 1
      // output the images
      char buffer[1000];
      vcl_sprintf(buffer, "./objimages/image_obj%d-poly%d.png",i, j-obj->start_frame_);
      vcl_string filename = buffer;
      poly->compute_bounding_box();

      int w = (int)vcl_floor(poly->get_max_x()-poly->get_min_x()+10+0.5);
      int h = (int)vcl_floor(poly->get_max_y()-poly->get_min_y()+10+0.5);

      int mx = (int)vcl_floor(poly->get_min_x());
      int my = (int)vcl_floor(poly->get_min_y());
      int maxx = (int)vcl_ceil(poly->get_max_x());
      int maxy = (int)vcl_ceil(poly->get_max_y());

      dbinfo_region_geometry_sptr geo = obs->geometry();
      vil1_memory_image_of<float> image_out(w,h);

      for (int y = 0; y<h; y++ ) 
        for (int x = 0; x<w; x++)
          image_out[y][x] = 255;

      dbinfo_feature_data_base_sptr d0 = (obs->features())[0]->data();
      
      assert(d0->format() == DBINFO_INTENSITY_FEATURE);
      dbinfo_feature_data<vbl_array_1d<float> >* cd0 = 
          dbinfo_feature_data<vbl_array_1d<float> >::ptr(d0);
      vbl_array_1d<float>& v0 = cd0->single_data();       
      unsigned npts0 = v0.size();

      for(unsigned k = 0; k<npts0; ++k)
      {
        float vv0 = v0[k];
        vgl_point_2d<float> coord = geo->point(k);
        int yy = (int)(coord.y()-my+5);
        int xx = (int)(coord.x()-mx+5);
        image_out[yy][xx] = vv0;
      }

      vil1_memory_image_of<unsigned char> output_img = brip_vil1_float_ops::convert_to_byte(image_out, 0, 255.0f);
      vil_image_resource_sptr output_sptr = vil1_to_vil_image_resource(output_img);
      vil_save_image_resource(output_sptr, filename.c_str());
      } else {
        obj->add_observation(0);  // add null pointer as observation
      }
      
      
#endif
    }

    if (obj->get_observations_size() == obj->polygon_cnt_)
      //database_objects.push_back(obj);
      database_objects->push_back(obj);
    else {
      vcl_cout << "problems in object " << *(obj) << " skipping!\n";
      continue;
    }
  }

  if (database_objects->size() > 0) {
    //vsl_add_to_binary_loader(dbru_object);
    vsl_b_ofstream obfile(osl_file_name);
    vsl_b_write(obfile, database_objects->size());
    for (unsigned i = 0; i<database_objects->size(); i++) {
      (*database_objects)[i]->b_write(obfile);
    }
    obfile.close();
    vcl_cout << "Objects and observations are written to binary output file\n";
  }

  //ofile.close();
  //return reinterpret_cast<int>(&database_objects);
  return 0;
}

int loaddb(const char *osl_file_name) 
{
  vbl_array_1d<dbru_object_sptr> *database = new vbl_array_1d<dbru_object_sptr>();
  vsl_b_ifstream obfile(osl_file_name);
  int size;
  vsl_b_read(obfile, size);
  for (int i = 0; i<size; i++) {
    dbru_object_sptr obj = new dbru_object();
    obj->b_read(obfile);
    database->push_back(obj);
  }

  obfile.close();
  vcl_cout << database->size() << " objects and observations are read from binary input file\n";
  return reinterpret_cast<int>(database);
}

int main_curve(int argc, char *argv[]) {
  vcl_cout << "computing shgms and finding mutual informations for a given database\n";

  vcl_string obj_file, out_osl, out_file; int n;
  vcl_cout << "argc: " << argc << vcl_endl;
  if (argc != 7) {
    vcl_cout << "Usage: <program name> <db_objs_file_name> <output_osl_name> <out_file> <n (use every n frames for each object)> <k (number of classes)> <line increment (20)>\n";
    return -1;
  }

  obj_file = argv[1];
  out_osl = argv[2];
  int increment = atoi(argv[6]);
  out_file = argv[3];
  n = atoi(argv[4]);
  int k = atoi(argv[5]);
  //int N = atoi(argv[4]);
  //int dummy = atoi(argv[5]);
  //bool esf_exists = (dummy == 0? true:false);
  //bool curve_matching = (dummy >= 2?true:false); // if curve matching then don't do shock matching 

  // CREATE DATABASE
  /*
  vbl_array_1d<dbru_object_sptr> *database = new vbl_array_1d<dbru_object_sptr>();
  createdb(reinterpret_cast<int>(database), obj_file.c_str(), dir_list.c_str(), out_osl.c_str());
  vcl_cout << "there are " << database->size() << " objects in the database\n";
  */

  //LOAD DATABASE PREVIOUSLY CREATED
  vbl_array_1d<dbru_object_sptr> *database = 
    reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (loaddb(out_osl.c_str()));

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
  vcl_ofstream of1((out_file+".out").c_str());
  of1 << "total poylgon hence db size: " << total_poly_cnt << vcl_endl;
  vcl_cout << "total poylgon hence db size: " << total_poly_cnt << vcl_endl;

  vbl_array_2d<double> curve_costs(total_poly_cnt, total_poly_cnt, 1000);
  vbl_array_2d<double> info_line_costs(total_poly_cnt, total_poly_cnt, 1000);
  vbl_array_2d<double> info_dt_costs(total_poly_cnt, total_poly_cnt, 1000);
  int curve_corrects = 0;
  int info_line_corrects = 0;
  int info_dt_corrects = 0;
  //: run the experiment on all the object

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

  int poly_cnt_out = 0; 
  int results_collected = 0;
  for (unsigned i0 = 0; i0 <database->size(); i0++) {
  //for (unsigned i0 = 0; i0 <2; i0++) {
    dbru_object_sptr obj0 = (*database)[i0];

    //don't consider this obj if it does not have same category vehicles to compare with
    bool skip = true;
    for (unsigned i = 0; i<database->size(); i++) {
      if (i == i0) continue;
      dbru_object_sptr obji = (*database)[i];
      if (obji->category_ == obj0->category_) {
        for (unsigned p0 = 0; p0 < obj0->polygon_cnt_; p0+=n) {
          dbru_label_sptr input_label = obj0->labels_[p0];
          for (unsigned pi = 0; pi < obji->polygon_cnt_; pi+=n) {
            dbru_label_sptr obj_label = obji->labels_[pi];
            if (check_labels(input_label, obj_label)) {
              skip = false;
              break;
            }
          }
          if (!skip)
            break;
        }
      }
      if (!skip)
        break;
    }

    if (skip) {
      of1 << obj0->category_<< "_obj " << i0 << " has no comparable instance in the db, skipping it\n";
      continue;
    }
    
    int cnt0 = 0;
    for (unsigned p0 = 0; p0<obj0->polygon_cnt_; p0+=n, cnt0++) {
      if ((obj0->polygons_[p0])->size() <= 0) continue;
      int best_curve_match_obj_id = -1;
      int best_line_match_obj_id = -1;
      int best_dt_match_obj_id = -1;
      int best_curve_match_poly_id = -1;
      int best_line_match_poly_id = -1;
      int best_dt_match_poly_id = -1;
      float best_curve_cost = 100000000;
      float best_line_info = -100000000;
      float best_dt_info = -100000000;
      int poly_cnt_in = 0;

      dbru_label_sptr input_label = obj0->labels_[p0];
      for (unsigned i = 0; i<database->size(); i++) {
      //for (unsigned i = 0; i<2; i++) {
        dbru_object_sptr obji = (*database)[i];
        if (i == i0) {
          poly_cnt_in += used_poly_cnts[i]; //obji->polygon_cnt_;
          continue;
        }
        
        int cnti = 0;
        for (unsigned pi = 0; pi < obji->polygon_cnt_; pi+=n, cnti++) {
          if ((obji->polygons_[pi])->size() <= 0) continue;
          dbru_label_sptr object_label = obji->labels_[pi];
          if (!check_labels(input_label, object_label)) continue;

          double curve_matching_cost, info_line;
          float info_dt;
    
          char buffer[1000];
          vcl_sprintf(buffer, "./outcmimages/obj%d-obj%d-poly%d-poly%d",i0, i, p0, pi);
          vcl_string filename = buffer;
          vcl_cout << filename << vcl_endl;
          vcl_string match_file = filename+"-cmatch.out";
          vcl_ifstream mf(match_file.c_str());
          if (mf.is_open()) {
            mf >> curve_matching_cost;
            mf >> info_line;
            mf >> info_dt;
            mf.close();
          } else {
            info_dt = dbru_object_matcher::minfo_thomas_curve_matching(obj0, obji, p0, pi,
                                              curve_matching_cost,
                                              info_line,
                                              increment,      // increment
                                              true,    // output dt info cost
                                              10.0f,   // R
                                              0.05f,   // for line fitting before matching
                                              "",//filename,  // output image filename
                                              true);  // verbose
            vcl_ofstream mfo(match_file.c_str());
            mfo << curve_matching_cost << " " << info_line << " " << info_dt << "\n";
            mfo.close();
          }
          curve_costs[poly_cnt_out+cnt0][poly_cnt_in+cnti] = curve_matching_cost;
          info_line_costs[poly_cnt_out+cnt0][poly_cnt_in+cnti] = info_line;
          info_dt_costs[poly_cnt_out+cnt0][poly_cnt_in+cnti] = info_dt;
          if (best_curve_cost > curve_matching_cost) {
            best_curve_cost = float(curve_matching_cost);
            best_curve_match_obj_id = i;
            best_curve_match_poly_id = pi;
          }
          if (best_line_info < info_line) {
            best_line_info = float(info_line);
            best_line_match_obj_id = i;
            best_line_match_poly_id = pi;
          }
          if (best_dt_info < info_dt) {
            best_dt_info = info_dt;
            best_dt_match_obj_id = i;
            best_dt_match_poly_id = pi;
          }
        }

        poly_cnt_in += used_poly_cnts[i]; //obji->polygon_cnt_;
      }

      if (best_curve_match_obj_id < 0 || best_line_match_obj_id < 0 || best_dt_match_obj_id < 0) {
        vcl_cout << "obj " << i0 << " poly " << p0 << " has no comparable instance in the db, skipping it\n";
        continue;
      }
      
      of1 << (*database)[i0]->category_ << "_obj" << i0 << "_poly " << p0 << "\n";
      of1 << "cm best match:\t\t" << (*database)[best_curve_match_obj_id]->category_ << "_obj" << best_curve_match_obj_id << "_poly" << best_curve_match_poly_id << "\n";
      of1 << "info line best match:\t\t" << (*database)[best_line_match_obj_id]->category_ << "_obj" << best_line_match_obj_id << "_poly" << best_line_match_poly_id << "\n";
      of1 << "info dt best match:\t\t" << (*database)[best_dt_match_obj_id]->category_ << "_obj" << best_dt_match_obj_id << "_poly" << best_dt_match_poly_id << "\n";

      if (category_id_map[(*database)[i0]->category_] == category_id_map[(*database)[best_curve_match_obj_id]->category_])
        curve_corrects++;
      if (category_id_map[(*database)[i0]->category_] == category_id_map[(*database)[best_line_match_obj_id]->category_])
        info_line_corrects++;
      if (category_id_map[(*database)[i0]->category_] == category_id_map[(*database)[best_dt_match_obj_id]->category_])
        info_dt_corrects++;
      results_collected++;

       // output the best match's image
      char buffer[1000];
      vcl_sprintf(buffer, "./outcmimages/obj%d-obj%d-poly%d-poly%d-best-line",i0, best_line_match_obj_id, p0, best_line_match_poly_id);
      vcl_string filename = buffer;
      double dummy;
      dbru_object_sptr obji = (*database)[best_line_match_obj_id];
      dummy = dbru_object_matcher::minfo_thomas_curve_matching(obj0, obji, p0, best_line_match_poly_id,
                                              dummy,
                                              dummy,
                                              increment,      // increment
                                              false,    // output dt info cost
                                              10.0f,   // R
                                              0.05f,   // for line fitting before matching
                                              filename,  // output image filename
                                              false);  // verbose
      vcl_sprintf(buffer, "./outcmimages/obj%d-obj%d-poly%d-poly%d-best-dt",i0, best_dt_match_obj_id, p0, best_dt_match_poly_id);
      filename = buffer;
      obji = (*database)[best_dt_match_obj_id];
      dummy = dbru_object_matcher::minfo_thomas_curve_matching(obj0, obji, p0, best_dt_match_poly_id,
                                              dummy,
                                              dummy,
                                              increment,      // increment
                                              true,    // output dt info cost
                                              10.0f,   // R
                                              0.05f,   // for line fitting before matching
                                              filename,  // output image filename
                                              false);  // verbose
       
    }
    poly_cnt_out += used_poly_cnts[i0]; //obj0->polygon_cnt_;
  }

  of1 << "total number of polygons: " << total_poly_cnt << " " << " results collected from " << results_collected << " polygons\n";
  of1 << "algorithm\tnumber of corrects\tpercentage\n";
  of1 << "curve matching\t" << curve_corrects << "\t" << (((double)curve_corrects/results_collected)*100.0f) << "\n";
  of1 << "info line\t" << info_line_corrects << "\t" << (((double)info_line_corrects/results_collected)*100.0f) << "\n";
  of1 << "info dt\t" << info_dt_corrects << "\t" << (((double)info_dt_corrects/results_collected)*100.0f) << "\n";

  of1.close();

  vcl_ofstream of((out_file+"matrix.out").c_str());
  of << "db size: " << total_poly_cnt << " names:\n";
  for (unsigned i = 0; i<database->size(); i++) {
  //for (unsigned i = 0; i<2; i++) {
    dbru_object_sptr obji = (*database)[i];
    for (unsigned j = 0; j<obji->polygon_cnt_; j+=n) {
      char buffer[1000];
      vcl_sprintf(buffer, "%s_obj%d-poly%d ",obji->category_.c_str(), i, j);
      vcl_string filename = buffer;
      of << filename << "\n";
    }
  }

  of << "\n curve costs: \n";
  of << curve_costs.rows() << " " << curve_costs.cols() << "\n";
  
  for (int i = 0; i<curve_costs.rows(); i++) {
    for (int j = 0; j<curve_costs.cols(); j++) {
      of << curve_costs[i][j] << " ";
    }
    of << vcl_endl;
  }

  of << "\n info distance transform costs: \n";
  of << info_dt_costs.rows() << " " << info_dt_costs.cols() << "\n";
  
  for (int i = 0; i<info_dt_costs.rows(); i++) {
    for (int j = 0; j<info_dt_costs.cols(); j++) {
      of << info_dt_costs[i][j] << " ";
    }
    of << vcl_endl;
  }

  of << "\n info line costs: \n";
  of << info_line_costs.rows() << " " << info_line_costs.cols() << "\n";
  
  for (int i = 0; i<info_line_costs.rows(); i++) {
    for (int j = 0; j<info_line_costs.cols(); j++) {
      of << info_line_costs[i][j] << " ";
    }
    of << vcl_endl;
  }

  of.close();
  
  return 0;
}

int main_create_db(int argc, char *argv[]) {
  vcl_cout << "extracting observations and creating OSL for a given database\n";

  vcl_string dir_list, obj_file, out_osl;
  vcl_cout << "argc: " << argc << vcl_endl;
  if (argc < 4) {
    vcl_cout << "Usage: <program name> <dir_list_filename> <db_objs_file_name> <output_osl_name>\n";
    return -1;
  }

  dir_list = argv[1];
  obj_file = argv[2];
  out_osl = argv[3];
  
  // CREATE DATABASE
  vbl_array_1d<dbru_object_sptr> *database = new vbl_array_1d<dbru_object_sptr>();
  createdb(reinterpret_cast<int>(database), obj_file.c_str(), dir_list.c_str(), out_osl.c_str());
  vcl_cout << "there are " << database->size() << " objects in the database\n";

  return 0;
}

int main_shock_selected(int argc, char *argv[]) {
  vcl_cout << "run shock matching on selected frames as database\n";

  vcl_string obj_file, out_osl, out_file;
  int n = 5;  // use every n frames in the database
  vcl_cout << "argc: " << argc << vcl_endl;
  if (argc != 8) {
    vcl_cout << "Usage: <program name> <db_objs_file_name> <output_osl_name> <out_file> <double p_threshold> <k (number of classes)> <query pairs list> <db pairs list> \n";
    return -1;
  }

  obj_file = argv[1];
  out_osl = argv[2];
  out_file = argv[3];
  double p_threshold = atof(argv[4]);
  int k = atoi(argv[5]);
  
  vcl_string query_list = argv[6];
  vcl_string db_list = argv[7];

  //LOAD DATABASE PREVIOUSLY CREATED
  vbl_array_1d<dbru_object_sptr> *database = 
    reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (loaddb(out_osl.c_str()));

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
    int i0 = query_pairs[q].first;
    int p0 = query_pairs[q].second;
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
      
    for (unsigned d = 0; d <database_pairs.size(); d++) {
      int i = database_pairs[d].first;
      int pi = database_pairs[d].second;
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
    
      char buffer[1000];
      vcl_sprintf(buffer, "./outsmimages/obj%d-obj%d-poly%d-poly%d",i0, i, p0, pi);
      vcl_string filename = buffer;
      vcl_cout << filename << vcl_endl;
      vcl_string match_file = filename+"-smatch.out";
      vcl_ifstream mf(match_file.c_str());
      if (mf.is_open()) {
        mf >> shock_matching_cost;
        mf >> info;
        mf.close();
      } else {      
        dbru_object_matcher::minfo_shock_matching(obj0, obji, p0, pi, obj0_trees, obji_trees,
                                          shock_matching_cost,
                                          info,
                                          "",  //filename // output image filename
                                          p_threshold,
                                          true);  // verbose
        vcl_ofstream mfo(match_file.c_str());
        mfo << shock_matching_cost << " " << info << "\n";
        mfo.close();
      }
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
   double dummy;
   dbru_object_sptr obji = (*database)[best_info_match_obj_id];
   vbl_array_1d< dbskr_tree_sptr > *obji_trees = database_objects_trees[best_info_match_obj_id];
   dbru_object_matcher::minfo_shock_matching(obj0, obji, p0, best_info_match_poly_id, obj0_trees, obji_trees,
                                           dummy,
                                           dummy,
                                           filename,  //filename // output image filename
                                           p_threshold,
                                           false);  // verbose
  }

  of1 << "total number of polygons: " << total_poly_cnt << " " << " results collected from " << results_collected << " polygons (should be equal to query size which is " << query_pairs.size() << " )\n";
  of1 << "algorithm\tnumber of corrects\tpercentage\n";
  of1 << "shock matching\t" << shock_corrects << "\t" << (((double)shock_corrects/results_collected)*100.0f) << "\n";
  of1 << "info \t" << info_corrects << "\t" << (((double)info_corrects/results_collected)*100.0f) << "\n";
  of1.close();

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

//: MAIN OPT SELECTED
int main_opt_selected(int argc, char *argv[]) {
  vcl_cout << "run mutual info maximization on selected frames as database\n";

  vcl_string obj_file, out_osl, out_file;
  int n = 5;  // use every n frames in the database
  vcl_cout << "argc: " << argc << vcl_endl;
  if (argc != 11) {
    vcl_cout << "Usage: <program name> <db_objs_file_name> <output_osl_name> <out_file> <double p_threshold> <k (number of classes)> <query pairs list> <db pairs list> <dx> <dr> <ds>\n";
    return -1;
  }

  obj_file = argv[1];
  out_osl = argv[2];
  out_file = argv[3];
  double p_threshold = atof(argv[4]);
  int k = atoi(argv[5]);
  
  vcl_string query_list = argv[6];
  vcl_string db_list = argv[7];
  float dx = argv[8]; float dr = argv[9]; float ds = argv[10];

  //LOAD DATABASE PREVIOUSLY CREATED
  vbl_array_1d<dbru_object_sptr> *database = 
    reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (loaddb(out_osl.c_str()));

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

  vbl_array_2d<double> info_costs(query_size, database_size, 1000);
  int info_corrects = 0;
  //: run the experiment on all the objects

  int results_collected = 0;
  for (unsigned q = 0; q <query_pairs.size(); q++) {
    int i0 = query_pairs[q].first;
    int p0 = query_pairs[q].second;
    dbru_object_sptr obj0 = (*database)[i0];
    if ((obj0->polygons_[p0])->size() <= 0) continue;
    
    int best_info_match_obj_id = -1;
    int best_info_match_poly_id = -1;
    float best_info = -100000000;
    dbru_label_sptr input_label = obj0->labels_[p0];
      
    for (unsigned d = 0; d <database_pairs.size(); d++) {
      int i = database_pairs[d].first;
      int pi = database_pairs[d].second;
      dbru_object_sptr obji = (*database)[i];
      if (i == i0) 
        continue;
        
      if ((obji->polygons_[pi])->size() <= 0) continue;
      dbru_label_sptr object_label = obji->labels_[pi];
      if (!check_labels(input_label, object_label)) continue;

      float info;
      char buffer[1000];
      vcl_sprintf(buffer, "./outoptimages/obj%d-obj%d-poly%d-poly%d",i0, i, p0, pi);
      vcl_string filename = buffer;
      vcl_cout << filename << vcl_endl;
      
      dbru_object_matcher::minfo_rigid_alignment(obj0->get_observation(p0), obji->get_observation(pi), dx, dr, ds);  // verbose
      info_costs[q][d] = info;
      if (best_info < info) {
        best_info = float(info);
        best_info_match_obj_id = i;
        best_info_match_poly_id = pi;
      }
   }
      
   if (best_info_match_obj_id < 0) {
     vcl_cout << "obj " << i0 << " poly " << p0 << " has no comparable instance in the db, skipping it\n";
     continue;
   }

   of1 << (*database)[i0]->category_ << "_obj" << i0 << "_poly " << p0 << "\n";
   of1 << "info best match:\t\t" << (*database)[best_info_match_obj_id]->category_ << "_obj" << best_info_match_obj_id << "_poly" << best_info_match_poly_id << "\n";
   if (category_id_map[(*database)[i0]->category_] == category_id_map[(*database)[best_info_match_obj_id]->category_])
    info_corrects++;
   results_collected++;
   // output the best match's image
   char buffer[1000];
   vcl_sprintf(buffer, "./outoptimages/obj%d-obj%d-poly%d-poly%d",i0, best_info_match_obj_id, p0, best_info_match_poly_id);
   vcl_string filename = buffer;
   double dummy;
   dbru_object_sptr obji = (*database)[best_info_match_obj_id];
   vil_image_resource_sptr dummy, image_r;
   dbru_object_matcher::minfo_rigid_alignment(obj0->get_observation(p0), obji->get_observation(pi), dx, dr, ds, dummy, dummy, image_r, false);  // verbose
   vil_save_image_resource(image_r, filename+"-opt.png");
  }

  of1 << "total number of polygons: " << total_poly_cnt << " " << " results collected from " << results_collected << " polygons (should be equal to query size which is " << query_pairs.size() << " )\n";
  of1 << "algorithm\tnumber of corrects\tpercentage\n";
  of1 << "info \t" << info_corrects << "\t" << (((double)info_corrects/results_collected)*100.0f) << "\n";
  of1.close();

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



int main_curve_selected(int argc, char *argv[]) {
  vcl_cout << "run curve matching on selected frames as database\n";

  vcl_string obj_file, out_osl, out_file;
  int n;  // use every n frames in the database
  vcl_cout << "argc: " << argc << vcl_endl;
  if (argc != 9) {
    vcl_cout << "Usage: <program name> <db_objs_file_name> <output_osl_name> <out_file> <n> <k (number of classes)> <increment> <query pairs list> <db pairs list> \n";
    return -1;
  }

  obj_file = argv[1];
  out_osl = argv[2];
  out_file = argv[3];
  n = atoi(argv[4]);
  int k = atoi(argv[5]);
  int increment = atoi(argv[6]);
  
  vcl_string query_list = argv[7];
  vcl_string db_list = argv[8];

  //LOAD DATABASE PREVIOUSLY CREATED
  vbl_array_1d<dbru_object_sptr> *database = 
    reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (loaddb(out_osl.c_str()));

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
  vcl_cout << " there are " << query_pairs.size() << " pairs in the query list and " << database_pairs.size() << " pairs in the database list\n";

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

  vbl_array_2d<double> curve_costs(query_size, database_size, 1000);
  vbl_array_2d<double> info_line_costs(query_size, database_size, 1000);
  vbl_array_2d<double> info_dt_costs(query_size, database_size, 1000);
  int curve_corrects = 0; int info_line_corrects = 0; int info_dt_corrects = 0;
  //: run the experiment on all the objects

  int results_collected = 0;
  for (unsigned q = 0; q <query_pairs.size(); q++) {
    int i0 = query_pairs[q].first;
    int p0 = query_pairs[q].second;
    dbru_object_sptr obj0 = (*database)[i0];
    if ((obj0->polygons_[p0])->size() <= 0) continue;
      
    int best_curve_match_obj_id = -1; int best_line_match_obj_id = -1; int best_dt_match_obj_id = -1;
    int best_curve_match_poly_id = -1; int best_line_match_poly_id = -1; int best_dt_match_poly_id = -1;
    float best_curve_cost = 100000000; float best_line_info = -100000000; float best_dt_info = -100000000;
    dbru_label_sptr input_label = obj0->labels_[p0];
      
    for (unsigned d = 0; d <database_pairs.size(); d++) {
      int i = database_pairs[d].first;
      int pi = database_pairs[d].second;
      dbru_object_sptr obji = (*database)[i];
      if (i == i0) 
        continue;
        
      if ((obji->polygons_[pi])->size() <= 0) continue;
      dbru_label_sptr object_label = obji->labels_[pi];
      if (!check_labels(input_label, object_label)) continue;

      double curve_matching_cost, info_line;
      float info_dt;
      char buffer[1000];
      vcl_sprintf(buffer, "./outcmimages/obj%d-obj%d-poly%d-poly%d",i0, i, p0, pi);
      vcl_string filename = buffer;
      vcl_cout << filename << vcl_endl;
      vcl_string match_file = filename+"-cmatch.out";
      vcl_ifstream mf(match_file.c_str());
      if (mf.is_open()) {
        mf >> curve_matching_cost;
        mf >> info_line;
        mf >> info_dt;
        mf.close();
      } else {
        info_dt = dbru_object_matcher::minfo_thomas_curve_matching(obj0, obji, p0, pi,
                                          curve_matching_cost,
                                          info_line,
                                          increment,      // increment
                                          true,    // output dt info cost
                                          10.0f,   // R
                                          0.05f,   // for line fitting before matching
                                          "",//filename,  // output image filename
                                          true);  // verbose
        vcl_ofstream mfo(match_file.c_str());
        mfo << curve_matching_cost << " " << info_line << " " << info_dt << "\n";
        mfo.close();
      }
      curve_costs[q][d] = curve_matching_cost;
      info_line_costs[q][d] = info_line;
      info_dt_costs[q][d] = info_dt;
      if (best_curve_cost > curve_matching_cost) {
        best_curve_cost = float(curve_matching_cost);
        best_curve_match_obj_id = i;
        best_curve_match_poly_id = pi;
      }
      if (best_line_info < info_line) {
        best_line_info = float(info_line);
        best_line_match_obj_id = i;
        best_line_match_poly_id = pi;
      }
      if (best_dt_info < info_dt) {
        best_dt_info = info_dt;
        best_dt_match_obj_id = i;
        best_dt_match_poly_id = pi;
      }
   }
      
   if (best_curve_match_obj_id < 0 || best_line_match_obj_id < 0 || best_dt_match_obj_id < 0) {
     vcl_cout << "obj " << i0 << " poly " << p0 << " has no comparable instance in the db, skipping it\n";
     continue;
   }

   of1 << (*database)[i0]->category_ << "_obj" << i0 << "_poly " << p0 << "\n";
   of1 << "cm best match:\t\t" << (*database)[best_curve_match_obj_id]->category_ << "_obj" << best_curve_match_obj_id << "_poly" << best_curve_match_poly_id << "\n";
   of1 << "info line best match:\t\t" << (*database)[best_line_match_obj_id]->category_ << "_obj" << best_line_match_obj_id << "_poly" << best_line_match_poly_id << "\n";
   of1 << "info dt best match:\t\t" << (*database)[best_dt_match_obj_id]->category_ << "_obj" << best_dt_match_obj_id << "_poly" << best_dt_match_poly_id << "\n";

   if (category_id_map[(*database)[i0]->category_] == category_id_map[(*database)[best_curve_match_obj_id]->category_])
     curve_corrects++;
   if (category_id_map[(*database)[i0]->category_] == category_id_map[(*database)[best_line_match_obj_id]->category_])
     info_line_corrects++;
   if (category_id_map[(*database)[i0]->category_] == category_id_map[(*database)[best_dt_match_obj_id]->category_])
     info_dt_corrects++;
   results_collected++;

    // output the best match's image
   char buffer[1000];
   vcl_sprintf(buffer, "./outcmimages/obj%d-obj%d-poly%d-poly%d-best-line",i0, best_line_match_obj_id, p0, best_line_match_poly_id);
   vcl_string filename = buffer;
   double dummy;
   dbru_object_sptr obji = (*database)[best_line_match_obj_id];
   dummy = dbru_object_matcher::minfo_thomas_curve_matching(obj0, obji, p0, best_line_match_poly_id,
                                              dummy,
                                              dummy,
                                              increment,      // increment
                                              false,    // output dt info cost
                                              10.0f,   // R
                                              0.05f,   // for line fitting before matching
                                              filename,  // output image filename
                                              false);  // verbose
   vcl_sprintf(buffer, "./outcmimages/obj%d-obj%d-poly%d-poly%d-best-dt",i0, best_dt_match_obj_id, p0, best_dt_match_poly_id);
   filename = buffer;
   obji = (*database)[best_dt_match_obj_id];
   dummy = dbru_object_matcher::minfo_thomas_curve_matching(obj0, obji, p0, best_dt_match_poly_id,
                                             dummy,
                                              dummy,
                                              increment,      // increment
                                              true,    // output dt info cost
                                              10.0f,   // R
                                              0.05f,   // for line fitting before matching
                                              filename,  // output image filename
                                              false);  // verbose
  }

  of1 << "total number of polygons: " << total_poly_cnt << " " << " results collected from " << results_collected << " polygons (should be equal to query size which is " << query_pairs.size() << " )\n";
  of1 << "algorithm\tnumber of corrects\tpercentage\n";
  of1 << "shock matching\t" << curve_corrects << "\t" << (((double)curve_corrects/results_collected)*100.0f) << "\n";
  of1 << "info line \t" << info_line_corrects << "\t" << (((double)info_line_corrects/results_collected)*100.0f) << "\n";
  of1 << "info dt \t" << info_dt_corrects << "\t" << (((double)info_dt_corrects/results_collected)*100.0f) << "\n";
  of1.close();

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

  of << "\n info distance transform costs: \n";
  of << info_dt_costs.rows() << " " << info_dt_costs.cols() << "\n";
  
  for (int i = 0; i<info_dt_costs.rows(); i++) {
    for (int j = 0; j<info_dt_costs.cols(); j++) {
      of << info_dt_costs[i][j] << " ";
    }
    of << vcl_endl;
  }

  of << "\n info line costs: \n";
  of << info_line_costs.rows() << " " << info_line_costs.cols() << "\n";
  
  for (int i = 0; i<info_line_costs.rows(); i++) {
    for (int j = 0; j<info_line_costs.cols(); j++) {
      of << info_line_costs[i][j] << " ";
    }
    of << vcl_endl;
  }

  of.close();
  
  return 0;
}

int main_shock(int argc, char *argv[]) {
  vcl_cout << "IN MAIN SHOCK\n";

  vcl_string obj_file, out_osl, out_file;
  int n = 5;  // use every n frames in the database
  vcl_cout << "argc: " << argc << vcl_endl;
  if (argc != 6) {
    vcl_cout << "Usage: <program name> <db_objs_file_name> <output_osl_name> <out_file> <p_threshold 0.1> <k (number of classes)>\n";
    return -1;
  }

  obj_file = argv[1];
  out_osl = argv[2];
  out_file = argv[3];
  //n = atoi(argv[4]);
  double p_threshold = atof(argv[4]);
  int k = atoi(argv[5]);
  
  //LOAD DATABASE PREVIOUSLY CREATED
  vbl_array_1d<dbru_object_sptr> *database = 
    reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (loaddb(out_osl.c_str()));

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
  of1 << "total poylgon hence db size: " << total_poly_cnt << vcl_endl;

  vbl_array_2d<double> shock_costs(total_poly_cnt, total_poly_cnt, 1000);
  vbl_array_2d<double> info_costs(total_poly_cnt, total_poly_cnt, 1000);
  int shock_corrects = 0;
  int info_corrects = 0;
  //: run the experiment on all the objects
  
  int poly_cnt_out = 0; 
  int results_collected = 0;
  //: keep the dbskr_tree_sptrs as they are created in an array for each object used
  //  not to repeat costly shock curve constructions each time they are reused
  vbl_array_1d< vbl_array_1d<dbskr_tree_sptr> * > database_objects_trees(database->size(), 0);
  for (unsigned i0 = 0; i0 <database->size(); i0++) {
  //for (unsigned i0 = 0; i0 <2; i0++) {
    dbru_object_sptr obj0 = (*database)[i0];

    //don't consider this obj if it does not have same category vehicles to compare with
    bool skip = true;
    for (unsigned i = 0; i<database->size(); i++) {
      if (i == i0) continue;
      dbru_object_sptr obji = (*database)[i];
      if (obji->category_ == obj0->category_) {
        
        for (unsigned p0 = 0; p0 < obj0->polygon_cnt_; p0+=n) {
          
          dbru_label_sptr input_label = obj0->labels_[p0];
          for (unsigned pi = 0; pi < obji->polygon_cnt_; pi+=n) {
            
            dbru_label_sptr obj_label = obji->labels_[pi];
            
            if (check_labels(input_label, obj_label)) {
              skip = false;
              break;
            }
          
          }
          if (!skip)
            break;
        }
      }
      if (!skip)
        break;
    }

    if (skip) {
      of1 << obj0->category_<< "_obj " << i0 << " has no comparable instance in the db, skipping it\n";
      continue;
    }

    vbl_array_1d<dbskr_tree_sptr> *obj0_trees = database_objects_trees[i0];
    if (!obj0_trees) {
      obj0_trees = new vbl_array_1d<dbskr_tree_sptr>();
      database_objects_trees[i0] = obj0_trees;
    }
    
    int cnt0 = 0;
    for (unsigned p0 = 0; p0<obj0->polygon_cnt_; p0+=n, cnt0++) {
      if ((obj0->polygons_[p0])->size() <= 0) continue;
      
      int best_shock_match_obj_id = -1;
      int best_info_match_obj_id = -1;
      int best_shock_match_poly_id = -1;
      int best_info_match_poly_id = -1;
      float best_shock_cost = 100000000;
      float best_info = -100000000;
      int poly_cnt_in = 0;
      dbru_label_sptr input_label = obj0->labels_[p0];
      
      for (unsigned i = 0; i<database->size(); i++) {
      //for (unsigned i = 0; i<2; i++) {
        dbru_object_sptr obji = (*database)[i];
        if (i == i0) {
          poly_cnt_in += used_poly_cnts[i];// obji->polygon_cnt_;
          continue;
        }

        vbl_array_1d<dbskr_tree_sptr> *obji_trees = database_objects_trees[i];
        if (!obji_trees) {
          obji_trees = new vbl_array_1d<dbskr_tree_sptr>();
          database_objects_trees[i] = obji_trees;
        }
        
        int cnti = 0; 
        for (unsigned pi = 0; pi < obji->polygon_cnt_; pi+=n, cnti++) {
          if ((obji->polygons_[pi])->size() <= 0) continue;
          dbru_label_sptr object_label = obji->labels_[pi];
          if (!check_labels(input_label, object_label)) continue;

          double shock_matching_cost, info;
    
          char buffer[1000];
          vcl_sprintf(buffer, "./outsmimages/obj%d-obj%d-poly%d-poly%d",i0, i, p0, pi);
          vcl_string filename = buffer;
          vcl_cout << filename << vcl_endl;
          vcl_string match_file = filename+"-smatch.out";
          vcl_ifstream mf(match_file.c_str());
          if (mf.is_open()) {
            mf >> shock_matching_cost;
            mf >> info;
            mf.close();
          } else {
            
            dbru_object_matcher::minfo_shock_matching(obj0, obji, p0, pi, obj0_trees, obji_trees,
                                              shock_matching_cost,
                                              info,
                                              "",  //filename // output image filename
                                              p_threshold,
                                              true);  // verbose
            vcl_ofstream mfo(match_file.c_str());
            mfo << shock_matching_cost << " " << info << "\n";
            mfo.close();
          }
          shock_costs[poly_cnt_out+cnt0][poly_cnt_in+cnti] = shock_matching_cost;
          info_costs[poly_cnt_out+cnt0][poly_cnt_in+cnti] = info;
          if (best_shock_cost > shock_matching_cost) {
            best_shock_cost = float(shock_matching_cost);
            best_shock_match_obj_id = i;
            best_shock_match_poly_id = pi;
          }
          if (best_info < info) {
            best_info = float(info);
            best_info_match_obj_id = i;
            best_info_match_poly_id = pi;
          }
        }

        poly_cnt_in += used_poly_cnts[i]; // obji->polygon_cnt_;
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
      double dummy;
      dbru_object_sptr obji = (*database)[best_info_match_obj_id];
      vbl_array_1d< dbskr_tree_sptr > *obji_trees = database_objects_trees[best_info_match_obj_id];
      dbru_object_matcher::minfo_shock_matching(obj0, obji, p0, best_info_match_poly_id, obj0_trees, obji_trees,
                                              dummy,
                                              dummy,
                                              filename,  //filename // output image filename
                                              p_threshold,
                                              false);  // verbose

    }
    poly_cnt_out += used_poly_cnts[i0]; // obj0->polygon_cnt_;
  }

  of1 << "total number of polygons: " << total_poly_cnt << " " << " results collected from " << results_collected << " polygons\n";
  of1 << "algorithm\tnumber of corrects\tpercentage\n";
  of1 << "shock matching\t" << shock_corrects << "\t" << (((double)shock_corrects/results_collected)*100.0f) << "\n";
  of1 << "info \t" << info_corrects << "\t" << (((double)info_corrects/results_collected)*100.0f) << "\n";
  of1.close();

  vcl_ofstream of((out_file+"matrix.out").c_str());
  of << "db size: " << total_poly_cnt << " names:\n";
  for (unsigned i = 0; i<database->size(); i++) {
  //for (unsigned i = 0; i<2; i++) {
    dbru_object_sptr obji = (*database)[i];
    for (unsigned j = 0; j<obji->polygon_cnt_; j+=n) {
      char buffer[1000];
      vcl_sprintf(buffer, "%s_obj%d_poly%d ",obji->category_.c_str(), i, j);
      vcl_string filename = buffer;
      of << filename << "\n";
    }
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

int main(int argc, char *argv[]) {
  //if (argc == 4)
  //  main_create_db(argc, argv);
  //else if (argc == 6)
  //  main_shock(argc, argv);
  //else if (argc == 7)
  //  main_curve(argc, argv);
  //else if (argc == 8) {
  //  main_shock_selected(argc, argv);
  //} else if (argc == 9) {
  //  main_curve_selected(argc, argv);
  //}
  
  main_opt_selected(argc, argv);
  /*else {
    vcl_cout << "Usage to create db: <program name> <video id path list file> <db_objs_file_name> <output_osl_name>\n";
    vcl_cout << vcl_endl;
    vcl_cout << "Usage for shocks: <program name> <db_objs_file_name> <output_osl_name> <out_file> <n (use every n frames for each object)> <k (number of classes)>\n";
    vcl_cout << vcl_endl;
    vcl_cout << "Usage for curve: <program name> <db_objs_file_name> <output_osl_name> <out_file> <n (use every n frames for each object)> <k (number of classes)> <increment for lines (20)>\n";
    vcl_cout << vcl_endl;
    vcl_cout << "Usage for shock selected: <program name> <db_objs_file_name> <output_osl_name> <out_file>  <double p_threshold> <k (number of classes)> <query pairs list> <db pairs list> \n";
    vcl_cout << vcl_endl;
    vcl_cout << "Usage for curve selected: <program name> <db_objs_file_name> <output_osl_name> <out_file> <n> <k (number of classes)> <increment> <query pairs list> <db pairs list> \n";
  }*/

  return 0;
}
