#ifndef _baml_mog_cxx_
#define _baml_mog_cxx_

#include "baml_mog.h"

#include <vcl_cassert.h>

//int baml_mog::counter = 0;

//-------------------------------------------------------
baml_mog::baml_mog(
  int num_mixtures ) :
    num_mixtures_( num_mixtures )
{
  num_components_ = 1;
  //counter++;
  components_ = new float*[num_components_];
};
  

//-----------------------------------------------------------
baml_mog::~baml_mog()
{
  //counter--;
  delete[] components_;
};


//----------------------------------------------------------
vnl_vector<int> 
baml_mog::component_sizes()
{
  vnl_vector<int> cs( 1 );
  cs(0) = 3*num_mixtures_;
  return cs;
};


//-----------------------------------------------------------
vnl_vector<int> 
baml_mog::component_ids()
{
  vnl_vector<int> ci( 1 );
  ci(0) = 0;
  return ci;
};


//----------------------------------------------------------
void 
baml_mog::initialize_component(
  int component_index )
{
  if( component_index == 0 ){
    for( int i = 0; i < 3*num_mixtures_; i++ )
      components_[0][i] = 0.0;
  }
};


//--------------------------------------------------------
float 
baml_mog::prob(
  float color,
  const vnl_vector<float>& light )
{
  float* d = components_[0];

  float p = 0;
  float total_weight = 0;
  for( int i = 0; i < num_used_mixtures(); i++ ){
    float weight = d[3*i];
    if( weight == 0 ) break;
    float mean = d[3*i+1];
    float sd = d[3*i+2];
    p += (float)( weight*.3989*exp( 
      -(color-mean)*(color-mean)/(2*sd*sd) )/sd );
    total_weight += weight;
  }

  // Case 1: No mixture mixture created yet.
  if( total_weight == 0 ) return 1.0;

  // Case 2: One or more mixture mixtures.
  p /= total_weight;
  if( !( p > .01 ) ) p = (float).01;
  return p;
};


//--------------------------------------------------------
float 
baml_mog::prob_log(
  float color,
  const vnl_vector<float>& light )
{
  float* d = components_[0];

  float p = 0;
  float total_weight = 0;
  for( int i = 0; i < num_used_mixtures(); i++ ){
    float weight = d[3*i];
    if( weight == 0 ) break;
    float mean = d[3*i+1];
    float sd = d[3*i+2];
    //if( color < .5 ) sd *= (float)(.25+.75*color/.5);// HACK
    p += (float)( weight*.3989*exp( 
      -(log(color+.01)/log(mean+.01))*(log(color+.01)/log(mean+.01))/(2*sd*sd) )/sd );
    total_weight += weight;
  }

  // Case 1: No mixture mixture created yet.
  if( total_weight == 0 ) return 1.0;

  // Case 2: One or more mixture mixtures.
  p /= total_weight;
  if( !( p > .01 ) ) p = (float).01;
  return p;
};


//-------------------------------------------------------
bool 
baml_mog::update(
  float color,
  float color_weight,
  const vnl_vector<float>& light )
{
  assert( color >= 0 );
  assert( color_weight >= 0 );
  float* d = components_[0];

  float starting_sd = (float).1;
  float min_sd = (float).025;
  int used_mixtures = num_used_mixtures();

  // For first observation, record in first mixture.
  if( used_mixtures == 0 ){
    d[0] = color_weight;
    d[1] = color;
    d[2] = starting_sd;
    return true;
  }

  // Otherwise find the mixture the color matches with.
  float weight, mean, sd;
  int mixture_match = -1;
  for( int i = 0; i < used_mixtures; i++ ){    
    mean = d[3*i+1];
    sd = d[3*i+2];
    if( fabs( color-mean ) < 2*sd ){
      mixture_match = i;
      weight = d[3*i];
      break;
    }
  }

  // Case 1: A mixture gets updated.
  if( mixture_match != -1 ){
    float prob_scale = color_weight/( weight + color_weight );
    d[3*mixture_match] += color_weight;
    d[3*mixture_match+1] += prob_scale*( color - mean );
    d[3*mixture_match+2] = sqrt( sd*sd + prob_scale*
      ( ((color-mean)*(color-mean)) - sd*sd ) );
    if( d[3*mixture_match+2] <  min_sd ) 
      d[3*mixture_match+2] = min_sd;
  }

  else{
    // Case 2: A node gets replaced.
    if( used_mixtures == num_mixtures_ )
      mixture_match = used_mixtures - 1;

    // Case 3: A node gets added.
    else{
      mixture_match = used_mixtures;
      used_mixtures++;
    }

    d[3*mixture_match] = color_weight;
    d[3*mixture_match+1] = color;
    d[3*mixture_match+2] = starting_sd;
  }

  // Now reorder the mixtures if necessary.
  for( int i = used_mixtures-1; i > 0; i-- ){
    int i3 = i*3;
    if( d[i3] > d[i3-3] ){
      weight = d[i3];
      mean = d[i3+1];
      sd = d[i3+2];
      d[i3] = d[i3-3];
      d[i3+1] = d[i3-2];
      d[i3+2] = d[i3-1];
      d[i3-3] = weight;
      d[i3-2] = mean;
      d[i3-1] = sd;
    }
  }
  return true;
};


//-------------------------------------------------------
float 
baml_mog::expected_color(
  const vnl_vector<float>& light )
{
  float* d = components_[0];
  float c = 0;
  float total_weight = 0;

  // TEMP HACK
  if( num_used_mixtures() == 0 ) return -1;
  else return d[1];

  for( int i = 0; i < num_used_mixtures(); i++ ){
    total_weight+= d[3*i];
    c+= d[3*i]*d[3*i+1];
  }
  if( total_weight <= 0 ) return -1;
  return c/total_weight;
};


//-------------------------------------------------------
void 
baml_mog::draw_x3d(
  vcl_ofstream& x3d_stream )
{

};


//------------------------------------
int baml_mog::num_used_mixtures()
{
  int used_mixtures = 0;
  for( int i = 0; i < num_mixtures_; i++ ){
    if( components_[0][3*i] == 0 ) break;
    used_mixtures++;
  }
  return used_mixtures;
};     


#endif // _baml_mog_cxx_
