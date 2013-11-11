#if !defined(SPLR_SYMMETRY_WORLDPT_ROSTER_H_)
#define SPLR_SYMMETRY_WORLDPT_ROSTER_H_

#include <biob/biob_worldpt_roster_sptr.h>
#include <splr/splr_symmetry.h>
#include <vcl_iostream.h>


/* Here and in symmetry_splat_collection we adopt a convention concerning the numbering of the points.
   The symmetry has a subroster that stores just the representative points.
   The number of a point p in the symmetry_worldpt_roster is
            x * n + y
   where x is the number of the coset containing p, y is the number
   in the subroster of the representative corresponding to p,
   and n is the number of representatives.
*/


class splr_symmetry_worldpt_roster : public biob_worldpt_roster {
  private:
    splr_symmetry * symmetry_;
    biob_worldpt_roster_sptr representatives_;
  public:
    splr_symmetry_worldpt_roster(splr_symmetry *symmetry, biob_worldpt_roster_sptr representatives) 
      : symmetry_(symmetry), representatives_(representatives) {}
    vcl_string class_id() { return "splr_symmetry_worldpt_roster"; }
    unsigned long num_points() const;
    worldpt point(biob_worldpt_index pti) const;
};



#endif
