// This is contrib/mleotta/vidreg/vidreg_registration.cxx
#include "vidreg_registration.h"
//:
// \file
#include <rgrl/rgrl_initializer.h>
#include <rgrl/rgrl_convergence_tester.h>
#include <rgrl/rgrl_converge_status.h>
#include <rgrl/rgrl_estimator.h>
#include <rgrl/rgrl_scale_estimator.h>
#include <rgrl/rgrl_weighter_unit.h>
#include <rgrl/rgrl_feature_set_location.h>
#include <rgrl/rgrl_matcher.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_scale_est_null.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_data_manager.h>
#include <rgrl/rgrl_scale.h>
#include <rgrl/rgrl_util.h>
#include <rgrl/rgrl_convergence_on_median_error.h>
#include <rgrl/rgrl_event.h>
#include <rgrl/rgrl_view.h>
#include <vcl_cassert.h>
#include <vcl_limits.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h>
#include <vidreg/vidreg_salient_group.h>
#include <vidreg/vidreg_initializer.h>

#include <rrel/rrel_tukey_obj.h>
#include <vul/vul_timer.h>

#include <vnl/vnl_transpose.h>

vidreg_registration::
vidreg_registration( rgrl_data_manager_sptr data,
                     rgrl_convergence_tester_sptr conv_tester )
  :data_( data ),
   current_data_( data ),
   conv_tester_( conv_tester ),
   num_xforms_tested_( 0 ),
   max_icp_iter_(25),
   expected_max_geometric_scale_ (0),
   expected_min_geometric_scale_ (0),
   iterations_for_scale_est_(-1),
   should_penalize_scaling_(false),
   current_stage_( 0 )
{
}

vidreg_registration::
vidreg_registration( rgrl_data_manager_sptr data )
  :data_( data ),
   current_data_( data ),
   num_xforms_tested_( 0 ),
   max_icp_iter_(25),
   expected_max_geometric_scale_ (0),
   expected_min_geometric_scale_ (0),
   iterations_for_scale_est_(-1),
   should_penalize_scaling_(false)
{
  double tolerance = 1.0;
  conv_tester_ = new rgrl_convergence_on_median_error( tolerance );
}

vidreg_registration::
~vidreg_registration()
{
}

void
vidreg_registration::
clear_results()
{
  num_xforms_tested_ = 0;
  best_salient_group_ = 0;
}

//: Running from multiple initial estimates, produced by the initializer during registration.
//  Loop through the set of initial estimates, and call the next \a run(.) in the loop.
void
vidreg_registration::
run( rgrl_initializer_sptr initializer )
{
  //  Clear previous results
  this->clear_results();

  rgrl_view_sptr                    initial_view;
  rgrl_scale_sptr                   prior_scale;

  while( initializer->next_initial( initial_view, prior_scale ) ) {

    DebugMacro(  1, "Try "<< num_xforms_tested_ << " initial estimate \n" );

    rgrl_mask_box initial_region = initial_view->region();
    vnl_vector<double> center(2);
    center[0] = (initial_region.x0()[0] + initial_region.x1()[0])/2.0;
    center[1] = (initial_region.x0()[1] + initial_region.x1()[1])/2.0;

    this->run_helper( initial_view, prior_scale );

    if ( best_salient_group_ && best_salient_group_->status()->is_good_enough() ){
      best_salient_group_->set_growth_center(center);
      break;
    }

    ++num_xforms_tested_;
  }

}

//: Running from a given initial estimate. 
//
//  Based on if data_->is_multi_feature(), call run_single_feature(.)
//  or run_multi_feature(.)
//
void
vidreg_registration::
run( rgrl_view_sptr   prior_view,
     rgrl_scale_sptr  prior_scale )
{
  //  Clear previous results
  this->clear_results();

  run_helper( prior_view, prior_scale );
}

