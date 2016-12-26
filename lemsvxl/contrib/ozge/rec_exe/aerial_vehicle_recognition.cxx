// This is aerial_vehicle_recognition.cpp
//:
//---------------------------------------------------------------------
// \file
// \brief Defines the entry point for the DLL application.
// \author Ozge Can Ozcanli
//
// \verbatim
//  Modifications
//  Ozge C Ozcanli - Dec 04, 06 - Major change to use 
//                                dbru_osl* directly instead of 
//                                vbl_array_1d<dbru_object_sptr>*
//                                so that category_id_map member of osl class can be utilized
// \endverbatim
// 
//---------------------------------------------------------------------
#include <vcl_cstdio.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vsl/vsl_binary_io.h>

#include <dbru/dbru_object_sptr.h>
#include <dbru/dbru_object.h>
#include <vidl1/vidl1_movie_sptr.h>
#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_io.h>

#include <vbl/vbl_array_1d.h>
#include <vul/vul_timer.h>
#include <vil1/vil1_memory_image_of.h>
#include <brip/brip_vil1_float_ops.h>
#include <vil1/vil1_vil.h>
#include <vil/vil_save.h>

#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_feature_data.h>
#include <dbinfo/dbinfo_intensity_feature.h>
#include <dbinfo/dbinfo_gradient_feature.h>
#include <dbinfo/dbinfo_observation_matcher.h>
#include <dbinfo/dbinfo_region_geometry_sptr.h>
#include <dbinfo/dbinfo_region_geometry.h>

#include <dbru/dbru_object_sptr.h>
#include <dbru/pro/dbru_osl_storage.h>
#include <dbru/dbru_osl.h>
#include <dbru/dbru_object.h>
#include <dbru/dbru_label.h>
#include <dbru/algo/dbru_object_matcher.h>
#include <dbcvr/dbcvr_cv_cor_sptr.h>
#include <dbcvr/dbcvr_cv_cor.h>
#include <dbru/dbru_rcor_sptr.h>
#include <dbru/dbru_rcor.h>

#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vgl/vgl_polygon.h>

#include "aerial_vehicle_recognition.h"



#if 0// version that uses a fixed category_id_map in the deliveries before Dec 04, 06
#define N_CLASSES  (6)
#endif

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
int createosl(char *db_object_list_filename, char *video_list_file_name, char *osl_file_name) 
{ 
#if 0 // version that creates the osl from command line in the deliveries before Dec 04, 06
      // not necessary anymore, because OSL is created via GUI utilities
  vcl_ofstream ofile("log.txt");
  vcl_ifstream fp(video_list_file_name);
  if (!fp) {
    ofile << "Problems in opening video directory list file!\n";
    return 1;
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
      return 2;
    }
    video_directories[id] = my_movie;
    fp >> id;
  }
  fp.close();
  
  ofile<< "loaded " << video_directories.size() << " video dirs from the file\n";
  
  //vbl_array_1d<dbru_object_sptr> *database_objects = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (handle);  
  vbl_array_1d<dbru_object_sptr> *database_objects = new vbl_array_1d<dbru_object_sptr>();
  database_objects->clear();

  vcl_ifstream dbfp(db_object_list_filename);
  if (!dbfp) {
    ofile << "Problems in opening db object list file!\n";
    return 3;
  }

  ofile << "reading database objects...\n";
  
  char buffer[1000]; 
  dbfp.getline(buffer, 1000);  // comment 
  vcl_string dummy;
  dbfp >> dummy;   // <contour_segmentation   
  dbfp >> dummy; // object_cnt="23">
  unsigned int size;
  sscanf(dummy.c_str(), "object_cnt=\"%d\">", &size); 

  for (unsigned i = 0; i<size; i++) {
    ofile << "reading database object: " << i << "...\n";
    dbru_object_sptr obj = new dbru_object();
    if (!obj->read_xml(dbfp)) { 
      ofile << "problems in reading database object number: " << i << vcl_endl;
      return 4;
    }

    //: get video file for this database object
    vidl1_movie_sptr my_movie = video_directories[obj->video_id_];
    
    ofile << "read: " << obj << " extracting observations assuming 1 polygon per frame" << vcl_endl;
    
    for (int j = obj->start_frame_; j<=obj->end_frame_; j++) {
      vidl1_frame_sptr frame = my_movie->get_frame(j);
      vil_image_resource_sptr imgr = frame->get_resource();
      vsol_polygon_2d_sptr poly = obj->get_polygon(j-obj->start_frame_);
      int s = poly->size();
      if (s > 0) {

        // get convex hull and use that
        ofile << "!!!creating convex hulled observations\n";
        vcl_vector<vgl_point_2d<double> > ps;
        for(unsigned int pj=0;pj<poly->size();pj++)
        {
          vgl_point_2d<double> p(poly->vertex(pj)->x(),poly->vertex(pj)->y());
          ps.push_back(p);
        }
        vcl_vector<vsol_point_2d_sptr> cps;
        vgl_convex_hull_2d<double> hullp(ps);
        vgl_polygon<double> psg=hullp.hull();
        for(unsigned int k=0;k<psg[0].size();k++)
          cps.push_back(new vsol_point_2d(psg[0][k].x(),psg[0][k].y()));
        vsol_polygon_2d_sptr c_poly = new vsol_polygon_2d(cps);
        ofile << "size before hull: " << s << " size after hull: " << c_poly->size() << vcl_endl;
        //dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, poly, true, true, false);
        dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, c_poly, true, true, false);
        obs->scan(0, imgr);
        obj->add_observation(obs);
        // output the images
        ofile << " try to save output image into ./v<videoid>_objimages directory\n";
        char buffer[1000];
        vcl_sprintf(buffer, "./v%d_objimages/image_obj%d-poly%d.png",obj->video_id_, i, j-obj->start_frame_);
        vcl_string filename = buffer;

        vcl_ofstream dummy_f(filename.c_str());
        bool create = false;
        if (dummy_f.is_open()) {
          dummy_f.close();
          create = true;
        } else {
          ofile << " cannot create the image file: " << filename << " (directory does not exist) " <<vcl_endl;
        }

        if (create) {
        
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
        }
      
      } else {
        obj->add_observation(0);  // add null pointer as observation
      }
    }

    if (obj->n_observations() == obj->polygon_cnt_)
      database_objects->push_back(obj);
    else {
      ofile << "problems in object " << *(obj) << " skipping!\n";
      continue;
    }
  }

  

#if 0 // version that writes the binary in the deliveries before Aug 03, 2006
  if (database_objects->size() > 0) {
    vsl_b_ofstream obfile(osl_file_name);
    vsl_b_write(obfile, database_objects->size());
    for (unsigned i = 0; i<database_objects->size(); i++) {
      (*database_objects)[i]->b_write(obfile);
    }
    obfile.close();
    ofile << "Objects and observations are written to binary output file\n";
  }

  ofile.close();
#endif

  if (database_objects->size() > 0) {
    dbru_osl_sptr osl = new dbru_osl();
    
    for (unsigned i = 0; i<database_objects->size(); i++)
      osl->add_object((*database_objects)[i]);

    dbru_osl_storage_sptr osl_str = dbru_osl_storage_new();
    osl_str->set_osl(osl);

    vsl_b_ofstream obfile(osl_file_name);
    osl_str->b_write(obfile);
    obfile.close();
    ofile << "Objects and observations are written to binary output file\n";
  }

  ofile.close();
