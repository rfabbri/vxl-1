//: This is dbmsh3dr_icp_rgrl.cxx
//  Ming-Ching Chang
//  Apr 09, 2007.

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <rsdl/rsdl_kd_tree.h>

#include <rrel/rrel_muset_obj.h>
#include <rrel/rrel_tukey_obj.h>

#include <rgrl/rgrl_feature_based_registration.h>
#include <rgrl/rgrl_feature_set.h>
#include <rgrl/rgrl_feature_set_location.h>
#include <rgrl/rgrl_initializer_prior.h>
#include <rgrl/rgrl_matcher_k_nearest.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_est_affine.h>
#include <rgrl/rgrl_est_rigid.h>

#include <rgrl/rgrl_feature_face_pt.h>
#include <rgrl/rgrl_scale_est_all_weights.h>
#include <rgrl/rgrl_scale_est_closest.h>
#include <rgrl/rgrl_weighter_m_est.h>
#include <rgrl/rgrl_convergence_on_weighted_error.h>
#include <rgrl/rgrl_match_set.h>

#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_converge_status.h>
#include <rgrl/rgrl_data_manager.h>

#include <rgrl/rgrl_object.h>
#include <rgrl/rgrl_event.h>
#include <rgrl/rgrl_command.h>

#include <dbmsh3d/algo/dbmsh3d_pt_normal.h>
#include <dbmsh3dr/dbmsh3dr_match.h>

// using command/observer pattern
class command_iteration_update: public rgrl_command
{
 public:
  void execute(rgrl_object* caller, const rgrl_event & event )
  {
    execute( (const rgrl_object*) caller, event );
  }

  void execute(const rgrl_object* caller, const rgrl_event & /*event*/ )
  {
    const rgrl_feature_based_registration* reg_engine =
      dynamic_cast<const rgrl_feature_based_registration*>(caller);
    rgrl_transformation_sptr trans = reg_engine->current_transformation();
    rgrl_trans_rigid* r_xform = rgrl_cast<rgrl_trans_rigid*>(trans);
    vcl_cout<<"xform: R = "<<r_xform->R()<<"t = "<<r_xform->t()<<vcl_endl;
  }
};

//: registration result is the best affine transformation and
//  the error of convergence.
//  return true if has final transform.
bool rgrl_rigid_icp_register (const vcl_vector<rgrl_feature_sptr>& fixedFV, 
                              const vcl_vector<rgrl_feature_sptr>& movingFV,
                              const vnl_matrix<double>& init_R, 
                              const vnl_vector_fixed<double,3>& init_t,
                              rgrl_trans_rigid& r_xform, double& error)
{
  vul_printf (vcl_cout, "rgrl_rigid_icp_register(): %d points to %d points.\n",
              movingFV.size(), fixedFV.size());
  
  const unsigned int dimension = 3;
  rgrl_feature_set_sptr moving_feature_set = new rgrl_feature_set_location<dimension>(movingFV);
  rgrl_feature_set_sptr fixed_feature_set = new rgrl_feature_set_location<dimension>(fixedFV);

  // Transformation estimator: use rigid, (affine: rgrl_est_affine).
  rgrl_estimator_sptr estimator = new rgrl_est_rigid (3);

  //set the initial transformation to init_hmatrix, default: identity
  rgrl_transformation_sptr init_trans = new rgrl_trans_rigid (init_R, init_t);

  //Initializer
  const rgrl_mask_box moving_image_region = moving_feature_set->bounding_box();
  rgrl_mask_sptr movingIM_sptr = new rgrl_mask_box (moving_image_region.x0(), moving_image_region.x1()); 
  const rgrl_mask_box fixed_image_region = fixed_feature_set->bounding_box();
  rgrl_mask_sptr fixedIM_sptr = new rgrl_mask_box (fixed_image_region.x0(), fixed_image_region.x1());
  rgrl_initializer_sptr initializer = new rgrl_initializer_prior( movingIM_sptr,
                                              fixedIM_sptr, estimator, init_trans);

  //Matcher
  unsigned int k = 1;
  rgrl_matcher_sptr cp_matcher = new rgrl_matcher_k_nearest( k );

  //Turn on debug info  cp_matcher->set_debug_flag (3);

  //Weighter
  vcl_auto_ptr<rrel_m_est_obj>  m_est_obj( new rrel_tukey_obj(4) );
  rgrl_weighter_sptr wgter = new rgrl_weighter_m_est(m_est_obj, false, false);

  //Scale estimator
  int max_set_size = 1000;  //maximum expected number of features
  vcl_auto_ptr<rrel_objective> muset_obj( new rrel_muset_obj( max_set_size , false) );

  rgrl_scale_estimator_unwgted_sptr unwgted_scale_est;
  rgrl_scale_estimator_wgted_sptr wgted_scale_est;

  unwgted_scale_est = new rgrl_scale_est_closest( muset_obj );
  wgted_scale_est = new rgrl_scale_est_all_weights();

  //convergence tester
  double tolerance = 0.1;
  rgrl_convergence_tester_sptr conv_test =
    new rgrl_convergence_on_weighted_error( tolerance );

  // BeginCodeSnippet
  rgrl_data_manager_sptr data = new rgrl_data_manager();
  data->add_data( moving_feature_set,   // data from moving image
                  fixed_feature_set,    // data from fixed image
                  cp_matcher,           // matcher for this data
                  wgter,                // weighter
                  unwgted_scale_est,    // unweighted scale estimator
                  wgted_scale_est );    // weighted scale estimator

  rgrl_feature_based_registration reg( data, conv_test );
  reg.set_expected_min_geometric_scale( 0.01/1000 );
  reg.add_observer( new rgrl_event_iteration(), new command_iteration_update());

  reg.run( initializer );
  // EndCodeSnippet

  if (reg.has_final_transformation()) { //return registration results.   
    rgrl_transformation_sptr final_trans = reg.final_transformation();
    rgrl_trans_rigid* rxform = rgrl_cast<rgrl_trans_rigid*>(final_trans);
    r_xform = (*rxform);
    error = reg.final_status()->error();
    return true;
  }
  else
    return false;
}


