#include <testlib/testlib_test.h>

#include <vnl/vnl_math.h>
#include <vil/vil_image_view.h>
#include <mw/algo/mw_sift_curve_algo.h>

#define DATA(I) (I).top_left_ptr()

static void
print_all(const mw_sift_curve_algo &computor, const mw_sift_curve &sc)
{
  vcl_cout << "Number of descriptor dimensions: " << sc.num_dims() << vcl_endl;
  vcl_cout << "Total number of scales: " << sc.num_scales() << vcl_endl;
  vcl_cout << "Number of curve samples : " << sc.num_samples() << vcl_endl;

  // Print all sigmas
  computor.print_sigmas();

  // Print all descriptors
  sc.print_all_descriptors();
}

MAIN( test_sift_curve )
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


  SUMMARY();
}