#endif
  return 0;
}

unsigned int loadosl(char *osl_file_name) 
{
#if 0 // version that uses vbl_array_1d<dbru_object_sptr> * in the deliveries before Dec 04, 06
  vbl_array_1d<dbru_object_sptr> *database = new vbl_array_1d<dbru_object_sptr>();
#endif

  vsl_b_ifstream obfile(osl_file_name);
  dbru_osl_storage_sptr osl_storage = dbru_osl_storage_new();
  osl_storage->b_read(obfile);
  obfile.close();
  vcl_cout << "---------------------------------\nTHIS OSL has ";
  vcl_cout << osl_storage->get_osl_size() << " objects\n";
  vcl_cout << "---------------------------------\n";
#if 0 // version that uses vbl_array_1d<dbru_object_sptr> * in the deliveries before Dec 04, 06
  for (unsigned i = 0; i<osl_storage->get_osl_size(); i++) {
    dbru_object_sptr objs = osl_storage->get_object(i);
    dbru_object o = *objs;
    dbru_object_sptr obj = new dbru_object(o);
    database->push_back(obj);
  }
#endif

#if 0  // version that reads the binary in the deliveries before Aug 03, 2006
  int size;
  vsl_b_read(obfile, size);
  for (int i = 0; i<size; i++) {
    dbru_object_sptr obj = new dbru_object();
    obj->b_read(obfile);
    database->push_back(obj);
  }
#endif

  obfile.close();

#if 0 // version that uses vbl_array_1d<dbru_object_sptr> * in the deliveries before Dec 04, 06
  return reinterpret_cast<unsigned int>(database);
#endif

  dbru_osl * osl = new dbru_osl(*(osl_storage->osl()));
  return reinterpret_cast<unsigned int>(osl);
}

//: create a database from selected observations in the database
unsigned int createdb(unsigned int osl_handle, char *selected_obs_list_file_name) 
{
  vcl_ofstream ofile("log.txt");

  if (!osl_handle) {
    ofile << "OSL pointer is zero, exiting!\n";
    return 0;
  }
#if 0 // version that uses vbl_array_1d<dbru_object_sptr> * in the deliveries before Dec 04, 06
  vbl_array_1d<dbru_object_sptr> *osl = 
    reinterpret_cast<vbl_array_1d<dbru_object_sptr> *>(osl_handle);
#endif
  dbru_osl * osl = reinterpret_cast<dbru_osl *>(osl_handle);

  if (osl->size() <= 0) {
    ofile << "Problems in getting osl\n";
    return 0;
  } else {
    ofile << "OSL loaded, number of objects: " << osl->size() << "\n";
  }

  vcl_ifstream fp2(selected_obs_list_file_name);
  if (!fp2) {
    ofile << "Problems in opening selected frames database list file!\n";
    return 0;
  }
 
  vbl_array_1d<vcl_pair<int, int> > *database_pairs = new vbl_array_1d<vcl_pair<int, int> >();
  int database_size = 0;
  fp2 >> database_size;
  for (int i = 0; i<database_size; i++) {  // 1 line for each object, first cnt then frame nos
    vcl_pair<int, int> obj_pair;
    fp2 >> obj_pair.first;
    fp2 >> obj_pair.second;
    database_pairs->push_back(obj_pair);
  }
  fp2.close();
  ofile.close();
  return reinterpret_cast<unsigned int>(database_pairs);
}

