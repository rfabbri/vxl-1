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

  mw_sift_curve_algo computor(image);

  dbdet_edgel_chain crv;
  mw_sift_curve sc;

  // corner case: empty curve -- should work
  vcl_cout << "\n--- Testing empty curve case ---\n";
  computor.compute(crv, &sc);

  print_all(computor, sc);

  TEST("Sanity check - number of samples", sc.num_samples(), crv.edgels.size());

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

  computor.compute(crv, &sc);

  print_all(computor, sc);

  TEST("Sample 1 non-empty? ", sc.is_valid(0, 0), true);
  TEST("Sample 2 non-empty? ", sc.is_valid(0, 1), true);
  TEST("Sample 3 empty? ", sc.is_valid(0, 2), false);

  // the same compute object can be used to compute descriptors for other curves
  // within the same image

  dbdet_edgel_chain crv2;
  mw_sift_curve sc2;

  computor.compute(crv2, &sc2);

  // Unambigous nearest-neighbor matching

  for (unsigned s=0; s < sc.num_scales(); ++s) {
    for (unsigned i=0; i < sc.num_samples(); ++i) {
      mw_sift_curve_algo::t_descriptor_float
        d = mw_sift_curve_algo::unambigous_nneighbor(sc, sc.descriptor(s,i));
      if (i == 2) {
        TEST("Sanity check - nn on itself w/invalid gives inf", d, vcl_numeric_limits<mw_sift_curve_algo::t_descriptor_float>::infinity());
        vcl_cout << "   d = " << d << vcl_endl;
      } else {
        TEST_NEAR("Sanity check - nn on itself gives 0 distance", d, 0, 1e-8);
      }
    }
  }

  // Compute all curves at the same time. Should save speed by processing in scale-order first,
  // reusing the gradient computed at each octave

  vcl_cout << "\n--- Testing compute_many ---\n";
  vcl_vector<dbdet_edgel_chain> ec_v;
  ec_v.push_back(crv);
  ec_v.push_back(crv2);

  vcl_vector<mw_sift_curve> sc_v;
  sc_v.push_back(sc);
  sc_v.push_back(sc2);

  computor.compute_many(ec_v, &sc_v);

  for (unsigned i=0; i < ec_v.size(); ++i) {
    TEST("Sanity check - number of samples", sc_v[i].num_samples(), ec_v[i].edgels.size());
  }

  TEST("Sample 1 non-empty? ", sc_v[0].is_valid(0, 0), true);
  TEST("Sample 2 non-empty? ", sc_v[0].is_valid(0, 1), true);
  TEST("Sample 3 empty? ", sc_v[0].is_valid(0, 2), false);


  // repeat the tests

  // Match all descriptors to all the others.
  // Best match for each descriptor should be itself,
  // with zero distance
//  for (unsigned i=0; i < descriptors.size(); ++i) {
//    for (unsigned k=0; k < descriptors.size(); ++k) {
//    }
//  }

  // Removing ambiguous matches
  //
  // A little test - match each hog to all the others within the same
  // curve, finding the nearest neighbor (different than itself) and second
  // nearest neighbor. If the ratio is larger than some threshold, such as used
  // in sift matching, then keep the feature.
  //
  // Alternatively, a simpler test:  keep only the features whose cost of
  // matching with any other feature withing the curve is larger than some
  // threshold. This should give corner points and the like.


  // Translation invariance

  // Given a query curve, find it in a set of curves detected on a slightly
  // different viewpoint.

  SUMMARY();
}
