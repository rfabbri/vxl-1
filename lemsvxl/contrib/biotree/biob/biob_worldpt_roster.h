#if !defined(BIOB_WORLDPT_ROSTER_H_)
#define BIOB_WORLDPT_ROSTER_H_

#include <biob/biob_worldpt_index.h>
#include <bio_defs.h>
#include <vbl/vbl_ref_count.h>

/*: \brief interface for a structure that holds an indexed set of worldpts
    \author P. N. Klein

    Represents an assignment of consecutive nonnegative integers to worldpts.
    Allows us to refer in other data structures to worldpts using a worldpt_index,
    which is a wrapped integer
*/

class biob_worldpt_roster : public vbl_ref_count{
  public:
  ~biob_worldpt_roster();
   virtual vcl_string class_id() { return "biob_worldpt_roster"; }
   //: How many points in the roster? (points indexed from 0 to num_points - 1)
    virtual unsigned long int num_points() const = 0;
   //:retrieve the point, given its index
    virtual worldpt point(biob_worldpt_index pti) const = 0;
    virtual void x_write_this(vcl_ostream& os);
};
#endif