//: create a list which contains likelihood values for each class 
//  rms: root mean square error for line fitting before curve matching
//       the smaller rms gets, the coarser the polygon gets, the faster the algorithm runs, but the worse the correspondence gets
//       typical value 0.05 is used in our experiments
//  n  : the number of top matches to be considered to be used in score generation
//       typical value is 2 or 3 depending on database size, and the number of instances from each category
int run_object_dt(unsigned int osl_handle, unsigned int db_handle, int i0, int p0, double *output_list, float rms, int n) {
  vcl_ofstream ofile("log.txt");

  if (!osl_handle) {
    ofile << "OSL pointer is zero, exiting!\n";
    return -1;
  }
#if 0 // version that uses vbl_array_1d<dbru_object_sptr> * in the deliveries before Dec 04, 06
  vbl_array_1d<dbru_object_sptr> *osl = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *>(osl_handle);
#endif
  dbru_osl * osl = reinterpret_cast<dbru_osl *>(osl_handle);

  if (osl->size() <= 0) {
    ofile << "Problems in getting osl\n";
    return -1;
  } else {
    ofile << "OSL loaded, number of objects: " << osl->size() << "\n";
  }

  if (!db_handle) {
    ofile << "Database pointer (db_handle) is zero, exiting!\n";
    return -1;
  }

  vbl_array_1d<vcl_pair<int, int> > *database_pairs = 
    reinterpret_cast<vbl_array_1d<vcl_pair<int, int> > *>(db_handle);

  vcl_map<vcl_string, int>& category_id_map = osl->get_category_id_map();
#if 0 // version that uses a fixed category_id_map in the deliveries before Dec 04, 06
  category_id_map["minivan"] = 0;
  category_id_map["suv"] = 0;
  category_id_map["utility"] = 0;
  category_id_map["car"] = 1;
  category_id_map["pick-up truck"] = 2;
  category_id_map["van"] = 3;
  category_id_map["hummer"] = 4;
  category_id_map["milcargo truck"] = 5;
#endif

  int database_size = database_pairs->size();
  vbl_array_1d<double> infos(database_size, -1000);

  dbru_object_sptr obj0 = (*osl)[i0];
  if ((obj0->polygons_[p0])->size() <= 0) {
    ofile << "Query object's polygon's size is 0\n";
    return -2;
  }
      
  dbru_label_sptr input_label = obj0->labels_[p0];
  for (int d = 0; d <database_size; d++) {
    int i = (*database_pairs)[d].first;
    int pi = (*database_pairs)[d].second;
    dbru_object_sptr obji = (*osl)[i];
    if (i == i0) {
      ofile << "Not matching against itself\n";
      continue;
    }

    if ((obji->polygons_[pi])->size() <= 0) continue;
    dbru_label_sptr object_label = obji->labels_[pi];
    if (!check_labels(input_label, object_label)) {
      ofile << "label mismatch (motion or view or shadow bin is different) for db object id: " << i << " polygon id " << pi << " skipping this instance\n";
      continue;
    }

    double curve_matching_cost, info_line, info_dt;
    int increment = 50;
    dbru_object_matcher::minfo_thomas_curve_matching(obj0, obji, p0, pi,
                                          curve_matching_cost,
                                          info_line,
                                          info_dt,
                                          increment,      // increment
                                          true,    // output dt info cost
                                          10.0f,   // R
                                          rms,   // for line fitting before matching
                                          false);  // verbose
    infos[d] = info_dt;    
  }

  // find best n values
  double total_info = 0;
 
  for (unsigned kk = 0; kk < category_id_map.size(); kk++)
    output_list[kk] = 0;
  
  int upper = n<database_size?n:database_size;
  for (int k = 0; k<upper; k++) {
    double max = infos[0];
    int max_d = 0;
    for (int d = 1; d <database_size; d++) {
      if (infos[d] > max) {
        max = infos[d];
        max_d = d;
      }
    }

    if (max == -1000) {
      ofile << "None of the items in db were comparable to the query\n";
      break;
    }

    int i = (*database_pairs)[max_d].first;
    int pi = (*database_pairs)[max_d].second;
    dbru_object_sptr obji = (*osl)[i];
    ofile << "match " << k << " info: " << infos[max_d] << " category: " << obji->category_ << "\n";
    total_info += max;
    output_list[category_id_map[obji->category_]] += max;
    infos[max_d] = -990;
  }

  if (total_info != 0) {
    for (unsigned kkk = 0; kkk < category_id_map.size(); kkk++)
      output_list[kkk] /= total_info;
  } else {
    for (unsigned kkk = 0; kkk < category_id_map.size(); kkk++)
      output_list[kkk] = 0;
  }
  
  return 0;
}
//: create a list which contains likelihood values for each class 
//  rms: root mean square error for line fitting before curve matching
//       the smaller rms gets, the coarser the polygon gets, the faster the algorithm runs, but the worse the correspondence gets
//       typical value 0.05 is used in our experiments
//  n  : the number of top matches to be considered to be used in score generation
//       typical value is 2 or 3 depending on database size, and the number of instances from each category
//  increment: internal parameter of line intersection algorithm which determines the density of correspondence
//             min value is 1, the densest and best results
//             typical 5 to 20, the higher the value, the faster the algorithm runs, but the worse the correspondence gets
int run_object_line(unsigned int osl_handle, unsigned int db_handle, int i0, int p0, double *output_list, float rms, int n, int increment) {
  vcl_ofstream ofile("log.txt");

  if (!osl_handle) {
    ofile << "OSL pointer (osl_handle) is zero, exiting!\n";
    return -1;
  }

 #if 0 // version that uses vbl_array_1d<dbru_object_sptr> * in the deliveries before Dec 04, 06
  vbl_array_1d<dbru_object_sptr> *osl = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *>(osl_handle);
#endif
  dbru_osl * osl = reinterpret_cast<dbru_osl *>(osl_handle);

  if (osl->size() <= 0) {
    ofile << "Problems in getting osl\n";
    return -1;
  } else {
    ofile << "OSL loaded, number of objects: " << osl->size() << "\n";
  }

  if (!db_handle) {
    ofile << "OSL pointer (db_handle) is zero, exiting!\n";
    return -1;
  }

  vbl_array_1d<vcl_pair<int, int> > *database_pairs = 
    reinterpret_cast<vbl_array_1d<vcl_pair<int, int> > *>(db_handle);

  vcl_map<vcl_string, int>& category_id_map = osl->get_category_id_map();
#if 0 // version that uses a fixed category_id_map in the deliveries before Dec 04, 06
  category_id_map["minivan"] = 0;
  category_id_map["suv"] = 0;
  category_id_map["utility"] = 0;
  category_id_map["car"] = 1;
  category_id_map["pick-up truck"] = 2;
  category_id_map["van"] = 3;
  category_id_map["hummer"] = 4;
  category_id_map["milcargo truck"] = 5;
#endif

  int database_size = database_pairs->size();
  vbl_array_1d<double> infos(database_size, -1000);

  dbru_object_sptr obj0 = (*osl)[i0];
  if ((obj0->polygons_[p0])->size() <= 0) {
    ofile << "Query object's polygon's size is 0\n";
    return -2;
  }
      
  dbru_label_sptr input_label = obj0->labels_[p0];
  for (int d = 0; d <database_size; d++) {
    int i = (*database_pairs)[d].first;
    int pi = (*database_pairs)[d].second;
    dbru_object_sptr obji = (*osl)[i];
    if (i == i0) {
      ofile << "Not matching against itself\n";
      continue;
    }

    if ((obji->polygons_[pi])->size() <= 0) continue;
    dbru_label_sptr object_label = obji->labels_[pi];
    if (!check_labels(input_label, object_label)) {
      ofile << "label mismatch (motion or view or shadow bin is different) for db object id: " << i << " polygon id " << pi << " skipping this instance\n";
      continue;
    }

    double curve_matching_cost, info_line, info_dt;
    int increment = 50;
    dbru_object_matcher::minfo_thomas_curve_matching(obj0, obji, p0, pi,
                                          curve_matching_cost,
                                          info_line,
                                          info_dt,
                                          increment,      // increment
                                          true,    // output dt info cost
                                          10.0f,   // R
                                          rms,   // for line fitting before matching
                                          false);  // verbose
    infos[d] = info_line;    
  }

  // find best n values
  double total_info = 0;
 
  for (unsigned kkk = 0; kkk < category_id_map.size(); kkk++)
    output_list[kkk] = 0;
  
  int upper = n<database_size?n:database_size;
  for (int k = 0; k<upper; k++) {
    double max = infos[0];
    int max_d = 0;
    for (int d = 1; d <database_size; d++) {
      if (infos[d] > max) {
        max = infos[d];
        max_d = d;
      }
    }

    if (max == -1000) {
      ofile << "None of the items in db were comparable to the query\n";
      break;
    }

    int i = (*database_pairs)[max_d].first;
    int pi = (*database_pairs)[max_d].second;
    dbru_object_sptr obji = (*osl)[i];
    ofile << "match " << k << " info: " << infos[max_d] << " category: " << obji->category_ << "\n";
    total_info += max;
    output_list[category_id_map[obji->category_]] += max;
    infos[max_d] = -990;
  }

  if (total_info != 0) {
    for (unsigned kkk = 0; kkk < category_id_map.size(); kkk++)
      output_list[kkk] /= total_info;
  } else {
    for (unsigned kkk = 0; kkk < category_id_map.size(); kkk++)
      output_list[kkk] = 0;
  }
  
  return 0;
}

