// This is mw_cvmatch_storage.h
#ifndef mw_cvmatch_storage_h
#define mw_cvmatch_storage_h
//:
//\file
//\brief Multiview curve match storage
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 05/05/2005 02:57:40 PM EDT
//
#include <bpro1/bpro1_storage.h>
#include <dbcvr/pro/dbcvr_curvematch_storage.h>
#include <mw/mw_cvmatch.h>
#include <mw/pro/mw_cvmatch_storage_sptr.h>

class mw_cvmatch_storage : public dbcvr_curvematch_storage
{
public:

  mw_cvmatch_storage() {}
  virtual ~mw_cvmatch_storage() {}
  virtual vcl_string type() const { return "mw_cvmatch"; }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "mw_cvmatch_storage"; }

  dbcvr_cvmatch_sptr get_curvematch() {
    return curvematch_;
  }
 
  void set_curvematch (dbcvr_cvmatch_sptr new_curvematch) {
    curvematch_ = new_curvematch;
  }
  
protected:

private:

  dbcvr_cvmatch_sptr curvematch_;

};

//: Create a smart-pointer to a vidpro1_shock_storage.
struct mw_cvmatch_storage_new : public mw_cvmatch_storage_sptr
{
  typedef mw_cvmatch_storage_sptr base;

  //: Constructor - creates a default vidpro1_curve_storage_sptr.
  mw_cvmatch_storage_new() : base(new mw_cvmatch_storage()) { }
};

#endif // mw_cvmatch_storage_h