//: Running from a given initial estimate. 
//
//  Based on if data_->is_multi_feature(), call run_single_feature(.)
//  or run_multi_feature(.)
//
void
vidreg_registration::
run_helper( rgrl_view_sptr   prior_view,
            rgrl_scale_sptr  prior_scale )
{
  if ( data_->is_multi_feature() ) {
    DebugMacro( 1, " Multi-feature Registration: \n" );
    this->register_multi_feature( prior_view, prior_scale );
  }
  else {
    DebugMacro( 1, " Single-feature Registration: \n" );
    this->register_single_feature( prior_view, prior_scale );
  }
}



//////////////// functions to access internal data  ////////////////////////

//: Return true if a salient group is found
bool
vidreg_registration::has_salient_group() const
{
  return (best_salient_group_!=NULL);
}


//: Return the salient group
vidreg_salient_group_sptr
vidreg_registration::salient_group() const
{
  return best_salient_group_;
}


//:  Return the number of initial transformations tested
unsigned
vidreg_registration::
num_initial_xforms_tested() const
{
  return num_xforms_tested_;
}


//: Set the max number of icp iteration per level
void
vidreg_registration::
set_max_icp_iter( unsigned iter )
{
   max_icp_iter_ = iter;
}

//: Set the expected max scale
void
vidreg_registration::
set_expected_max_geometric_scale( double scale)
{
  expected_max_geometric_scale_ = scale;
}

//: Set the expected min scale
void
vidreg_registration::
set_expected_min_geometric_scale( double scale)
{
  expected_min_geometric_scale_ = scale;
}

//: Set the number of iteration for scale estimation
void
vidreg_registration::
set_iterations_for_scale_est( int iter)
{
  iterations_for_scale_est_ = iter;
}

//: penalize transformation that involves scaling of the registraion area
void
vidreg_registration::
 penalize_scaling( bool penalize)
{
  should_penalize_scaling_ = penalize;
}


//: Return the current match sets
rgrl_set_of<rgrl_match_set_sptr>  const&
vidreg_registration::
current_match_sets() const
{
  return current_match_sets_;
}

//:  Return the current view
rgrl_view_sptr
vidreg_registration::
current_view() const
{
  return current_view_;
}

//:  Return the current stage
unsigned
vidreg_registration::
current_stage() const
{
  return current_stage_;
}

//:  Return the iteration at current stage
unsigned
vidreg_registration::
iterations_at_current_stage() const
{
  return iterations_at_stage_;
}


//////////////////// private functions //////////////////////////