int run_object_opt(unsigned int osl_handle, unsigned int db_handle, int i0, int p0, double *output_list, int n, float ratio, float dx, float dr, float ds)
{
vcl_ofstream ofile("log.txt");

  if (!osl_handle) {
    ofile << "OSL pointer (osl_handle) is zero, exiting!\n";
    return -1;
  }

#if 0 // version that uses vbl_array_1d<dbru_object_sptr> * in the deliveries before Dec 04, 06
  vbl_array_1d<dbru_object_sptr> *osl = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *>(osl_handle);
#endif
  dbru_osl * osl = reinterpret_cast<dbru_osl *>(osl_handle);

  if (osl->size() <= 0) {
    ofile << "Problems in getting osl\n";
    return -1;
  } else {
    ofile << "OSL loaded, number of objects: " << osl->size() << "\n";
  }

  if (!db_handle) {
    ofile << "OSL pointer (db_handle) is zero, exiting!\n";
    return -1;
  }

  vbl_array_1d<vcl_pair<int, int> > *database_pairs = 
    reinterpret_cast<vbl_array_1d<vcl_pair<int, int> > *>(db_handle);

  vcl_map<vcl_string, int>& category_id_map = osl->get_category_id_map();
#if 0 // version that uses a fixed category_id_map in the deliveries before Dec 04, 06
  category_id_map["minivan"] = 0;
  category_id_map["suv"] = 0;
  category_id_map["utility"] = 0;
  category_id_map["car"] = 1;
  category_id_map["pick-up truck"] = 2;
  category_id_map["van"] = 3;
  category_id_map["hummer"] = 4;
  category_id_map["milcargo truck"] = 5;
#endif

  int database_size = database_pairs->size();
  vbl_array_1d<double> infos(database_size, -1000);

  dbru_object_sptr obj0 = (*osl)[i0];
  if ((obj0->polygons_[p0])->size() <= 0) {
    ofile << "Query object's polygon's size is 0\n";
    return -2;
  }
      
  dbru_label_sptr input_label = obj0->labels_[p0];
  dbinfo_observation_sptr obs0 = obj0->get_observation(p0);
  if (!obs0) {
    ofile << "Query object's observation does not exist\n";
    return -2;
  }

  for (int d = 0; d <database_size; d++) {
    int i = (*database_pairs)[d].first;
    int pi = (*database_pairs)[d].second;
    dbru_object_sptr obji = (*osl)[i];
    if (i == i0) {
      ofile << "Not matching against itself\n";
      continue;
    }

    if ((obji->polygons_[pi])->size() <= 0) continue;
    dbru_label_sptr object_label = obji->labels_[pi];
    if (!check_labels(input_label, object_label)) {
      ofile << "label mismatch (motion or view or shadow bin is different) for db object id: " << i << " polygon id " << pi << " skipping this instance\n";
      continue;
    }

    dbinfo_observation_sptr obsi = obji->get_observation(pi);
    
    float info = dbru_object_matcher::minfo_rigid_alignment_search(obs0, obsi, dx, dr, ds, ratio);
    infos[d] = info;    
  }

  // find best n values
  double total_info = 0;
 
  for (unsigned kkk = 0; kkk < category_id_map.size(); kkk++)
    output_list[kkk] = 0;
  
  int upper = n<database_size?n:database_size;
  for (int k = 0; k<upper; k++) {
    double max = infos[0];
    int max_d = 0;
    for (int d = 1; d <database_size; d++) {
      if (infos[d] > max) {
        max = infos[d];
        max_d = d;
      }
    }

    if (max == -1000) {
      ofile << "None of the items in db were comparable to the query\n";
      break;
    }

    int i = (*database_pairs)[max_d].first;
    int pi = (*database_pairs)[max_d].second;
    dbru_object_sptr obji = (*osl)[i];
    ofile << "match " << k << " info: " << infos[max_d] << " category: " << obji->category_ << "\n";
    total_info += max;
    output_list[category_id_map[obji->category_]] += max;
    infos[max_d] = -990;
  }

  if (total_info != 0) {
    for (unsigned kkk = 0; kkk < category_id_map.size(); kkk++)
      output_list[kkk] /= total_info;
  } else {
    for (unsigned kkk = 0; kkk < category_id_map.size(); kkk++)
      output_list[kkk] = 0;
  }
  
  return 0;
}
int run_observation_dt(unsigned int osl_handle, unsigned int db_handle, unsigned int observationhandle, unsigned int polygonhandle, unsigned int labelhandle, double *output_list, float rms, int verbose) {
  vul_timer t2;

  if (!osl_handle) {
    vcl_cout << "OSL pointer (osl_handle) is zero, exiting!\n";
    return -1;
  }
  if (!db_handle) {
    vcl_cout << "Database pointer (db_handle) is zero, exiting!\n";
    return -1;
  }
  if (!observationhandle) {
    vcl_cout << "Observation pointer (observationhandle) is zero, exiting!\n";
    return -1;
  }

#if 0 // version that uses vbl_array_1d<dbru_object_sptr> * in the deliveries before Dec 04, 06
  vbl_array_1d<dbru_object_sptr> *osl = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *>(osl_handle);
#endif
  dbru_osl * osl = reinterpret_cast<dbru_osl *>(osl_handle);

  if (osl->size() <= 0) {
    vcl_cout << "Problems in getting osl\n";
    return -1;
  } else if (verbose) {
    vcl_cout << "OSL loaded, number of objects: " << osl->size() << "\n";
  }

  vbl_array_1d<vcl_pair<int, int> > *database_pairs = 
    reinterpret_cast<vbl_array_1d<vcl_pair<int, int> > *>(db_handle);

  vcl_map<vcl_string, int>& category_id_map = osl->get_category_id_map();
#if 0 // version that uses a fixed category_id_map in the deliveries before Dec 04, 06
  category_id_map["minivan"] = 0;
  category_id_map["suv"] = 0;
  category_id_map["utility"] = 0;
  category_id_map["car"] = 1;
  category_id_map["pick-up truck"] = 2;
  category_id_map["van"] = 3;
  category_id_map["hummer"] = 4;
  category_id_map["milcargo truck"] = 5;
#endif

  int database_size = database_pairs->size();
  vbl_array_1d<double> infos(database_size, -1000);

  dbinfo_observation * obs0 = reinterpret_cast<dbinfo_observation *>(observationhandle);

  if ((obs0->geometry()->poly(0))->size() <= 0) {
    vcl_cout << "Query object's polygon's size is 0\n";
    return -2;
  }
   
  //: we have no control to match against itself
  vcl_vector<double> max_info_class(category_id_map.size(), 0);
  float max_info = 0;
  int max_osl_id = (*database_pairs)[0].first;
  int max_osl_pid = (*database_pairs)[0].second;
  for (int d = 0; d <database_size; d++) {
    int i = (*database_pairs)[d].first;
    int pi = (*database_pairs)[d].second;
    dbru_object_sptr obji = (*osl)[i];
   
    if ((obji->polygons_[pi])->size() <= 0) continue;
    //dbru_label_sptr object_label = obji->labels_[pi];
    
    //: CAUTION: for now, we're not checking label matches, since input label
    //  is not created properly yet.
    //if (!check_labels(input_label, object_label)) {
    //  ofile << "label mismatch (motion or view or shadow bin is different) for db object id: " << i << " polygon id " << pi << " skipping this instance\n";
    //  continue;
    //}
    dbinfo_observation_sptr obsi = obji->get_observation(pi);
    
    vul_timer t;
    double curve_matching_cost;
                                                                                                //R         //restricted_cvmatch_ratio //verbose = false
    dbcvr_cv_cor_sptr sil_cor = dbru_object_matcher::compute_curve_alignment(obs0, obsi, curve_matching_cost, 10.0f, rms, 0.25f, false);
    dbru_rcor_sptr rcor = dbru_object_matcher::generate_rcor_curve_matching_dt(obs0, obsi, sil_cor, false);
    
    vcl_vector <vcl_pair< unsigned, unsigned > >& corrs = rcor->get_correspondences();
    float info = dbinfo_observation_matcher::minfo(obs0, obsi, corrs, false);

    //vcl_cout << "matching t: "<< (t.real()/1000.0f) << " seconds, info: " << info <<vcl_endl;
    infos[d] = info;    
    double dummy = vcl_pow(double(2.0f), double(info));  
    if (dummy > max_info_class[category_id_map[obji->category_]])
      max_info_class[category_id_map[obji->category_]] = dummy;
    if (info > max_info) {
      max_info = info;
      max_osl_id = i;
      max_osl_pid = pi;
    }
  }

  if (verbose)
    vcl_cout << "Nearest neighbor from osl id: " << max_osl_id << ", poly id: " << max_osl_pid << " category: " << (*osl)[max_osl_id]->category_ << vcl_endl;
  // find the likelihood for each class
  // probability(class|query) = sum_{i=1}^{all class}( 2^{info(query, class_{i})} ) / 
  //                           (# of class prototypes * sum_{j=1}^{all database} ( 2^{info(query, database_{j}) ))
  // n argument is not used anymore
  //int upper = n<database_size?n:database_size;
  double total_info = 0;
  for (unsigned class_id = 0; class_id < category_id_map.size(); class_id++)
    total_info+=max_info_class[class_id];

  for (unsigned class_id = 0; class_id < category_id_map.size(); class_id++)
    if (max_info_class[class_id] > 0.0f) 
      output_list[class_id] = max_info_class[class_id]/total_info;
    else
      output_list[class_id] = 0.0f;

  if (verbose) {
    for (vcl_map<vcl_string, int>::iterator iter = category_id_map.begin(); iter != category_id_map.end(); iter++) {
      vcl_cout << "max " << iter->first << " info: " << max_info_class[iter->second] << " mutual info score: " << output_list[iter->second] << vcl_endl;
    }
    vcl_cout << "overall t2: "<< (t2.real()/1000.0f) << " seconds.\n<------------------------------------------------>" <<vcl_endl;
  }
  
  return 0;
}
//: observation, polygon and label constitutes the feature list extracted from Frame i by Measurement and FEX module
//  output_list contains the raw mutual info score for each category
//  if verbose = 0 --> no text printout to standard output
//  else           --> printout status reports
int run_observation_dt2(unsigned int osl_handle, unsigned int db_handle, unsigned int observationhandle, unsigned int polygonhandle, unsigned int labelhandle, double *output_list, float rms, int verbose) {
  vul_timer t2;

  if (!osl_handle) {
    vcl_cout << "OSL pointer (osl_handle) is zero, exiting!\n";
    return -1;
  }
  if (!db_handle) {
    vcl_cout << "Database pointer (db_handle) is zero, exiting!\n";
    return -1;
  }
  if (!observationhandle) {
    vcl_cout << "Observation pointer (observationhandle) is zero, exiting!\n";
    return -1;
  }

#if 0 // version that uses vbl_array_1d<dbru_object_sptr> * in the deliveries before Dec 04, 06
  vbl_array_1d<dbru_object_sptr> *osl = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *>(osl_handle);
#endif
  dbru_osl * osl = reinterpret_cast<dbru_osl *>(osl_handle);

  if (osl->size() <= 0) {
    vcl_cout << "Problems in getting osl\n";
    return -1;
  } else if (verbose) {
    vcl_cout << "OSL loaded, number of objects: " << osl->size() << "\n";
  }

  vbl_array_1d<vcl_pair<int, int> > *database_pairs = 
    reinterpret_cast<vbl_array_1d<vcl_pair<int, int> > *>(db_handle);

  vcl_map<vcl_string, int>& category_id_map = osl->get_category_id_map();
#if 0 // version that uses a fixed category_id_map in the deliveries before Dec 04, 06
  category_id_map["minivan"] = 0;
  category_id_map["suv"] = 0;
  category_id_map["utility"] = 0;
  category_id_map["car"] = 1;
  category_id_map["pick-up truck"] = 2;
  category_id_map["van"] = 3;
  category_id_map["hummer"] = 4;
  category_id_map["milcargo truck"] = 5;
#endif

  int database_size = database_pairs->size();
  vbl_array_1d<double> infos(database_size, -1000);

  //dbinfo_observation * obs00 = reinterpret_cast<dbinfo_observation *>(observationhandle);
  //dbinfo_observation_sptr obs0 = obs00;
  dbinfo_observation * obs0 = reinterpret_cast<dbinfo_observation *>(observationhandle);

  if ((obs0->geometry()->poly(0))->size() <= 0) {
    vcl_cout << "Query object's polygon's size is 0\n";
    return -2;
  }
   
  //: we have no control to match against itself
  vcl_vector<double> max_info_class(category_id_map.size(), 0);
  float max_info = 0;
  int max_osl_id = (*database_pairs)[0].first;
  int max_osl_pid = (*database_pairs)[0].second;
  for (int d = 0; d <database_size; d++) {
    int i = (*database_pairs)[d].first;
    int pi = (*database_pairs)[d].second;
    dbru_object_sptr obji = (*osl)[i];
   
    if ((obji->polygons_[pi])->size() <= 0) continue;
    //dbru_label_sptr object_label = obji->labels_[pi];
    
    //: CAUTION: for now, we're not checking label matches, since input label
    //  is not created properly yet.
    //if (!check_labels(input_label, object_label)) {
    //  ofile << "label mismatch (motion or view or shadow bin is different) for db object id: " << i << " polygon id " << pi << " skipping this instance\n";
    //  continue;
    //}
    dbinfo_observation_sptr obsi = obji->get_observation(pi);
    
    vul_timer t;
    double curve_matching_cost;
                                                                                                //R         //restricted_cvmatch_ratio //verbose = false
    dbcvr_cv_cor_sptr sil_cor = dbru_object_matcher::compute_curve_alignment(obs0, obsi, curve_matching_cost, 10.0f, rms, 0.25f, false);
    dbru_rcor_sptr rcor = dbru_object_matcher::generate_rcor_curve_matching_dt(obs0, obsi, sil_cor, false);
    
    vcl_vector <vcl_pair< unsigned, unsigned > >& corrs = rcor->get_correspondences();
    float info = dbinfo_observation_matcher::minfo(obs0, obsi, corrs, false);

    //vcl_cout << "matching t: "<< (t.real()/1000.0f) << " seconds, info: " << info <<vcl_endl;
    infos[d] = info;    
    double dummy = vcl_pow(double(2.0f), double(info));  
    if (dummy > max_info_class[category_id_map[obji->category_]])
      max_info_class[category_id_map[obji->category_]] = dummy;
    if (info > max_info) {
      max_info = info;
      max_osl_id = i;
      max_osl_pid = pi;
    }
  }

  vcl_cout << "Nearest neighbor from osl id: " << max_osl_id << ", poly id: " << max_osl_pid << " category: " << (*osl)[max_osl_id]->category_ << vcl_endl;
  // find the likelihood for each class
  // probability(class|query) = sum_{i=1}^{all class}( 2^{info(query, class_{i})} ) / 
  //                           (# of class prototypes * sum_{j=1}^{all database} ( 2^{info(query, database_{j}) ))
  // n argument is not used anymore
  //int upper = n<database_size?n:database_size;
  for (unsigned class_id = 0; class_id<category_id_map.size(); class_id++)
    output_list[class_id] = max_info_class[class_id];
    
  if (verbose) {
    for (vcl_map<vcl_string, int>::iterator iter = category_id_map.begin(); iter != category_id_map.end(); iter++) {
      vcl_cout << "max " << iter->first << " info: " << max_info_class[iter->second] << " mutual info score: " << output_list[iter->second] << vcl_endl;
    }
    vcl_cout << "overall t2: "<< (t2.real()/1000.0f) << " seconds.\n<------------------------------------------------>" <<vcl_endl;
  }

  return 0;
}

