// This is brl/bseg/dbinfo/dbinfo_observation_generator.h
#ifndef dbinfo_observation_generator_h_
#define dbinfo_observation_generator_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a generator for observations
//
// \author
//  J.L. Mundy - April 10, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <dbinfo/dbinfo_observation_sptr.h>


class dbinfo_observation_generator 
{
 public:
  //Constructors/destructor
  ~dbinfo_observation_generator();

  //: generate a set of randomly placed observations about a seed observation
  static bool uniform_about_seed(const unsigned nobs,
                                 dbinfo_observation_sptr const& seed,
                                 vcl_vector<dbinfo_observation_sptr>& obs,
                                 const float dx, const float dy,
                                 const float dtheta = 0,
                                 const float dscale = 0,
                                 const float sufficient_points_thresh = 0.9f);
  

  //: generate a set of randomly placed observations about a seed observation
  // return the corresponding transforms as well
  static bool uniform_about_seed(const unsigned nobs,
                                 dbinfo_observation_sptr const& seed,
                                 vcl_vector<dbinfo_observation_sptr>& obs,
                                 vcl_vector<vgl_h_matrix_2d<float> >& xforms,
                                 const float dx, const float dy,
                                 const float dtheta = 0,
                                 const float dscale = 0,
                                 const float sufficient_points_thresh = 0.9f);

static bool uniform_about_seed_affine_no_skew(const unsigned nobs,
                                              dbinfo_observation_sptr const& seed,
                                              vcl_vector<dbinfo_observation_sptr>& obs,
                                              const float dx, const float dy,
                                              const float dtheta=0, const float dscale=0,
                                              const float daspect=0,
                                              const float sufficient_points_thresh=0.9f);

  static 
bool uniform_about_seed_affine_no_skew(const unsigned nobs,
                                       dbinfo_observation_sptr const& seed,
                                       vcl_vector<dbinfo_observation_sptr>& obs,
                                       vcl_vector<vgl_h_matrix_2d<float> >& xforms,
                                       const float dx, const float dy,
                                       const float dtheta=0,
                                       const float dscale=0,
                                       const float daspect=0,
                                       const float sufficient_points_thresh=0.9f);
static bool uniform_in_interval_affine_no_skew(const unsigned nobs,
                                   dbinfo_observation_sptr const& seed,
                                   vcl_vector<dbinfo_observation_sptr>& obs,
                                   const float x_min, const float x_max,
                                   const float y_min, const float y_max,
                                   const float theta_min,
                                   const float theta_max,
                                   const float scale_min,
                                   const float scale_max,
                                   const float aspect_min,
                                   const float aspect_max,
                                               const float sufficient_points_thresh=0.9f);

static bool
uniform_in_interval_affine_no_skew(const unsigned nobs,
                                   dbinfo_observation_sptr const& seed,
                                   vcl_vector<dbinfo_observation_sptr>& obs,
                                   vcl_vector<vgl_h_matrix_2d<float> >& xforms,
                                   const float x_min, const float x_max,
                                   const float y_min, const float y_max,
                                   const float theta_min,
                                   const float theta_max,
                                   const float scale_min,
                                   const float scale_max,
                                   const float aspect_min,
                                   const float aspect_max,
                                   const float sufficient_points_thresh=0.9f);


  //: generate a single observation from a seed and a transform
 static dbinfo_observation_sptr 
   generate(dbinfo_observation_sptr const& seed, 
            vgl_h_matrix_2d<float> const& H,
            const float sufficient_points_thresh = 0.9f);
                                   
 protected:
  dbinfo_observation_generator();// static methods
};

#endif // dbinfo_observation_generator_h_
