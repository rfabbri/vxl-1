#ifndef bvaml_ray_cxx_
#define bvaml_ray_cxx_

#include "bvaml_ray.h"

#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>


//------------------------------------------
bvaml_ray::bvaml_ray(
  bvaml_params* params,
  int length ) :
  params_( params )
{
  voxels.reserve( length );
};


//------------------------------------------
float
bvaml_ray::prob(
  float color,
  const vnl_vector<float>& light )
{
  float sumPcPw = 0;
  float hard_mult = 1;
  for( unsigned v = 0; v < voxels.size(); v++ ){
    if( voxels[v]->occupancy_prob[0] < params_->empty_thresh ) continue;
    sumPcPw += hard_mult*voxels[v]->occupancy_prob[0]*voxels[v]->appearance->prob( color, light );
    hard_mult *= ( 1- voxels[v]->occupancy_prob[0] );
  }
  return sumPcPw/(1-hard_mult);
}


//------------------------------------------
float
bvaml_ray::update(
  float color,
  const vnl_vector<float>& light )
{
  int num_voxels = voxels.size();
  vcl_vector<float> color_prob; color_prob.reserve( num_voxels );

  // Pass 1: Get the color probabilities and compute total color and
  // weight probabilities.
  float sumPcPw = 0;
  float hard_mult = 1;
  for( int v = 0; v < num_voxels; v++ ){
    if( voxels[v]->occupancy_prob[0] < params_->empty_thresh ){
      color_prob.push_back( 0.0 ); continue; }
    color_prob.push_back( voxels[v]->appearance->prob( color, light ) );
    sumPcPw += hard_mult*voxels[v]->occupancy_prob[0]*color_prob[v];
    hard_mult *= (1-voxels[v]->occupancy_prob[0]);
  }
  float sumPw = 1-hard_mult;

  // Pass 2: Update the voxels.
  float part_sumPcPw = 0, part_sumPw = 0;
  hard_mult = 1;
  for( int v = 0; v < num_voxels; v++ ){
    if( voxels[v]->occupancy_prob[0] < params_->empty_thresh ) continue;
    float occupancy_update =
      (part_sumPcPw+hard_mult*color_prob[v])/ //(part_sumPw+hard_mult))/
      (sumPcPw);//sumPw);
    part_sumPw += hard_mult*voxels[v]->occupancy_prob[0];
    part_sumPcPw += hard_mult*voxels[v]->occupancy_prob[0]*color_prob[v];
    hard_mult *= (1-voxels[v]->occupancy_prob[0]);
    float color_weight = voxels[v]->occupancy_prob[0]/sumPw;
    if( !( color_weight > 0 ) )
      color_weight = 0;

    voxels[v]->appearance->update( color, color_weight, light );
    voxels[v]->occupancy_prob[0] *= occupancy_update;
    
    if( !( voxels[v]->occupancy_prob[0] > .00001) ) 
      voxels[v]->occupancy_prob[0] = (float).00001; //HACK
    if( !( voxels[v]->occupancy_prob[0] < 1) ) 
      voxels[v]->occupancy_prob[0] = (float).99999; //HACK
  }

  float this_prob = sumPcPw/sumPw;
  if( this_prob < params_->min_prob_to_allow_empty_voxels &&
      params_->empty_thresh > 0 ){
    for( int v = 0; v < num_voxels; v++ ){
      if( voxels[v]->occupancy_prob[0] < params_->empty_thresh )
        voxels[v]->occupancy_prob[0] = 2*params_->empty_thresh;
    }
  }
  return this_prob;
};


//----------------------------------------------
float 
bvaml_ray::expected_color(
  const vnl_vector<float>& light )
{
  float sumEcPw = 0;
  float hard_mult = 1;
  for( unsigned v = 0; v < voxels.size(); v++ ){
    sumEcPw += hard_mult*voxels[v]->occupancy_prob[0]*voxels[v]->appearance->expected_color( light );
    hard_mult *= (1-voxels[v]->occupancy_prob[0]);
  }
  return sumEcPw/(1-hard_mult);
};


//------------------------------------------------
void 
bvaml_ray::store_color(
  float color )
{
  float hard_mult = 1;
  for( unsigned v = 0; v < voxels.size(); v++ ){
    voxels[v]->misc_storage[0] = color;
    voxels[v]->misc_storage[1] = hard_mult*voxels[v]->occupancy_prob[0];
    hard_mult *= (1-voxels[v]->occupancy_prob[0]);
  }
};


//------------------------------------------------
void 
bvaml_ray::predicted_color(
  float& color,
  float& prob_not_seen )
{
  prob_not_seen = 0;
  float sumEcPw = 0, sumPw = 0;

  float hard_mult = 1;
  for( unsigned v = 0; v < voxels.size(); v++ ){
    float this_color = voxels[v]->misc_storage[0];
    float this_prob_seen = voxels[v]->misc_storage[1];
    if( this_prob_seen >= 0 ){
      sumEcPw += hard_mult*voxels[v]->occupancy_prob[0]*this_color;
      sumPw += hard_mult*voxels[v]->occupancy_prob[0];
    }
    prob_not_seen += hard_mult*voxels[v]->occupancy_prob[0]*( 1-this_prob_seen );
    hard_mult *= (1-voxels[v]->occupancy_prob[0]);
  }
  color = sumEcPw/sumPw;
};


//--------------------------------------------------
// NOT COMPLETE
void 
bvaml_ray::distribution(
  baml_mog* mog,
  const vnl_vector<float>& light )
{
  float hard_mult = 1;
  for( unsigned v = 0; v < voxels.size(); v++ ){
    float this_color = voxels[v]->appearance->expected_color( light );
    if( this_color >= 0 )
      mog->update( this_color, hard_mult*voxels[v]->occupancy_prob[0], light );
    hard_mult *= (1-voxels[v]->occupancy_prob[0]);
  }
};


#endif // bvaml_ray_cxx_