int run_observation_line(unsigned int osl_handle, unsigned int db_handle, unsigned int observationhandle, unsigned int polygonhandle, unsigned int labelhandle, double *output_list, float rms, int increment, int verbose) {
  vul_timer t2;

  if (!osl_handle) {
    vcl_cout << "OSL pointer (osl_handle) is zero, exiting!\n";
    return -1;
  }
  if (!db_handle) {
    vcl_cout << "Database pointer (db_handle) is zero, exiting!\n";
    return -1;
  }
  if (!observationhandle) {
    vcl_cout << "Observation pointer (observationhandle) is zero, exiting!\n";
    return -1;
  }

#if 0 // version that uses vbl_array_1d<dbru_object_sptr> * in the deliveries before Dec 04, 06
  vbl_array_1d<dbru_object_sptr> *osl = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *>(osl_handle);
#endif
  dbru_osl * osl = reinterpret_cast<dbru_osl *>(osl_handle);

  if (osl->size() <= 0) {
    vcl_cout << "Problems in getting osl\n";
    return -1;
  } else if (verbose) {
    vcl_cout << "OSL loaded, number of objects: " << osl->size() << "\n";
  }

  vbl_array_1d<vcl_pair<int, int> > *database_pairs = 
    reinterpret_cast<vbl_array_1d<vcl_pair<int, int> > *>(db_handle);

  vcl_map<vcl_string, int>& category_id_map = osl->get_category_id_map();
#if 0 // version that uses a fixed category_id_map in the deliveries before Dec 04, 06
  category_id_map["minivan"] = 0;
  category_id_map["suv"] = 0;
  category_id_map["utility"] = 0;
  category_id_map["car"] = 1;
  category_id_map["pick-up truck"] = 2;
  category_id_map["van"] = 3;
  category_id_map["hummer"] = 4;
  category_id_map["milcargo truck"] = 5;
#endif

  int database_size = database_pairs->size();
  vbl_array_1d<double> infos(database_size, -1000);

  dbinfo_observation * obs0 = reinterpret_cast<dbinfo_observation *>(observationhandle);

  if ((obs0->geometry()->poly(0))->size() <= 0) {
    vcl_cout << "Query object's polygon's size is 0\n";
    return -2;
  }
   
  //: we have no control to match against itself
  // find nearest neighbor from each class
  vcl_vector<double> max_info_class(category_id_map.size(), 0);
  float max_info = 0;
  int max_osl_id = (*database_pairs)[0].first;
  int max_osl_pid = (*database_pairs)[0].second;
  for (int d = 0; d <database_size; d++) {
    int i = (*database_pairs)[d].first;
    int pi = (*database_pairs)[d].second;
    dbru_object_sptr obji = (*osl)[i];
   
    if ((obji->polygons_[pi])->size() <= 0) continue;
    //dbru_label_sptr object_label = obji->labels_[pi];
    
    //: CAUTION: for now, we're not checking label matches, since input label
    //  is not created properly yet.
    //if (!check_labels(input_label, object_label)) {
    //  ofile << "label mismatch (motion or view or shadow bin is different) for db object id: " << i << " polygon id " << pi << " skipping this instance\n";
    //  continue;
    //}
    dbinfo_observation_sptr obsi = obji->get_observation(pi);
    
    vul_timer t;
    double curve_matching_cost;
                                                                                                //R         //restricted_cvmatch_ratio //verbose = false
    dbcvr_cv_cor_sptr sil_cor = dbru_object_matcher::compute_curve_alignment(obs0, obsi, curve_matching_cost, 10.0f, rms, 0.25f, false);
    dbru_rcor_sptr rcor = dbru_object_matcher::generate_rcor_curve_matching_line(obs0, obsi, sil_cor, increment, false, false);
    
    vcl_vector <vcl_pair< unsigned, unsigned > >& corrs = rcor->get_correspondences();
    float info = dbinfo_observation_matcher::minfo(obs0, obsi, corrs, false);

    //vcl_cout << "match t: "<< (t.real()/1000.0f) << " seconds " <<vcl_endl;
    infos[d] = info;    
    double dummy = vcl_pow(double(2.0f), double(info));  
    if (dummy > max_info_class[category_id_map[obji->category_]])
      max_info_class[category_id_map[obji->category_]] = dummy;
    if (info > max_info) {
      max_info = info;
      max_osl_id = i;
      max_osl_pid = pi;
    }
  }

  //vcl_cout << "Nearest neighbor from osl id: " << max_osl_id << ", poly id: " << max_osl_pid << " category: " << (*osl)[max_osl_id]->category_ << vcl_endl;
  // find the likelihood for each class
  // probability(class|query) = sum_{i=1}^{all class}( 2^{info(query, class_{i})} ) / 
  //                           (# of class prototypes * sum_{j=1}^{all database} ( 2^{info(query, database_{j}) ))
  // n argument is not used anymore
  //int upper = n<database_size?n:database_size;
  double total_info = 0;
  for (unsigned class_id = 0; class_id < category_id_map.size(); class_id++)
    total_info+=max_info_class[class_id];

  for (unsigned class_id = 0; class_id < category_id_map.size(); class_id++)
    if (max_info_class[class_id] > 0.0f) 
      output_list[class_id] = max_info_class[class_id]/total_info;
    else
      output_list[class_id] = 0.0f;

  if (verbose) {
    for (vcl_map<vcl_string, int>::iterator iter = category_id_map.begin(); iter != category_id_map.end(); iter++) {
      vcl_cout << "max " << iter->first << " info: " << max_info_class[iter->second] << " mutual info score: " << output_list[iter->second] << vcl_endl;
    }
    vcl_cout << "overall t2: "<< (t2.real()/1000.0f) << " seconds.\n<------------------------------------------------>" <<vcl_endl;
  }
  
  return 0;
}

