#ifndef _bvaml_supervoxel_cxx_
#define _bvaml_supervoxel_cxx_

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <baml/baml_appearance.h>
#include <baml/baml_mog.h>
#include <baml/baml_multi_mog.h>
#include "bvaml_supervoxel.h"
#include "bvaml_log_writer.h"
#include "bvaml_voxel.h"


//---------------------------------------------
bvaml_supervoxel::bvaml_supervoxel(
  bvaml_params* params,
  vcl_string file_namebase ) :
    file_namebase_( file_namebase ),
    params_( params ),
    occupancy_cache_( NULL ),
    misc_cache_( NULL ),
    appearance_cache_( NULL ),
    num_appearance_components_( 0 )
{

};
 

//----------------------------------------
bvaml_supervoxel::~bvaml_supervoxel()
{ 
  clear();
}


//----------------------------------------
void
bvaml_supervoxel::get_voxel(
  const vgl_point_3d<int>& voxel_index,
  bvaml_voxel& voxel,
  bool load_occupancy,
  bool load_misc,
  bool load_appearance,
  const vnl_vector<float>* local_light )
{
  int start_index =
    voxel_index.x() + 
    voxel_index.y()*params_->supervoxel_length +
    voxel_index.z()*params_->supervoxel_length*params_->supervoxel_length;

  if( !load_occupancy )
    voxel.occupancy_prob = NULL;
  else{
    if( occupancy_cache_ == NULL )
      load_occupancies();
    voxel.occupancy_prob = occupancy_cache_ + start_index;
  }

  if( !load_misc )
    voxel.misc_storage = NULL;
  else{
    if( misc_cache_ == NULL )
      load_misc_storage();
    voxel.misc_storage = misc_cache_ + params_->misc_storage_size*start_index;
  }

  if( voxel.appearance != NULL )
    delete voxel.appearance;

  if( !load_appearance )
    voxel.appearance = NULL;
  else{
    if( appearance_cache_ == NULL )
      load_local_appearance( *local_light );
    voxel.appearance = new_local_appearance();
    for( int a = 0; a < num_appearance_components_; a++ ){
      if( appearance_cache_[a] != NULL )
        voxel.appearance->set_component( a, 
          appearance_cache_[a] + appearance_sizes_[a]*start_index );
      else
        voxel.appearance->set_component( a, NULL );
    }
  }
};


//-------------------------------------------------------------
void 
bvaml_supervoxel::load_occupancies()
{
  bool initialized = load_cache( occupancy_filename(), 1, occupancy_cache_ );

  if( !initialized ){
    bvaml_voxel prototype( params_ );
    for( int x = 0; x < params_->supervoxel_length; x++ ){
      for( int y = 0; y < params_->supervoxel_length; y++ ){
        for( int z = 0; z < params_->supervoxel_length; z++ ){
          get_voxel( vgl_point_3d<int>(x,y,z), prototype, true, false, false, NULL );
          prototype.initialize_occupancy();
        }
      }
    }
  }
};


//-------------------------------------------------------------
void 
bvaml_supervoxel::load_misc_storage()
{
  bool initialized = load_cache( misc_filename(), params_->misc_storage_size, misc_cache_ );

  if( !initialized ){
    bvaml_voxel prototype( params_ );
    for( int x = 0; x < params_->supervoxel_length; x++ ){
      for( int y = 0; y < params_->supervoxel_length; y++ ){
        for( int z = 0; z < params_->supervoxel_length; z++ ){
          get_voxel( vgl_point_3d<int>(x,y,z), prototype, false, true, false, NULL );
          prototype.initialize_misc();
        }
      }
    }
  }
};



