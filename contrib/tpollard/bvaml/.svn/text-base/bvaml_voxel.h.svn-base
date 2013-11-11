#ifndef _bvaml_voxel_h_
#define _bvaml_voxel_h_

//:
// \file
// \brief A class for a appearance model of a voxel.
// \author Thomas Pollard
// \date 12/20/06
// 
//   This is....

#include <baml/baml_appearance.h>
#include "bvaml_params.h"


class bvaml_voxel{

public:

  // Create a voxel from a data vector.
  bvaml_voxel(
    bvaml_params* params );

  ~bvaml_voxel();
  
  // Set initial values. 
  void initialize_occupancy();
  void initialize_misc();

  // Data pointers
  baml_appearance* appearance;
  float* occupancy_prob;
  float* misc_storage;

protected:
  
  bvaml_params* params_;

private:

  bvaml_voxel( bvaml_voxel const& v );
};


#endif // _bvaml_voxel_h_
