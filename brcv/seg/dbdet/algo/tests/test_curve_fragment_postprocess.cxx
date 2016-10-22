// This is brcv/seg/dbdet/tests/test_curve_fragment_postprocess.cxx
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_rgb.h>
#include <vil/vil_print.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_file_matrix.h>
#include <vnl/vnl_file_vector.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/algo/dbdet_curve_fragment_cues.h>
#include <dbdet/algo/dbdet_curve_fragment_ranker.h>
#include <dbdet/algo/dbdet_cem_file_io.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>
#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/algo/dbdet_load_edg.h>
#include <dbtest_root_dir.h>


#define DATA(I) (I).top_left_ptr()
static const double tolerance=1e-3;

void
load_dataset(vil_image_view<vil_rgb<vxl_byte> > &img, dbdet_curve_fragment_graph &frags, dbdet_edgemap_sptr &edgemap, y_trained_parameters &beta)
{
  // Perform all operations Yuliang does:
  //
  // Load Image --------------------------
  //
  // c = 1;
  // img_src_path = 'Data/VOC2007_img/';
  // img_files = dir([img_src_path '*009684.jpg']);
  // img = imread([img_src_path img_files(c).name]);
  //  
  // Load edgemap --------------------------
  //  [~, edgemap, ~] = load_edg([edge_src_path img_files(c).name(1:end-4) '.edg']);
  //
  // Load contours --------------------------
  //  input_file = [cem_src_path img_files(c).name(1:end-4) '.cem'];
  //  [CEM, ~, ~] = load_contours(input_file);
  // 
  // Load Beta ---------------------------
  //
  //    bet_src = 'training/'
  //    prefix = 'TO_SEL_';
  // 
  //
  // input_beta_2 = load([beta_src prefix 'beta_of_cues_for_seletion.txt']);
  // fmean_2 = input_beta_2(1,:);
  // std_2 = input_beta_2(2,:);
  // beta_2 = input_beta_2(3,:);
  // beta_2 = beta_2./fstd_2;
  //
  
  vcl_string root = dbtest_root_dir();
  vcl_string base_path = root + "/brcv/seg/dbdet/algo/tests/test_data/";

  vcl_string image_path = base_path + "2018.jpg";
  vcl_string frags_path = base_path + "2018.cem";
  vcl_string edge_path = base_path + "2018.edg";
  vcl_string beta_path = base_path + "gPb_SEL_beta_of_cues_for_seletion.txt";

  img = vil_convert_to_component_order(vil_convert_to_n_planes(3,
        vil_convert_stretch_range (vxl_byte(), vil_load(image_path.c_str()))));
  dbdet_load_cem(frags_path, frags);
  dbdet_load_edg(edge_path, true, 1.0, edgemap);

  // debug XXX
  //  dbdet_save_edg("tes-edg", edgemap); /*WORKS*/
  //  dbdet_save_cem("tes-cem", edgemap, frags); /* FAILS */
  vnl_matrix<double> tmp_beta =
    static_cast<vnl_matrix<double> > (vnl_file_matrix<double>(beta_path.c_str()));
  beta = y_trained_parameters(tmp_beta.data_block());
}

void
detailed_test()
{
  // Generate a small image
  unsigned r=5,c=7;

  vil_image_view <vil_rgb<vxl_byte> > image(c,r,1);

  // TODO make this an rgb image
  image.fill(1);

  image(2,3)=0;
  image(2,0)=0;
  image(0,0)=0;
  image(3,2)=0;
  image(6,4)=0;
  DATA(image)[34]=0;

  std::cout << "Testing image:" << std::endl;
  vil_print_all(std::cout, image);

  // Build edge map
  static unsigned const n_edgels = 2;
  const double xytheta[n_edgels][3] = {
    {2.5, 3.5, vnl_math::pi/4},
    {3.5, 2.5, vnl_math::pi/3}
  };

  dbdet_edgemap_sptr em = new dbdet_edgemap(image.ni(), image.nj());
  for (unsigned i=0; i < n_edgels; i++)
    em->insert(new dbdet_edgel(
      vgl_point_2d<double>(xytheta[i][0], xytheta[i][1]), xytheta[i][2]));
  
  // Compute the descriptor for a curve

  dbdet_curve_fragment_cues cue_computer(image, *em);

  {
  dbdet_edgel_chain crv;
  y_feature_vector v;

  vcl_cout << "\n--- Testing empty curve case ---\n";
  cue_computer.compute_all_cues(crv, &v);
  }

  { vcl_cout << "\n--- Testing 3-edgel curve case ---\n";
  dbdet_edgel_chain crv;
  y_feature_vector v;

  dbdet_edgel e1;
  e1.pt.set(3.,3.);
  e1.tangent = 0;
  crv.push_back(&e1);

  dbdet_edgel e2;
  e2.pt.set(4.,2.);
  e2.tangent = vnl_math::pi/2.;
  crv.push_back(&e2);

  dbdet_edgel e3;
  e3.pt.set(5.5,0.8);
  e3.tangent = vnl_math::pi/4.;
  crv.push_back(&e3);

  cue_computer.compute_all_cues(crv, &v);
  }

  // Compute all curves at the same time. 

  // computor.compute_many(ec_v, &sc_v);


  //--------------------------------------------------------------
  /*
  vnl_vector<double> rank;
  dbdet_curve_fragment_ranker(frags, img, beta, &rank);*/
}

//--------------------------------------------------------------
// More complex comparisons against ground truth
void
realistic_test()
{
  vil_image_view<vil_rgb<vxl_byte> > img;
  dbdet_curve_fragment_graph curve_fragment_graph;
  dbdet_edgemap_sptr edgemap_sptr;
  y_trained_parameters beta;
  load_dataset(img, curve_fragment_graph, edgemap_sptr, beta);

  vnl_vector<double> rank;
  dbdet_curve_fragment_ranker(curve_fragment_graph.frags, edgemap_sptr, img, beta, &rank);

  vcl_string root = dbtest_root_dir();
  vcl_string rank_path = root + "/brcv/seg/dbdet/algo/tests/test_data/rank.txt";
  
  vnl_vector<double> gt_rank =
    static_cast<vnl_vector<double> > (vnl_file_vector<double>(rank_path.c_str()));

  TEST("rank size match", rank.size(), gt_rank.size());
  for (unsigned i=0; i < gt_rank.size(); ++i)
    TEST_NEAR("Test rank[i]",rank[i], gt_rank[i], tolerance);
}

//: Test the dbdet_curve_fragment_* functions
MAIN( test_curve_fragment_postprocess )
{ 
  START ("Test dbdet curve framgment post-processing (Depends of compute_cues test)");

  detailed_test();
  realistic_test();
  SUMMARY();
}
