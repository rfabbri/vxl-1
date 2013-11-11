#include "biob_roster_to_grid_mapping.h"
#include <vgl/vgl_distance.h>
#include <vcl_limits.h>

  /* Returns a vector whose entry i is the biob_worldpt_index of the point
     in sample_roster closest to point i of the grid.
     If no sample_roster point is within the voxel corresponding to grid point i,
     entry i is biob_worldpt_index(sample_roster->num_points()) to
     indicate no close enough sample_roster point found

     Iterate through the samples.  
     For each, find the voxel containing it.
     Find the distance to the middle of the voxel.
     If the distance is less than the best distance found previously,
     assign the response to the voxel
  */

void biob_roster_to_grid_mapping(biob_worldpt_roster_sptr sample_roster,  const biob_grid_worldpt_roster & grid_roster,   vcl_vector<biob_worldpt_index>  & which_sample){
  which_sample.resize(grid_roster.num_points());
  //Initialize which_sample entries to sample_roster->num_points() to indicate no suitable point found yet for coresponding grid point
  for (unsigned int i = 0; i < grid_roster.num_points(); ++i){
    which_sample[i] = biob_worldpt_index(sample_roster->num_points());
  }

  vcl_vector<double> distances(grid_roster.num_points(), double(vcl_numeric_limits<unsigned long long>::max()));//initialize to big number

  for (unsigned long int i = 0; i < sample_roster->num_points(); ++i){
    //get the sample point
    worldpt sample_pt = sample_roster->point(biob_worldpt_index(i));
    //get the index of the voxel containing sample_pt
    biob_worldpt_index voxel_index;
    int within_voxel = grid_roster.voxel(sample_pt, voxel_index);
    if (within_voxel){
        //find the distance to the center of the voxel
        double distance = vgl_distance(sample_pt, grid_roster.point(voxel_index));
       //if the distance is less, update the distance and the response
        assert(voxel_index.index() < grid_roster.num_points());
        if (distance < distances[voxel_index.index()]){
           distances[voxel_index.index()] = distance;
           which_sample[voxel_index.index()] = biob_worldpt_index(i);
       }
    }
  }
}
