#include "biob_explicit_neighborhood_structure.h"
#include "biob_grid_worldpt_roster.h"
#include <vcl_cassert.h>
#include <vgl/vgl_distance.h>
#include "biob_find_roster_bounding_box.h"

typedef vcl_list<biob_worldpt_index> list_t;
typedef list_t::const_iterator iterator_t;

void add_nearby_points(const biob_worldpt_roster_sptr roster, worldpt pt, const vcl_list<biob_worldpt_index> & candidates,
                       double distance, vcl_list<biob_worldpt_index> & nearby_points){
  for (iterator_t it = candidates.begin(); it != candidates.end(); ++it){
    if (vgl_distance(roster->point(*it), pt) <= distance){
      nearby_points.push_back(*it);
    }
  }
}

void biob_explicit_neighborhood_structure::populate(biob_worldpt_roster_sptr roster, double resolution){
  /* Construct a vector of lists with a component for each voxel in a grid.
     Add each point to the list of the voxel containing it.
     Next, for each point p, go through all points p' in the voxel containing p and the neighbors of that voxel;
        for each p' close enough to p, add to p's list.
  */
  neighbors_vector_.resize(roster->num_points());
  biob_grid_worldpt_roster grid(biob_find_roster_bounding_box(&(*roster), resolution), resolution);
  vcl_vector<vcl_list<biob_worldpt_index> > voxel_inhabitants(grid.num_points());
  //first find the list for each voxel of the roster points in that voxel
  for (unsigned long int i = 0; i < roster->num_points(); ++i){
    biob_worldpt_index grid_pti;
    //find the voxel containing point i of the roster
    bool found = grid.voxel(roster->point(biob_worldpt_index(i)), grid_pti);
    assert(found);
    //add point i to that voxel's list
    voxel_inhabitants[grid_pti.index()].push_back(biob_worldpt_index(i));
  }
  //now for each roster point, go through points in that point's voxel and neighboring voxels.
  for (unsigned long int i = 0; i < roster->num_points(); ++i){
    biob_worldpt_index grid_pti;
    //find the voxel containing point i of the roster
    bool found = grid.voxel(roster->point(biob_worldpt_index(i)), grid_pti);
    assert(found);
    //get the list of voxels containing that voxel and neighbors
    list_t grid_neighbors = grid.neighboring_voxels(grid_pti);
    for (iterator_t it = grid_neighbors.begin(); it != grid_neighbors.end(); ++it){
      add_nearby_points(roster, roster->point(biob_worldpt_index(i)),
                        voxel_inhabitants[it->index()], resolution, 
                        neighbors_vector_[i]);
    }
  }
}

  
