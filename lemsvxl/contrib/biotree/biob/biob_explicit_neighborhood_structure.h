#if !defined(BIOB_EXPLICIT_NEIGHBORHOOD_STRUCTURE_H_)
#define BIOB_EXPLICIT_NEIGHBORHOOD_STRUCTURE_H_

#include "biob_worldpt_neighborhood_structure.h"
#include "biob_worldpt_roster_sptr.h"
#include <vcl_vector.h>

class biob_explicit_neighborhood_structure : public biob_worldpt_neighborhood_structure {
 public:
  const neighbors_t neighbors(biob_worldpt_index pti) const {
    return neighbors_vector_[pti.index()];
  }
  void populate(biob_worldpt_roster_sptr roster, double resolution);
 private:
  vcl_vector<neighbors_t> neighbors_vector_;
};

#endif
