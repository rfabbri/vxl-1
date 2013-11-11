// This is contrib/mleotta/vidreg/vidreg_matcher.cxx

//:
// \file


#include "vidreg_matcher.h"
#include <vcl_cassert.h>
#include <vcl_utility.h>
#include <vcl_algorithm.h>
#include <vcl_iterator.h>
#include <vcl_deque.h>

#include <vcl_fstream.h>
#include <vul/vul_file.h>

#include <rrel/rrel_tukey_obj.h>
#include <rrel/rrel_muset_obj.h>
#include <rsdl/rsdl_bins_2d.h>
#include <vnl/vnl_double_2.h>
#include <rgrl/rgrl_scale_est_all_weights.h>
#include <rgrl/rgrl_weighter_m_est.h>
#include <rgrl/rgrl_scale_est_closest.h>
#include <rgrl/rgrl_feature_face_pt.h>
#include <rgrl/rgrl_feature_set_location.h>
#include <rgrl/rgrl_trans_translation.h>
#include <rgrl/rgrl_trans_similarity.h>
#include <rgrl/rgrl_est_similarity2d.h>
#include <rgrl/rgrl_data_manager.h>
#include <rgrl/rgrl_matcher_k_nearest.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_converge_status.h>
#include <rgrl/rgrl_convergence_on_weighted_error.h>


#include <vidreg/vidreg_feature_pt_desc.h>
#include <vidreg/vidreg_feature_edgel.h>
#include <vidreg/vidreg_initializer.h>
#include <vidreg/vidreg_registration.h>
#include <vidreg/vidreg_salient_group.h>


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
      const vidreg_registration* reg_engine =
          dynamic_cast<const vidreg_registration*>(caller);
      rgrl_transformation_sptr trans = reg_engine->current_view()->xform_estimate();
      rgrl_trans_similarity* xform = rgrl_cast<rgrl_trans_similarity*>(trans);
      vcl_cout<<"Xform A= "<<xform->A()<<"\nt= "<<xform->t()<<vcl_endl;
    }
};


//: Constructor
vidreg_matcher::vidreg_matcher()
{
}


//: Destructor
vidreg_matcher::~vidreg_matcher()
{
}


bool vidreg_matcher::assisted_match(const vcl_vector<rgrl_transformation_sptr>& xforms,
                                    const vidreg_feature_group& last_features,
                                    const vidreg_feature_group& new_features)
{
  salient_groups_.clear();

  rgrl_feature_set_sptr fixed_edgel_set = new rgrl_feature_set_location<2>(last_features.edgels);
  rgrl_feature_set_sptr moving_edgel_set = new rgrl_feature_set_location<2>(new_features.edgels);
  rgrl_feature_set_sptr fixed_corner_set = new rgrl_feature_set_location<2>(last_features.corners);


  rgrl_mask_sptr from_roi = new rgrl_mask_box(moving_edgel_set->bounding_box());
  rgrl_mask_sptr to_roi = new rgrl_mask_box(fixed_edgel_set->bounding_box());
  rgrl_estimator_sptr      estimator      = new rgrl_est_similarity2d();

  vcl_vector<rgrl_match_set_sptr> matches;
  matches.push_back(new rgrl_match_set( fixed_edgel_set->type(), fixed_edgel_set->type()));
  matches.push_back(new rgrl_match_set( fixed_corner_set->type(), fixed_corner_set->type()));
  vcl_vector<rgrl_scale_sptr> scales;
  rgrl_converge_status_sptr status = NULL;
  for(unsigned int i=0; i< xforms.size(); ++i){
    rgrl_view_sptr view = new rgrl_view(  from_roi,
                                          to_roi,
                                          moving_edgel_set->bounding_box(),
                                          moving_edgel_set->bounding_box(),
                                          estimator,
                                          xforms[i] );
    vidreg_salient_group_sptr group = new vidreg_salient_group(view, matches, scales, status);
    salient_groups_.push_back(group);
  }

  assign_features(new_features.edgels, *fixed_edgel_set, 0);
  assign_features(new_features.corners, *fixed_corner_set, 1);

  group_edges(new_features.edges);

  return true;
}