//: registration of single feature type at each stage/resolution
void 
vidreg_registration::
register_single_feature( rgrl_view_sptr           initial_view,
                         rgrl_scale_sptr          scale )
{
  rgrl_converge_status_sptr         current_status;
  rgrl_feature_set_sptr             from_set;
  rgrl_feature_set_sptr             to_set;
  rgrl_matcher_sptr                 matcher;
  rgrl_scale_estimator_unwgted_sptr unwgted_scale_est;
  rgrl_scale_estimator_wgted_sptr   wgted_scale_est;
  rgrl_weighter_sptr                weighter;
  rgrl_match_set_sptr               match_set;
  vcl_vector<rgrl_estimator_sptr>   xform_estimators;
  rgrl_estimator_sptr               xform_estimator;
  bool                              failed, scale_in_range;
  unsigned                          prev_resol = 0;

  failed = false;
  scale_in_range = true;
  rgrl_view_sptr new_view = initial_view;

  assert ( current_data_->has_stage( initial_view->resolution() ) );

  do { // for each stage/resolution
    current_data_->get_data_at_stage( initial_view->resolution(),
                              from_set, to_set,
                              matcher,  weighter,
                              unwgted_scale_est, wgted_scale_est,
                              xform_estimators);
    match_set = 0;
    current_stage_ = initial_view->resolution();

    DebugMacro(  1, " Current resolution "<< initial_view->resolution() <<'\n' );

    // If no estimator found for the current stage, the default is
    // from the initializer. Feature_based can only deal with one
    // estimator in each stage/resolution. If more than one is found,
    // only the first is kept.
    //
    if ( xform_estimators.empty() )
      xform_estimator = initial_view->xform_estimator();
    else xform_estimator = xform_estimators[0];
    assert ( xform_estimator );

    iterations_at_stage_ = 0; //keeps track of total iter at stage
    current_status = 0;
    bool should_estimate_scale = true;
    int  scale_est_count = 0;

    do { // for each re-match

      current_view_ = new_view;
      DebugMacro(  2, " Computing matches and scales\n" );
      // Compute matches, and scales for each feature set.
      //
      match_set = matcher->compute_matches( *from_set,
                                            *to_set,
                                            *current_view_,
                                            *scale );

      current_match_sets_.clear();
      current_match_sets_.push_back( match_set );
      DebugMacro(  2, "      Matches: " << match_set->from_size() <<'\n' );

      // For the first iteration, use prior scale or estimate the
      // scales without weights (since we don't have any...)
      //
      if ( iterations_for_scale_est_ >= 0 &&
           scale_est_count > iterations_for_scale_est_ ) {
        should_estimate_scale = false;
      }

      rgrl_scale_sptr  new_scale = 0;
      if ( !should_estimate_scale ) {
        DebugMacro(  2, "No scale estimation\n" );
      }
      else if ( match_set->from_size() == 0 ) {
        DebugMacro(0, "vidreg_registration:: Empty match set!!!\n");
        failed = true;
        continue;
      }
      else { //should_estimate_scale && match_set->from_size() > 0
        if ( !wgted_scale_est ) {
          new_scale = unwgted_scale_est->
            estimate_unweighted( *match_set, scale, should_penalize_scaling_ );
        } else {
          if ( iterations_at_stage_ == 0 && !scale ) {
            assert ( unwgted_scale_est );
            new_scale = unwgted_scale_est->
              estimate_unweighted( *match_set, scale, should_penalize_scaling_);
          }
          else {
            weighter->compute_weights(*scale, *match_set);
            new_scale = wgted_scale_est->
              estimate_weighted( *match_set, scale, should_penalize_scaling_);
          }
        }

        // If the new scale exists, and the geometric scale is above
        // the lower bound, replace the old one by the new one
        if ( new_scale ) {
          DebugMacro( 2, "New geometric scale = "<<new_scale->geometric_scale()<<'\n' );
          if ( new_scale->has_geometric_scale() &&
               new_scale->geometric_scale() < expected_min_geometric_scale_ ) {
            should_estimate_scale = false;
            if (!scale) scale = new_scale;
            scale->set_geometric_scale(expected_min_geometric_scale_);
            DebugMacro( 2, "Scale below expected_min_geometric_scale. Set to expected_min_geometric_scale.\n" );
          }
          else {
            scale = new_scale;
          }
          scale_est_count++;
        }
      }
      DebugMacro( 2, "Current geometric scale = "<<scale->geometric_scale()<<'\n' );
      assert ( scale );

      // If the scale is above the upper bound of the expected
      // geometric scale return with failure
      if ( expected_max_geometric_scale_ > 0 &&
           scale->has_geometric_scale() &&
           scale->geometric_scale() > expected_max_geometric_scale_) {
        scale_in_range = false;
        failed = true;
        continue;
      }

      DebugMacro(  2, " Estimate the transformation\n" );

      // Transformation estimation using a simplified irls
      // (Iterative-Reweighted Least-Squares) routine.
      //
      // The match sets and the scales are fixed, but the associated
      // weights are updated (hence reweighted-least-squares)
      // throughout the estimation.
      //
      rgrl_transformation_sptr xform = current_view_->xform_estimate();
      if ( !rgrl_util_irls( match_set, scale, weighter,
                            *conv_tester_, xform_estimator,
                            xform,
                            false,                   // no fast mapping
                            this->debug_flag() ) ) {
        failed = true;
        continue; //no valid xform, so exit the loop
      }
      current_view_->set_xform_estimate(xform);

      new_view = expand_view(current_view_);

      // For debugging
      //
      this->invoke_event(rgrl_event_iteration());

      // Update the weights and scale estimates based on the new transform
      //
      DebugMacro(  2, " Updating scale estimates and checking for validity\n" );

      match_set->remap_from_features( *current_view_->xform_estimate() );
      weighter->compute_weights( *scale, *match_set );

#if 0 // do we need to do this?
      // compute the scaling factors  
      {
        bool ret_success;
        vnl_vector<double> scaling;
        ret_success = rgrl_util_geometric_scaling_factors( *match_set, scaling );
        if( ret_success ) 
          current_xform_estimate_->set_scaling_factors( scaling );
        else 
          WarningMacro( "cannot compute scaling factors!!!" );
      }
#endif

      // Perform convergence test
      //
      DebugMacro(  2, " Perform convergence test\n" );
      current_status =
        conv_tester_->compute_status( current_status,
                                      *current_view_, *new_view,
                                      match_set, scale,
                                      should_penalize_scaling_ );
      DebugMacro(  3, "run: (iterations = " << iterations_at_stage_
                   << ") oscillation count = " << current_status->oscillation_count() << '\n' );
      DebugMacro(  3, "run: error = " << current_status->error() << vcl_endl );
      DebugMacro(  3, "run: error_diff = " << current_status->error_diff() << vcl_endl );

      ++iterations_at_stage_;



    } while ( !failed &&
             !current_status->has_converged() &&
             !current_status->has_stagnated() &&
             iterations_at_stage_ < max_icp_iter_ );

    if ( failed ) {
      if ( !scale_in_range )
        DebugMacro(  1, " Geometric scale above the expected value\n" );
      else
        DebugMacro( 1, " Failed with empty match set, or irls estimation\n" );
      continue;
    }
    if ( current_status->has_converged() )
      DebugMacro(  1, " CONVERGED\n" );
    if ( current_status->has_stagnated() )
      DebugMacro(  1, " STAGNATED\n" );
    if ( iterations_at_stage_ == max_icp_iter_ )
      DebugMacro(  1, " ICP iteration reached maximum ("<<max_icp_iter_<<" )\n" );


    
    // Move to the next resolution with proper initialization if not
    // at the finest level
    //
    prev_resol = current_view_->resolution();
    //initialize_for_next_resolution( from_image_region, to_image_region, 
    //                                current_xform_estimate_, resolution );
    int level_diff = prev_resol - initial_view->resolution();
    double dim_increase = current_data_->dimension_increase_for_next_stage(prev_resol);
//  double scale_multipler = vcl_pow(dim_increase, level_diff);
    scale->set_geometric_scale( scale->geometric_scale()*
                                vcl_pow(dim_increase, level_diff) );

  } while ( !failed &&
             (!current_view_->resolution() == 0 ||
              !prev_resol == 0 ) );

  DebugMacro( 1, "Estimation complete\n" );

  //   At this point the iterations are over for this initial
  //   estimate.  If the estimate is successful, if it is the first
  //   one tested or it is the best seen thus far, record information
  //   about this estimate.
  //

  if ( !failed ){
    DebugMacro( 1, "Obj value after convergence = "<<current_status->objective_value()<<'\n');

    best_salient_group_ = new vidreg_salient_group(current_view_,
                                                    vcl_vector<rgrl_match_set_sptr>(1,match_set),
                                                    vcl_vector<rgrl_scale_sptr>(1,scale),
                                                    current_status);
    DebugMacro( 1, "Set best xform estimate\n" );

  }
}

