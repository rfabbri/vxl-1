#if !defined(BIOB_WORLDPT_NEIGHBORHOOD_STRUCTURE_H_)
#define BIOB_WORLDPT_NEIGHBORHOOD_STRUCTURE_H_

#include "biob_worldpt_index.h"
#include <vcl_list.h>

class biob_worldpt_neighborhood_structure {
 public:
  virtual ~biob_worldpt_neighborhood_structure(){}
  typedef vcl_list<biob_worldpt_index> neighbors_t;
  virtual const neighbors_t neighbors(biob_worldpt_index pti) const = 0;
};

#endif