bool vidreg_matcher::match(const vcl_vector<vidreg_salient_group_sptr>& groups,
                           const vidreg_feature_group& last_features,
                           const vidreg_feature_group& new_features)
{
  salient_groups_.clear();

  rgrl_data_manager_sptr main_data, orig_data;
  rgrl_view_sptr prior_view;
  setup(last_features,new_features,main_data,prior_view);
  orig_data = main_data;

  rgrl_initializer_sptr initializer = new vidreg_initializer(last_features.corners,
                                                             new_features.corners,
                                                             prior_view);
  initializer->set_prior_geometric_scale(0.5);
  initializer->set_debug_flag(0);

  rgrl_view_sptr     initial_view;
  rgrl_scale_sptr    prior_scale;
  while(vidreg_salient_group_sptr group = run_registration(main_data, initializer))
  {
    salient_groups_.push_back(group);
    vidreg_initializer* init_ptr = dynamic_cast<vidreg_initializer*>(initializer.ptr());
    assert(init_ptr);
    init_ptr->remove_covered_matches(*group->view()->xform_estimate(), 2.0);
    main_data = remove_covered_features(group, main_data);
  }

  for(unsigned i=0; i<salient_groups_.size(); ++i){
    // compare to all existing groups to detect duplicates
    rgrl_trans_similarity* new_xform = rgrl_cast<rgrl_trans_similarity*>(salient_groups_[i]->view()->xform_estimate());
    for(unsigned j=0; j<i; ++j){
      rgrl_trans_similarity* old_xform = rgrl_cast<rgrl_trans_similarity*>(salient_groups_[j]->view()->xform_estimate());
      double d = xform_distance(*old_xform, *new_xform);
      if( d < 16.0 ){
        vcl_cout << "found duplicate" << vcl_endl;
        salient_groups_.erase(salient_groups_.begin()+i);
        --i;
        break;
      }
    }
  }



#if 0

  double tolerance = 0.5;
  rgrl_convergence_tester_sptr conv_test =
      new rgrl_convergence_on_weighted_error( tolerance );


  for(unsigned i=0; i<groups.size(); ++i){
    // get only unique feautures
    vidreg_feature_group_sptr fixed_features = groups[i]->make_feature_group(1.0);
    rgrl_data_manager_sptr data;
    setup(*fixed_features,new_features,data,prior_view);
    reg_queue.push_back(data);
  }

  salient_groups_.resize(groups.size(),NULL);

  rgrl_view_sptr     initial_view;
  rgrl_scale_sptr    prior_scale;
  while( initializer->next_initial( initial_view, prior_scale ) ) {
    vidreg_salient_group_sptr group = NULL;
    for(unsigned idx=0; idx<reg_queue.size(); ++idx){
      if(!reg_queue[idx])
        continue;
      rgrl_data_manager_sptr curr_data = reg_queue[idx];

      vidreg_registration reg( curr_data, conv_test );
      reg.set_expected_min_geometric_scale( 0.1 ); //scale cannot go lower than 0.1
      reg.set_expected_max_geometric_scale( 1.5 ); //scale cannot go above 1.5
      reg.set_max_icp_iter( 5 );
      reg.run(initial_view, prior_scale);

      group = reg.salient_group();
      if(group){
        salient_groups_[idx] = group;
        reg_queue[idx] = NULL;
        vcl_cout << "found group " << idx << vcl_endl;
        break;
      }
    }
    if(group){
      vidreg_initializer* init_ptr = dynamic_cast<vidreg_initializer*>(initializer.ptr());
      assert(init_ptr);
      init_ptr->remove_covered_matches(*group->view()->xform_estimate(), 2.0);

#if 0
      for(unsigned idx=0; idx<reg_queue.size(); ++idx){
        if(!reg_queue[idx])
          continue;
        reg_queue[idx] = remove_covered_features(group, reg_queue[idx]);
      }
#endif
      main_data = remove_covered_features(group, main_data);
    }
    else{
      vidreg_registration reg( main_data, conv_test );
      reg.set_expected_min_geometric_scale( 0.1 ); //scale cannot go lower than 0.1
      reg.set_expected_max_geometric_scale( 1.5 ); //scale cannot go above 1.5
      reg.set_max_icp_iter( 5 );
      reg.run(initial_view, prior_scale);

      group = reg.salient_group();
      if(group){
        vcl_cout << "making a new group" <<vcl_endl;
        salient_groups_.push_back(group);
        vidreg_initializer* init_ptr = dynamic_cast<vidreg_initializer*>(initializer.ptr());
        assert(init_ptr);
        init_ptr->remove_covered_matches(*group->view()->xform_estimate(), 2.0);
        main_data = remove_covered_features(group, main_data);
      }
    }
  }


#if 0
  if(groups.empty()){
    rgrl_data_manager_sptr data;
    rgrl_initializer_sptr initializer;
    setup(last_features,new_features,data,initializer);
    reg_queue.push_back(reg_pair(data,initializer));
  }

  for(unsigned i=0; i<groups.size(); ++i){
    vidreg_feature_group_sptr fixed_features = groups[i]->make_feature_group();
    rgrl_data_manager_sptr data;
    rgrl_initializer_sptr initializer;
    setup(*fixed_features,new_features,data,initializer);
    reg_queue.push_back(reg_pair(data,initializer));
  }

//#endif
  while(!reg_queue.empty()){
    rgrl_data_manager_sptr data = reg_queue.front().first;
    rgrl_initializer_sptr initializer = reg_queue.front().second;
    reg_queue.pop_front();

    vidreg_salient_group_sptr group = run_registration(data, initializer);
    if(group){
      // compare to all existing groups to detect duplicates
      rgrl_trans_similarity* new_xform = rgrl_cast<rgrl_trans_similarity*>(group->view()->xform_estimate());
      bool duplicate = false;
      for(unsigned i=0; i<salient_groups_.size(); ++i){
        rgrl_trans_similarity* old_xform = rgrl_cast<rgrl_trans_similarity*>(salient_groups_[i]->view()->xform_estimate());
        double d = xform_distance(*old_xform, *new_xform);
        if( d < 16.0 ){
          vcl_cout << "found duplicate" << vcl_endl;
          duplicate = true;
          break;
        }
      }

      if(!duplicate){
        salient_groups_.push_back(group);
        vidreg_initializer* init_ptr = dynamic_cast<vidreg_initializer*>(initializer.ptr());
        assert(init_ptr);
        init_ptr->remove_covered_matches(*group->view()->xform_estimate(), 2.0);
        data = remove_covered_features(group, data);
        reg_queue.push_back(reg_pair(data,initializer));
      }
    }
  }
#endif

#endif


  for(unsigned i=0; i<salient_groups_.size(); ++i){
    vcl_cout<< "------------------------------\nGroup #"<<i<<vcl_endl;
    vcl_cout<<"Final xform:"<<vcl_endl;
    rgrl_transformation_sptr trans = salient_groups_[i]->view()->xform_estimate();
    rgrl_trans_similarity* xform = rgrl_cast<rgrl_trans_similarity*>(trans);
    vcl_cout<<"A = "<< xform->A()<<"t = "<<xform->t()
        <<"\nFinal alignment error = "<<salient_groups_[i]->status()->error()<<vcl_endl;
  }

  rgrl_feature_set_sptr to_set = new rgrl_feature_set_location<2>(last_features.edgels);
  assign_features(new_features.edgels, *to_set, 0);
  to_set = new rgrl_feature_set_location<2>(last_features.corners);
  assign_features(new_features.corners, *to_set, 1);

  group_edges(new_features.edges);

  //robust_crop(*salient_groups_[0]);
  //robust_crop(*salient_groups_[1]);

  
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;
  const unsigned num_last = groups.size();
  const unsigned num_curr = salient_groups_.size();
  vnl_matrix<double> match_matrix(num_curr,num_last,0.0);
  vcl_vector<vcl_pair<double,vidreg_salient_group_sptr> > weight_pairs;

  for(unsigned i=0; i<num_curr; ++i){
    const rgrl_match_set& match_set = *salient_groups_[i]->matches()[0];
    weight_pairs.push_back(vcl_pair<double,vidreg_salient_group_sptr>(0.0,salient_groups_[i]));
    for ( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ) {
      double from_weight = salient_groups_[i]->weight(fi.from_feature());
      weight_pairs[i].first += from_weight;
      for ( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
        for(unsigned j=0; j<num_last; ++j){
          double to_weight = groups[j]->weight(ti.to_feature());
          match_matrix(i,j) += from_weight * to_weight;
        }
      }
    }
  }

  if(num_last == 0){
    // sort salient groups by total weight
    vcl_sort(weight_pairs.begin(), weight_pairs.end(),
             vcl_greater<vcl_pair<double,vidreg_salient_group_sptr> >() );
    vcl_vector<vidreg_salient_group_sptr> new_order;
    for(unsigned i=0; i<num_curr; ++i)
      new_order.push_back(weight_pairs[i].second);
    salient_groups_ = new_order;
  }
  else{
    double max_v = 1.0;
    while(max_v > 0.0){
      max_v = 0.0;
      unsigned max_i=0, max_j=0;
      for(unsigned i=0; i<num_curr; ++i){
        for(unsigned j=0; j<num_last; ++j){
          double v = match_matrix(i,j);
          if(v > max_v){
            max_v = v;
            max_i = i;
            max_j = j;
          }
        }
      }
      if(max_v <= 0.0)
        break;
      vidreg_salient_group_sptr g = salient_groups_[max_i];

      //FIXME previous group recording disabled
      //g->set_previous_group(groups[max_j]);
      vcl_cout << " matched "<<max_i<< " to "<< max_j<<vcl_endl;
      if(max_i != max_j && max_j < num_curr){
        // swap position in the current groups to match old groups
        salient_groups_[max_i] = salient_groups_[max_j];
        salient_groups_[max_j] = g;
        match_matrix.set_row(max_i,match_matrix.get_row(max_j));
        max_i = max_j;
      }
      match_matrix.set_row(max_i,0.0);
      match_matrix.set_column(max_j,0.0);
    }
  }

  return true;
}