//: registration of multiple feature type at each stage/resolution
void
vidreg_registration::
register_multi_feature( rgrl_view_sptr           initial_view,
                        rgrl_scale_sptr          prior_scale )
{
  rgrl_converge_status_sptr                     current_status;
  vcl_vector<rgrl_feature_set_sptr>             from_sets;
  vcl_vector<rgrl_feature_set_sptr>             to_sets;
  vcl_vector<rgrl_matcher_sptr>                 matchers;
  vcl_vector<rgrl_scale_estimator_unwgted_sptr> unwgted_scale_ests;
  vcl_vector<rgrl_scale_estimator_wgted_sptr>   wgted_scale_ests;
  vcl_vector<rgrl_weighter_sptr>                weighters;
  vcl_vector<rgrl_estimator_sptr>               xform_estimators;
  rgrl_estimator_sptr                           xform_estimator;
  bool                                          failed, scale_in_range, use_prior_scale;
  unsigned                                      prev_resol = 0;

  use_prior_scale = false;
  if ( prior_scale ) use_prior_scale = true;
  scale_in_range = true;
  failed = false;

  rgrl_view_sptr new_view = initial_view;

  assert ( current_data_->has_stage( initial_view->resolution() ) );

  do { // for each stage/resolution
    current_data_->get_data_at_stage( initial_view->resolution(),
                              from_sets, to_sets, matchers,
                              weighters, unwgted_scale_ests,
                              wgted_scale_ests, xform_estimators);
    current_stage_ = initial_view->resolution();
    unsigned data_count = from_sets.size();

    DebugMacro(  1, " Current resolution "<< initial_view->resolution() <<'\n' );

    // Initialize the size of match_sets and scales to be the same as
    // the from_sets
    //
    current_match_sets_.clear();
    current_scales_.clear();
    current_match_sets_.resize( data_count );
    current_scales_.resize( data_count );

    // If no estimator found for the current stage, the default is
    // from the initializer. Feature_based can only deal with one
    // estimator in each stage/resolution. If more than one is found,
    // only the first is kept.
    //
    if ( xform_estimators.size() == 0 )
      xform_estimator = initial_view->xform_estimator();
    else xform_estimator = xform_estimators[0];
    assert ( xform_estimator );

    // If the initialization comes with a prior scale, initialize the
    // scales using the prior scale.
    //
    if ( use_prior_scale ) {
      DebugMacro(  2, "Prior scale = "<<prior_scale->geometric_scale()<<'\n' );
      for (  unsigned int fs = 0; fs < data_count; ++fs )
        current_scales_[fs] = prior_scale;
    }

    iterations_at_stage_ = 0; //keeps track of total iter at level
    current_status = 0;
    vcl_vector<bool> should_estimate_scale(data_count, true);
    int  scale_est_count = 0;

    bool global_stage = false, global_stage_done = false;

    do { // for each re-matching

      current_view_ = new_view;

      DebugMacro(  2, " Computing matches and scales\n" );
      // Compute matches, and scales for each feature set.
      //
      for ( unsigned int fs=0; fs < data_count; ++fs ) {
        DebugMacro(  2, "   Data set " << fs << vcl_endl );
        rgrl_match_set_sptr new_matches =
          matchers[fs]->compute_matches( *from_sets[fs],
                                         *to_sets[fs],
                                         *current_view_,
                                         *current_scales_[fs] );
        DebugMacro(  2, "      Matches: " << new_matches->from_size() <<" ("<<new_matches<<")\n" );


        // For the first iteration, use prior scale or estimate the
        // scales without weights (since we don't have any...)
        //
        if ( iterations_for_scale_est_ >= 0 &&
             scale_est_count > iterations_for_scale_est_ ) {
          should_estimate_scale[fs] = false;
        }

        rgrl_scale_sptr new_scale = 0;
        if ( !should_estimate_scale[fs] ) {
          DebugMacro(  2, "No scale estimation\n" );
        }
        else if ( new_matches->from_size() == 0 ) {
          DebugMacro(0, "Empty match set!!!\n");
          failed = true;
          continue;
        }
        else { //should_estimate_scale && new_matches->from_size() > 0
          if ( !wgted_scale_ests[fs] ) {
            new_scale = unwgted_scale_ests[fs]->
              estimate_unweighted( *new_matches, current_scales_[fs], should_penalize_scaling_ );
          } else {
            if ( !current_scales_[fs] ) {
              assert ( unwgted_scale_ests[fs] );
              new_scale = unwgted_scale_ests[fs]->
                estimate_unweighted( *new_matches, current_scales_[fs], should_penalize_scaling_);
            }
            else {
              weighters[fs]->compute_weights(*current_scales_[fs], *new_matches);
              new_scale = wgted_scale_ests[fs]->
                estimate_weighted( *new_matches, current_scales_[fs], should_penalize_scaling_);
            }
          }

          if ( new_scale && new_scale->has_geometric_scale() ) {
            DebugMacro(  2, "New geometric scale = "<<new_scale->geometric_scale()<<'\n' );
            if ( new_scale->geometric_scale() < expected_min_geometric_scale_ ) {
              //should_estimate_scale[fs] = false;
              current_scales_[fs] = new_scale;
              current_scales_[fs]->set_geometric_scale(expected_min_geometric_scale_);
              DebugMacro( 2, "Scale below expected_min_geometric_scale. Set to expected_min_geometric_scale.\n" );
            }
            else {
              current_scales_[fs] = new_scale;
            }
            scale_est_count++;
          }
          else{
            DebugMacro_abv( 1, "  Cannot estimate new scale. Do NOT use this match set\n" );
            new_matches = new rgrl_match_set( new_matches->from_type(),
                                              new_matches->to_type());
            current_scales_[fs] = 0;
          }
        }


        // If the scale is above the upper bound of the expected
        // geometric scale return with failure
        if ( expected_max_geometric_scale_ > 0 &&
             current_scales_[fs] &&
             current_scales_[fs]->has_geometric_scale() &&
             current_scales_[fs]->geometric_scale() > expected_max_geometric_scale_) {
          scale_in_range = false;
          failed = true;
        }

        // Keep new ones and discard old ones
        //
        current_match_sets_[fs] = new_matches;
      }

      if(failed)
        break;

      DebugMacro(  2, " Estimate the transformation\n" );

      // Estimate the transformation using a simplified irls
      // (Iterative-Reweighted Least-Squares) routine.
      //
      // The match sets and the scales are fixed, but the associated
      // weights are updated (hence reweighted-least-squares)
      // throughout the estimation.
      rgrl_transformation_sptr xform = current_view_->xform_estimate();
      if ( !rgrl_util_irls( current_match_sets_, current_scales_, weighters,
                            *conv_tester_, xform_estimator,
                            xform,
                            false,                   // no fast mapping
                            this->debug_flag() ) ) {
        failed = true;
        break; //no valid xform, so exit the loop
      }
      current_view_->set_xform_estimate(xform);

      new_view = expand_view(current_view_);

      // For debugging ...
      //
      this->invoke_event(rgrl_event_iteration());

      // Update the weights and scale estimates based on the new transform
      //
      DebugMacro(  2, " Updating scale estimates and checking for validity\n" );
      for ( unsigned int fs=0; fs < data_count; ++fs ) {
        if ( current_match_sets_[fs]->from_size() > 0 ) {
          current_match_sets_[fs]->remap_from_features( *current_view_->xform_estimate() );
          weighters[fs]->compute_weights( *current_scales_[fs], *current_match_sets_[fs] );


        }
      }

      // Perform convergence test
      //
      DebugMacro(  2, " Perform convergence test\n" );
      current_status =
          conv_tester_->compute_status( current_status,
                                        *current_view_, *new_view,
                                        current_match_sets_, current_scales_,
                                        should_penalize_scaling_ );

      DebugMacro( 3, "run: (iterations = " << iterations_at_stage_
                  << ") oscillation count = " << current_status->oscillation_count() << '\n' );
      DebugMacro( 3, "run: error = " << current_status->error() << vcl_endl );
      DebugMacro( 3, "run: error_diff = " << current_status->error_diff() << vcl_endl );

      if( current_status->is_failed() ){
        failed = true;
        break;
      }

      ++iterations_at_stage_;

      if(global_stage)
        global_stage_done = true;

      if(!global_stage &&
          (current_status->has_converged() ||
          iterations_at_stage_ >= max_icp_iter_) ){
        new_view->set_region(new_view->global_region());
        //current_status = new rgrl_converge_status( false, false, false, false,
        //                                           0.0, 0, 0.0 );
        iterations_at_stage_ = 0;
        global_stage = true;
        DebugMacro(  2, "expand to full region!" <<vcl_endl );
      }


    } while ( !failed && !global_stage_done &&
             !current_status->has_converged() &&
             !current_status->has_stagnated() &&
             iterations_at_stage_ < max_icp_iter_ );

      for ( unsigned int fs=0; fs < data_count; ++fs ){
        if(!current_scales_[fs]){
          failed = true;
          break;
        }
      }


      if ( failed ) {
        if ( !scale_in_range )
          DebugMacro(  2, " Geometric scale above the expected value\n" );
        else
          DebugMacro(  2, " Failed with empty match set, or feature_based\n" );
        continue;
      }
      if ( current_status->has_converged() )
        DebugMacro( 1, " CONVERGED\n" );
      if ( current_status->has_stagnated() )
        DebugMacro( 1, " STAGNATED\n" );
      if ( iterations_at_stage_ == max_icp_iter_ )
        DebugMacro( 1, " ICP iteration reached maximum ("<<max_icp_iter_<<" )\n" );

      // Move to the next resolution with proper initialization if not
      // already at the finest level
      //
      prev_resol = current_view_->resolution();
      //initialize_for_next_resolution( from_image_region, to_image_region,
      //**                              current_xform_estimate_, resolution );
      int level_diff = prev_resol - current_view_->resolution();
      double dim_increase = current_data_->dimension_increase_for_next_stage(prev_resol);
      double scale_multipler = vcl_pow(dim_increase, level_diff);
      for ( unsigned int fs=0; fs < data_count; ++fs ) {
        if(current_scales_[fs])
        current_scales_[fs]->set_geometric_scale( current_scales_[fs]->geometric_scale()*scale_multipler );
      }

      use_prior_scale = true;
      prior_scale = current_scales_[0]; //Assuming scales[0] is a good approximate

  } while ( !failed &&
             (!current_view_->resolution() == 0 ||
              !prev_resol == 0) );

  DebugMacro( 1, "Estimation complete\n" );

  //   At this point the iterations are over for this initial
  //   estimate.  If the estimation is successful, and if it is the
  //   first one tested or it is the best seen thus far, record
  //   information about this estimate.
  //

  if ( !failed ){
    DebugMacro( 1, "Obj value after convergence = "<<current_status->objective_value()<<'\n');

    vcl_vector<rgrl_match_set_sptr> best_matches(current_match_sets_.size(),0);
    vcl_vector<rgrl_scale_sptr> best_scales(current_scales_.size(),0);
    for(unsigned i=0; i<current_match_sets_.size(); ++i){
      best_matches[i] = current_match_sets_[i];
      best_scales[i] = current_scales_[i];
    }
    best_salient_group_ = new vidreg_salient_group(current_view_,
                                                   best_matches,
                                                   best_scales,
                                                   current_status);

    DebugMacro( 1, "Set best xform estimate\n" );

  }
}


