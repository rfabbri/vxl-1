// create shock patches from a given image and sampled coarse shock graph

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
#include <dbskr/algo/dbskr_shock_patch_selector.h>
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





void individual_usage() {
    vcl_cout << "Usage : <program name> <image_file> <esf_file> <output_dir> <min_depth> <max_depth> <depth_int> <pruning_depth> <sorting threshold (0 if no aproiri elimination before sorting> <overlap threshold> <1 or 0>\n";
    vcl_cout << " if last value is 1 then image contrast criteria is used for sorting, otherwise contour ratio is used\n";
    vcl_cout << "pruning depth is the depth of tree which will be constructed at the root of a patch and all the other patches rooted at a node,\n";
    vcl_cout << " covered by this pruning tree will be pruned, typically 2. Min depth should be larger than two, otherwise nothing gets pruned at min depth\n";
    vcl_cout << "polygon area threshold is typically 20\n";
    vcl_cout << "the less the contour color contrast measure the better, so if the value is less than the given threshold then that patch will be added\n";
    vcl_cout << "overlap threshold: amount of overlap in terms of number of nodes in v_graphs to prune patches, typically %80, e.g. 0.8f\n";
    vcl_cout << "contrast measure values are in [0,1], so the threshold should be in this range\n";
}

int min_depth, max_depth, depth_int, pruning_depth;
  float threshold, overlap_threshold;