vidreg_salient_group_sptr
vidreg_matcher::run_registration(const rgrl_data_manager_sptr& data,
                                 const rgrl_initializer_sptr& initializer)
{
  double tolerance = 0.5;
  rgrl_convergence_tester_sptr conv_test =
      new rgrl_convergence_on_weighted_error( tolerance );

  vidreg_registration reg( data, conv_test );
  reg.set_expected_min_geometric_scale( 0.1 ); //scale cannot go lower than 0.1
  reg.set_expected_max_geometric_scale( 1.5 ); //scale cannot go above 1.5
  reg.set_max_icp_iter( 5 );

  reg.set_debug_flag(0);

#if DEBUG
  reg.add_observer( new rgrl_event_iteration(), new command_iteration_update());
#endif

  reg.run(initializer);

  return reg.salient_group();
}


void
vidreg_matcher::setup(const vidreg_feature_group& fixed,
                      const vidreg_feature_group& moving,
                      rgrl_data_manager_sptr& data,
                      rgrl_view_sptr& prior_view) const
{
  rgrl_feature_set_sptr fixed_edgel_set = new rgrl_feature_set_location<2>(fixed.edgels);
  rgrl_feature_set_sptr moving_edgel_set = new rgrl_feature_set_location<2>(moving.edgels);

  rgrl_feature_set_sptr fixed_point_set = new rgrl_feature_set_location<2>(fixed.corners);
  rgrl_feature_set_sptr moving_point_set = new rgrl_feature_set_location<2>(moving.corners);

  unsigned int k = 1;
  rgrl_matcher_sptr cp_matcher = new rgrl_matcher_k_nearest( k , 2.0);

  vcl_auto_ptr<rrel_m_est_obj>  m_est_obj( new rrel_tukey_obj(4) );
  rgrl_weighter_sptr wgter = new rgrl_weighter_m_est(m_est_obj, true, true);


  vcl_auto_ptr<rrel_objective> muset_obj( new rrel_muset_obj(0, false) );

  rgrl_scale_estimator_unwgted_sptr unwgted_scale_est =
      new rgrl_scale_est_closest( muset_obj );
  rgrl_scale_estimator_wgted_sptr wgted_scale_est =
      new rgrl_scale_est_all_weights();


  data = new rgrl_data_manager();
  //data->add_data(moving_set, fixed_set, cp_matcher);
  data->add_data( moving_edgel_set,   // data from moving image
                  fixed_edgel_set,    // data from fixed image
                  cp_matcher,         // matcher for this data
                  wgter,              // weighter
                  unwgted_scale_est,  // unweighted scale estimator
                  wgted_scale_est);   // weighted scale estimator

  data->add_data( moving_point_set,   // data from moving image
                  fixed_point_set,    // data from fixed image
                  cp_matcher,         // matcher for this data
                  wgter,              // weighter
                  unwgted_scale_est,  // unweighted scale estimator
                  wgted_scale_est);   // weighted scale estimator


  rgrl_mask_sptr from_roi = new rgrl_mask_box(moving_edgel_set->bounding_box());
  rgrl_mask_sptr to_roi = new rgrl_mask_box(fixed_edgel_set->bounding_box());
  rgrl_transformation_sptr init_transform = new rgrl_trans_similarity(2);
  rgrl_estimator_sptr      estimator      = new rgrl_est_similarity2d();
  estimator->set_debug_flag(0);
  prior_view = new rgrl_view( from_roi,
                              to_roi,
                              moving_edgel_set->bounding_box(),
                              moving_edgel_set->bounding_box(),
                              estimator,
                              init_transform );

}


