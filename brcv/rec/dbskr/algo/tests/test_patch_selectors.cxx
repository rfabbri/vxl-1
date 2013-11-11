#include <testlib/testlib_test.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/algo/dbsk2d_extract_subgraph.h>
#include <dbskr/algo/dbskr_shock_patch_model_selector.h>
#include <dbskr/algo/dbskr_shock_patch_selector.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbsk2d/dbsk2d_file_io.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <vil/vil_load.h>
#include <vul/vul_file.h>
#include <dbsk2d/algo/dbsk2d_compute_bounding_box.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d.h>

static void test_patch_selectors(int argc, char* argv[])
{
  testlib_test_start("testing patch selectors ");
 
  dbsk2d_xshock_graph_fileio file_io;
/*  dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph("bonefishes.esf");

  print_shock_graph(sg);

  TEST("load_xshock_graph() ", !sg, false);
  TEST("load_xshock_graph() ", sg->number_of_vertices(), 14);

  dbskr_shock_patch_model_selector sel(sg);
  TEST("dbskr_shock_patch_model_selector() ", sel.patch_sets().size(), 0);

  sel.extract(1, false);
  TEST("extract() ", sel.patch_sets().size(), 1);
  TEST("extract() ", sel.patch_sets()[0]->size(), 4);

  sel.prune_same_patches(1);  // should not prune anything
  TEST("prune_same_patches() ", sel.patch_sets()[0]->size(), 4);
  
  sel.extract(2, false);
  TEST("extract() ", sel.patch_sets().size(), 2);
  TEST("extract() ", sel.patch_sets()[1]->size(), 4);

  sel.prune_same_patches(2); // should not prune anything
  TEST("prune_same_patches() ", sel.patch_sets()[1]->size(), 4);
  
  sel.extract(3, false);
  TEST("extract() ", sel.patch_sets().size(), 3);
  TEST("extract() ", sel.patch_sets()[2]->size(), 4);

  sel.prune_same_patches(3); // should not prune anything
  TEST("prune_same_patches() ", sel.patch_sets()[2]->size(), 3);
  
  sel.extract(6, false);
  TEST("extract() ", sel.patch_sets().size(), 4);
  TEST("extract() ", sel.patch_sets()[3]->size(), 4);

  dbsk2d_shock_node_sptr node = sg->get_node(1056);
  TEST("get_node() ", node->id(), 1056);

  dbskr_v_graph_sptr v_g1 = construct_v_graph(sg, node, 6);
  print_v_graph(v_g1);
  TEST("construct_v_graph() ", v_g1->number_of_vertices(), 10);

  dbskr_v_graph_sptr v_g1_1 = construct_v_graph(sg, node, 6);
  print_v_graph(v_g1_1);
  TEST("same() ", v_g1->same(*v_g1_1), true);
  

  dbsk2d_shock_node_sptr node2 = sg->get_node(1100);
  TEST("get_node() ", node2->id(), 1100);

  dbskr_v_graph_sptr v_g2 = construct_v_graph(sg, node2, 6);
  print_v_graph(v_g2);
  TEST("same() ", v_g1->same(*v_g2), true);
  TEST("same() ", v_g2->same(*v_g1), true);

  TEST("construct_v_graph() ", v_g2->number_of_vertices(), 10);
  TEST("node_overlap() ", v_g1->node_overlap(*v_g2), 1);

  sel.prune_same_patches(6);
  TEST("prune_same_patches() ", sel.patch_sets().size(), 4);
  TEST("prune_same_patches() ", sel.patch_sets()[3]->size(), 1);

  dbskr_v_graph_sptr v_g_2_1 = construct_v_graph(sg, node, 3);
  print_v_graph(v_g_2_1);
  
  dbskr_v_graph_sptr v_g_2_2 = construct_v_graph(sg, node2, 3);
  print_v_graph(v_g_2_2);
  
  dbskr_v_graph_sptr v_g_2_3 = construct_v_graph(sg, sg->get_node(870), 3);
  print_v_graph(v_g_2_3);
  
  dbskr_v_graph_sptr v_g_2_4 = construct_v_graph(sg, sg->get_node(829), 3);
  print_v_graph(v_g_2_4);
  TEST("same() ", v_g_2_1->same(*v_g_2_3), true);
  TEST("same() ", v_g_2_3->same(*v_g_2_4), false);

  dbskr_v_graph_sptr v_g_870_2 = construct_v_graph(sg, sg->get_node(870), 2);
  print_v_graph(v_g_870_2);
  
  dbskr_v_graph_sptr v_g_829_3 = construct_v_graph(sg, sg->get_node(829), 3);
  print_v_graph(v_g_829_3);
  TEST("same() ", v_g_870_2->same(*v_g_829_3), true);
  
  dbskr_shock_patch_model_selector sel2(sg);
  sel2.extract(1);
  sel2.prune_same_patches(1);
  sel2.extract(2);
  sel2.prune_same_patches(2);
  sel2.extract(3);
  sel2.prune_same_patches(3);
  sel2.extract(4);
  sel2.prune_same_patches(4);
  sel2.extract(5);
  sel2.prune_same_patches(5);
  sel2.extract(6);
  sel2.prune_same_patches(6);
  sel2.extract(7);
  sel2.prune_same_patches(7);
  sel2.extract(8);
  sel2.prune_same_patches(8);

  TEST(" prune_same_patches_at_all_depths() ", sel2.patch_sets().size(), 8);
  sel2.prune_same_patches_at_all_depths();
  vcl_cout << " size after: " << sel2.patch_sets().size() << vcl_endl;
  TEST(" prune_same_patches_at_all_depths() ", sel2.patch_sets().size(), 3);

  TEST(" find_min_full_depth() ", find_min_full_depth(sg), 3);
*/


/*
vcl_string image_file = "tomato1-090-180.png";
vcl_string esf_file = "tomato1-090-180.esf";
vcl_string boundary_file = "tomato1-090-180_boundary.bnd";

vcl_string kept_dir_name = "./kept/";
vcl_string discarded_dir_name = "./discarded/";
vcl_string output_name = "Horse_horse122-patch_strg.bin";

bool contour_ratio = false;
bool circular_ends = true;
float area_threshold_ratio = 0.01f;
float overlap_threshold = 0.8f;
float sort_threshold = 0.5f;
bool keep_pruned = true;
bool save_images = true;
bool save_discarded_images = false;
/*
extract_subgraph_and_find_shock_patches(image_file, esf_file, boundary_file, kept_dir_name, discarded_dir_name, output_name,
                                        contour_ratio, circular_ends, area_threshold_ratio, overlap_threshold,
                                             6, 18, 3, 2, sort_threshold, keep_pruned, save_images, save_discarded_images);


//test loading
output_name = "/vision/projects/kimia/categorization/visual_fragment_regions/CVPR08-expts/bmvc07-opelt-horses/dataset/Horse_horse122/Horse_horse122-color-s-6-e-18-i-3-pd-2-st-0.5-ot-0.8/Horse_horse122-color-s-6-e-18-i-3-pd-2-st-0.5-ot-0.8-patch_strg.bin";
vcl_cout << "loading: " << output_name << vcl_endl;
 dbskr_shock_patch_storage_sptr st = dbskr_shock_patch_storage_new();
    vsl_b_ifstream ifs(output_name.c_str());
    st->b_read(ifs);
    ifs.close();
    vcl_cout << "loaded a shock patch storage with size: " << st->size() << vcl_endl;
*/

/*
  dbsk2d_shock_graph_sptr sg2 = file_io.load_xshock_graph("Horse_horse116.esf");
  //dbsk2d_shock_graph_sptr sg2 = file_io.load_xshock_graph("tomato1-090-180.esf");
  //print_shock_graph(sg2);

  TEST("load_xshock_graph() ", !sg2, false);
  vcl_cout << "sg2 # of vertices: " << sg2->number_of_vertices()  << vcl_endl;
  int cnt = 0;
  for (dbsk2d_shock_graph::vertex_iterator v_itr = sg2->vertices_begin(); v_itr != sg2->vertices_end(); v_itr++)
  { 
    if ((*v_itr)->degree() < 3)
      continue;
    cnt++;
  }
  vcl_cout << "number of degree 3 nodes: " << cnt << vcl_endl;

  dbskr_shock_patch_selector selector(sg2);

  vil_image_resource_sptr img_sptr = vil_load_image_resource("Horse_horse116.png");
  //vil_image_resource_sptr img_sptr = vil_load_image_resource("tomato1-090-180.png");
  TEST("vil_load_image_resource() ", !img_sptr, false);


  vsol_box_2d_sptr bbox = new vsol_box_2d();
  vcl_vector< vsol_spatial_object_2d_sptr > geoms;

  dbsk2d_file_io::load_bnd_v3_0("Horse_horse116_boundary.bnd", geoms);
  //dbsk2d_file_io::load_bnd_v3_0("tomato1-090-180_boundary.bnd", geoms);

  //: find the bounding box 
  for (unsigned i = 0; i <geoms.size(); i++) {
    if (geoms[i]->cast_to_curve())
      if (geoms[i]->cast_to_curve()->cast_to_line()) {
        vsol_line_2d_sptr line = geoms[i]->cast_to_curve()->cast_to_line();
        bbox->add_point(line->p0()->x(), line->p0()->y());
        bbox->add_point(line->p1()->x(), line->p1()->y());
      }
  }

  vcl_cout << "bbox area: " << bbox->area() << vcl_endl;
  TEST("bbox area() ", !bbox->area(), false);

  float area_threshold = (img_sptr->ni()*img_sptr->nj())*0.01f;
  vcl_cout << "area_threshold: " << area_threshold << vcl_endl;

  selector.set_area_threshold(area_threshold);

  selector.set_image(img_sptr);
  bool circular_ends = true;
  //selector.extract(18, circular_ends);
  //vcl_cout << " all patches extracted at depth 18\n";
  //selector.prune_same_patches(18);
  //vcl_cout << " same patches are pruned at depth 18\n";
  //selector.clear_bounding_box_contours(18, bbox);
  //vcl_cout << " bounding box contours are cleared\n";
  //TEST("find_and_sort_wrt_color_contrast() ", !selector.find_and_sort_wrt_color_contrast(18, 0.5f), false);
  //vcl_cout << " sorted wrt color contrast\n";
  //selector.prune(18, 2, true);
  //vcl_cout << " pruning done\n";
  float sort_threshold = 0.5;
  bool keep_pruned = false;
  dbskr_shock_patch_storage_sptr output = dbskr_shock_patch_storage_new();
  dbskr_shock_patch_storage_sptr discarded = dbskr_shock_patch_storage_new();
  //for (int d = 6; d <= 18; d += 3) {
  int d = 2;
    vcl_cout << "depth: " << d << vcl_endl;
    selector.extract(d, circular_ends);
    selector.prune_same_patches(d);
    //selector.prune_bounding_box_patches(d, bbox);
    selector.clear_bounding_box_contours(d, bbox);
    //if (!contour_ratio) {
      if (!selector.find_and_sort_wrt_color_contrast(d, sort_threshold))
        selector.find_and_sort_wrt_app_contrast(d, sort_threshold);
    //} else
    //  selector.find_and_sort_wrt_contour_ratio(d, sort_threshold);
   selector.prune(d, 2, keep_pruned);
    vcl_cout << " ...... DONE!\n";
  //}
  selector.prune_overlaps(0.8f, keep_pruned, true);
  //for (int d = 6; d <= 18; d += 3) {
    selector.create_shocks_and_add_to_storage(d, output);
    if (keep_pruned)
      selector.add_discarded_to_storage(d, discarded);
  //}

  vcl_cout << "output size: " << output->size() << vcl_endl;
  vcl_cout << "discarded size: " << discarded->size() << vcl_endl;
  */

// test detection
#if 0
  vcl_string image_file = "horse1-090-180.png";
  vcl_string esf_file = "horse1-090-180.esf";
  vcl_string boundary_file = "horse1-090-180_boundary.bnd";
  vcl_string kept_dir_name = "./model-kept/";
  vul_file::make_directory(kept_dir_name);
  vcl_string discarded_dir_name = "./model-discarded/";
  vul_file::make_directory(discarded_dir_name);
  vcl_string output_name = "horse1-090-180-nocirc-color-s-5-e-5-i-1-pd-2-st-0.5-ot-0.8-patch_strg.bin";

  bool contour_ratio = false;
  bool circular_ends = true;
  float area_threshold_ratio = 0.01f;
  float overlap_threshold = 0.8f;
  float sort_threshold = 0.5f;
  bool keep_pruned = true;
  bool save_images = true;
  bool save_discarded_images = false;
  extract_subgraph_and_find_shock_patches(image_file, esf_file, boundary_file, kept_dir_name, discarded_dir_name, output_name,
                                        contour_ratio, circular_ends, area_threshold_ratio, overlap_threshold,
                                             5, 5, 1, 2, sort_threshold, keep_pruned, save_images, save_discarded_images);

  vcl_string image_file2 = "Horse_horse116.png";
  vcl_string esf_file2 = "Horse_horse116.esf";
  vcl_string boundary_file2 = "Horse_horse116_boundary.bnd";
  vcl_string kept_dir_name2 = "./116kept/";
  vul_file::make_directory(kept_dir_name2);
  vcl_string discarded_dir_name2 = "./116discarded/";
  vul_file::make_directory(discarded_dir_name2);
  vcl_string output_name2 = "Horse_horse116-patch_strg.bin";

  bool contour_ratio2 = false;
  bool circular_ends2 = true;
  float area_threshold_ratio2 = 0.01f;
  float overlap_threshold2 = 0.5f;
  float sort_threshold2 = 0.5f;
  bool keep_pruned2 = true;
  bool save_images2 = true;
  bool save_discarded_images2 = false;
  extract_subgraph_and_find_shock_patches(image_file2, esf_file2, boundary_file2, kept_dir_name2, discarded_dir_name2, output_name2,
                                        contour_ratio2, circular_ends2, area_threshold_ratio2, overlap_threshold2,
                                             10, 10, 1, 5, sort_threshold2, keep_pruned2, save_images2, save_discarded_images2);

  #endif

  //vcl_string image_file = "kk0732.pgm";
  //vcl_string esf_file = "kk0732.esf";
  //vcl_string bnd_file = "kk0732_boundary.bnd";
  vcl_string image_file = "easterncottontail.pgm";
  vcl_string esf_file = "easterncottontail.esf";
  vcl_string bnd_file = "easterncottontail_boundary.bnd";

  dbsk2d_xshock_graph_fileio loader;
  dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(esf_file.c_str());

  vcl_vector<vsol_spatial_object_2d_sptr> conts;
  vsol_box_2d_sptr box = new vsol_box_2d();
  //: find the bounding box 
  dbsk2d_file_io::load_bnd_v3_0(bnd_file, conts);
  for (unsigned i = 0; i <conts.size(); i++) {
    if (conts[i]->cast_to_curve()) {
      if (conts[i]->cast_to_curve()->cast_to_line()) 
        conts[i]->cast_to_curve()->cast_to_line()->compute_bounding_box();
      else if (conts[i]->cast_to_curve()->cast_to_polyline())
        conts[i]->cast_to_curve()->cast_to_polyline()->compute_bounding_box();
    } else if (conts[i]->cast_to_region()) {
      if (conts[i]->cast_to_region()->cast_to_polygon())
        conts[i]->cast_to_region()->cast_to_polygon()->compute_bounding_box();
    }
    box->grow_minmax_bounds(conts[i]->get_bounding_box());
  }

  //dbsk2d_compute_bounding_box(sg);
  //vsol_box_2d_sptr box = sg->get_bounding_box();

  vcl_string kept_dir_name = "./model-kept/";
  vul_file::make_directory(kept_dir_name);
  vcl_string discarded_dir_name = "./model-discarded/";
  vul_file::make_directory(discarded_dir_name);
  vcl_string output_name = "kk0732-patch_strg.bin";

  bool contour_ratio = false;
  bool circular_ends = true;
  float area_threshold_ratio = 0.01f;
  float overlap_threshold = 0.8f;
  float sort_threshold = 0.5f;
  bool keep_pruned = true;
  bool save_images = true;
  bool save_discarded_images = false;
  find_shock_patches(image_file, sg, box, kept_dir_name, discarded_dir_name, output_name,
                                        contour_ratio, circular_ends, area_threshold_ratio, overlap_threshold,
                                             1, 5, 1, 2, sort_threshold, keep_pruned, save_images, save_discarded_images);
}

TESTMAIN_ARGS(test_patch_selectors)
