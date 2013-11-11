// This is brcv/trk/dbctrk/dbctrk_storage.h
#ifndef dbctrk_storage_h_
#define dbctrk_storage_h_

//:
// \file
// \brief The vidpro1 storage class for ctrk ctrks.
// \author Vishal Jain, (vj@lems.brown.edu)
// \date Sat Nov 16 2003
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include <dbctrk/pro/dbctrk_storage_sptr.h>
#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <dbctrk/dbctrk_tracker_curve.h>
#include <vcl_vector.h>

//: Storage class for ctrk 
class dbctrk_storage : public bpro1_storage {
public:

  //: Constructor
  dbctrk_storage(){}
  //: Constructor
  dbctrk_storage( const vcl_vector<dbctrk_tracker_curve_sptr>& tracked_curves);
  //: Destructor
  virtual ~dbctrk_storage(){}

  //: Returns the type string "ctrk"
  virtual vcl_string type() const { return "dbctrk"; }

  //: Set the ctrk storage vector
  void set_tracked_curves(const vcl_vector<dbctrk_tracker_curve_sptr>& tracked_curves);

  //: Retrieve the ctrk storage vector
  void get_tracked_curves(vcl_vector<dbctrk_tracker_curve_sptr>& tracked_curves) const;

  virtual vcl_string is_a() const {return "dbctrk_storage";}

  virtual bpro1_storage* clone() const;

  //: Return IO version number;
  short version() const;
  
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);
 private:
  //: The vector of  ctrk smart pointers
  vcl_vector< dbctrk_tracker_curve_sptr > tracked_curves_;  
};


//: Create a smart-pointer to a dbctrk_storage.
struct dbctrk_storage_new : public dbctrk_storage_sptr
{
  typedef dbctrk_storage_sptr base;

  //: Constructor - creates a default dbctrk_storage_sptr.
  dbctrk_storage_new() : base(new dbctrk_storage()) { }

  //: Constructor - creates a dbctrk_storage_sptr with ctrks.
  dbctrk_storage_new(const vcl_vector<dbctrk_tracker_curve_sptr>& tracked_curves)
   : base(new dbctrk_storage(tracked_curves )) { }
};


#endif //dbctrk_storage_h_