//: remove features from the data manager if covered by this motion group
rgrl_data_manager_sptr
vidreg_matcher::remove_covered_features(const vidreg_salient_group_sptr& group,
                                        const rgrl_data_manager_sptr& data)
{
  vcl_vector<rgrl_feature_set_sptr>             from_sets;
  vcl_vector<rgrl_feature_set_sptr>             to_sets;
  vcl_vector<rgrl_matcher_sptr>                 matchers;
  vcl_vector<rgrl_scale_estimator_unwgted_sptr> unwgted_scale_ests;
  vcl_vector<rgrl_scale_estimator_wgted_sptr>   wgted_scale_ests;
  vcl_vector<rgrl_weighter_sptr>                weighters;
  vcl_vector<rgrl_estimator_sptr>               xform_estimators;

  data->get_data( from_sets, to_sets, matchers,
                  weighters, unwgted_scale_ests,
                  wgted_scale_ests, xform_estimators);

  typedef rgrl_match_set::from_iterator from_iter;
  typedef from_iter::to_iterator        to_iter;
  typedef vcl_vector<rgrl_feature_sptr> feat_vector;

  const unsigned data_count = from_sets.size();

  vcl_set<rgrl_feature_sptr> from_covered, to_covered;
  rgrl_data_manager_sptr new_data = new rgrl_data_manager();

  for(unsigned fs=0; fs<data_count; ++fs)
  {
    //  for each from image feature being matched
    rgrl_mask_sptr from_region = group->view()->from_image_roi();
    feat_vector from_vec, to_vec;
    from_sets[fs]->features_in_region(from_vec, from_region->bounding_box() );
    vcl_sort(from_vec.begin(), from_vec.end());
    rgrl_mask_sptr to_region = group->view()->to_image_roi();
    to_sets[fs]->features_in_region(to_vec, to_region->bounding_box() );
    vcl_sort(to_vec.begin(), to_vec.end());

    for ( from_iter fitr = group->matches()[fs]->from_begin();
          fitr != group->matches()[fs]->from_end(); ++fitr )
    {

      if ( fitr.empty() )  continue;

      rgrl_feature_sptr from_feature = fitr.from_feature();

      for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr )
      {
        //  for each match with a "to" image feature
        rgrl_feature_sptr to_feature = titr.to_feature();
        if(titr.cumulative_weight() > 0.5)
        {
          from_covered.insert(from_feature);
          to_covered.insert(to_feature);
        }
      }
    }
    vcl_vector<rgrl_feature_sptr> remaining_from;
    vcl_back_insert_iterator<vcl_vector<rgrl_feature_sptr> > rf_ii(remaining_from);
    vcl_set_difference(from_vec.begin(),from_vec.end(),
                      from_covered.begin(),from_covered.end(), rf_ii);
    rgrl_feature_set_sptr from_set = new rgrl_feature_set_location<2>(remaining_from);

    //vcl_cout << "from covered size: "<< from_covered.size() <<vcl_endl;
    //vcl_cout << "original size from: "<< from_vec.size() << " new size from: "<< remaining_from.size() << vcl_endl;

    vcl_vector<rgrl_feature_sptr> remaining_to;
    vcl_back_insert_iterator<vcl_vector<rgrl_feature_sptr> > rt_ii(remaining_to);
    vcl_set_difference(to_vec.begin(),to_vec.end(),
                      to_covered.begin(),to_covered.end(), rt_ii);
    rgrl_feature_set_sptr to_set = new rgrl_feature_set_location<2>(remaining_to);

    //vcl_cout << "to covered size: "<< to_covered.size() <<vcl_endl;
    //vcl_cout << "original size to: "<< to_vec.size() << " new size to: "<< remaining_to.size() << vcl_endl;

    new_data->add_data( from_set,                // data from moving image
                        to_set,                  // data from fixed image
                        matchers[fs],            // matcher for this data
                        weighters[fs],           // weighter
                        unwgted_scale_ests[fs],  // unweighted scale estimator
                        wgted_scale_ests[fs]);   // weighted scale estimator
  }

  return new_data;
}


