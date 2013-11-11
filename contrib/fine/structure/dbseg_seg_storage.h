// This is contrib/fine/dbseg_seg_storage.h
#ifndef dbseg_seg_storage_h_
#define dbseg_seg_storage_h_


//:
// \file
// \brief The dbseg storage class for dbseg_seg_object
// \author Eli Fine (eli_fine@brown.edu)
// \date 8/1/08
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include "dbseg_seg_storage_sptr.h"
#include <vsol/vsol_spatial_object_2d_sptr.h>

//#include "dbseg_seg_object.h"
#include <structure/dbseg_seg_object_base.h>

#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_string.h>

class dbseg_seg_storage : public bpro1_storage {

public:
  typedef vcl_map< vcl_string, dbseg_seg_object_base> data_map;
  //typedef vcl_map<vcl_string, vcl_vector<double> > attribute_map;

  //: Constructor
  dbseg_seg_storage();

  dbseg_seg_storage(dbseg_seg_object_base* object);

  //: Destructor
  virtual ~dbseg_seg_storage();

  //: Return the type identifier string
  virtual vcl_string type() const { return "seg"; }
 
  /*
  //: Register vsol_spatial_object_2d types for I/O
  virtual void register_binary_io() const;
    */

  //: Return IO version number;
  short version() const;
  
  /*
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);
  */

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbseg_seg_storage"; }


  //: Add an object to the group named \p which
  void add_object(dbseg_seg_object_base* object);

    dbseg_seg_object_base* get_object();

protected:

private:
 
  dbseg_seg_object_base* object_; 

  
  //: Is the binary I/O registered
  static bool registered_; 
};



//: Create a smart-pointer to a dbseg_seg_storage.
struct dbseg_seg_storage_new : public dbseg_seg_storage_sptr
{
  typedef dbseg_seg_storage_sptr base;

  //: Constructor - creates a default dbseg_seg_storage_sptr.
  dbseg_seg_storage_new() : base(new dbseg_seg_storage()) { }
};


#endif //dbseg_seg_storage_h_


