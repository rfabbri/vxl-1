#if !defined(BIOB_SUBSET_WORLDPT_ROSTER_H_)
#define BIOB_SUBSET_WORLDPT_ROSTER_H_

#include "biob_worldpt_roster_sptr.h"
#include "biob_worldpt_index.h"
#include <vcl_vector.h>

class biob_subset_worldpt_roster : public biob_worldpt_roster {
 public:
  ~biob_subset_worldpt_roster();
  typedef vcl_vector<biob_worldpt_index> which_points_t;
  typedef const which_points_t const_which_points_t;
  biob_subset_worldpt_roster(const biob_worldpt_roster_sptr subroster_ptr)
    : subroster_ptr_(subroster_ptr) {}
    vcl_vector<biob_worldpt_index> & which_points() {
      return which_points_;
    }
    const vcl_vector<biob_worldpt_index> & const_which_points() const{
      return which_points_;
    }
    unsigned long int num_points() const {return which_points_.size();}
    worldpt point(biob_worldpt_index pti) const {
      return subroster_ptr_->point(which_points_[pti.index()]);
    }
 private:
    const biob_worldpt_roster_sptr subroster_ptr_;
    which_points_t which_points_;
};

#endif