//: assign the features to only one the the resulting salient groups
void vidreg_matcher::assign_features(const vcl_vector<rgrl_feature_sptr>& from_features,
                                     const rgrl_feature_set& to_set,
                                     unsigned f_idx)
{

  const unsigned group_count = salient_groups_.size();

  typedef vcl_vector<rgrl_feature_sptr> feat_vector;
  typedef feat_vector::const_iterator feat_iter;

  
  vcl_vector<unsigned > num_duplicates(group_count,0);
  vcl_vector<vcl_pair<unsigned,unsigned> > num_matches;

  vcl_auto_ptr<rrel_m_est_obj>  m_est_obj( new rrel_tukey_obj(4) );
  double geometric_scale = 0.25;

  vcl_vector<rgrl_transformation_sptr> xforms(group_count,NULL);
  for(unsigned gs=0; gs<group_count; ++gs)
  {
    xforms[gs] = salient_groups_[gs]->view()->xform_estimate();
    num_matches.push_back(vcl_pair<unsigned,unsigned>(0,gs));
  }

  vcl_vector<rgrl_match_set_sptr> matches(group_count,NULL);

  for(unsigned gs=0; gs<group_count; ++gs){
    rgrl_match_set_sptr old_matches = salient_groups_[gs]->matches()[f_idx];
    matches[gs] = new rgrl_match_set( old_matches->from_type(), old_matches->to_type(),
                                      old_matches->from_label(), old_matches->to_label() );
    matches[gs]->reserve(from_features.size() );
  }

  //  generate the matches for each feature of this feature type in the current region
  for ( feat_iter fitr = from_features.begin(); fitr != from_features.end(); ++fitr )
  {
    vcl_vector<double> weights(group_count,0.0);
    double weight_sum = 0.0;
    for(unsigned gs=0; gs<group_count; ++gs)
    {
      rgrl_feature_sptr mapped = (*fitr)->transform( *xforms[gs] );

      if ( !salient_groups_[gs]->view()->to_image_roi()->inside(mapped->location()) ){
        weight_sum += 1.0; // add to the sum since this might match if in bounds
        continue;   // feature is invalid
      }
      feat_vector matching_features;
      to_set.k_nearest_features(matching_features, mapped, 1 );
      if(matching_features.empty())
        continue;

      double geometric_err = matching_features[0]->geometric_error( *mapped );
      double weight = m_est_obj->wgt( geometric_err / geometric_scale );
      weights[gs] = weight;
      weight_sum += weight;
      weight *= matching_features[0]->absolute_signature_weight( mapped );

      matches[gs]->add_feature_and_matches( *fitr, mapped, matching_features );
    }

    if(weight_sum > 0.0)
    {
      for(unsigned gs=0; gs<group_count; ++gs)
      {
        if(weights[gs] > 0.0){
          double norm_weight = weights[gs]/weight_sum;
          salient_groups_[gs]->assign_weight(*fitr, norm_weight);
        }
      }
    }
  }

  for(unsigned gs=0; gs<group_count; ++gs){
    salient_groups_[gs]->set_matches(f_idx,matches[gs]);
  }
}


