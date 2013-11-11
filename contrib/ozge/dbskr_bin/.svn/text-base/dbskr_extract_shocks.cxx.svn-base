// create shock patches from a given image and sampled coarse shock graph

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/dbskr_utilities.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_plane.h>
#include <brip/brip_vil_float_ops.h>

#include <vul/vul_timer.h>
#include <vul/vul_file.h>

#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation.h>
//#include <dbinfo/dbinfo_observation_matcher.h>

void shock_usage() {
  vcl_cout << "Shock from Image Usage: <program name> <image_file> <output_file> <edge_detection_sigma> <pruning color threshold> <avg grad mag thres to keep the curve fragments>\n";
  vcl_cout << "edge detection sigma is usually 1\n";
  vcl_cout << "pruning color threshold is the threshold for LAB space color contrast, typically 0.4, used to prune out curve fragments\n";
}
int main_shock(int argc, char *argv[]) {
  vcl_cout << "extracting shock graph!\n";

  //: out file contains the wrong mathces if any
  vcl_string image_file, output_file;

  vcl_cout << "argc: " << argc << vcl_endl;
  
  if (argc != 6) {
    shock_usage();  
    return -1;
  }
  image_file = argv[1];
  output_file = argv[2];
  double edge_detection_sigma = atof(argv[3]);
  double pruning_color_threshold = atof(argv[4]);
  double avg_grad_mag_thres = atof(argv[5]);  //5.0;

  //: load the image
  vil_image_resource_sptr img_sptr = vil_load_image_resource(image_file.c_str());
  
  double length_thresh = 1.0; // prune out the polylines that are shorter in length than this threshold before shock computation, typically 2 pixels
  unsigned min_size_to_keep = 1;
  double pruning_region_width = 5.0; // the width of the region to consider on both sides of a polyline to determine the color contrast, typically 5 pixels
  double rms = 0.05; // root mean square error used in line fitting process before shock computation, typically 0.05

  //double app_t_low = 0.2, app_t_high = 0.6, cont_t_low = 0.3, cont_t_high = 0.9;
  double cont_t = 0.9, app_t = 0.6;
  
  unsigned smoothing_nsteps = 5;
  //dbsk2d_shock_graph_sptr sg = extract_shock(img_sptr, length_thresh, pruning_region_width, 
  //                                           pruning_color_threshold, rms, smoothing_nsteps,
  //                                           app_t_low, app_t_high, cont_t_low, cont_t_high, true, output_file, edge_detection_sigma);
  dbsk2d_shock_graph_sptr sg = extract_shock(img_sptr, min_size_to_keep, avg_grad_mag_thres, length_thresh, 
                                             pruning_region_width, pruning_color_threshold, 
                                             rms, smoothing_nsteps,
                                             cont_t, app_t, true, output_file, edge_detection_sigma);

  return 0;
}

void shock_from_mask_usage() {
   vcl_cout << "Shock From Mask Usage: <program name> <image_file> <output_file> <number of smoothing steps for discrete smoothing> \n";
}

int main_from_mask(int argc, char *argv[]) {
  vcl_cout << "extracting shock graph from a mask!\n";

  //: out file contains the wrong mathces if any
  vcl_string image_file, output_file;

  vcl_cout << "argc: " << argc << vcl_endl;
  
  if (argc != 4) {
    shock_from_mask_usage();  
    return -1;
  }
  image_file = argv[1];
  output_file = argv[2];
  int n_steps = atoi(argv[3]);

  //: load the image
  vil_image_resource_sptr img_sptr = vil_load_image_resource(image_file.c_str());
  
  dbsk2d_shock_graph_sptr sg = extract_shock_from_mask(img_sptr, true, output_file, n_steps, 0.05);
  return 0;
}

