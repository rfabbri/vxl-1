
#ifndef _baml_multi_mog_cxx_
#define _baml_multi_mog_cxx_

#include <vcl_cassert.h>

#include "baml_multi_mog.h"
#include "baml_mog.h"


// Component storage is as follows:
// 0 : mog for light


//-------------------------------------------------------
baml_multi_mog_local::baml_multi_mog_local(  
  int num_mixtures,
  const vnl_vector<float>& light ) :
    num_mixtures_( num_mixtures ),
    num_light_bins_( 8 )
{
  light_ = light;
  num_components_ = 1;
  components_ = new float*[1];
};
  

//-----------------------------------------------------------
baml_multi_mog_local::~baml_multi_mog_local()
{
  delete[] components_;
};


//----------------------------------------------------------
vnl_vector<int> 
baml_multi_mog_local::component_sizes()
{
  baml_mog mog( num_mixtures_ );
  vnl_vector<int> cs( 1 );
  cs(0) = mog.component_sizes()[0];
  return cs;
};


//-----------------------------------------------------------
vnl_vector<int> 
baml_multi_mog_local::component_ids()
{
  vnl_vector<int> ci( 1 );
  ci(0) = get_light_bin( light_ );
  return ci;
};


//----------------------------------------------------------
void 
baml_multi_mog_local::initialize_component(
  int component_index )
{
  if( component_index == 0 ){
    baml_mog mog( num_mixtures_ );
    mog.set_component( 0, components_[0] );
    mog.initialize_component(0);
  }
};


//----------------------------------------------------------
float 
baml_multi_mog_local::prob(
  float color,
  const vnl_vector<float>& light )
{
  baml_mog mog( num_mixtures_ );
  mog.set_component( 0, components_[0] );
  return mog.prob( color, light );
};


//----------------------------------------------------------
bool 
baml_multi_mog_local::update(
  float color,
  float color_weight,
  const vnl_vector<float>& light )
{
  baml_mog mog( num_mixtures_ );
  mog.set_component( 0, components_[0] );
  return mog.update( color, color_weight, light );
};


//---------------------------------------------------------
float 
baml_multi_mog_local::expected_color(
  const vnl_vector<float>& light )
{
  baml_mog mog( num_mixtures_ );
  mog.set_component( 0, components_[0] );
  return mog.expected_color( light );
};


//--------------------------------------------------------------
void 
baml_multi_mog_local::draw_x3d(
  vcl_ofstream& x3d_stream )
{
  
};


//---------------------------------------------------------------
int 
baml_multi_mog_local::get_light_bin(
  const vnl_vector<float>& light )
{
  /*
  int x = (int)floor( num_light_bins_*.5*(light(0)+1) );
  if( x < 0 ) x = 0; if( x >= num_light_bins_ ) x = num_light_bins_-1;
  int y = (int)floor( num_light_bins_*.5*(light(1)+1) );
  if( y < 0 ) y = 0; if( y >= num_light_bins_ ) y = num_light_bins_-1;
  int bin = y+x*num_light_bins_;
  assert( bin >= 0 && bin < num_light_bins_*num_light_bins_ );
  return bin; 
*/

  //return (int)floor( light[0]+0.5 ); // PLASTICVILLE HACK

  int bin = (int)( num_light_bins_*( asin(light(2))+1.58 )/3.14 );
  if( bin < 0 ) bin = 0;
  if( bin >= num_light_bins_ ) bin = num_light_bins_-1;
  return bin; 
};



#endif // _baml_multi_mog_cxx_