//: A metric on the space of 2D similarity xforms
double vidreg_matcher::xform_distance(const rgrl_trans_similarity& xform1,
                                      const rgrl_trans_similarity& xform2) const
{
  return (xform1.t()-xform2.t()).squared_magnitude();
}

namespace {

void robust_bounds(vcl_vector<double>& data,
                   double& min, double& max)
{
  typedef vcl_vector<double>::iterator Ditr;
  long median_idx = data.size()/2;
  vcl_vector<double>::iterator median = data.begin() + median_idx;
  vcl_nth_element(data.begin(), median, data.end());

  vcl_vector<double> dev_left, dev_right;
  dev_left.reserve(median_idx);
  dev_right.reserve(median_idx);
  for(Ditr i=data.begin(); i!=median; ++i)
    dev_left.push_back(*median - *i);
  for(Ditr i=median+1; i!=data.end(); ++i)
    dev_right.push_back(*i - *median);

  vcl_sort(dev_left.begin(), dev_left.end());
  vcl_sort(dev_right.begin(), dev_right.end());

  vcl_stringstream fname;
  unsigned num = 0;
  fname << "left"<<num<<".txt";
  while(vul_file::exists(fname.str())){
    fname.str("");
    fname << "left"<<++num<<".txt";
  }
  vcl_ofstream file(fname.str().c_str());
  for(Ditr i=dev_left.begin(); i!=dev_left.end(); ++i)
    file << *i << "\n";

  double last_dev = 0.0;
  for(Ditr i=dev_left.begin(); i!=dev_left.end(); ++i){
    if( (*i-last_dev) > 2.0 )
      break;
    last_dev = *i;
  }
  min = *median - last_dev;

  last_dev = 0.0;
  for(Ditr i=dev_right.begin(); i!=dev_right.end(); ++i){
    if( (*i-last_dev) > 2.0 )
      break;
    last_dev = *i;
  }
  max = *median + last_dev;
}


double bounding_box_sum(const rsdl_bins_2d<double,double>& bins,
                        vnl_double_2& x0, vnl_double_2& x1)
{
  vcl_vector<double> weights;
  vcl_vector<vnl_vector_fixed<double,2> > points;
  bins.points_in_bounding_box(x0,x1,points,weights);
  vnl_double_2 center = (x0+x1)*0.5;
  vnl_vector_fixed<double,2> new_x0(center), new_x1(center);
  double sum = 0.0;
  for(unsigned i=0; i<points.size(); ++i){
    if(points[i][0] < new_x0[0]) new_x0[0] = points[i][0];
    if(points[i][1] < new_x0[1]) new_x0[1] = points[i][1];
    if(points[i][0] > new_x1[0]) new_x1[0] = points[i][0];
    if(points[i][1] > new_x1[1]) new_x1[1] = points[i][1];
    sum += weights[i];
  }
  x0 = new_x0;
  x1 = new_x1;
  return sum;
}

}; // end anonymous namespace


