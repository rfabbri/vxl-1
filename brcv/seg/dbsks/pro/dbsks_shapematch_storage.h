// This is shp/dbsks/pro/dbsks_shapematch_storage.h
#ifndef dbsks_shapematch_storage_h_
#define dbsks_shapematch_storage_h_

//:
// \file
// \brief Storage class to hold costs of arcs defined on an image
// \author Nhon Trinh, (ntrinh@lems.brown.edu)
// \date Nov 20, 2007
//
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbsks_shapematch_storage_sptr.h"
#include <bpro1/bpro1_storage.h>
#include <dbsks/dbsks_dp_match_sptr.h>
#include <dbsks/dbsks_local_match_sptr.h>


//: Storage class for image_costs
class dbsks_shapematch_storage : public bpro1_storage {
public:
  // ------------ Initialization  ------------
  //: Constructor
  dbsks_shapematch_storage();
  
  //: Destructor
  virtual ~dbsks_shapematch_storage(){}

  // ------------ Data access ------------------
  dbsks_dp_match_sptr dp_engine() const
  { return this->dp_engine_; }

  void set_dp_engine(const dbsks_dp_match_sptr& dp_engine)
  { this->dp_engine_ = dp_engine; }

  dbsks_local_match_sptr lm_engine() const
  { return this->lm_engine_; }

  void set_lm_engine(const dbsks_local_match_sptr& lm_engine)
  { this->lm_engine_ = lm_engine; }

  // ------  Binary IO ----------------------
  //: Returns the type string
  virtual vcl_string type() const { return "dbsks_shapematch"; }

  //: Return IO version number;
  short version() const;
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;
  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  // ---------- bpro1 ---------------------
  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbsks_shapematch_storage"; }

protected:
  dbsks_dp_match_sptr dp_engine_;
  dbsks_local_match_sptr lm_engine_;
};


//: Create a smart-pointer to a dbsks_shapematch_storage.
struct dbsks_shapematch_storage_new : public dbsks_shapematch_storage_sptr
{
  typedef dbsks_shapematch_storage_sptr base;

  //: Constructor - creates a dbsks_shapematch_storage_sptr.
  dbsks_shapematch_storage_new() 
    : base(new dbsks_shapematch_storage()) { }
};


#endif //dbsks_shapematch_storage_h_