void extract_subgraph_and_find_shock_patches_from_map(vcl_string image_file, 
                                             vcl_string esf_file, 
                                             vcl_string boundary_file,
                                             vcl_string kept_dir_name, 
                                             vcl_string discarded_dir_name, 
                                             vcl_string output_name,
                                             bool contour_ratio,
                                             bool circular_ends, 
                                             float area_threshold_ratio,
                                             float overlap_threshold,
                                             bool keep_pruned,
                                             bool save_images, 
                                             bool save_discarded_images)
{
  //: load the image
  vil_image_view<float> L_, A_, B_;
  vil_image_view<vxl_byte> I_;
  vil_image_resource_sptr img_sptr, img_r, img_g, img_b;
  img_sptr = vil_load_image_resource(image_file.c_str());
  set_images(img_sptr, I_, L_, A_, B_, img_r, img_g, img_b);     
  
  //: load esf and create trees
  dbsk2d_shock_graph_sptr sg = read_esf_from_file(esf_file.c_str());
  vcl_cout << "loaded esf...\n";

 // create the output storage class
  dbskr_shock_patch_storage_sptr output = dbskr_shock_patch_storage_new();
  dbskr_shock_patch_storage_sptr discarded = dbskr_shock_patch_storage_new();
  
  int cnt = 0;  
  
  dbskr_shock_patch_selector selector(sg);
  float area_threshold;
  if (img_sptr)
    area_threshold = (img_sptr->ni()*img_sptr->nj())*area_threshold_ratio;
  else {
    if (!sg->get_bounding_box())
      dbsk2d_compute_bounding_box(sg);
    area_threshold = (float)sg->get_bounding_box()->area()*area_threshold_ratio;
  }
  selector.set_area_threshold(area_threshold);

  selector.set_image(img_sptr);

  for (int d = min_depth; d <= max_depth; d += depth_int) {
    vcl_cout << "depth: " << d << vcl_endl;
    selector.extract(d, circular_ends);
    selector.prune_same_patches(d);
    if (!contour_ratio) {
      if (!selector.find_and_sort_wrt_color_contrast(d, threshold))
        selector.find_and_sort_wrt_app_contrast(d, threshold);
    } else
      selector.find_and_sort_wrt_contour_ratio(d, threshold);
    selector.prune(d, pruning_depth, keep_pruned);
    vcl_cout << " ...... DONE!\n";
  }
  selector.prune_overlaps(overlap_threshold, keep_pruned, true);
  for (int d = min_depth; d <= max_depth; d += depth_int) {
    selector.create_shocks_and_add_to_storage(d, output);
    if (keep_pruned)
      selector.add_discarded_to_storage(d, discarded);
  }
    
  if (save_images) {
    vcl_cout << "saving images, total # of patches: " << output->size() << "... \n";
  for (unsigned i = 0; i < output->size(); i++) {
    dbskr_shock_patch_sptr sp = output->get_patch(i);
    save_image_poly(sp, kept_dir_name, img_r, img_g, img_b);
    vcl_cout << ".";

    if (keep_pruned && save_discarded_images) {
      vcl_cout << " disc: ";
    vcl_vector<dbskr_shock_patch_sptr>* pruned_set = selector.pruned_set(sp);
    if (pruned_set) {
      for (unsigned i = 0; i < pruned_set->size(); i++) {
        dbskr_shock_patch_sptr psp = (*pruned_set)[i];
        char buffer[1000];
        sprintf(buffer, "%d", sp->id());
        vcl_string cnt_str = buffer;
        sprintf(buffer, "%d", sp->depth());
        vcl_string d_str = buffer;
        vcl_string discarded_dir_name_patch = kept_dir_name+cnt_str+"_"+d_str+"_prn_set/";
        vul_file::make_directory_path(discarded_dir_name_patch);
        save_image_poly(psp, discarded_dir_name_patch, img_r, img_g, img_b);
        vcl_cout << ".";
      }
    }
    }
    
  }
  }

  if (save_discarded_images) {
    vcl_cout << " saving discarded images, total #: " << discarded->size() << "... ";
    for (unsigned i = 0; i < discarded->size(); i++) {
      dbskr_shock_patch_sptr sp = discarded->get_patch(i);
      save_image_poly(sp, discarded_dir_name, img_r, img_g, img_b);
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



int main_individual(int argc, char *argv[]) {
  vcl_cout << "Extracting patches from an individual image!\n";

  //: out file contains the wrong mathces if any
  vcl_string image_file, esf_file, output_dir;

  vcl_cout << "argc: " << argc << vcl_endl;
  
  if (argc != 11) {
    individual_usage();
    return -1;
  }
  image_file = argv[1]; esf_file = argv[2]; output_dir = argv[3];
  min_depth = atoi(argv[4]); max_depth = atoi(argv[5]); depth_int = atoi(argv[6]); pruning_depth = atoi(argv[7]);
  threshold = float(atof(argv[8]));
  overlap_threshold = float(atof(argv[9]));
  vcl_cout << "min_depth: " << min_depth << " max_depth: " << max_depth << " depth int: " << depth_int << vcl_endl;
  bool contour_ratio = true;
  int iii = atoi(argv[10]);
  if (iii == 1)
    contour_ratio = false;

  vcl_string::size_type pos = esf_file.find_last_of("//");
  vcl_string::size_type pos2 = esf_file.find_last_of(".");
  vcl_string only_esf_name = esf_file.substr(pos+1, pos2);
  vcl_string boundary_file = esf_file.substr(0, pos2) + "_boundary.bnd";

  vcl_string kept_dir_name = output_dir+only_esf_name+"__kept/";
  vcl_string discarded_dir_name = output_dir+only_esf_name+"__discarded/";
  vul_file::make_directory_path(kept_dir_name);
  vul_file::make_directory_path(discarded_dir_name);
  vcl_string output_name = output_dir+only_esf_name+"_patch_storage.bin";

  //: extract each subgraph and find the shock patches
  bool circular_ends = false;
  bool keep_pruned = true;
  bool save_images = true;
  bool save_discarded_images = true;
  float area_threshold_ratio = 0.01f; // %1 of the total area
 
  extract_subgraph_and_find_shock_patches(image_file, esf_file, boundary_file, kept_dir_name, discarded_dir_name, 
    output_name, contour_ratio, circular_ends, area_threshold_ratio, overlap_threshold, 
    min_depth, max_depth, depth_int, pruning_depth, threshold, keep_pruned, save_images, save_discarded_images);
  //extract_subgraph_and_find_shock_patches_from_map(image_file, esf_file, boundary_file, kept_dir_name, discarded_dir_name, 
  //  output_name, contour_ratio, circular_ends, area_threshold_ratio, overlap_threshold, keep_pruned, save_images, save_discarded_images);
   
  return 0;
}

void eth_usage() {
  vcl_cout << "Usage : <program name> <image_dir> <category name> <view_sptr> <esf_dir (bnd dir)> <output_dir> <min_depth> <max_depth> <depth_int> <pruning_depth> <sorting threshold (0 if no aproiri elimination before sorting> <overlap threshold> <1 or 0>\n";
}

int main_eth(int argc, char *argv[]) {
  vcl_cout << "extracting shock patches from eth set!\n";
  vcl_string image_dir, esf_dir, output_dir, view_str, cat_str;

  if (argc != 13) {
    eth_usage();
    return -1;
  }
  //int min_depth, max_depth, depth_int, pruning_depth;
  //float threshold;
  
  vil_image_view<float> L_, A_, B_;
  vil_image_view<vxl_byte> I_;
  vil_image_resource_sptr img_sptr;
  
  image_dir = argv[1]; cat_str = argv[2]; view_str = argv[3]; esf_dir = argv[4]; output_dir = argv[5];
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

  vcl_vector<vcl_string> cats;
  cats.push_back("horse");cats.push_back("dog");cats.push_back("cow");cats.push_back("apple");cats.push_back("pear");
  cats.push_back("cup");cats.push_back("car");cats.push_back("tomato");
  

  //: same for all the patches
  bool circular_ends = true;
  bool keep_pruned = true;
  bool save_images = true;
  bool save_discarded_images = false;
  float area_threshold_ratio = 0.01f; // %1 of the total area

  unsigned i = 0;
  for ( ; i < cats.size(); i++) {
    if (cat_str == cats[i])
      break;
  }
  
  //: load the images and extract
  //for (unsigned i = 0; i < cats.size(); i++) {
    // outputs are written as e.g. ./cow/cow1-090-180/patches_contour_<min_depth>_<max_depth>_<depth_int>_<pruning_depth>_<sorting threshold>
    //     or                      ./cow/cow1-090-180/patches_color_<min_depth>_<max_depth>_<depth_int>_<pruning_depth>_<sorting threshold>
    vcl_string out_dir = output_dir+cats[i]+"/";
    vul_file::make_directory_path(out_dir);
    vcl_cout << cats[i] << "\n";
    for (unsigned j = 1; j < 11; j++) {
      vcl_ostringstream oss;
      oss << j;

      vcl_string ins_dir = out_dir+cats[i]+oss.str()+"-"+view_str+"-patches"+sort_type+"_"+argv[6]+"_"+argv[7]+"_"+argv[8]+"_"+argv[9]+"_"+argv[10]+"_"+argv[11]+"/";
      vul_file::make_directory_path(ins_dir);

      vcl_string ins_name = cats[i]+oss.str()+"-"+view_str;
      vcl_cout << "\t\t" << ins_name << vcl_endl;

      vcl_string image_file = image_dir+cats[i]+oss.str()+"/"+ins_name+".png";
      vcl_string esf_file = esf_dir + cats[i] + "/" + ins_name + ".esf";
      vcl_string bnd_file = esf_dir + cats[i] + "/" + ins_name + "_boundary.bnd";
      vcl_string kept_dir_name = ins_dir+"kept/";
      vcl_string discarded_dir_name = ins_dir+"pruned/";
      vul_file::make_directory_path(kept_dir_name);
      vul_file::make_directory_path(discarded_dir_name);
      vcl_string output_name = ins_dir+ins_name+"_patch_storage.bin";

      extract_subgraph_and_find_shock_patches(image_file, esf_file, bnd_file, kept_dir_name, 
        discarded_dir_name, 
        output_name, contour_ratio, circular_ends, 
        area_threshold_ratio, overlap_threshold, 
        min_depth, max_depth, depth_int, pruning_depth, threshold, keep_pruned, save_images, save_discarded_images);
    }
  //}
  return 0;
}

void opelt_usage() {
  vcl_cout << "Usage : <program name> <image_dir> <class_initial> ";
  vcl_cout << "<class_list> <esf_dir (bnd dir)> <output_dir> <min_depth> ";
  vcl_cout << "<max_depth> <depth_int> <pruning_depth> ";
  vcl_cout << "<sorting threshold (0 if no aproiri elimination before sorting> <overlap threshold> <1 or 0>\n";
}

int main_opelt_train(int argc, char *argv[]) {
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
      area_threshold_ratio, overlap_threshold, 
      min_depth, max_depth, depth_int, pruning_depth, threshold, keep_pruned, save_images, save_discarded_images);
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc == 13)
    //main_eth(argc, argv);
    main_opelt_train(argc, argv);
  else if (argc == 11)
    main_individual(argc, argv);
  else {
    individual_usage();
    vcl_cout << "\n\n";
    //eth_usage();
    opelt_usage();
    vcl_cout << "\n\n";
  }

  return 0;
}