//: robustly crop the strongest features to get a bounding box
void
vidreg_matcher::robust_crop(vidreg_salient_group& group) const
{
  typedef vcl_map<rgrl_feature_sptr, double> Wmap;
  typedef Wmap::const_iterator Wmap_citr;

  const Wmap& weight_map = group.weight_map();
  vnl_double_2 center = group.growth_center();
  rgrl_mask_box full_region = group.view()->region();

  rsdl_bins_2d<double,double> bins(vnl_double_2(full_region.x0()),
                                   vnl_double_2(full_region.x1()),
                                   vnl_double_2(10,10));
  for(Wmap_citr i=weight_map.begin(); i!=weight_map.end(); ++i){
    if(i->second > .9)
      bins.add_point(i->first->location(), i->second);
  }

  vnl_double_2 x0(center), x1(center);
  x0 -= 30.0;
  x1 += 30.0;

  double sum = bounding_box_sum(bins,x0,x1);

  vcl_cout << "total weight = "<<sum << vcl_endl;
  center = (x0+x1)*0.5;

  // test left
  {
    vnl_double_2 p0(x0[0]-(center[0]-x0[0]),x0[1]);
    vnl_double_2 p1(x0[0],x1[1]);
    double new_sum = bounding_box_sum(bins,p0,p1);
    vcl_cout << "left sum = "<<new_sum<<vcl_endl;
  }

  // test right
  {
    vnl_double_2 p0(x1[0],x0[1]);
    vnl_double_2 p1(x1[0]+(center[0]-x0[0]),x1[1]);
    double new_sum = bounding_box_sum(bins,p0,p1);
    vcl_cout << "right sum = "<<new_sum<<vcl_endl;
  }

  // test up
  {
    vnl_double_2 p0(x0[0],x0[1]-(center[1]-x0[1]));
    vnl_double_2 p1(x1[0],x0[1]);
    double new_sum = bounding_box_sum(bins,p0,p1);
    vcl_cout << "up sum = "<<new_sum<<vcl_endl;
  }

  // test down
  {
    vnl_double_2 p0(x0[0],x1[1]);
    vnl_double_2 p1(x1[0],x1[1]+(center[1]-x0[1]));
    double new_sum = bounding_box_sum(bins,p0,p1);
    vcl_cout << "down sum = "<<new_sum<<vcl_endl;
  }

#if 0
  vcl_cout << "old center: "<<center<<vcl_endl;
  for(unsigned c = 0; c<30; ++c){
    vcl_vector<double> weights;
    vcl_vector<vnl_vector_fixed<double,2> > points;
    bins.points_in_bounding_box(x0,x1,points,weights);
  
    vnl_double_2 sum(0.0,0.0), sum2(0.0,0.0);
    double weight_sum = 0.0;
    for(unsigned i=0; i<points.size(); ++i){
      sum += points[i] * weights[i];
      sum2 += element_product(points[i],points[i]) * weights[i];
      weight_sum += weights[i];
    }
    assert(weight_sum > 0);
    vnl_double_2 mean(sum/weight_sum);
    vnl_double_2 stdev(sum2 - element_product(sum,sum)/weight_sum);
    stdev[0] = vcl_sqrt(stdev[0]/weight_sum);
    stdev[1] = vcl_sqrt(stdev[1]/weight_sum);

    //vcl_cout << "mean: "<<mean<< "  stdev*3: "<<stdev3<<vcl_endl;

    x0 = mean - stdev*2.5;
    x1 = mean + stdev*2.5;
  }
  vcl_cout << "---------------------------------------"<<vcl_endl;
#endif
  group.view()->set_region(rgrl_mask_box(x0,x1));

}


