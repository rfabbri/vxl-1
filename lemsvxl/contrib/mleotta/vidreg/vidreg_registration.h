// This is contrib/mleotta/vidreg/vidreg_registration.h
#ifndef vidreg_registration_h_
#define vidreg_registration_h_
//:
// \file
// \brief A registration routine 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 7/27/06
//
// Based on feature-based registration by Charlene Tsai
//
// \verbatim
//  Modifications
// \endverbatim


#include <rgrl/rgrl_object.h>
#include <rgrl/rgrl_data_manager_sptr.h>
#include <rgrl/rgrl_convergence_tester_sptr.h>
#include <rgrl/rgrl_estimator_sptr.h>
#include <rgrl/rgrl_transformation_sptr.h>
#include <rgrl/rgrl_set_of.h>
#include <rgrl/rgrl_scale_sptr.h>
#include <rgrl/rgrl_initializer_sptr.h>
#include <rgrl/rgrl_converge_status_sptr.h>
#include <rgrl/rgrl_match_set_sptr.h>
#include <rgrl/rgrl_weighter_sptr.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_view_sptr.h>
#include <vidreg/vidreg_salient_group_sptr.h>

//: Feature-based registration
//
//  The registration engine takes a data storage of single- or
//  multiple-stage/resolution. Each stage may contain multiple feature
//  types. Different transformation estimators may be assigned to
//  different stages. The limitations of feature-based registration
//  are fixed registration region and estimator at each stage.
//
class vidreg_registration: public rgrl_object
{
 public:
  //: Initialize with the data storage and the convergence tester
  vidreg_registration( rgrl_data_manager_sptr data,
                       rgrl_convergence_tester_sptr conv_tester );
  //: Initialize with the data storage.
  //
  //  The default convergence tester is set to median error
  vidreg_registration( rgrl_data_manager_sptr data );

  ~vidreg_registration();

  // Defines type-related functions
  rgrl_type_macro( vidreg_registration, rgrl_object );

  
  //:  Clear record of results from previous runs
  void clear_results();

  //: Running from multiple initial estimates, produced by the initializer during registration
  //
  //  Loop through the set of initial estimates.
  void run( rgrl_initializer_sptr initializer );

  //: Running from a given initial estimate.
  //
  //  Based on if data_->is_multi_feature(), call run_single_feature(.)
  //  or run_multi_feature(.)
  //
  void run( rgrl_view_sptr            prior_view,
            rgrl_scale_sptr           prior_scale = 0 );

  //////////////// functions to access internal data  ////////////////////////

  //: Return true if a salient group is found
  bool has_salient_group() const;

  //: Return the salient group
  vidreg_salient_group_sptr salient_group() const;

  //:  Return the number of initial transformations tested
  //
  //   The value might be greater than 1 if using initializer which
  //   provides a number of initial estimates.
  unsigned num_initial_xforms_tested() const;

  //: Set the max number of icp iteration per stage
  //
  //  The default is 25 at initialization
  void set_max_icp_iter( unsigned iter );

  //: Set the expected maximum geometric scale
  //
  //  If \a expected_max_geometric_scale_ is never set, the default
  //  has no effect on the registration.
  void set_expected_max_geometric_scale( double scale);

  //: Set the expected minimum geometric scale
  //
  //  If \a expected_min_geometric_scale_ is never set, the default
  //  has no effect on the registration.
  void set_expected_min_geometric_scale( double scale);

  //: Set the number of iterations during which the scale is to be estimated
  //
  //  If \a iterations_for_scale_est_ is never set, the default
  //  has no effect on the registration.
  void set_iterations_for_scale_est( int iter);

  //: penalize transformation that involves scaling of the registraion area
  void penalize_scaling( bool );

  //: Return the current match sets
  rgrl_set_of<rgrl_match_set_sptr>  const& current_match_sets() const;

  //:  Return the current view
  rgrl_view_sptr current_view() const;

  //:  Return the current stage
  unsigned current_stage() const;

  //:  Return the current iterations_at_stage_
  unsigned iterations_at_current_stage() const;

 private:
   //: Running from a given initial estimate.
   //
   //  Based on if data_->is_multi_feature(), call run_single_feature(.)
   //  or run_multi_feature(.)
   //
   virtual void run_helper( rgrl_view_sptr            prior_view,
                            rgrl_scale_sptr           prior_scale );

  //: registration of single feature type at each stage/resolution
   virtual void register_single_feature( rgrl_view_sptr           initial_view,
                                         rgrl_scale_sptr          scale );

  //: registration of multiple feature type at each stage/resolution
   virtual void register_multi_feature( rgrl_view_sptr           initial_view,
                                        rgrl_scale_sptr          prior_scale );

   //: Try to expand the view
   rgrl_view_sptr expand_view(const rgrl_view_sptr& view);


  //: Scale/shrink the registration region and the \a xform_est according to the \a dim_increase_for_next_stage at \a current_resol
  void initialize_for_next_resolution(  rgrl_mask_box            & from_image_region,
                                        rgrl_mask_box            & to_image_region,
                                        rgrl_transformation_sptr & xform_estimate,
                                        unsigned                 & current_resol ) const;

 private:

  rgrl_data_manager_sptr data_;
  rgrl_data_manager_sptr current_data_;
  rgrl_convergence_tester_sptr conv_tester_;

  vidreg_salient_group_sptr    best_salient_group_;

  unsigned num_xforms_tested_;
  unsigned max_icp_iter_;
  double   expected_max_geometric_scale_; //for early termination
  double   expected_min_geometric_scale_;
  int      iterations_for_scale_est_;
  bool     should_penalize_scaling_;

  // data members to keep track of current status
  unsigned                          current_stage_;
  unsigned                          iterations_at_stage_;
  rgrl_set_of<rgrl_scale_sptr>      current_scales_;
  rgrl_set_of<rgrl_match_set_sptr>  current_match_sets_;
  rgrl_view_sptr                    current_view_;
};

#endif // vidreg_registration_h_