void
vidreg_registration::
initialize_for_next_resolution(  rgrl_mask_box            & from_image_region,
                                 rgrl_mask_box            & to_image_region,
                                 rgrl_transformation_sptr & xform_estimate,
                                 unsigned                 & current_resol ) const
{
  // Find the next available level
  //
  unsigned new_resol = current_resol;
  int next_resol = new_resol - 1;
  for ( ; next_resol >=0 ; --next_resol) {
    if ( current_data_->has_stage(next_resol) ) {
      new_resol = next_resol;
      break;
    }
  }

  // If no next available level return
  //
  if ( new_resol == current_resol ) return;


  // Scale the components of the current view for the initial view of the next
  // available level
  //
  int level_diff = current_resol - new_resol;
  double dim_increase = current_data_->dimension_increase_for_next_stage(current_resol);
  double scale = vcl_pow(dim_increase, level_diff);

  from_image_region.set_x0( from_image_region.x0()*scale );
  from_image_region.set_x1( from_image_region.x1()*scale );

  xform_estimate =  xform_estimate->scale_by( scale );
  current_resol = new_resol;
}


rgrl_view_sptr
vidreg_registration::expand_view(const rgrl_view_sptr& view)
{

  rgrl_mask_box region = view->region();

  rgrl_mask_box global_region = view->global_region();

  rgrl_transformation_sptr xform = view->xform_estimate();

  vnl_vector<double> center = region.x0();
  center += region.x1();
  center /= 2.0;

  const double var_in_from_feature_loc = 0.25; //std = 0.5, by guess

  vnl_vector<double> normal( 2, 0.0 );
  vnl_vector<double> p( center );
  vnl_vector<double> xformed_normal( 2 );
  vnl_vector<double> xformed_p( 2 );
  vnl_matrix<double> trans_err_covar( 2, 2 );
  vnl_matrix<double> jac_loc;

  for( unsigned d=0; d < 2; ++d ) {
    // Faces intersecting at x0
    {
      // center of face in direction d
      p[d] = region.x0()[d];
      double half_length = p[d] - center[d];
      normal[d] = half_length < 0.0 ? -1.0 : 1.0;
      xform->map_location( p, xformed_p );
      xform->map_normal( p, normal, xformed_normal );
      xform->jacobian_wrt_loc( jac_loc, p );

      // all the error
      trans_err_covar = xform->transfer_error_covar( p ) +
          var_in_from_feature_loc * ( vnl_transpose( jac_loc ) * jac_loc );

      //trans_err_covar = scale_estimate * xform.transfer_error_covar( p );
      //       vcl_cout << "p="<<p<<"\n";
      //      vcl_cout << "Xformed p="<<xformed_p<<"\n";
      //       vcl_cout << "Normal="<<normal<<"\n";
      //       vcl_cout << "Xformed normal="<<xformed_normal<<"\n";
      //       vcl_cout << "Xfer error covar=\n"<<trans_err_covar<<"\n";

      //double sigma_s_sqr = dot_product( xformed_normal, trans_err_covar*xformed_normal );
      //vcl_cout << ((d==0)?"Left ":"Right ") << sigma_s_sqr <<vcl_endl;
    }

    // Faces intersecting at x1
    {
      // center of face in direction d
      p[d] = region.x1()[d];
      double half_length = p[d] - center[d];
      normal[d] = half_length < 0.0 ? -1.0 : 1.0;
      xform->map_location( p, xformed_p );
      xform->map_normal( p, normal, xformed_normal );
      xform->jacobian_wrt_loc( jac_loc, p );
      
      // all the error
      trans_err_covar = xform->transfer_error_covar( p ) +
          var_in_from_feature_loc * ( vnl_transpose( jac_loc ) * jac_loc );

      //trans_err_covar = scale_estimate * xform.transfer_error_covar( p );
      
      //double sigma_s_sqr = dot_product( xformed_normal, trans_err_covar*xformed_normal );
      //vcl_cout << ((d==0)?"Up ":"Down ") << sigma_s_sqr <<vcl_endl;
    }

    // reset for next iteration
    p[d] = center[d];
    normal[d] = 0.0;
  }

  vnl_vector<double> growth(region.x1()-center);
  growth *= 1.5;
  vnl_vector<double> new_x0(center-growth), new_x1(center+growth);

  // clip
  if(new_x0[0] < global_region.x0()[0]) new_x0[0] = global_region.x0()[0];
  if(new_x0[1] < global_region.x0()[1]) new_x0[1] = global_region.x0()[1];
  if(new_x1[0] > global_region.x1()[0]) new_x1[0] = global_region.x1()[0];
  if(new_x1[1] > global_region.x1()[1]) new_x1[1] = global_region.x1()[1];

  region.set_x0(new_x0);
  region.set_x1(new_x1);

  //vcl_cout << "new region " << region <<vcl_endl;

  view->set_region(region);

  return new rgrl_view( view->from_image_roi(),
                        view->to_image_roi(),
                        region,
                        global_region,
                        view->xform_estimator(),
                        xform,
                        view->resolution() );
}


