// match shock patches 

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
//#include <vcl_sstream.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/algo/dbsk2d_compute_bounding_box.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/dbskr_sm_cor.h>
#include <dbskr/dbskr_utilities.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <dbskr/dbskr_tree.h>

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

#include <bsol/bsol_algs.h>

dbsk2d_shock_graph_sptr read_esf_from_file(vcl_string fname) {
  dbsk2d_xshock_graph_fileio loader;
  dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(fname);
  return sg;
}

vsol_box_2d_sptr read_bbox(vcl_string filename) {
  vcl_ifstream fp(filename.c_str());
  if (!fp) {
    vcl_cout<<" Unable to Open "<< filename <<vcl_endl;
    return 0;
  }
  char buffer[2000];

  //2)Read in file header.
  fp.getline(buffer,2000); //# PASCAL Annotation Version 1.00
  fp.getline(buffer,2000); //
  fp.getline(buffer,2000); //Image size (X x Y x C) : 0 x 0 x 0
  fp.getline(buffer,2000); //Database : "The Oxford/Graz data extension"
  fp.getline(buffer,2000); //Objects with ground truth : 1 { "PASHorse" }
  fp.getline(buffer,2000); //
  fp.getline(buffer,2000); //# Note that there might be other objects in the image
  fp.getline(buffer,2000); //# for which ground truth data has not been provided.
  fp.getline(buffer,2000); //
  fp.getline(buffer,2000); //# Top left pixel co-ordinates : (1, 1)
  fp.getline(buffer,2000); //
  fp.getline(buffer,2000); //# Details for object 1 ("PASHorse")
  fp.getline(buffer,2000); //Original label for object 1 "PASHorse" : ""
  fp.getline(buffer,2000); //Original label for object 1 "PASHorse" : ""
  vcl_cout << " read: " << buffer << vcl_endl;
  
  //Bounding box for object 1 "PASHorse" (Xmin, Ymin) - (Xmax, Ymax) : (5, 7) - (231, 170)
  fp.getline(buffer,2000);
  vcl_string dum;
  vcl_istringstream oss(vcl_string(buffer).c_str());
  int min_x, min_y, max_x, max_y;
  char c;
  oss >> dum;  //b
  vcl_cout << dum << vcl_endl;
  oss >> dum;  //b
  vcl_cout << dum << vcl_endl;
  oss >> dum;  //f
  vcl_cout << dum << vcl_endl;
  oss >> dum;  // ob
  vcl_cout << dum << vcl_endl;
  oss >> dum;  // 1 
  vcl_cout << dum << vcl_endl;
  oss >> dum;  // "
  vcl_cout << dum << vcl_endl;
  oss >> dum; //(
  vcl_cout << dum << vcl_endl;
  oss >> dum; // Y
  vcl_cout << dum << vcl_endl;
  oss >> dum; // -
  vcl_cout << dum << vcl_endl;
  oss >> dum; // (
  vcl_cout << dum << vcl_endl;
  oss >> dum; // Y
  vcl_cout << dum << vcl_endl;
  oss >> dum; // :
  vcl_cout << dum << vcl_endl;
  oss >> c;
  vcl_cout << "c: " << c << vcl_endl;
  oss >> min_x;
  vcl_cout << "min_x: " << min_x << vcl_endl;
  oss >> c;
  vcl_cout << "c: " << c << vcl_endl;
  oss >> min_y;
  vcl_cout << "min_y: " << min_y << vcl_endl;
  oss >> c;
  vcl_cout << "c: " << c << vcl_endl;
  oss >> dum;
  vcl_cout << "dum: " << dum << vcl_endl;
  oss >> c;
  vcl_cout << "c: " << c << vcl_endl;
  oss >> max_x;
  vcl_cout << "max_x: " << max_x << vcl_endl;
  oss >> c;
  vcl_cout << "c: " << c << vcl_endl;
  oss >> max_y;
  vcl_cout << "max_y: " << max_y << vcl_endl;

  vcl_cout << "read minx: " << min_x << " miny: " << min_y << " max_x: " << max_x << " max_y " << max_y << vcl_endl;

  vsol_box_2d_sptr b = new vsol_box_2d();
  b->add_point(min_x, min_y);
  b->add_point(max_x, max_y);
  vcl_cout << "box minx: " << b->get_min_x() << " miny: " << b->get_min_y() << " max_x: " << b->get_max_x() << " max_y " << b->get_max_y() << vcl_endl;
  return b;
}

void individual_usage()
{
  vcl_cout << "\nIndividual Usage: <program name> <shock_patch_dir1> <shock_patch_dir2> <ins_name1> <ins_name2> <match_dir> <shock_curve_sample_ds> <match_with_circular_completions> <similarity threshold> <gt bbox file if any>\n";
  vcl_cout << "Last argument is 1 if matching with circular completions at the leaf scurves, otherwise 0 (open boundary matching)\n";
  vcl_cout << "Use the image patches with similarities less than threshold to their best matching model patch to create the detection bounding box\n";
  vcl_cout << "Use the image patches if only there are at least N different model patches that are matching to the patches of this object\n";
  //vcl_cout << "shock curve sample ds is typically 2.0f\n";
}

