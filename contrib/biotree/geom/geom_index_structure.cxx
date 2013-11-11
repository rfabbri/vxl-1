#include "geom_index_structure.h"
#include <biob/biob_find_roster_bounding_box.h>
#include "geom_volume.h"

/* Invariant for the enumerator:
   Between calls to constructor and next(),
   1) has_next_ is true iff next_to_return_ has a point ready 
   2) if has_next_ is true, 
      done_with_this_voxel() is false iff inhabitants_iterator points to 
      the subsequent point to return
*/


bool geom_index_structure::enumerator::done_with_this_voxel(){
  return inhabitants_iterator_ == voxel_to_inhabitants_[grid_index_.index()].end();
}

//go to the next good point if there is a next good point within current voxel
void geom_index_structure::enumerator::advance_point(){
  //assumes grid_index_ refers to a voxel
  while (!done_with_this_voxel() && 
    !(probe_volume_->contains(roster_->point(*inhabitants_iterator_)))){
          ++inhabitants_iterator_;
  }
  //P1: We're done with the voxel entirely or we've found a good point.
}

//go to the first good point starting at a subsequent voxel if there is a good point
void geom_index_structure::enumerator::advance_voxel(){
  bool must_go_to_next_voxel = true;
  while (must_go_to_next_voxel && voxel_enumerator_->has_next()){
    geom_voxel_specifier spec = voxel_enumerator_->next();
    grid_index_ = grid_.index_3d_2_1d(spec.x_index_, spec.y_index_, spec.z_index_);
    inhabitants_iterator_ = voxel_to_inhabitants_[grid_index_.index()].begin();
    advance_point();
    must_go_to_next_voxel = done_with_this_voxel();
  }
  //P2: If done_with_this_voxel() is true, there are no more good points
  //P3: If it is false, inhabitants_iterator_ is pointing to a good point
}


geom_index_structure::enumerator::enumerator(biob_worldpt_roster_sptr roster,
                                             const biob_grid_worldpt_roster & grid,
                                             const voxel_to_inhabitants_t& voxel_to_inhabitants,
                                             geom_probe_volume_sptr probe_volume,
                                             geom_voxel_enumerator_sptr voxel_enumerator)
  : roster_(roster), grid_(grid), voxel_to_inhabitants_(voxel_to_inhabitants), probe_volume_(probe_volume), voxel_enumerator_(voxel_enumerator){
  //test for no voxels at all
  if (!voxel_enumerator_->has_next()){
    has_next_ = false;
  }
  else {
    advance_voxel();
    if (done_with_this_voxel()){
      has_next_ = false;
      //By P2, satisfies invariant 1 (and, trivially, invariant 2)
    }
    else {
      has_next_ = true;
      next_to_return_ = *inhabitants_iterator_;
      //By P3, satisfies invariant 1.
      //Now make inhabitants_iterator_ point to *next* good point if one exists.
      ++inhabitants_iterator_;
      advance_point();
      //By P3, if done_with_this_voxel() is false, invariant 2 is satisfied: inhabitants_iterator_ points to a good point
      if (done_with_this_voxel()){//if true, keep going until inhabitants_iterator_ points to a good point or no more points
        advance_voxel();
        //By P2 and P3, invariant 2 holds.
      }
    }
      //invariants 1 and 2 now hold.
  }
}

bool geom_index_structure::enumerator::has_next(){
  return has_next_;
}


biob_worldpt_index geom_index_structure::enumerator::next(){
  assert(has_next_);
  //at this point, by invariant 2, if done_with_this_voxel is true then we're out of points
  //except for the one stored in next_to_return_ (which will be returned at the end of this invocation)
  biob_worldpt_index to_return = next_to_return_;
  if (done_with_this_voxel()){
    has_next_ = false;
    //after this call, invariant 1 will hold
  }
  else {
    //by invariant 2, inhabitants_iterator_; points to a good point
    next_to_return_ = *inhabitants_iterator_;
    has_next_ = true;
    //after this call, invariant 1 will be true
    //now must reestablish invariant 2
    ++ inhabitants_iterator_;//move to next candidate point
    advance_point();//move to next good point within current voxel.
    //if done_with_this_voxel() is false, we've reestablished invariant 2
    //otherwise...
    if (done_with_this_voxel()){//if no good point within current voxel.
      advance_voxel();//then move to voxel where there is a good point.
      //P2 => if done_with_this_voxel() is true, no more good points
      //P3 => if false, inhabitants_iterator_ is pointing to a good point
      //thus we've re-established invariant 2.
    }
  }
  return to_return;
}

geom_index_structure::geom_index_structure(biob_worldpt_roster_sptr roster, double resolution)
  /* Construct a vector of lists with a component for each voxel in a grid.
     Add each point to the list of the voxel containing it.
  */
  : roster_(roster), grid_box_(biob_find_roster_bounding_box(roster, resolution)), resolution_(resolution),
    grid_(grid_box_, resolution_), voxel_to_inhabitants_(grid_.num_points()) {
   // find the list for each voxel of the roster points in that voxel
  for (unsigned long int i = 0; i < roster->num_points(); ++i){
    biob_worldpt_index grid_pti;
    //find the voxel containing point i of the roster
    bool found = grid_.voxel(roster->point(biob_worldpt_index(i)), grid_pti);
    assert(found);
    //add point i to that voxel's list
    voxel_to_inhabitants_[grid_pti.index()].push_back(biob_worldpt_index(i));
  }
}  

biob_worldpt_index_enumerator_sptr geom_index_structure::enclosed_by(geom_probe_volume_sptr volume){
  return new geom_index_structure::enumerator(roster_, grid_, voxel_to_inhabitants_, volume, volume->grid_cells(grid_box_, grid_.spacing()));
}
