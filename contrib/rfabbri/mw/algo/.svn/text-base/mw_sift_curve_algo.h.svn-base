// This is mw_sift_curve_algo.h
#ifndef MW_SIFT_CURVE_ALGO_H
#define MW_SIFT_CURVE_ALGO_H
//:
//\file
//\brief Code supporting SIFT descriptors attached to curve fragments
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 11/30/08 20:48:48 EST
//

#include <vcl_cmath.h>
#include <vcl_limits.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_vector_fixed.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <mw/mw_sift_curve.h>
#include <mw/algo/siftpp/sift.hpp>

// Functions to build and manipulate SIFT descriptors attached to curve fragments
class mw_sift_curve_algo {
public:

typedef VL::float_t t_descriptor_float;

#define MW_SIGMAN .5
#define MW_LEVELS 1 // number of levels per octave
#define MW_OMIN -1

  // Parameters used in reality:
  //
  // float const sigman = .5 ; //< small presmoothing
  // int   levels       = 3 ;
  // Number of levels per octave
  // int const   omin   = -1 ; //< first
  //
  // construct using default parameters
  // Initialize Gaussian scale space parameters
  // 
  // _im_pt  Source image data
  // _width  Source image width
  // _height Source image height
  // _sigman Nominal smoothing value of the input image.
  // _sigma0 Base smoothing level.
  // _O      Number of octaves.
  // _S      Number of levels per octave.
  // _omin   First octave.
  // _smin   First level in each octave.
  // _smax   Last level in each octave.
   
  // Set defaults as to mimmick the matlab version
  mw_sift_curve_algo(const vil_image_view< float > &image_view)
    :
    sift_(image_view.top_left_ptr(), 
          image_view.ni(), 
          image_view.nj(),
          MW_SIGMAN,  //< sigman
          1.6 * powf(2.0f, 1.0f / MW_LEVELS), //< Base smoothing level as I use in matlab
          1,
  //          vcl_max
  //          (int
  //           (vcl_floor
  //            (log2
  //             (vcl_min(image_view.ni(),image_view.nj()))) - MW_OMIN -3), 1), //< O, downsample up to 8x8 images
          MW_LEVELS, //< S
          MW_OMIN, //< omin
          -1, // smin as I use in matlab
          MW_LEVELS+1 // smax as I use in matlab
          )
  { }

#undef MW_SIGMAN
#undef MW_LEVELS
#undef MW_OMIN

  ~mw_sift_curve_algo()
  {}

  // returns the sigma values for each scale
  void get_sigmas(vcl_vector<double> *sigmas) const;
 
  void print_sigmas() const {
    vcl_cout << "sigmas: " ;
    vcl_vector<double> sigmas;
    get_sigmas(&sigmas);
    if (sigmas.size()) {
      unsigned i=0;
      for (; i+1 < sigmas.size(); ++i)
        vcl_cout << sigmas[i] << ", ";
      vcl_cout << sigmas[i] << ";\n";
    } else {
      vcl_cout << "empty\n";
    }
  }

  void compute( const dbdet_edgel_chain &crv, mw_sift_curve *s_crv);

  void compute_many(const vcl_vector<dbdet_edgel_chain> &ec_v, vcl_vector<mw_sift_curve> *ptr_sc_v);

  static t_descriptor_float unambigous_nneighbor(
      const mw_sift_curve &sc, 
      const vnl_vector_fixed <t_descriptor_float, MW_SIFT_CURVE_NUM_DIMS> &descr) 
  {
    t_descriptor_float dbest = vcl_numeric_limits<t_descriptor_float>::infinity();
    t_descriptor_float dsecond_best = vcl_numeric_limits<t_descriptor_float>::infinity();
    for (unsigned s=0; s < sc.num_scales(); ++s) {
      for (unsigned i=0; i < sc.num_samples(); ++i) {
        // compute the distance of this descriptor to the other
        if (sc.is_valid(s, i)) {
          t_descriptor_float d = vnl_vector_ssd<t_descriptor_float>(descr, sc.descriptor(s, i));
          if (d < dbest) {
            dbest = d;
          } else {
            if (d < dsecond_best)
              dsecond_best = d;
          }
        }
      }
    }
    // Lowe's criterion: accept the match only if unique
    t_descriptor_float thresh=1.5;
    if(dbest != vcl_numeric_limits<t_descriptor_float>::infinity() && thresh * dbest > dsecond_best)
      return vcl_numeric_limits<t_descriptor_float>::infinity();
    return dbest;
  }

private:
  VL::Sift sift_;
};


#endif // MW_SIFT_CURVE_ALGO_H
