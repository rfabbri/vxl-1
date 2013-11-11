#ifndef _bvaml_ray_h_
#define _bvaml_ray_h_

//:
// \file
// \brief ...
// \author Thomas Pollard
// \date 01/15/07
// 
//   This is....


#include <vnl/vnl_vector.h>

#include "bvaml_params.h"
#include "bvaml_voxel.h"
#include <baml/baml_mog.h>


class bvaml_ray {

public:

  bvaml_ray( 
    bvaml_params* params,
    int length );

  // Get the probability of seeing the color.
  float prob(
    float color,
    const vnl_vector<float>& light );

  // Update the occupancy and mixture probabilities, and return the
  // color probability.
  float update(
    float color,
    const vnl_vector<float>& light );

  // Get the expected color on this ray.
  float expected_color(
    const vnl_vector<float>& light );

  // Store the given color in each voxel on the ray, in the last MoG
  // mode with weight = P(V=X).
  void store_color(
    float color );
  void predicted_color(
    float& color,
    float& prob_not_seen );

  // Train the mog distribution on all voxels on the ray.
  void distribution(
    baml_mog* mog,
    const vnl_vector<float>& light );

  // Storage for the ray voxels.
  vcl_vector< bvaml_voxel* > voxels;

protected:

  bvaml_params* params_;
};


#endif // _bvaml_ray_h_