//: observation, polygon and label constitutes the feature list extracted from Frame i by Measurement and FEX module
//  output_list contains the raw mutual info score for each category
//  if verbose = 0 --> no text printout to standard output
//  else           --> printout status reports
int run_observation_line2(unsigned int osl_handle, unsigned int db_handle, unsigned int observationhandle, unsigned int polygonhandle, unsigned int labelhandle, double *output_list, float rms, int increment, int verbose) {
  vul_timer t2;

  if (!osl_handle) {
    vcl_cout << "OSL pointer (osl_handle) is zero, exiting!\n";
    return -1;
  }
  if (!db_handle) {
    vcl_cout << "Database pointer (db_handle) is zero, exiting!\n";
    return -1;
  }
  if (!observationhandle) {
    vcl_cout << "Observation pointer (observationhandle) is zero, exiting!\n";
    return -1;
  }

#if 0 // version that uses vbl_array_1d<dbru_object_sptr> * in the deliveries before Dec 04, 06
  vbl_array_1d<dbru_object_sptr> *osl = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *>(osl_handle);
#endif
  dbru_osl * osl = reinterpret_cast<dbru_osl *>(osl_handle);

  if (osl->size() <= 0) {
    vcl_cout << "Problems in getting osl\n";
    return -1;
  } else if (verbose) {
    vcl_cout << "OSL loaded, number of objects: " << osl->size() << "\n";
  }

  vbl_array_1d<vcl_pair<int, int> > *database_pairs = 
    reinterpret_cast<vbl_array_1d<vcl_pair<int, int> > *>(db_handle);

  vcl_map<vcl_string, int>& category_id_map = osl->get_category_id_map();
#if 0 // version that uses a fixed category_id_map in the deliveries before Dec 04, 06
  category_id_map["minivan"] = 0;
  category_id_map["suv"] = 0;
  category_id_map["utility"] = 0;
  category_id_map["car"] = 1;
  category_id_map["pick-up truck"] = 2;
  category_id_map["van"] = 3;
  category_id_map["hummer"] = 4;
  category_id_map["milcargo truck"] = 5;
#endif

  int database_size = database_pairs->size();
  vbl_array_1d<double> infos(database_size, -1000);

  dbinfo_observation * obs0 = reinterpret_cast<dbinfo_observation *>(observationhandle);

  if ((obs0->geometry()->poly(0))->size() <= 0) {
    vcl_cout << "Query object's polygon's size is 0\n";
    return -2;
  }
   
  //: we have no control to match against itself
  // find nearest neighbor from each class
  vcl_vector<double> max_info_class(category_id_map.size(), 0);
  float max_info = 0;
  int max_osl_id = (*database_pairs)[0].first;
  int max_osl_pid = (*database_pairs)[0].second;
  for (int d = 0; d <database_size; d++) {
    int i = (*database_pairs)[d].first;
    int pi = (*database_pairs)[d].second;
    dbru_object_sptr obji = (*osl)[i];
   
    if ((obji->polygons_[pi])->size() <= 0) continue;
    //dbru_label_sptr object_label = obji->labels_[pi];
    
    //: CAUTION: for now, we're not checking label matches, since input label
    //  is not created properly yet.
    //if (!check_labels(input_label, object_label)) {
    //  ofile << "label mismatch (motion or view or shadow bin is different) for db object id: " << i << " polygon id " << pi << " skipping this instance\n";
    //  continue;
    //}
    dbinfo_observation_sptr obsi = obji->get_observation(pi);
    
    vul_timer t;
    double curve_matching_cost;
                                                                                                //R         //restricted_cvmatch_ratio //verbose = false
    dbcvr_cv_cor_sptr sil_cor = dbru_object_matcher::compute_curve_alignment(obs0, obsi, curve_matching_cost, 10.0f, rms, 0.25f, false);
    dbru_rcor_sptr rcor = dbru_object_matcher::generate_rcor_curve_matching_line(obs0, obsi, sil_cor, increment, false, false);
    
    vcl_vector <vcl_pair< unsigned, unsigned > >& corrs = rcor->get_correspondences();
    float info = dbinfo_observation_matcher::minfo(obs0, obsi, corrs, false);

    //vcl_cout << "match t: "<< (t.real()/1000.0f) << " seconds " <<vcl_endl;
    infos[d] = info;    
    double dummy = vcl_pow(double(2.0f), double(info));  
    if (dummy > max_info_class[category_id_map[obji->category_]])
      max_info_class[category_id_map[obji->category_]] = dummy;
    if (info > max_info) {
      max_info = info;
      max_osl_id = i;
      max_osl_pid = pi;
    }
  }

  //vcl_cout << "Nearest neighbor from osl id: " << max_osl_id << ", poly id: " << max_osl_pid << " category: " << (*osl)[max_osl_id]->category_ << vcl_endl;
  // find the likelihood for each class
  // probability(class|query) = sum_{i=1}^{all class}( 2^{info(query, class_{i})} ) / 
  //                           (# of class prototypes * sum_{j=1}^{all database} ( 2^{info(query, database_{j}) ))
  // n argument is not used anymore
  //int upper = n<database_size?n:database_size;

  for (unsigned class_id = 0; class_id<category_id_map.size(); class_id++)
    output_list[class_id] = max_info_class[class_id];

  if (verbose) {
    for (vcl_map<vcl_string, int>::iterator iter = category_id_map.begin(); iter != category_id_map.end(); iter++) {
      vcl_cout << "max " << iter->first << " info: " << max_info_class[iter->second] << " mutual info score: " << output_list[iter->second] << vcl_endl;
    }
    vcl_cout << "overall t2: "<< (t2.real()/1000.0f) << " seconds.\n<------------------------------------------------>" <<vcl_endl;
  }
  
  return 0;
}


