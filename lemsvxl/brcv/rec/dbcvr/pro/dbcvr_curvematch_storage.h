/**************************************************************/
/*  Name: MingChing (mcchang)
//  File: bvis1_curvematch_storage.h
//  Asgn: bvis
//  Date: Thu Aug 28 14:28:55 EDT 2003
***************************************************************/

#ifndef dbcvr_curvematch_storage_header
#define dbcvr_curvematch_storage_header

#include <bpro1/bpro1_storage.h>
#include <dbcvr/dbcvr_cvmatch.h>
#include <dbcvr/dbcvr_cvmatch_sptr.h>
#include "dbcvr_curvematch_storage_sptr.h"
#include <dbcvr/dbcvr_cv_cor_sptr.h>

class dbcvr_curvematch_storage : public bpro1_storage 
{
public:

  dbcvr_curvematch_storage() {}
  virtual ~dbcvr_curvematch_storage() {}
  virtual vcl_string type() const { return "curvematch"; }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "vidpro1_curvematch_storage"; }

  dbcvr_cvmatch_sptr get_curvematch() {
    return curvematch_;
  }
 
  void set_curvematch (dbcvr_cvmatch_sptr new_curvematch) {
    curvematch_ = new_curvematch;
  }

  dbcvr_cv_cor_sptr get_curve_cor() {
    return cv_cor_;
  }
 
  void set_cv_cor (dbcvr_cv_cor_sptr new_curve_cor) {
    cv_cor_ = new_curve_cor;
  }

private:

  dbcvr_cvmatch_sptr curvematch_;
  dbcvr_cv_cor_sptr cv_cor_;

};

//: Create a smart-pointer to a vidpro1_shock_storage.
struct dbcvr_curvematch_storage_new : public dbcvr_curvematch_storage_sptr
{
  typedef dbcvr_curvematch_storage_sptr base;

  //: Constructor - creates a default vidpro1_curve_storage_sptr.
  dbcvr_curvematch_storage_new() : base(new dbcvr_curvematch_storage()) { }
};

#endif
