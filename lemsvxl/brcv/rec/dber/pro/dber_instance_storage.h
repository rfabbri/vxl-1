// This is brcv/rec/dbru/pro/dber_instance_storage.h
#ifndef dber_instance_storage_h_
#define dber_instance_storage_h_
//:
// \file
// \brief Storage class for arrays of dbrl_multiple_instances

// \author Ozge Can Ozcanli
// \date Nov 08 2006
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_storage.h>

#include <dber/pro/dber_instance_storage_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <dbru/dbru_multiple_instance_object_sptr.h>

#include <vbl/vbl_array_1d.h>

//: Storage class for dbskr_edit_distance
class dber_instance_storage : public bpro1_storage 
{
public:
  //: Constructor
  dber_instance_storage();

  //: Destructor
  virtual ~dber_instance_storage();

  virtual vcl_string type() const { return "dber_instance_storage"; }

  //: Return IO version number;
  short version() const;

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dber_instance_storage"; }

  //: set the instances in each frame
  void set_frames(vcl_vector< vcl_vector<dbru_multiple_instance_object_sptr> >& frames) { frames_ = frames; }

  //: get the pointer to frame array
  vcl_vector< vcl_vector<dbru_multiple_instance_object_sptr> >& get_frames(void) { return frames_; }

  //: get the number of frames stored in OSL
  unsigned int size(void) { return frames_.size(); }

  // for visualization
  unsigned get_nrows() { return n_rows_; }
  unsigned get_ncols() { return n_cols_; }
  void set_nrows(unsigned n) { n_rows_ = n; }
  void set_ncols(unsigned n) { n_cols_ = n; }

private:

  vcl_vector< vcl_vector<dbru_multiple_instance_object_sptr> > frames_;
  unsigned n_rows_, n_cols_;  // for visualization of the OSL
};

#include <dber/pro/dber_instance_storage_sptr.h>

//: Create a smart-pointer to a dber_instance_storage.
struct dber_instance_storage_new : public dber_instance_storage_sptr
{
  typedef dber_instance_storage_sptr base;

  //: Constructor - creates a default dber_instance_storage_sptr.
  dber_instance_storage_new() : base(new dber_instance_storage()) { }
};

#endif //dber_instance_storage_h_

