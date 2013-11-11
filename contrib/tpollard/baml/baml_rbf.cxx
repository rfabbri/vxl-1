#ifndef _baml_rbf_cxx_
#define _baml_rbf_cxx_

#include "baml_rbf.h"

#include <vcl_cassert.h>

/*
//--------------------------------------------
baml_rbf::baml_rbf()
{
  outer_nbhd_rad_ = .2;
  inner_nbhd_rad_ = .1;
  min_mode_rad_ = .05;
  max_mode_rad_ = .2;
  color_peak_var_ = .1*.1;
  color_peak_scale_ = 1;
};


//---------------------------------------------
float 
baml_rbf::prob(
  float color,
  const vnl_vector<float>& light )
{
  if( light_nodes_.size() == 0 )
    return 0;

  // Consider each sample's contribution to the total probability.
  float lower_color_bound = 0.0, upper_color_bound = 1.0;
  float lcb_change = 0.0, ucb_change = 0.0;
  float total_prob = 0;
  float region_prob = 0;
  for( int l = 0; l < light_nodes_.size(); l++ ){
    float ld = light_distance( light_nodes_[l], light );

    // Find the range of colors that this sample gives probability to.
    if( ld > outer_nbhd_rad_ ) continue;
    float lower_mode_bound, upper_mode_bound;
    get_mode_bounds( ld, color_nodes_[l], lower_mode_bound, upper_mode_bound );
    float mode_diam = upper_mode_bound - lower_mode_bound;
    float mode_prob = 1.0/mode_diam;

    // Update the estimate of the tightest bounds around the new color.
    if( lower_color_bound < upper_mode_bound && upper_mode_bound < color ){
      lower_color_bound = upper_mode_bound; lcb_change = -mode_prob; }
    if( lower_color_bound < lower_mode_bound && lower_mode_bound < color ){
      lower_color_bound = lower_mode_bound; lcb_change = mode_prob; }
    if( upper_color_bound > lower_mode_bound && lower_mode_bound > color ){
      upper_color_bound = lower_mode_bound; ucb_change = mode_prob; }
    if( upper_color_bound > upper_mode_bound && upper_mode_bound > color ){
      upper_color_bound = upper_mode_bound; ucb_change = -mode_prob; }

    // Update the probability in this region.
    if( color < upper_mode_bound && color > lower_mode_bound )
      region_prob+= mode_prob;
    total_prob+= 1.0;
  }
  if( total_prob == 0 ) return 0;
  float color_bound_diam = upper_color_bound - lower_color_bound;
  float color_bound_mid = lower_color_bound + .5*color_bound_diam;
  region_prob /= total_prob;

  // Return the probability, giving a bonus probability to peaks.
  //if( lcb_change > 0.0 && ucb_change < 0.0 )
    //return region_prob + region_prob*color_peak_scale_*exp(
      //-(color-color_bound_mid)*(color-color_bound_mid)/color_peak_var_ );
  return region_prob;

};


//----------------------------------------------
bool
baml_rbf::update(
  float color,
  const vnl_vector<float>& light )
{
  // Get all samples within the inner nbhd radius.
  vcl_vector<int> inner_neighbors;
  for( int l = 0; l < light_nodes_.size(); l++ ){
    float ld = light_distance( light, light_nodes_[l] );
    if( ld < inner_nbhd_rad_ ) inner_neighbors.push_back(l);
  }

  // Add if no neighbors.
  if( inner_neighbors.size() == 0 ){
    light_nodes_.push_back( light );
    color_nodes_.push_back( color );
    return true;
  }

  // Try to find a consistent neighbor.
  for( int n = 0; n < inner_neighbors.size(); n++ ){
    float ld = light_distance( light, light_nodes_[inner_neighbors[n]] );
    float lower_mode_bound, upper_mode_bound;
    get_mode_bounds( ld, color_nodes_[inner_neighbors[n]], 
      lower_mode_bound, upper_mode_bound );
    if( color > lower_mode_bound && color < upper_mode_bound ){
      // Boost this mode's weight
      return false;
    }
  }

  //float color_prob = prob( color, light );
  //if( color_prob > prob_thresh ) return false;

  // If no consistent neighbor add to model.
  light_nodes_.push_back( light );
  color_nodes_.push_back( color );
  return true;
};


//---------------------------------------------
float
baml_rbf::best_color(
  const vnl_vector<float>& light )
{
  // Poor brute force implementation.
  float best_color = 1.0, best_color_prob = 0;
  for( float c = 0; c < 1; c+=.025 ){
    float this_color_prob = prob( c, light );
    if( this_color_prob > best_color_prob ){
      best_color_prob = this_color_prob;
      best_color = c;
    }
  }
  return best_color;
};

//---------------------------------------------
bool 
baml_rbf::write_model(
  vnl_vector<float>& data )
{
  return true;
};


//---------------------------------------------
bool 
baml_rbf::read_model(
  const vnl_vector<float>& data )
{
  return true;
};


//-----------------------------------------------
float 
baml_rbf::light_distance(
  const vnl_vector<float>& light1,
  const vnl_vector<float>& light2 )
{
  int light_dim = light1.size();
  float dl = 0;
  for( int d = 0; d < light_dim; d++ ) dl += 
    (light1(d)-light2(d))*(light1(d)-light2(d));
  return sqrt( dl );
};


//------------------------------------------------
void 
baml_rbf::get_mode_bounds(
  float ld,
  float mode_center,
  float& lower_mode_bound,
  float& upper_mode_bound )
{
  float mode_bound_rad = min_mode_rad_ + 
    (max_mode_rad_-min_mode_rad_)*ld/outer_nbhd_rad_;
  lower_mode_bound = mode_center - mode_bound_rad;
  if( lower_mode_bound < 0 ) lower_mode_bound = 0;
  upper_mode_bound = mode_center + mode_bound_rad;
  if( upper_mode_bound > 1 ) upper_mode_bound = 1;
};

*/
#endif // _baml_rbf_cxx_
