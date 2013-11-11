// create shock patches from a binary model image

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

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
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/algo/dbskr_shock_patch_model_selector.h>
#include <dbsk2d/algo/dbsk2d_compute_bounding_box.h>
#include <dbsk2d/dbsk2d_file_io.h>

#include <dbil/algo/dbil_color_conversions.h>

#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <bbas/bsol/bsol_algs.h>
#include <vsol/vsol_polygon_2d.h>

#include <vgl/vgl_polygon_scan_iterator.h>

void save_image_poly(dbskr_shock_patch_sptr sp, 
                     vcl_string name_initial, 
                     vil_image_resource_sptr img) {

  dbsk2d_shock_graph_sptr sg = sp->shock_graph();
  bool binterpolate = true;
  double interpolate_ds = 1.0;
  bool subsample = true;
  double subsample_ds = 1.0;
  double poly_area_threshold = 20.0f;

  if (!sg || !sg->number_of_vertices() || !sg->number_of_edges() ) {
    vcl_cout << "Patch graph sg is not computed or has 0 nodes and endges !! IMAGE NOT SAVED for " << sp->id() << "\n";
    return; 
  }
  
  vsol_polygon_2d_sptr poly = trace_boundary_from_graph(sg, binterpolate, subsample, 
    interpolate_ds, subsample_ds, poly_area_threshold);
  
  if (!poly) {
    vcl_cout << "Patch graph outer boundary could not be traced from the shock graph!! IMAGE NOT SAVED for " << sp->id() << "\n";
    return; 
  }

  dbinfo_observation_sptr obs = new dbinfo_observation(0, img, poly, true, false, false);
  vil_image_resource_sptr out_img = obs->image_cropped(false);  // no background noise

  char buffer[1000];
  sprintf(buffer, "%d", sp->id());
  vcl_string cnt_str = buffer;
  sprintf(buffer, "%d", sp->depth());
  vcl_string d_str = buffer;
  vil_save_image_resource(out_img, (name_initial+cnt_str+"_"+d_str+".png").c_str()); 
}

int min_depth, max_depth, depth_int, pruning_depth;
  float threshold, overlap_threshold;