void eth_usage() {
  vcl_cout << "Shock from ETH Usage: <program name> <image_dir> <output_dir> <view string> <edge_detection_sigma> <pruning_color_threshold>\n";
  vcl_cout << "view string is e.g. 090-180\n";
  vcl_cout << "edge detection sigma is usually 1\n";
  vcl_cout << "pruning color threshold is the threshold for LAB space color contrast, typically 0.4, used to prune out curve fragments\n";
}
int main_eth(int argc, char *argv[]) {
  vcl_cout << "extracting shock graphs from eth set!\n";

  //: out file contains the wrong mathces if any
  vcl_string image_dir, output_dir, view_str;

  vcl_cout << "argc: " << argc << vcl_endl;
  
  if (argc != 6) {
    eth_usage();
    return -1;
  }
  image_dir = argv[1];
  output_dir = argv[2];
  view_str = argv[3];
  double edge_detection_sigma = atof(argv[4]);
  double pruning_color_threshold = atof(argv[5]);

  vcl_vector<vcl_string> cats;
  //cats.push_back("cow");
  //cats.push_back("horse");
  //cats.push_back("dog");
  cats.push_back("car");
  cats.push_back("apple");
  cats.push_back("pear");
  cats.push_back("cup");
  cats.push_back("tomato");

  double length_thresh = 1.0; // prune out the polylines that are shorter in length than this threshold before shock computation, typically 2 pixels
  double pruning_region_width = 5.0; // the width of the region to consider on both sides of a polyline to determine the color contrast, typically 5 pixels
  double rms = 0.05; // root mean square error used in line fitting process before shock computation, typically 0.05

  double app_t_low = 0.1, app_t_high = 0.5, cont_t_low = 0.1, cont_t_high = 0.5;
  
  unsigned smoothing_nsteps = 10;

  //: load the images and extract
  for (unsigned i = 0; i < cats.size(); i++) {
    vcl_string out_dir = output_dir+cats[i]+"/";
    vul_file::make_directory_path(out_dir);
    vcl_cout << cats[i] << "\n";
    for (unsigned j = 1; j < 11; j++) {
      vcl_ostringstream oss;
      oss << j;
      vcl_string ins_name = cats[i]+oss.str()+"-"+view_str;
      vcl_cout << "\t\t" << ins_name << vcl_endl;
      vil_image_resource_sptr img_sptr = vil_load_image_resource((image_dir+cats[i]+oss.str()+"/"+ins_name+".png").c_str());
      if (!img_sptr) {
        vcl_cout << "Problems in loading the image: " << (image_dir+ins_name+".png");
        continue;
      }
      /*
      dbsk2d_shock_graph_sptr sg = extract_shock(img_sptr, length_thresh, pruning_region_width, 
                                             pruning_color_threshold, rms, smoothing_nsteps,
                                             app_t_low, app_t_high, cont_t_low, cont_t_high, true, out_dir+ins_name, edge_detection_sigma);      
      */

      //extract shock from mask
      vil_image_resource_sptr img_mask_sptr = vil_load_image_resource((image_dir+cats[i]+oss.str()+"/maps/"+ins_name+"-map.png").c_str());
      dbsk2d_shock_graph_sptr sg = extract_shock_from_mask(img_mask_sptr, true, out_dir+ins_name+"-map", smoothing_nsteps, 0.05);  
    }
  }
  return 0;
}