//: float ratio in [0,1]: the ratio of query region randomly searched, performance gets better as ratio increases, but takes longer to complete a match
//  dx: translation range, dr: rotation range, ds: scale range
int run_observation_opt(unsigned int osl_handle, unsigned int db_handle, unsigned int observationhandle, unsigned int polygonhandle, unsigned int labelhandle, double *output_list, float ratio, float dx, float dr, float ds, int verbose)
{
  vul_timer t2;

  if (!osl_handle) {
    vcl_cout << "OSL pointer (osl_handle) is zero, exiting!\n";
    return -1;
  }
  if (!db_handle) {
    vcl_cout << "Database pointer (db_handle) is zero, exiting!\n";
    return -1;
  }
  if (!observationhandle) {
    vcl_cout << "Observation pointer (observationhandle) is zero, exiting!\n";
    return -1;
  }

#if 0 // version that uses vbl_array_1d<dbru_object_sptr> * in the deliveries before Dec 04, 06
  vbl_array_1d<dbru_object_sptr> *osl = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *>(osl_handle);
#endif
  dbru_osl * osl = reinterpret_cast<dbru_osl *>(osl_handle);

  if (osl->size() <= 0) {
    vcl_cout << "Problems in getting osl\n";
    return -1;
  } else if (verbose) {
    vcl_cout << "OSL loaded, number of objects: " << osl->size() << "\n";
  }

  vbl_array_1d<vcl_pair<int, int> > *database_pairs = 
    reinterpret_cast<vbl_array_1d<vcl_pair<int, int> > *>(db_handle);

  vcl_map<vcl_string, int>& category_id_map = osl->get_category_id_map();
#if 0 // version that uses a fixed category_id_map in the deliveries before Dec 04, 06
  category_id_map["minivan"] = 0;
  category_id_map["suv"] = 0;
  category_id_map["utility"] = 0;
  category_id_map["car"] = 1;
  category_id_map["pick-up truck"] = 2;
  category_id_map["van"] = 3;
  category_id_map["hummer"] = 4;
  category_id_map["milcargo truck"] = 5;
#endif

  int database_size = database_pairs->size();
  vbl_array_1d<double> infos(database_size, -1000);

  dbinfo_observation * obs0 = reinterpret_cast<dbinfo_observation *>(observationhandle);

  if ((obs0->geometry()->poly(0))->size() <= 0) {
    vcl_cout << "Query object's polygon's size is 0\n";
    return -2;
  }
   
  //: we have no control to match against itself
  // find nearest neighbor from each class
  vcl_vector<double> max_info_class(category_id_map.size(), 0);
  float max_info = 0;
  int max_osl_id = (*database_pairs)[0].first;
  int max_osl_pid = (*database_pairs)[0].second;
  for (int d = 0; d <database_size; d++) {
    int i = (*database_pairs)[d].first;
    int pi = (*database_pairs)[d].second;
    dbru_object_sptr obji = (*osl)[i];
   
    if ((obji->polygons_[pi])->size() <= 0) continue;
    //dbru_label_sptr object_label = obji->labels_[pi];
    
    //: CAUTION: for now, we're not checking label matches, since input label
    //  is not created properly yet.
    //if (!check_labels(input_label, object_label)) {
    //  ofile << "label mismatch (motion or view or shadow bin is different) for db object id: " << i << " polygon id " << pi << " skipping this instance\n";
    //  continue;
    //}
    dbinfo_observation_sptr obsi = obji->get_observation(pi);
    
    vul_timer t;
    float info = dbru_object_matcher::minfo_rigid_alignment_search(obs0, obsi, dx, dr, ds, ratio);
    //vcl_cout << "osl id: " << i << " pid: " << pi << " info: " << info << " t: "<< (t.real()/1000.0f) << " seconds " <<vcl_endl;
    infos[d] = info;    
    double dummy = vcl_pow(double(2.0f), double(info));  
    if (dummy > max_info_class[category_id_map[obji->category_]])
      max_info_class[category_id_map[obji->category_]] = dummy;
    if (info > max_info) {
      max_info = info;
      max_osl_id = i;
      max_osl_pid = pi;
    }
  }

  //vcl_cout << "Nearest neighbor from osl id: " << max_osl_id << ", poly id: " << max_osl_pid << " category: " << (*osl)[max_osl_id]->category_ << vcl_endl;
  // find the likelihood for each class
  // probability(class|query) = sum_{i=1}^{all class}( 2^{info(query, class_{i})} ) / 
  //                           (# of class prototypes * sum_{j=1}^{all database} ( 2^{info(query, database_{j}) ))
  // n argument is not used anymore
  //int upper = n<database_size?n:database_size;
  double total_info = 0;
  for (unsigned class_id = 0; class_id < category_id_map.size(); class_id++)
    total_info+=max_info_class[class_id];

  if (total_info > 0) {
    for (unsigned class_id = 0; class_id < category_id_map.size(); class_id++)
      if (max_info_class[class_id] > 0.0f) 
        output_list[class_id] = max_info_class[class_id]/total_info;
      else
        output_list[class_id] = 0.0f;
  }

  if (verbose) {
    for (vcl_map<vcl_string, int>::iterator iter = category_id_map.begin(); iter != category_id_map.end(); iter++) {
      vcl_cout << "max " << iter->first << " info: " << max_info_class[iter->second] << " mutual info score: " << output_list[iter->second] << vcl_endl;
    }
    vcl_cout << "overall t2: "<< (t2.real()/1000.0f) << " seconds.\n<------------------------------------------------>" <<vcl_endl;
  }
  
  return 0;
}


