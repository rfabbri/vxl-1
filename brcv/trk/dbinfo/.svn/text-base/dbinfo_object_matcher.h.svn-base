// This is brcv/trk/dbinfo/dbinfo_object_matcher.h
#ifndef dbinfo_object_matcher_h_
#define dbinfo_object_matcher_h_
//---------------------------------------------------------------------
//:
// \file
// \brief A class for static object matching methods
// 
// \author
//  O. C. Ozcanli - November 08, 2005
//
// \verbatim
//  Modifications
//
//   Amir Tamrakar   Nov 31 2005      Added more methods by breaking up the
//                                    original methods to get curve and region
//                                    correspondences from observations
//
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_utility.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_array_1d.h>
#include <vil/vil_image_resource_sptr.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>

class dbinfo_object_matcher : public vbl_ref_count
{
 protected:
  //: no default constructor, static methods only
  dbinfo_object_matcher();

 public:
  //:default destructor
  ~dbinfo_object_matcher(){}

  //: For debugging, make a image from an observation
static  vil_image_resource_sptr make_image(dbinfo_observation_sptr obs,
                                           int w, int h, 
                                           double translation_x,
                                           double translation_y, bool background_noise = true);
 static vil_image_resource_sptr 
   make_alignment_image(vil_image_resource_sptr const& image_q, 
                        dbinfo_observation_sptr const&  final_obs);

 static float
   minfo_alignment_in_interval(dbinfo_observation_sptr obsq, 
                               dbinfo_observation_sptr obsdb,
                               const float xmin, const float xmax,
                               const float ymin, const float ymax, 
                               const float theta_min, const float theta_max,
                               const float scale_min, const float scale_max,
                               const float aspect_min, const float aspect_max,
                               const unsigned n_intervals,
                               const float valid_thresh,
                               vil_image_resource_sptr& alignment_image);

  //: find the rigid transformation that aligns observation0 to 
  // observationi such that mutual information is maximised and 
  // return this max mutual info
  static float minfo_rigid_alignment_rand(dbinfo_observation_sptr obs0, 
                                          dbinfo_observation_sptr obsi, 
                                          float dx, float dr, float ds, float ratio, unsigned Nob,
                                          vil_image_resource_sptr& image_r1,
                                          vil_image_resource_sptr& image_r2,
                                          vil_image_resource_sptr& image_r3,
                                          bool verbose = false,
                                          float forced_scale = 1.0f,
                                          bool use_intensity = true,
                                          bool use_gradient = true);
  
static float minfo_rigid_alignment_rand(dbinfo_observation_sptr obsq, 
                                        dbinfo_observation_sptr obsdb, 
                                        vgl_h_matrix_2d<float> & best_trans,
                                        float dx, float dr, float ds, float da,
                                        float ratio, float valid_thresh, unsigned Nob,
                                        vil_image_resource_sptr& image_match,
                                        bool verbose = false,
                                        float forced_scale = 1.0f,
                                        bool use_intensity = true,
                                        bool use_gradient = true);

static float minfo_rigid_alignment_rand(dbinfo_observation_sptr obsq, 
                                         dbinfo_observation_sptr obsdb, 
                                         vgl_h_matrix_2d<float> & best_trans,
                                         float dx, float dr, float ds, float ratio, unsigned Nob,
                                         vil_image_resource_sptr& image_q,
                                         vil_image_resource_sptr& image_rdb,
                                         vil_image_resource_sptr& image_match,
                                         bool verbose,
                                         float forced_scale,
                                         bool use_intensity,
                                         bool use_gradient); 
 
};

#endif // dbinfo_object_matcher_h_