//-------------------------------------------------------------
void 
bvaml_supervoxel::load_local_appearance(
  const vnl_vector<float>& local_light )
{
  if( appearance_cache_ != NULL )
    return;
  local_light_ = local_light;

  // Set up voxel parameters around this local light.
  bvaml_voxel prototype( params_ );
  prototype.appearance = new_local_appearance();
  appearance_sizes_ = prototype.appearance->component_sizes();
  appearance_ids_ = prototype.appearance->component_ids();
  num_appearance_components_ = appearance_sizes_.size();


  appearance_cache_ = new float*[ num_appearance_components_ ];
  for( int a = 0; a < num_appearance_components_; a++ )
    appearance_cache_[a] = NULL;

  // Load each cache, record what needs to be initialized.
  vcl_vector<bool> appearance_initialized;
  bool all_appearances_initialized = true;
  for( int a = 0; a < num_appearance_components_; a++ ){
    appearance_initialized.push_back( load_cache( appearance_filename( appearance_ids_[a] ), 
      appearance_sizes_[a], appearance_cache_[a] ) );
    if( !appearance_initialized[a] ) all_appearances_initialized = false;
  }

  // Initialize if necessary.
  if( !all_appearances_initialized ){
    for( int x = 0; x < params_->supervoxel_length; x++ ){
      for( int y = 0; y < params_->supervoxel_length; y++ ){
        for( int z = 0; z < params_->supervoxel_length; z++ ){
          get_voxel( vgl_point_3d<int>(x,y,z), prototype, false, false, true, &local_light );
          for( int a = 0; a < num_appearance_components_; a++ )
            if( !appearance_initialized[a] )
              prototype.appearance->initialize_component(a);
        }
      }
    }
  }
};


//-------------------------------------------------------------
void 
bvaml_supervoxel::save()
{
  save_cache( occupancy_filename(), 1, occupancy_cache_ );
  save_cache( misc_filename(), params_->misc_storage_size, misc_cache_ );
  for( int a = 0; a < num_appearance_components_; a++ )
    save_cache( appearance_filename( appearance_ids_[a] ), appearance_sizes_[a], appearance_cache_[a] );
  clear();
};


//--------------------------------------------------------------
void 
bvaml_supervoxel::clear()
{
  if( occupancy_cache_ != NULL ){
    delete[] occupancy_cache_;
    occupancy_cache_ = NULL;
  }
  if( misc_cache_ != NULL ){
    delete[] misc_cache_;
    misc_cache_ = NULL;
  }
  if( appearance_cache_ != NULL ){
    for( int a = 0; a < num_appearance_components_; a++ )
      if( appearance_cache_[a] != NULL )
        delete[] appearance_cache_[a];
    delete[] appearance_cache_;
    appearance_cache_ = NULL;
  }
  num_appearance_components_ = 0;
};


//-------------------------------------------------------------
bool
bvaml_supervoxel::load_cache(
  vcl_string file_name,
  int data_size,
  float*& cache,
  bool allocate_if_needed )
{
  if( cache != NULL )
    return true;

  int cache_size = data_size*params_->supervoxel_length*
      params_->supervoxel_length*params_->supervoxel_length;

  // Read from file if it exists.
  vcl_ifstream s( file_name.c_str(), vcl_ifstream::binary );
  if( s.good() ){
    cache = new float[ cache_size ];
    char* byte_cache = (char*)cache;
    s.read( byte_cache, cache_size*sizeof(float) );
    return true;
  }
  else if( allocate_if_needed ){
    cache = new float[ cache_size ];
    return false;
  }
  
  return false;
};


//--------------------------------------------------------------
void 
bvaml_supervoxel::save_cache(
  vcl_string file_name,
  int data_size,
  float*& cache )
{
  if( cache == NULL ) return;

  int cache_size = data_size*params_->supervoxel_length*
    params_->supervoxel_length*params_->supervoxel_length;

  vcl_ofstream s( file_name.c_str(), vcl_ifstream::binary );
  if( s.good() ){
    char* byte_cache = (char*)cache;
    s.write( byte_cache, cache_size*sizeof(float) );
  }
};


//--------------------------------------------------------------
vcl_string 
bvaml_supervoxel::occupancy_filename()
{
  vcl_string s = file_namebase_;
  s = s + "_o";
  return s;
};


//--------------------------------------------------------------
vcl_string 
bvaml_supervoxel::misc_filename()
{
  vcl_string s = file_namebase_;
  s = s + "_m";
  return s;
};


//------------------------------------------------------------
vcl_string 
bvaml_supervoxel::appearance_filename( int i )
{
  vcl_string s = file_namebase_;
  char numberbuffer[8];
  s = s + "_a"; s = s + itoa(i,numberbuffer,10);
  return s;
};


//----------------------------------------------------------------
baml_appearance* 
bvaml_supervoxel::new_local_appearance()
{
  if( params_->appearance_model == 0 )
    return new baml_mog( params_->num_mixtures );
  else if( params_->appearance_model == 1 )
    return new baml_multi_mog_local( params_->num_mixtures, local_light_ );

  vcl_cerr << "ERROR: Appearance model not yet implemented.  ";
  return NULL;
};


#endif // _bvaml_supervoxel_cxx_