void opelt_train_usage() {
  vcl_cout << "Shock from Opelt Usage: <program name> <image_dir z:/images/.../ > <output_dir> <image list> <edge_detection_sigma> <color pruning threshold> <avg grad mag threshold (typically 5.0)> <class initial e.g. Horse_horse>\n";
  vcl_cout << "edge detection sigma is usually 1\n";
//  vcl_cout << "pruning color threshold is the threshold for LAB space color contrast, typically 0.4, used to prune out curve fragments\n";
}
int main_opelt_train(int argc, char *argv[]) {
  vcl_cout << "extracting shock graphs from opelt train set!\n";

  //: out file contains the wrong mathces if any
  vcl_string image_dir, output_dir, train_images_list, class_initial;

  vcl_cout << "argc: " << argc << vcl_endl;
  
  if (argc != 8) {
    opelt_train_usage();
    return -1;
  }

  image_dir = argv[1];
  output_dir = argv[2];
  train_images_list = argv[3];
  double edge_detection_sigma = atof(argv[4]);
  double pruning_color_threshold = atof(argv[5]);
  double avg_grad_mag_thres = atof(argv[6]);
  class_initial = argv[7];

  vcl_vector<vcl_string> image_names;
  vcl_ifstream fp(train_images_list.c_str());
  if (!fp) {
    vcl_cout<<" Unable to Open "<< train_images_list <<vcl_endl;
    return 0;
  }

  while (!fp.eof()) {
    vcl_string name;
    fp >> name;
    if (name.size() > 2) {
      //image_names.push_back(name);
      //vcl_cout << " name: " << name << vcl_endl;
      int nbr = atoi(name.c_str());
      char buffer[1000];
      sprintf(buffer, "%d", nbr);
      vcl_string nbr_str = buffer;
      vcl_cout << nbr << " " << nbr_str << vcl_endl;
      image_names.push_back(nbr_str);
    }
  }
  fp.close();
  vcl_cout << "training set: \n";
  for (unsigned i = 0; i < image_names.size(); i++) {
    vcl_string::size_type pos = image_names[i].find_last_of(class_initial.c_str());
    vcl_string::size_type pos2 = image_names[i].find_last_of(".");
    vcl_string name = image_names[i].substr(pos+1, pos2);
    
    //vcl_string name = image_names[i].substr(class_initial.length(), image_names[i].length()-4);
    image_names[i] = name;   // actually id
    vcl_cout << image_names[i] << "\n";
  }
  
  double length_thresh = 1.0; // prune out the polylines that are shorter in length than this threshold before shock computation, typically 2 pixels
  unsigned min_size_to_keep = 1;
//  double avg_grad_mag_thres = 5.0;
  double pruning_region_width = 5.0; // the width of the region to consider on both sides of a polyline to determine the color contrast, typically 5 pixels
  double rms = 0.05; // root mean square error used in line fitting process before shock computation, typically 0.05

  double app_t = 0.1, cont_t = 0.5;
  
  unsigned smoothing_nsteps = 2;

  //: load the images and extract
  for (unsigned i = 0; i < image_names.size(); i++) {
    vcl_string ins_name = output_dir+class_initial+image_names[i];
    vcl_cout << "\t\t" << ins_name << vcl_endl;
      //vil_image_resource_sptr img_sptr = vil_load_image_resource((image_dir+"bg_graz_"+image_names[i]+".png").c_str());
      vil_image_resource_sptr img_sptr = vil_load_image_resource((image_dir+"image-"+image_names[i]+".png").c_str());
      if (!img_sptr) {
        vcl_cout << "Problems in loading the image: " << (image_dir+"image-"+image_names[i]+".png");
        continue;
      }
      //dbsk2d_shock_graph_sptr sg = extract_shock(img_sptr, length_thresh, pruning_region_width, 
      //                                       pruning_color_threshold, rms, smoothing_nsteps,
      //                                       app_t_low, app_t_high, cont_t_low, cont_t_high, true, ins_name, edge_detection_sigma);
      
      dbsk2d_shock_graph_sptr sg = extract_shock(img_sptr, min_size_to_keep, avg_grad_mag_thres, length_thresh, pruning_region_width, pruning_color_threshold, rms, smoothing_nsteps,
                                             cont_t, app_t, true, ins_name, edge_detection_sigma);
      sg = 0; // delete all its sources
  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc == 8) 
    main_opelt_train(argc, argv);
  //else if (argc == 6)
  //  main_eth(argc, argv);
  else if (argc == 6)
    main_shock(argc, argv);
  else if (argc == 4)
    main_from_mask(argc, argv);
  else {
    shock_usage();
    vcl_cout << vcl_endl;
    eth_usage();
    vcl_cout << vcl_endl;
    opelt_train_usage();
  }

  return 0;
}