void extract_subgraph_and_find_shock_patches(vcl_string image_file, 
                                             vcl_string esf_file,
                                             vcl_string kept_dir_name, 
                                             vcl_string output_name,
                                             bool circular_ends, 
                                             bool save_images)
{

   vcl_cout << "extracting from an image NOT from a MASK\n";

  //: load the image
  vil_image_resource_sptr img_sptr;
  img_sptr = vil_load_image_resource(image_file.c_str());
  
  //: trace the outer boundary of the binary image

 // create the output storage class
  dbskr_shock_patch_storage_sptr output = dbskr_shock_patch_storage_new();

  dbsk2d_shock_graph_sptr sg = extract_shock_from_mask(img_sptr, true, esf_file, 1, 0.05f);
  
  dbskr_shock_patch_model_selector selector(sg);
  
  for (int d = min_depth; d <= max_depth; d++) {
    vcl_cout << "depth: " << d << vcl_endl;
    selector.extract(d, circular_ends);
    vcl_cout << " ...... DONE!\n";
  }

  for (int d = min_depth; d <= max_depth; d++) {
    selector.add_to_storage(d, output);
  }
    
  if (save_images) {
    vcl_cout << "saving images, total # of patches: " << output->size() << "... \n";
    for (unsigned i = 0; i < output->size(); i++) {
      dbskr_shock_patch_sptr sp = output->get_patch(i);
      save_image_poly(sp, kept_dir_name, img_sptr);
      vcl_cout << ".";
    }
  }

  vcl_cout <<"\n";
    
  vsl_b_ofstream bfs(output_name.c_str());
  output->b_write(bfs);
  bfs.close();

  vcl_cout << output->size() << " patches in the storage, saving shocks..\n";
  //: save esfs for each patch to load later
  vcl_string str_name_end = "patch_storage.bin";
  for (unsigned i = 0; i < output->size(); i++) {
    dbskr_shock_patch_sptr sp = output->get_patch(i);
    vcl_string patch_esf_name = output_name.substr(0, output_name.length()-str_name_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_xshock_graph_fileio file_io;
    file_io.save_xshock_graph(sp->shock_graph(), patch_esf_name);
  }

  return;
}

void individual_usage() {
  vcl_cout << "Usage : <program name> <binary image_file> <output_dir> <min_depth> <max_depth>\n";
}

int main_individual(int argc, char *argv[]) {
  vcl_cout << "Extracting patches from an individual model image!\n";

  //: out file contains the wrong mathces if any
  vcl_string image_file, output_dir;

  vcl_cout << "argc: " << argc << vcl_endl;
  
  if (argc != 5) {
    individual_usage();
    return -1;
  }
  image_file = argv[1]; output_dir = argv[2];
  min_depth = atoi(argv[3]); max_depth = atoi(argv[4]); 
  vcl_cout << "min_depth: " << min_depth << " max_depth: " << max_depth << vcl_endl;
  
  vcl_string::size_type pos = image_file.find_last_of("//");
  vcl_string::size_type pos2 = image_file.find_last_of(".");
  vcl_string only_name = image_file.substr(pos+1, pos2);

  vcl_string out_dir = output_dir + only_name +"-"+argv[3]+"-"+argv[4]+"/";
  vul_file::make_directory_path(out_dir);

  vcl_string esf_file = out_dir + "/" + only_name;

  vcl_string kept_dir_name = out_dir+"patches/";
  vul_file::make_directory_path(kept_dir_name);
  vcl_string output_name = out_dir+only_name+"_patch_storage.bin";

  //: extract each subgraph and find the shock patches
  bool circular_ends = true;
  bool keep_pruned = true;
  bool save_images = true;
  bool save_discarded_images = true;
  float area_threshold_ratio = 0.01f; // %1 of the total area
 
  extract_subgraph_and_find_shock_patches(image_file, esf_file, kept_dir_name, output_name, circular_ends, save_images);

  return 0;
}


void opelt_usage() {
  vcl_cout << "Usage : <program name> <image_dir> <class_initial> <class_list> <esf_dir (bnd dir)> <output_dir> <min_depth> <max_depth> <depth_int> <pruning_depth> <sorting threshold (0 if no aproiri elimination before sorting> <overlap threshold> <1 or 0>\n";
}

int main_opelt_train(int argc, char *argv[]) {
#if 0
  vcl_cout << "extracting shock patches!\n";
  vcl_string image_dir, esf_dir, output_dir, class_initial, class_list, cat_str;

  if (argc != 13) {
    eth_usage();
    return -1;
  }
  //int min_depth, max_depth, depth_int, pruning_depth;
  //float threshold;
  
  vil_image_view<float> L_, A_, B_;
  vil_image_view<vxl_byte> I_;
  vil_image_resource_sptr img_sptr;
  
  image_dir = argv[1]; class_initial = argv[2]; class_list = argv[3]; esf_dir = argv[4]; output_dir = argv[5]; 
  min_depth = atoi(argv[6]); max_depth = atoi(argv[7]); depth_int = atoi(argv[8]); 
  pruning_depth = atoi(argv[9]); 
  vcl_cout << "min_depth: " << min_depth << " max_depth: " << max_depth << " depth int: " << depth_int << vcl_endl;
  threshold = float(atof(argv[10]));
  overlap_threshold = float(atof(argv[11]));
  int iii = atoi(argv[12]);
  vcl_string sort_type = "_contour_";
  bool contour_ratio = true;
  if (iii == 1) {
    contour_ratio = false;
    sort_type = "_color_";
  }

  //: same for all the patches
  bool circular_ends = true;
  bool keep_pruned = true;
  bool save_images = true;
  bool save_discarded_images = false;
  float area_threshold_ratio = 0.01f; // %1 of the total area

  vcl_vector<vcl_string> image_names;
  vcl_ifstream fp(class_list.c_str());
  if (!fp) {
    vcl_cout<<" Unable to Open "<< class_list <<vcl_endl;
    return 0;
  }

  while (!fp.eof()) {
    vcl_string name;
    fp >> name;
    vcl_cout << "read: " << name << vcl_endl;
    if (name.size() > 2) {
      //image_names.push_back(name);

      int nbr = atoi(name.c_str());
      char buffer[1000];
      sprintf(buffer, "%d", nbr);
      vcl_string nbr_str = buffer;
      vcl_cout << nbr << " " << nbr_str << vcl_endl;
      image_names.push_back(nbr_str);
    }
  }
  fp.close();
  vcl_cout << "class set: \n";
  for (unsigned i = 0; i < image_names.size(); i++) {
  /*  vcl_string::size_type pos = image_names[i].find_last_of(class_initial.c_str());
    vcl_string::size_type pos2 = image_names[i].find_last_of(".");
    vcl_string name = image_names[i].substr(pos+1, pos2);
    
    //vcl_string name = image_names[i].substr(class_initial.length(), image_names[i].length()-4);
    image_names[i] = name;   // actually id
    */
    vcl_cout << image_names[i] << "\n";
  }

  vcl_string out_dir = output_dir+class_initial+"/";
  vul_file::make_directory_path(out_dir);
  for (unsigned i = 0; i < image_names.size(); i++) {

    vcl_string ins_dir = out_dir+class_initial+image_names[i]+"-patches"+sort_type+"_"+argv[6]+"_"+argv[7]+"_"+argv[8]+"_"+argv[9]+"_"+argv[10]+"_"+argv[11]+"/";
    vcl_cout << "ins_dir: " << ins_dir << vcl_endl;
    
    vul_file::make_directory_path(ins_dir);

    vcl_string ins_name = class_initial+image_names[i];
    vcl_cout << "\t\t" << ins_name << vcl_endl;

    vcl_string image_file = image_dir+image_names[i]+".png";
    vcl_string esf_file = esf_dir + class_initial+image_names[i]+".esf";
    vcl_string bnd_file = esf_dir + class_initial+image_names[i]+"_boundary.bnd";
    vcl_string kept_dir_name = ins_dir+"kept/";
    vcl_string discarded_dir_name = ins_dir+"pruned/";
    vul_file::make_directory_path(kept_dir_name);
    vul_file::make_directory_path(discarded_dir_name);
    vcl_string output_name = ins_dir+ins_name+"_patch_storage.bin";

    extract_subgraph_and_find_shock_patches(image_file, esf_file, bnd_file, kept_dir_name, 
      discarded_dir_name, 
      output_name, contour_ratio, circular_ends, 
      area_threshold_ratio, overlap_threshold, keep_pruned, save_images, save_discarded_images);
  }
#endif
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc == 13)
    main_opelt_train(argc, argv);
  else if (argc == 5)
    main_individual(argc, argv);
  else {
    individual_usage();
    vcl_cout << "\n\n";
    opelt_usage();
    vcl_cout << "\n\n";
  }

  return 0;
}


