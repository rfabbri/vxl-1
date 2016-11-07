#include <testlib/testlib_test.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/algo/dbsk2d_extract_subgraph.h>
#include <dbskr/algo/dbskr_shock_patch_model_selector.h>
#include <dbskr/algo/dbskr_shock_patch_selector.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/algo/dbskr_shock_path_finder.h>
#include <dbsk2d/dbsk2d_file_io.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <vil/vil_load.h>
#include <vul/vul_file.h>
#include <dbgrl/algo/dbgrl_algs.h>

#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_compute_scurve.h>
#include <dbskr/dbskr_sm_cor.h>


static void test_shock_patch_match(int argc, char* argv[])
{
  testlib_test_start("testing graph algos ");

  // shock patch match params
  dbskr_tree_edit_params params;
  params.elastic_splice_cost_ = false;
  params.circular_ends_ = false;
  params.combined_edit_ = false;
  params.scurve_sample_ds_ = 1.0f;
  params.scurve_interpolate_ds_ = 1.0f;
  params.curve_matching_R_ = 6.0f;     // localized_edit is off if not set
  
  //: load the st1
  vcl_string name1 = "donkey1";
  vcl_string path_initial = "Y:\\";
  vcl_string sep = "\\";
  //vcl_string st_file1 = "donkey1-model-nocirc-s-1-e-10-i-1-patch_strg.bin";
  //vcl_string st_file1 = "Y:\\99-db\\orl-exps\\patches\\prototype_patches\\patch_set\\donkey1\\donkey1_p_e_data_1_10_1_2_on_off_on_0.5_off_0.01_0.8_on_on_off_png\\donkey1_p_e_data_1_10_1_2_on_off_on_0.5_off_0.01_0.8_on_on_off_png-patch_strg.bin";
  vcl_string st_file1 = path_initial + "99-db" + sep + "orl-exps" + sep + "patches" + sep + "prototype_patches" + sep + "patch_set" + sep + name1 + sep + name1 + "_p_e_data_1_10_1_2_on_off_on_0.5_off_0.01_0.8_on_on_off_png" + sep + name1 + "_p_e_data_1_10_1_2_on_off_on_0.5_off_0.01_0.8_on_on_off_png-patch_strg.bin";
  //vcl_string sg_file1 = "Y:\\99-db\\orl-exps\\dataset\\donkey1\\donkey1.esf";
  vcl_string sg_file1 = path_initial + "99-db" + sep + "orl-exps" + sep + "shocks" + sep + "protos" + sep + "shock_set" + sep + name1 + sep + name1 + ".esf";
  //vcl_string img_file1 = "Y:\\99-db\\orl-exps\\dataset\\donkey1\\donkey1.pgm";
  vcl_string img_file1 = path_initial + "99-db" + sep + "orl-exps" + sep + "dataset" + sep + name1 + sep + name1 + ".pgm";

  dbskr_shock_patch_storage_sptr st1 = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs(st_file1.c_str());
  st1->b_read(ifs);
  ifs.close();

  vcl_string str_name_end = "patch_strg.bin";

  //: read the esfs as well
  vcl_cout << st1->size() << " patches in str1, reading shocks..\n";
  st1->load_patch_shocks_and_create_trees(st_file1, str_name_end, 
    params.elastic_splice_cost_, params.circular_ends_, params.combined_edit_, params.scurve_sample_ds_, params.scurve_interpolate_ds_);
  
  //vcl_string st_file2 = "horse1-090-180-patch_strg.bin";
  //vcl_string st_file2 = "horse1-090-180-nocirc-color-s-5-e-5-i-1-pd-2-st-0.5-ot-0.8-patch_strg.bin";
  //vcl_string st_file2 = "dog2-nocirc-color-s-1-e-5-i-1-pd-2-st-0.5-ot-0.8-patch_strg.bin";
  //vcl_string st_file2 = "Y:\\99-db\\orl-exps\\patches\\prototype_patches\\patch_set\\cat1\\cat1_p_e_data_1_10_1_2_on_off_on_0.5_off_0.01_0.8_on_on_off_png\\cat1_p_e_data_1_10_1_2_on_off_on_0.5_off_0.01_0.8_on_on_off_png-patch_strg.bin";
  vcl_string name2 = "dog2";
  vcl_string set_name = "del-5-0.5-1";
  vcl_string st_file2 = path_initial + "99-db" + sep + "orl-exps" + sep + "patches" + sep + set_name + sep + "patch_set" + sep + name2 + sep + name2 + "_p_e_data_1_5_1_2_off_off_on_0.5_off_0.01_0.8_on_on_off_png" + sep + name2 + "_p_e_data_1_5_1_2_off_off_on_0.5_off_0.01_0.8_on_on_off_png-patch_strg.bin";
  //vcl_string sg_file2 = "Y:\\99-db\\orl-exps\\dataset\\cat1\\cat1.esf";
  vcl_string sg_file2 = path_initial + "99-db" + sep + "orl-exps" + sep + "shocks" + sep + set_name + sep + "shock_set" + sep + name2 + sep + name2 + ".esf";
  //vcl_string img_file2 = "Y:\\99-db\\orl-exps\\dataset\\cat1\\cat1.pgm";
  vcl_string img_file2 = path_initial + "99-db" + sep + "orl-exps" + sep + "dataset" + sep + name2 + sep + name2 + ".pgm";
  dbskr_shock_patch_storage_sptr st2 = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs2(st_file2.c_str());
  st2->b_read(ifs2);
  ifs2.close();

  //: read the esfs as well
  vcl_cout << st2->size() << " patches in str2, reading shocks..\n";
  st2->load_patch_shocks_and_create_trees(st_file2, str_name_end, 
    params.elastic_splice_cost_, params.circular_ends_, params.combined_edit_, params.scurve_sample_ds_, params.scurve_interpolate_ds_);

  dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
  //vcl_string out_name = "donkey1-horse1-match.bin";
  //vcl_string out_name = "donkey1-model-nocirc-s-1-e-10-i-1-horse1-090-180-nocirc-color-s-5-e-5-i-1-pd-2-st-0.5-ot-0.8-nocirc-s-1.0-i-1.0-R-6.0.bin";
  //vcl_string out_name = "donkey1-model-nocirc-s-1-e-10-i-1-dog2-nocirc-color-s-1-e-5-i-1-pd-2-st-0.5-ot-0.8-nocirc-s-1.0-i-1.0-R-6.0.bin";
  //vcl_string out_name = params_.match_folder_ + inp.name1 + "-" + inp.name2 + params_.shock_match_params_.output_file_postfix_ + ".bin";
  //vcl_string out_name = "donkey1-model-nocirc-s-1-e-10-i-1-dog2-nocirc-color-s-1-e-5-i-1-pd-2-st-0.5-ot-0.8-nocirc-s-1.0-i-1.0-R-6.0.bin";
  //vcl_string out_name = "Y:\\99-db\\orl-exps\\matches\\protos-protos\\match_set\\donkey1\\donkey1-cat1_e_d_data_1_1_off_off_6_off_off_off.bin";
  //vcl_string out_name = "Y:\\99-db\\orl-exps\\matches\\protos-del-1-1-1\\match_set\\skyhawk\\skyhawk-cat2_e_d_data_1_1_off_off_6_off_off_off.bin";
  //vcl_string out_name = "Y:\\99-db\\orl-exps\\matches\\protos-del-1-1-1\\match_set\\donkey1\\donkey1-cat2_e_d_data_1_1_off_off_6_off_off_off.bin";
  vcl_string out_name = path_initial + "99-db" + sep + "orl-exps" + sep + "matches" + sep + "protos-" + set_name + sep + "match_set" + sep + name1 + sep + name1 + "-" + name2 + "_e_d_data_1_1_off_off_6_off_off_off.bin";
  if (vul_file::exists(out_name)) {
    vsl_b_ifstream bfs(out_name.c_str());
    match->b_read(bfs);
    bfs.close();
  } else {
    vcl_vector<dbskr_shock_patch_sptr>& pv1 = st1->get_patches();
    vcl_vector<dbskr_shock_patch_sptr>& pv2 = st2->get_patches();
    patch_cor_map_type& map = match->get_map();
    match->edit_params_ = params;
      
    //vcl_cout << pv1.size() << " patches: ";
    for (unsigned i = 0; i < pv1.size(); i++) {
      vcl_cout << i << " ";
      find_patch_correspondences(pv1[i], pv2, map, params);
      pv1[i]->kill_tree();
    }
    //vcl_cout << "\n match map size: " << map.size() << " map[pv1[0]->id()] size: " << map[pv1[0]->id()]->size() << vcl_endl;
    
    vsl_b_ofstream bfs(out_name.c_str());
    match->b_write(bfs);
    bfs.close();
  }

   //: prepare id maps for this match
  vcl_map<int, dbskr_shock_patch_sptr> model_map;
  for (unsigned ii = 0; ii < st1->size(); ii++) 
    model_map[st1->get_patch(ii)->id()] = st1->get_patch(ii);
  match->set_id_map1(model_map);

  vcl_map<int, dbskr_shock_patch_sptr> query_map;
  for (unsigned ii = 0; ii < st2->size(); ii++) 
    query_map[st2->get_patch(ii)->id()] = st2->get_patch(ii);
  match->set_id_map2(query_map);

  TEST("test match compute length norm costs", match->compute_length_norm_costs_of_cors(), true); // sorts the matches after recomputation of the normalized cost

  //: load shock graphs and create the path finders
  dbsk2d_xshock_graph_fileio file_io;
  dbsk2d_shock_graph_sptr sg1 = file_io.load_xshock_graph(sg_file1);
  TEST("load_xshock_graph() ", !sg1, false);
  dbskr_shock_path_finder f1(sg1);
  TEST("test path finder1", f1.construct_v(), true);

  dbsk2d_shock_graph_sptr sg2 = file_io.load_xshock_graph(sg_file2);
  TEST("load_xshock_graph() ", !sg2, false);
  dbskr_shock_path_finder f2(sg2);
  TEST("test path finder2", f2.construct_v(), true);

  vsol_box_2d_sptr detection_box;
  float threshold = 0.8f;
  float upper_threshold = 1.2f;

  dbskr_tree_edit_params e_params;
  e_params.circular_ends_ = false;
  e_params.curve_matching_R_ = 6.0f;
  e_params.scurve_interpolate_ds_ = 1.0f;
  e_params.scurve_sample_ds_ = 1.0f;

  vil_image_resource_sptr img_p = vil_load_image_resource((img_file1).c_str());
  vil_image_resource_sptr img_q = vil_load_image_resource((img_file2).c_str());

  //st1->create_ps_images(img_p, "imgs_skyhawk", true, vil_rgb<int>(1,0,0));
  //st2->create_ps_images(img_q, "imgs_cat2_del_1_1_1", true, vil_rgb<int>(1,0,0));

  TEST("test match detect instance", 
    //match->detect_instance(detection_box, f1, f2, threshold, upper_threshold, 1.0f, 1.0f, e_params, true, 0.2f, img_p, img_q, "out"), true);
    match->detect_instance_using_paths(detection_box, f1, f2, threshold, upper_threshold, 1.0f, 1.0f, e_params, true, true, false, 50.0f, 0.3f, img_p, img_q, "out"), true);

  if (detection_box) {
    vcl_cout << "detected box with area: " << detection_box->area() << " box: " << *detection_box << vcl_endl;
  } else {
    vcl_cout << "no detection\n";
  }

  
}

TESTMAIN_ARGS(test_shock_patch_match)
