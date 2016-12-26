// This is brl/bseg/dbinfo/dbinfo_match_optimizer.h

#ifndef dbinfo_match_optimizer_h_
#define dbinfo_match_optimizer_h_

//---------------------------------------------------------------------
//:
// \file
// \brief A class for refining the transformation aligning two observations
//
// \author
//  J.L. Mundy - September 30, 2005
//
// \verbatim
//  Modifications
//  O. C. Ozcanli - January 23, 2005 : commented out assertion in dbinfo_cost_func::f(vnl_vector<double> const& x)
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_cost_function.h>
#include <vil/vil_image_resource_sptr.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
class dbinfo_match_optimizer;


//: A least squares cost function for registering video frames
// by minimizing square difference in intensities
class dbinfo_cost_func : public vnl_cost_function
{
public:
  //: Constructors/Destructor

  dbinfo_cost_func(const unsigned number_of_parameters);

  ~dbinfo_cost_func(){}
  
  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  fx has been sized appropriately before the call.
  virtual double f(vnl_vector<double> const& x);

 
  void set_max_info(const float max_info){max_info_ = max_info;}
  void set_frame(const unsigned frame){frame_ = frame;}
  void set_resource(vil_image_resource_sptr const& resc){resc_ = resc;}
  void set_obs0(dbinfo_observation_sptr const & obs0){obs0_ = obs0;}
  void set_initial(dbinfo_observation_sptr const & obs)
    {initial_obs_ = obs; current_obs_ = obs;}
  void set_radius(const double radius){radius_=radius;}
  void set_thresh(const float thresh){sufficient_points_thresh_=thresh;}
  void set_debug_level(const unsigned level){debug_level_=level;}
  void set_forced_scale(const float s){forced_scale_=s;}
  void set_use_intensity(bool use_int){use_intensity_ = use_int;}
  void set_use_gradient(bool use_grad){use_gradient_ = use_grad;}
  dbinfo_observation_sptr current_obs(){return current_obs_;}

protected:

  //: number of transform parameters
  unsigned no_params_;

  //: current frame
  unsigned frame_;

  //:current frame image
  vil_image_resource_sptr resc_;
  
  //: maximum mutual information
  float max_info_;
  //:observation at start of track
  dbinfo_observation_sptr obs0_;

  //:observation from prior optimization update
  dbinfo_observation_sptr initial_obs_;

  //:observation from prior optimization update
  dbinfo_observation_sptr current_obs_;
  
  //:the radius of the tracking region
  double radius_;
  //:The fraction of points necessary to compute information
  float sufficient_points_thresh_;
  //: debug level
  unsigned debug_level_;
  //: for debug
  float forced_scale_;
  //: include intensity mutual informationin match score
  bool use_intensity_;
  //: include gradient dir mutual informationin match score
  bool use_gradient_;
 private:
  //:not sensible
  dbinfo_cost_func();
  //
};

class dbinfo_match_optimizer : public vbl_ref_count
{

 public:
  enum trans_spec
    {
      TRANS_SPEC_UNKNOWN = 0,
      TRANSLATION = 1,
      TRANSLATION_ROTATION = 2,
      TRANSLATION_ROTATION_SCALE = 3
    };


  //:defaults 
  dbinfo_match_optimizer();
  dbinfo_match_optimizer(const float max_info, 
                         const float dx, 
                         const float dtheta = 0,
                         const float dscale = 0);

  ~dbinfo_match_optimizer(){}

  //:initialization/access
  void set_frame(const unsigned frame){frame_ = frame;}
  void set_resource(vil_image_resource_sptr const& resc){resc_ = resc;}
  void set_obs0(dbinfo_observation_sptr const & obs0);
  void set_best_seed(dbinfo_observation_sptr const & seed){best_seed_ = seed;}
  trans_spec transformation_type(){return spec_;}
  unsigned no_of_parameters(){return no_params_;}
  dbinfo_observation_sptr optimized_obs(){return optimized_obs_;}
  void set_debug_level(const unsigned level){debug_level_ = level;}
  void set_fraction(const double fraction){fraction_ = fraction;}
  void set_thresh(const float thresh){sufficient_points_thresh_ = thresh;}
  void set_forced_scale(const float s){forced_scale_=s;}
  void set_use_intensity(bool use_int){use_intensity_ = use_int;}
  void set_use_gradient(bool use_grad){use_gradient_ = use_grad;}
  //:the main function
  bool optimize();
  //: The current set of parameters (un-normalized)
  vcl_vector<double> current_params();  
  //: the current transform corresponding to the parameters
  vgl_h_matrix_2d<double> current_transform();
  //: The optimization cost value
  double current_cost(){return current_cost_;}  
 protected:
  //:debug level
  unsigned debug_level_;

  //: Maximum value that the combined mutual information can attain
  float max_info_;

  //:The transform parameters
  float dx_, dtheta_, dscale_;

  //:The number of parameters
  unsigned no_params_;

  //:The parameter vector
  vnl_vector<double> x_;

  //:the current cost value_;
  double current_cost_;

  //:the class of transformation
  trans_spec spec_;

  //: current frame
  unsigned frame_;

  //:current frame image
  vil_image_resource_sptr resc_;

  //:observation at start of track
  dbinfo_observation_sptr obs0_;

  //:best observation from current track seeds
  dbinfo_observation_sptr best_seed_;

  //: final optimized result
  dbinfo_observation_sptr optimized_obs_;

  //:the radius of the tracking region
  double radius_;

  //:Radius fraction for intializing the simplex
  double fraction_;

  //:Threshold on fraction of total points sufficient to compute information
  float sufficient_points_thresh_;
  //: For debug
  float forced_scale_;
  //: include intensity mutual informationin match score
  bool use_intensity_;
  //: include gradient dir mutual informationin match score
  bool use_gradient_;
};

#endif // dbinfo_match_optimizer_h_