//: use edge connectivity to resolve match conflicts
void
vidreg_matcher::group_edges(const vcl_vector<vidreg_edge>& edges)
{
  vcl_vector<vidreg_edge> sorted_edges(edges);
  vcl_sort(sorted_edges.begin(), sorted_edges.end(), vidreg_edge::dec_size_order);

  typedef vcl_vector<vidreg_edge>::const_iterator Eitr;
  const unsigned group_count = salient_groups_.size();

  for(Eitr e=sorted_edges.begin(); e!=sorted_edges.end(); ++e){
    vidreg_feature_edgel* edgel = e->head();
    unsigned total = 0, matched = 0;
    vcl_vector<unsigned> votes(group_count,0);
    for(; edgel && edgel->next() != e->head(); edgel = edgel->next()){
      bool is_matched = false;
      for(unsigned gs=0; gs<group_count; ++gs){
        double weight = salient_groups_[gs]->weight(edgel);
        if( weight > 0.0 )
          is_matched = true;
        if( weight == 1.0){
          ++votes[gs];
          break;
        }
      }
      if(is_matched)
        ++matched;
      ++total;
    }

    int label = -1;
    unsigned max_votes = 0, sec_votes = 0;
    for(unsigned gs=0; gs<group_count; ++gs){
      if(votes[gs] > max_votes){
        sec_votes = max_votes;
        max_votes = votes[gs];
        label = gs;
      }
    }
    if(max_votes < 2*sec_votes)
      label = -1;

    if( label >=0 ){
      for(edgel = e->head(); edgel && edgel->next() != e->head(); edgel = edgel->next()){
        for(unsigned gs=0; gs<group_count; ++gs){
          if(gs == (unsigned)label)
            salient_groups_[gs]->assign_weight(edgel,1.0);
          else
            salient_groups_[gs]->assign_weight(edgel,0.0);
        }
      }
    }
  }

#if 0
  for(Eitr e=edge_heads.begin(); e!=edge_heads.end(); ++e){
    vidreg_feature_edgel* edgel = *e;
    unsigned count = 0, label_count = 0;
    int label = -1, last_label = -1, global_label = -1;
    vcl_vector<unsigned> votes(group_count,0);
    for(; edgel && edgel->next() != *e; edgel = edgel->next()){
      label = -1;
      for(unsigned gs=0; gs<group_count; ++gs){
        if(salient_groups_[gs]->weight(edgel) == 1.0){
          label = gs;
          ++votes[gs];
          break;
        }
      }
      if(label >= 0){
        if(last_label == label){
          ++label_count;
        }
        else{
          label_count = 1;
        }
      }
      else
        label_count = 0;

      if( label_count > 3 ){
        if(global_label == -1)
          global_label = label;
        else if(global_label != label)
          global_label = -2;
      }
      last_label = label;
      ++count;
    }


    if( global_label >=0 ){
      for(edgel = *e; edgel && edgel->next() != *e; edgel = edgel->next()){
        for(unsigned gs=0; gs<group_count; ++gs){
          if(gs == global_label)
            salient_groups_[gs]->assign_weight(edgel,1.0);
          else
            salient_groups_[gs]->assign_weight(edgel,0.0);
        }
      }
    }

    else{
      vcl_cout << "label: "<<global_label<< " size: "<< count;
      for(unsigned gs=0; gs<group_count; ++gs)
        vcl_cout <<" v["<<gs<<"]="<< votes[gs];
      vcl_cout << vcl_endl;
    }
  }
#endif


}


