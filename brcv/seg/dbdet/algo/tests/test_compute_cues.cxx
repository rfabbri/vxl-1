// This is brcv/seg/dbdet/tests/test_compute_cues.cxx
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_rgb.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_file_matrix.h>
#include <vnl/vnl_file_vector.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/algo/dbdet_curve_fragment_cues.h>
#include <dbdet/algo/dbdet_cem_file_io.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>
#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/algo/dbdet_load_edg.h>
#include <dbtest_root_dir.h>


static const double tolerance=1e-3;

void load_dataset(vil_image_view<vil_rgb<vxl_byte> > &img, dbdet_curve_fragment_graph &frags, dbdet_edgemap_sptr &edgemap)
{

  vcl_string root = dbtest_root_dir();
  vcl_string base_path = root + "/brcv/seg/dbdet/algo/tests/test_data/";

  vcl_string image_path = base_path + "2018.jpg";
  vcl_string frags_path = base_path + "2018.cem";
  vcl_string edge_path = base_path + "2018.edg";

  img = vil_convert_to_component_order(vil_convert_to_n_planes(3,
        vil_convert_stretch_range (vxl_byte(), vil_load(image_path.c_str()))));
  dbdet_load_edg(edge_path, true, 1.0, edgemap);
  dbdet_load_cem(frags_path, frags);
}

void cues_test()
{
  vil_image_view<vil_rgb<vxl_byte> > img;
  dbdet_curve_fragment_graph curve_fragment_graph;
  dbdet_edgemap_sptr edgemap_sptr;
  load_dataset(img, curve_fragment_graph, edgemap_sptr);

  dbdet_curve_fragment_cues cues(img, (*edgemap_sptr));

  vcl_string root = dbtest_root_dir();
  vcl_string cues_path = root + "/brcv/seg/dbdet/algo/tests/test_data/cues.txt";
  
  vnl_matrix<double> gt_cues =
    static_cast<vnl_matrix<double> > (vnl_file_matrix<double>(cues_path.c_str()));
  
  dbdet_edgel_chain_list & frags = curve_fragment_graph.frags;
  
  unsigned j = 0;
  for (dbdet_edgel_chain_list_const_iter it=frags.begin(); it != frags.end(); it++, j++) {

    y_feature_vector fv;
    cues.compute_all_cues(*(*it), &fv);

    TEST_NEAR("Y_BG_GRAD", fv[Y_BG_GRAD], gt_cues.get(j, Y_BG_GRAD), tolerance);
    TEST_NEAR("Y_SAT_GRAD", fv[Y_SAT_GRAD], gt_cues.get(j, Y_SAT_GRAD), tolerance);
    TEST_NEAR("Y_HUE_GRAD", fv[Y_HUE_GRAD], gt_cues.get(j, Y_HUE_GRAD), tolerance);
    TEST_NEAR("Y_ABS_K", fv[Y_ABS_K], gt_cues.get(j, Y_ABS_K), tolerance);
    TEST_NEAR("Y_EDGE_SPARSITY", fv[Y_EDGE_SPARSITY], gt_cues.get(j, Y_EDGE_SPARSITY), tolerance);
    TEST_NEAR("Y_WIGG", fv[Y_WIGG], gt_cues.get(j, Y_WIGG), tolerance);
    TEST_NEAR("Y_LEN", fv[Y_LEN], gt_cues.get(j, Y_LEN), tolerance);
    TEST_NEAR("Y_MEAN_CONF", fv[Y_MEAN_CONF], gt_cues.get(j, Y_MEAN_CONF), tolerance);
    //for now tests just for the first iter
    break;
  }
}

//: Test the dbdet_curve_fragment_* functions
MAIN( test_compute_cues )
{ 
  START ("Test dbdet compute cues");
  cues_test();
  SUMMARY();
}
