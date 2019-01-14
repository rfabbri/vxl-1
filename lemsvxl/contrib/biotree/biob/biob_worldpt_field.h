#if !defined(BIOB_WORLDPT_FIELD_H_)
#define BIOB_WORLDPT_FIELD_H_

#include <vector>
#include <vbl/vbl_ref_count.h>
#include "biob_worldpt_roster.h"
#include "biob_worldpt_roster_sptr.h"

template <class T>
class biob_worldpt_field: public vbl_ref_count {
 public:
  typedef std::vector<T> values_t;
 private:
  biob_worldpt_roster_sptr roster_;
  std::string roster_type_;
  values_t values_;
 public:
  typedef T element_t;
  biob_worldpt_roster_sptr roster() const {return roster_;}
  std::string roster_type() { return roster_type_; }
  std::vector<T> & values() {return values_;}
  const std::vector<T> & const_values() const {return values_;}
  biob_worldpt_field(biob_worldpt_roster_sptr roster)
    : roster_(roster), roster_type_(roster->class_id()), values_(roster_->num_points()) {}
    biob_worldpt_field() {}
    void set_roster(biob_worldpt_roster_sptr roster){
      roster_ = roster;
      roster_type_ = roster_->class_id();
      values_.resize(roster_->num_points());
    }
  void set_values(std::vector<T> values) { values_= values; }
};

template<class T>
void x_write(std::ostream& os, biob_worldpt_field<T> f);


#endif