//: float ratio in [0,1]: the ratio of query region randomly searched, performance gets better as ratio increases, but takes longer to complete a match
//  dx: translation range, dr: rotation range, ds: scale range
//  output_list contains the raw mutual info score for each category
//  if verbose = 0 --> no text printout to standard output
//  else           --> printout status reports
int run_observation_opt2(unsigned int osl_handle, unsigned int db_handle, unsigned int observationhandle, unsigned int polygonhandle, unsigned int labelhandle, double *output_list, float ratio, float dx, float dr, float ds, int verbose)
{
  vul_timer t2;

  if (!osl_handle) {
    vcl_cout << "OSL pointer (osl_handle) is zero, exiting!\n";
    return -1;
  }
  if (!db_handle) {
    vcl_cout << "Database pointer (db_handle) is zero, exiting!\n";
    return -1;
  }
  if (!observationhandle) {
    vcl_cout << "Observation pointer (observationhandle) is zero, exiting!\n";
    return -1;
  }

#if 0 // version that uses vbl_array_1d<dbru_object_sptr> * in the deliveries before Dec 04, 06
  vbl_array_1d<dbru_object_sptr> *osl = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *>(osl_handle);
#endif
  dbru_osl * osl = reinterpret_cast<dbru_osl *>(osl_handle);

  if (osl->size() <= 0) {
    vcl_cout << "Problems in getting osl\n";
    return -1;
  } else if (verbose) {
    vcl_cout << "OSL loaded, number of objects: " << osl->size() << "\n";
  }

  vbl_array_1d<vcl_pair<int, int> > *database_pairs = 
    reinterpret_cast<vbl_array_1d<vcl_pair<int, int> > *>(db_handle);

  vcl_map<vcl_string, int>& category_id_map = osl->get_category_id_map();
#if 0 // version that uses a fixed category_id_map in the deliveries before Dec 04, 06
  category_id_map["minivan"] = 0;
  category_id_map["suv"] = 0;
  category_id_map["utility"] = 0;
  category_id_map["car"] = 1;
  category_id_map["pick-up truck"] = 2;
  category_id_map["van"] = 3;
  category_id_map["hummer"] = 4;
  category_id_map["milcargo truck"] = 5;
#endif

  int database_size = database_pairs->size();
  vbl_array_1d<double> infos(database_size, -1000);

  dbinfo_observation * obs0 = reinterpret_cast<dbinfo_observation *>(observationhandle);

  if ((obs0->geometry()->poly(0))->size() <= 0) {
    vcl_cout << "Query object's polygon's size is 0\n";
    return -2;
  }
   
  //: we have no control to match against itself
  // find nearest neighbor from each class
  vcl_vector<double> max_info_class(category_id_map.size(), 0);
  float max_info = 0;
  int max_osl_id = (*database_pairs)[0].first;
  int max_osl_pid = (*database_pairs)[0].second;
  for (int d = 0; d <database_size; d++) {
    int i = (*database_pairs)[d].first;
    int pi = (*database_pairs)[d].second;
    dbru_object_sptr obji = (*osl)[i];
   
    if ((obji->polygons_[pi])->size() <= 0) continue;
    //dbru_label_sptr object_label = obji->labels_[pi];
    
    //: CAUTION: for now, we're not checking label matches, since input label
    //  is not created properly yet.
    //if (!check_labels(input_label, object_label)) {
    //  ofile << "label mismatch (motion or view or shadow bin is different) for db object id: " << i << " polygon id " << pi << " skipping this instance\n";
    //  continue;
    //}
    dbinfo_observation_sptr obsi = obji->get_observation(pi);
    
    vul_timer t;
    float info = dbru_object_matcher::minfo_rigid_alignment_search(obs0, obsi, dx, dr, ds, ratio);
    //vcl_cout << "osl id: " << i << " pid: " << pi << " info: " << info << " t: "<< (t.real()/1000.0f) << " seconds " <<vcl_endl;
    infos[d] = info;    
    double dummy = vcl_pow(double(2.0f), double(info));  
    if (dummy > max_info_class[category_id_map[obji->category_]])
      max_info_class[category_id_map[obji->category_]] = dummy;
    if (info > max_info) {
      max_info = info;
      max_osl_id = i;
      max_osl_pid = pi;
    }
  }

  //vcl_cout << "Nearest neighbor from osl id: " << max_osl_id << ", poly id: " << max_osl_pid << " category: " << (*osl)[max_osl_id]->category_ << vcl_endl;
  // find the likelihood for each class
  // probability(class|query) = sum_{i=1}^{all class}( 2^{info(query, class_{i})} ) / 
  //                           (# of class prototypes * sum_{j=1}^{all database} ( 2^{info(query, database_{j}) ))
  // n argument is not used anymore
  //int upper = n<database_size?n:database_size;

  for (unsigned class_id = 0; class_id < category_id_map.size(); class_id++) 
   output_list[class_id] = max_info_class[class_id];

  if (verbose) {
    for (vcl_map<vcl_string, int>::iterator iter = category_id_map.begin(); iter != category_id_map.end(); iter++) {
      vcl_cout << "max " << iter->first << " info: " << max_info_class[iter->second] << " mutual info score: " << output_list[iter->second] << vcl_endl;
    }
    vcl_cout << "overall t2: "<< (t2.real()/1000.0f) << " seconds.\n<------------------------------------------------>" <<vcl_endl;
  }
  
  return 0;
}
