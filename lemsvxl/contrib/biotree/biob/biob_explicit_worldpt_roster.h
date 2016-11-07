#if !defined(BIOB_EXPLICIT_WORLDPT_ROSTER_H_)
#define BIOB_EXPLICIT_WORLDPT_ROSTER_H_

#include <biob/biob_worldpt_roster.h>
#include <vcl_vector.h>
#include <vsl/vsl_vector_io.h>

//So far, keeping an io member in this class

class biob_explicit_worldpt_roster : public biob_worldpt_roster {

 public: //should be private:
    typedef vcl_vector<worldpt> points_container;
    points_container points_;
  public:
    biob_worldpt_index add_point(worldpt pt);
    worldpt point(biob_worldpt_index pti) const;
    unsigned long int num_points() const;
    vcl_vector<worldpt> points() { return points_;}
    void insert(const biob_worldpt_roster * other){
       for (unsigned long int i = 0; i < other->num_points(); ++i){
         add_point(other->point(biob_worldpt_index(i)));
       }
    }
    int operator==(biob_explicit_worldpt_roster & other){
      return points_ == other.points_;
    }
    void x_write_this(vcl_ostream &os);
};



#endif