//: For sorting pairs by their second elements cost
inline bool
fine_cost_less( const vcl_pair<dbskr_shock_patch_sptr, vcl_pair<int, dbskr_sm_cor_sptr> >& left,
                const vcl_pair<dbskr_shock_patch_sptr, vcl_pair<int, dbskr_sm_cor_sptr> >& right )
{
  return (left.second.second)->final_norm_cost() < (right.second.second)->final_norm_cost();
}

int main_individual(int argc, char *argv[]) {
  vcl_cout << "Matching shock patches of the test image with the patches of the model, assuming the second shock patch storage is the model storage!!!\n";

  //: out file contains the wrong mathces if any
  vcl_string st_file1_dir, st_file2_dir, match_dir, ins_name1, ins_name2, bbox_file;

  vcl_cout << "argc: " << argc << vcl_endl;
  
  //if (argc != 8 && argc != 9) {
  if (argc != 9 && argc != 10) {
    individual_usage();
    return -1;
  }
  st_file1_dir = argv[1];
  st_file2_dir = argv[2];
  ins_name1 = argv[3];
  ins_name2 = argv[4];
  match_dir = argv[5];
  float sc_samp_ds = (float)atof(argv[6]);
  bool match_with_circular_completions = (atoi(argv[7]) == 1 ? true : false);
  float sim_threshold = (float)atof(argv[8]);
  //int N = atoi(argv[7]);
  vsol_box_2d_sptr box_gt;  // ground truth box
  if (argc == 10) {
    bbox_file = argv[9];
    box_gt = read_bbox(bbox_file);
  }

  // create the input storage class
  //vcl_string st_file1 = st_file1_dir + ins_name1 + "-patches_color__6_18_3_2_0.5_0.8\\"+ins_name1+"_patch_storage.bin";
  vcl_string st_file1 = st_file1_dir + ins_name1 + "-patches_color__6_6_1_2_0.3_0.7\\"+ins_name1+"_patch_storage.bin";
  dbskr_shock_patch_storage_sptr st1 = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs(st_file1.c_str());
  st1->b_read(ifs);
  ifs.close();

  vcl_string str_name_end = "patch_storage.bin";

  //: read the esfs as well
  vcl_cout << st1->size() << " patches in str1, reading shocks..\n";
  //: load esfs for each patch
  for (unsigned iii = 0; iii < st1->size(); iii++) {
    dbskr_shock_patch_sptr sp = st1->get_patch(iii);
    vcl_string patch_esf_name = st_file1.substr(0, st_file1.length()-str_name_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_xshock_graph_fileio file_io;
    dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
  }

  // create the input storage class
  vcl_string st_file2 = st_file2_dir + ins_name2 + "-map.esf_patch_storage.bin";
  dbskr_shock_patch_storage_sptr st2 = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs2(st_file2.c_str());
  st2->b_read(ifs2);
  ifs2.close();

  //: read the esfs as well
  vcl_cout << st2->size() << " patches in str2, reading shocks..\n";
  //: load esfs for each patch
  for (unsigned iii = 0; iii < st2->size(); iii++) {
    dbskr_shock_patch_sptr sp = st2->get_patch(iii);
    vcl_string patch_esf_name = st_file2.substr(0, st_file2.length()-str_name_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_xshock_graph_fileio file_io;
    dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
  }

  vcl_cout << "read the shock patch storages...\n";

  vcl_vector<dbskr_shock_patch_sptr>& pv1 = st1->get_patches();
  vcl_vector<dbskr_shock_patch_sptr>& pv2 = st2->get_patches();

  dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
  patch_cor_map_type& map = match->get_map();

  vcl_string output_file = match_dir + ins_name1 + "-" + ins_name2 + "-map-match-"+argv[6]+"-no-circ.bin";
  vcl_cout << "output_file: " << output_file << vcl_endl;
  //check if the outputfile exists
  if (!vul_file::exists(output_file)) {
    match->shock_pruning_threshold_ = 0.8f;
    match->scurve_sample_ds_ = sc_samp_ds;
    match->elastic_splice_cost_ = true;

    //: set the parameters in the patches
    for (unsigned i = 0; i < pv1.size(); i++) {
      pv1[i]->shock_pruning_threshold_ = match->shock_pruning_threshold_;
      pv1[i]->scurve_sample_ds_ = match->scurve_sample_ds_;
      pv1[i]->elastic_splice_cost_ = match->elastic_splice_cost_;
    }
    vcl_cout << vcl_endl;

    for (unsigned i = 0; i < pv2.size(); i++) {
      pv2[i]->shock_pruning_threshold_ = match->shock_pruning_threshold_;
      pv2[i]->scurve_sample_ds_ = match->scurve_sample_ds_;
      pv2[i]->elastic_splice_cost_ = match->elastic_splice_cost_;
    }

    vcl_cout << pv1.size() << " patches: ";
    for (unsigned i = 0; i < pv1.size(); i++) {
      vcl_cout << i << " ";
      find_patch_correspondences(pv1[i], pv2, map, match_with_circular_completions, true);
      pv1[i]->kill_tree();
    }
     
    vcl_cout << "writing bin file\n";
    vsl_b_ofstream bfs(output_file.c_str());
    match->b_write(bfs);
    bfs.close();
  } else {  // read the file
    vsl_b_ifstream ifs(output_file.c_str());
    match->b_read(ifs);
    ifs.close();
  }
  //: create html file

  dbskr_shock_patch_match_sptr new_match = match->construct_match_just_cost();
  new_match->resort_wrt_norm_cost();

  //vcl_string first_patch_images_dir = st_file1_dir + ins_name1 + "-patches_color__6_18_3_2_0.5_0.8\\kept\\";
  vcl_string first_patch_images_dir = st_file1_dir + ins_name1 + "-patches_color__6_6_1_2_0.3_0.7\\kept\\";
  vcl_string second_patch_images_dir = st_file2_dir + ins_name2 + "-map.esf__kept\\";
  vcl_string out_html = match_dir + ins_name2 + "-map-" + ins_name1 + "-match-"+argv[6]+"-no-circ-top-10.html";

  // Ozge todo: create_html_top_n_match method is missing, changed the name in dbskr_rec_algs ??
  //if (create_html_top_n_match(new_match, second_patch_images_dir, first_patch_images_dir, 10, out_html, ins_name2 + "-" + ins_name1))
  // vcl_cout << "html created!\n";
  //else
  //  vcl_cout << "problems in html!\n";

  //: find image patches with best model matches less than given threshold 
  vcl_vector<vcl_pair<dbskr_shock_patch_sptr, vcl_pair<int, dbskr_sm_cor_sptr> > > good_patches; 
  for (unsigned i = 0; i < pv1.size(); i++) {
      vcl_cout << i << " ";
      vcl_pair<int, dbskr_sm_cor_sptr> p = match->get_best_match(pv1[i]->id());
      if (p.second->final_norm_cost() < sim_threshold) {
        vcl_pair<dbskr_shock_patch_sptr, vcl_pair<int, dbskr_sm_cor_sptr> > pp;
        pp.first = pv1[i];
        pp.second = p;
        good_patches.push_back(pp);
      }
      
  }
  if (!good_patches.size()) {
    vcl_cout << "No object detected in this image!!\n";
    if (!box_gt)
      vcl_cout << "CORRECT non-detecion\n";
    else
      vcl_cout << "WRONG  there was a bbox gt but MISSED the entire object!\n";
    return 0;
  }

  vcl_sort(good_patches.begin(), good_patches.end(), fine_cost_less);
  vcl_cout << "there are " << good_patches.size() << " patches passing the threshold\n";
  
  vsol_box_2d_sptr detection_box = new vsol_box_2d();
  //int number_of_different_model_patches = 0;
  //vcl_map<int> seen_so_far;
  for (unsigned i = 0; i < good_patches.size(); i++) {
    dbskr_shock_patch_sptr sp = good_patches[i].first;
    dbsk2d_compute_bounding_box(sp->shock_graph());
    vsol_box_2d_sptr box = sp->shock_graph()->get_bounding_box(); 
    
    vcl_cout << "patch: " << sp->id() << " passed with " << good_patches[i].second.second->final_norm_cost() << " matching to: " << good_patches[i].second.first << "\n";
    detection_box->grow_minmax_bounds(box);
  }
  vcl_cout << "object detected at: (" << detection_box->get_min_x() << ", " << detection_box->get_min_y() << ") ";
  vcl_cout << "(" << detection_box->get_max_x() << ", " << detection_box->get_max_y() << ") ";

  if (box_gt) {
    vsol_box_2d_sptr inters;
    if (bsol_algs::intersection(detection_box, box_gt, inters)) {
      vsol_box_2d_sptr uni;
      bsol_algs::box_union(detection_box, box_gt, uni);
      double ratio = inters->area()/uni->area();
      if (ratio >= 0.5) 
        vcl_cout << "CORRECT detecion box overlap with uni box with: " << ratio << " \n";
      else 
        vcl_cout << "WRONG detecion box overlap with uni box with: " << ratio << " \n";
    } else 
      vcl_cout << "WRONG there was bbox gt, but DETECTED a box with NO Intersection!!!\n";
  } else {
    vcl_cout << "WRONG there was NO bbox gt, but DETECTED something!!!\n";
  }

  return 0;
}


int main(int argc, char *argv[]) {
  if (argc == 9 || argc == 10)
    main_individual(argc, argv);
  else {
    individual_usage();
  }

  return 0;
}
