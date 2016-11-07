#include <splr/splr_symmetry_worldpt_roster.h>
#include <splr/splr_pizza_slice_symmetry.h>
#include <biob/biob_grid_worldpt_roster.h>

#include <vsl/vsl_basic_xml_element.h>
#include <vcl_string.h>

unsigned long splr_symmetry_worldpt_roster::num_points() const {
 return representatives_->num_points() * symmetry_->size();
}

worldpt splr_symmetry_worldpt_roster::point(biob_worldpt_index pti) const{
  //find the coset containing that element.
  unsigned int coset = pti.index() / representatives_->num_points();
  //find the representative within that coset
  biob_worldpt_index representative_pti(pti.index() % representatives_->num_points());
  //should fix this later---need to handle case where pizza slice symmetry applies to a single uniform orbit
  //get transformation taking specified point to its representative
  splr_symmetry_struct result = symmetry_->apply(0, coset);
  //get the actual representative point from the roster of representatives
  worldpt pt = representatives_->point(representative_pti);
  //apply the transformation and return the transformed point
  return result.from_representative_(pt);
}
