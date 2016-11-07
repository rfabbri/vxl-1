// This is brcv/rec/dbru/pro/dbru_rcor_storage.h
#ifndef dbru_rcor_storage_h_
#define dbru_rcor_storage_h_
//:
// \file
// \brief Storage class for region correspondence
// \author Amir Tamrakar
// \date Nov 29 2005
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include <dbru/dbru_rcor_sptr.h>

#include <dbru/pro/dbru_rcor_storage_sptr.h>

//: Storage class for dbskr_edit_distance
class dbru_rcor_storage : public bpro1_storage 
{
public:
  //: Constructor
  dbru_rcor_storage();

  //: Destructor
  virtual ~dbru_rcor_storage();

  virtual vcl_string type() const { return "region_cor"; }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbru_rcor_storage"; }

  //: set the region correspondence
  void set_rcor(dbru_rcor_sptr rcor) { rcor_ = rcor; }

  //: get the region correspondence
  dbru_rcor_sptr get_rcor(void) { return rcor_; }

private:

  dbru_rcor_sptr rcor_;
};

//: Create a smart-pointer to a dbru_rcor_storage.
struct dbru_rcor_storage_new : public dbru_rcor_storage_sptr
{
  typedef dbru_rcor_storage_sptr base;

  //: Constructor - creates a default dbru_rcor_storage_sptr.
  dbru_rcor_storage_new() : base(new dbru_rcor_storage()) { }
};

#endif //dbru_rcor_storage_h_
