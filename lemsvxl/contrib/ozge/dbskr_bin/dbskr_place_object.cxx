// place matched patches on the given test image

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

void individual_usage()
{
  vcl_cout << "\nIndividual Usage: <program name> <shock_patch_dir1> <shock_patch_dir2> <ins_name1> <ins_name2> <image1 file> <image2 file> <match_dir> <shock_curve_sample_ds> <N>\n";
  vcl_cout << "shock curve sample ds is typically 2.0f\n";
}

//: For sorting pairs by their second elements cost
inline bool
fine_cost_less( const vcl_pair<dbskr_shock_patch_sptr, vcl_pair<int, dbskr_sm_cor_sptr> >& left,
                const vcl_pair<dbskr_shock_patch_sptr, vcl_pair<int, dbskr_sm_cor_sptr> >& right )
{
  return (left.second.second)->final_norm_cost() < (right.second.second)->final_norm_cost();
}

int main(int argc, char *argv[]) {
  vcl_cout << "Matching shock patches of the test image with the patches of the model, assuming the second shock patch storage is the model storage!!!\n";

  //: out file contains the wrong mathces if any
  vcl_string st_file1_dir, st_file2_dir, match_dir, ins_name1, ins_name2, bbox_file, img_file1, img_file2;

  vcl_cout << "argc: " << argc << vcl_endl;
  
  if (argc != 10) {
    individual_usage();
    return -1;
  }

  st_file1_dir = argv[1];
  st_file2_dir = argv[2];
  ins_name1 = argv[3];
  ins_name2 = argv[4];
  img_file1 = argv[5];
  img_file2 = argv[6];
  match_dir = argv[7];
  float sc_samp_ds = (float)atof(argv[8]);
  vcl_string sc_samp_ds_str = argv[8];
  int N = atoi(argv[9]);
  
  // create the input storage class
  vcl_string st_file1 = st_file1_dir + ins_name1 + "-patches_color__6_18_3_2_0.5_0.8\\"+ins_name1+"_patch_storage.bin";
  
  //vcl_string st_file1 = st_file1_dir + ins_name1 + "-patches_color__6_6_1_2_0.3_0.7\\"+ins_name1+"_patch_storage.bin";
  
  //vcl_string st_file1 = st_file1_dir + ins_name1 + ".esf_patch_storage.bin";
  vcl_cout << "storage file 1: " << st_file1 << vcl_endl;
  if (!vul_file::exists(st_file1)) {
    vcl_cout << "storge file 1 does not exist!!\n";
    return 0;
  }
  
  dbskr_shock_patch_storage_sptr st_test = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs(st_file1.c_str());
  st_test->b_read(ifs);
  ifs.close();

  vcl_string str_name_end = "patch_storage.bin";

  //: read the esfs as well
  vcl_cout << st_test->size() << " patches in str1, reading shocks..\n";
  //: load esfs for each patch
  for (unsigned iii = 0; iii < st_test->size(); iii++) {
    dbskr_shock_patch_sptr sp = st_test->get_patch(iii);
    vcl_string patch_esf_name = st_file1.substr(0, st_file1.length()-str_name_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_xshock_graph_fileio file_io;
    dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
  }

  // create the input storage class
  vcl_string st_file2 = st_file2_dir + ins_name2 + ".esf_patch_storage.bin";

  if (!vul_file::exists(st_file2)) {
    vcl_cout << "storge file 2: " << st_file2 << " does not exist!!\n";
    return 0;
  }

  dbskr_shock_patch_storage_sptr st_model = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs2(st_file2.c_str());
  st_model->b_read(ifs2);
  ifs2.close();

  //: read the esfs as well
  vcl_cout << st_model->size() << " patches in str2, reading shocks..\n";
  //: load esfs for each patch
  for (unsigned iii = 0; iii < st_model->size(); iii++) {
    dbskr_shock_patch_sptr sp = st_model->get_patch(iii);
    vcl_string patch_esf_name = st_file2.substr(0, st_file2.length()-str_name_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_xshock_graph_fileio file_io;
    dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
  }

  vcl_cout << "read the shock patch storages...\n";

  vcl_vector<dbskr_shock_patch_sptr>& pv_test = st_test->get_patches();
  vcl_vector<dbskr_shock_patch_sptr>& pv_model = st_model->get_patches();

  dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
  patch_cor_map_type& map = match->get_map();

  vcl_string output_file = match_dir + ins_name1 + "-" + ins_name2 + "-match-"+sc_samp_ds_str+"-no-circ.bin";
  vcl_cout << "output_file: " << output_file << vcl_endl;
  //check if the outputfile exists
  if (!vul_file::exists(output_file)) {
    vcl_cout << "could not find: " << output_file << "!!!\n";
    return 0;
  } else {  // read the file
    vsl_b_ifstream ifs(output_file.c_str());
    match->b_read(ifs);
    ifs.close();
  }
  vcl_map<int, dbskr_shock_patch_sptr> map_test, map_model;
  for (unsigned i = 0; i < pv_test.size(); i++) 
    map_test[pv_test[i]->id()]= pv_test[i];
  for (unsigned i = 0; i < pv_model.size(); i++) 
    map_model[pv_model[i]->id()]= pv_model[i];

  //: create html file

  dbskr_shock_patch_match_sptr new_match = match->construct_match();
  new_match->resort_wrt_norm_cost();
  new_match->set_id_map1(map_model);  // id map 1 is model in new_match 
  new_match->set_id_map2(map_test);  // id map 2 is test in new_match

  //vcl_string first_patch_images_dir = st_file1_dir + ins_name1 + "-patches_color__6_18_3_2_0.5_0.8\\kept\\";
  //vcl_string first_patch_images_dir = st_file1_dir + ins_name1 + "-patches_color__6_6_1_2_0.3_0.7\\kept\\";
  vcl_string model_patch_images_dir = st_file2_dir + ins_name2 + ".esf__kept\\";
  vil_image_resource_sptr img_test = vil_load_image_resource(img_file1.c_str());
  vil_image_resource_sptr img_model = vil_load_image_resource(img_file2.c_str());
  vcl_string out_html = match_dir + ins_name2 + "-" + ins_name1 + "-match-"+sc_samp_ds_str+"-no-circ-top-10-placements.html";
  vcl_string out_html_images_dir = match_dir + ins_name2 + "-" + ins_name1 + "-match-"+sc_samp_ds_str+"-no-circ-top-10-placement-images\\";

  vul_file::make_directory_path(out_html_images_dir.c_str());

  if (create_html_top_n_placements(new_match, img_test, img_model, model_patch_images_dir, 10, out_html_images_dir, out_html, ins_name2 + "-" + ins_name1))
   vcl_cout << "html created!\n";
  else
    vcl_cout << "problems in html!\n";

  return 0;
}

/*
int main(int argc, char *argv[]) {
  if (argc == 9 || argc == 10)
    main_individual(argc, argv);
  else {
    individual_usage();
  }

  return 0;
}
*/

