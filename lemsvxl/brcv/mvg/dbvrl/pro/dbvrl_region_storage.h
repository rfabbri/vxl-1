// This is brcv/mvg/dbvrl/pro/dbvrl_region_storage.h
#ifndef dbvrl_region_storage_h_
#define dbvrl_region_storage_h_

//:
// \file
// \brief The vidpro1 storage class for dbvrl.
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 4/20/04
//
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbvrl_region_storage_sptr.h"
#include <bpro1/bpro1_storage.h>
#include <dbvrl/dbvrl_region_sptr.h>
#include <dbvrl/dbvrl_transform_2d_sptr.h>

//: Storage class for vimt_tranform_2d
class dbvrl_region_storage : public bpro1_storage {
public:

  //: Constructor
  dbvrl_region_storage(){} 
  //: Constructor
  dbvrl_region_storage( const dbvrl_region_sptr& region,
                        const dbvrl_transform_2d_sptr& xform );

  //: Destructor
  virtual ~dbvrl_region_storage(){}

  //: Returns the type string "bvrl"
  virtual vcl_string type() const { return "bvrl"; }

  //: Return IO version number;
  short version() const;
  
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbvrl_region_storage"; }

  //: Set the region 
  void set_region( const dbvrl_region_sptr& region );

  //: Return the region
  dbvrl_region_sptr region() const;

  //: Set the transform 
  void set_transform( const dbvrl_transform_2d_sptr& xform );

  //: Return the transform
  dbvrl_transform_2d_sptr transform() const;

  
protected:

  //: The region
  dbvrl_region_sptr region_;

  //: The transform
  dbvrl_transform_2d_sptr xform_;

};


//: Create a smart-pointer to a dbvrl_region_storage.
struct dbvrl_region_storage_new : public dbvrl_region_storage_sptr
{
  typedef dbvrl_region_storage_sptr base;

  //: Constructor - creates a dbvrl_region_storage_sptr.
  dbvrl_region_storage_new() 
    : base(new dbvrl_region_storage()) { }

  //: Constructor - creates a dbvrl_region_storage_sptr with transform.
  dbvrl_region_storage_new( const dbvrl_region_sptr& region, 
                            const dbvrl_transform_2d_sptr& xform )
    : base(new dbvrl_region_storage( region, xform )) { }
};


#endif //dbvrl_region_storage_h_
