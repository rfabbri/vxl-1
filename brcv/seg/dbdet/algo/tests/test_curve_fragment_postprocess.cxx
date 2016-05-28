// This is brcv/seg/dbdet/tests/test_curve_fragment_postprocess.cxx
#include <testlib/testlib_test.h>

#include <vil/vil_image_view.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/algo/dbdet_curve_fragment_cues.h>
#include <dbdet/algo/dbdet_curve_fragment_ranker.h>

#define DATA(I) (I).top_left_ptr()
static const double tolerance=1e-3;

void
load_dataset(&img, &frags, &edgemap, &beta)
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
}

void
detailed_tests()
{
  // Generate a small image
  unsigned r=5,c=7;

  vil_image_view < float > image(r,c,1);

  image.fill(1);

  image(3,2)=0;
  image(0,2)=0;
  image(0,0)=0;
  image(4,4)=0;
  DATA(image)[34]=0;

  // Compute the descriptor for a curve

  // Match the curve to itself by matching the SIFTs point-wise as a set.
  // Dynamic programming may be used to speed this up by making use of the
  // ordering along the curve.

  // Curve to itself should have match cost equal to zero.

  // TODO build edge map

  dbdet_curve_fragment_cues cue_computer(image, edgemap);

  dbdet_edgel_chain crv;
  y_feature_vector v;

  // corner case: empty curve -- should work
  vcl_cout << "\n--- Testing empty curve case ---\n";

  cue_computer.compute_all_cues(crv, &v);

  vcl_cout << "\n--- Testing 3-edgel curve case ---\n";

  dbdet_edgel e1;
  e1.pt.set(3.,3.);
  e1.tangent = 0;
  crv.push_back(&e1);

  dbdet_edgel e2;
  e2.pt.set(4.,2.);
  e2.tangent = vnl_math::pi/2.;
  crv.push_back(&e2);

  dbdet_edgel e3;
  e3.pt.set(5.5,0.8); //< out of bounds
  e3.tangent = vnl_math::pi/4.;
  crv.push_back(&e3);

  cue_computer.compute(crv, &v);

  // Compute all curves at the same time. 

  // computor.compute_many(ec_v, &sc_v);


  //--------------------------------------------------------------
  vnl_vector<double> rank;
  dbdet_curve_fragment_ranker(frags, img, beta, &rank);
}

//--------------------------------------------------------------
// More complex comparisons against ground truth
void
realistic_test()
{
  vil_image_view<vxl_byte> img;
  load_dataset(&img, &frags, &edgemap, &beta);

  vnl_vector<double> rank;
  dbdet_curve_fragment_ranker(frags, img, beta, &rank);

  // ground truth rank
  double gt_rank_arr [] = { /* put comma separated ground truth rank from matlab for this dataset */ } 
  unsigned n_gt = /* number of data points */
  vnl_vector<double> gt_rank(gt_rank_arr, n_gt);

  TEST("rank size match", rank.size(), n_gt);
  
  for (unsigned i=0; i < n_gt; ++i) {
    TEST_NEAR(rank[i], gt_rank[i], tolerance);
  }
}

//: Test the dbdet_curve_fragment_* functions
MAIN( test_curve_fragment_postprocess )
{ 
  START ("Test dbdet curve framgment post-processing");

  detailed_test();
  realistic_test();
  SUMMARY();
}
