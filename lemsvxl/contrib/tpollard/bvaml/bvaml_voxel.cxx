#ifndef _bvaml_voxel_cxx_
#define _bvaml_voxel_cxx_

#include "bvaml_voxel.h"

#include <vcl_cassert.h>
#include <vnl/vnl_vector_fixed.h>


//-----------------------------------------------
bvaml_voxel::bvaml_voxel(
  bvaml_params* params ) :
    params_( params ),
    occupancy_prob( NULL ),
    misc_storage( NULL ),
    appearance( NULL )
{

};


//-------------------------------------------------
bvaml_voxel::~bvaml_voxel()
{
  if( appearance != NULL )
    delete appearance;
};


//---------------------------------------------
void 
bvaml_voxel::initialize_occupancy()
{
  *occupancy_prob = params_->initial_occupancy_prob();
};


//-----------------------------------------------------------
void 
bvaml_voxel::initialize_misc()
{
  for( int i = 0; i < params_->misc_storage_size; i++ )
    misc_storage[i] = -1.0;
};


#endif // _bvaml_voxel_cxx_
