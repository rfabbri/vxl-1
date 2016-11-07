// This is brcv/trk/dbmrf/pro/dbmrf_bmrf_storage.h
#ifndef dbmrf_bmrf_storage_h_
#define dbmrf_bmrf_storage_h_

//:
// \file
// \brief The vidpro1 storage class for bmrf network data.
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 1/23/04
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include "dbmrf_bmrf_storage_sptr.h"
#include <bmrf/bmrf_network_sptr.h>

//: Storage class for bmrf network data
class dbmrf_bmrf_storage : public bpro1_storage {
public:

  //: Constructor
  dbmrf_bmrf_storage(){}
  //: Constructor
  dbmrf_bmrf_storage( const bmrf_network_sptr& network );
  //: Destructor
  virtual ~dbmrf_bmrf_storage(){}

  //: Returns the type string "bmrf"
  virtual vcl_string type() const { return "bmrf"; }
  
  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbmrf_bmrf_storage"; }
    
  //: Return IO version number;
  short version() const;
  
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return the network
  bmrf_network_sptr network() const;

private:
  //: A smart pointer to the network
  bmrf_network_sptr network_;

};


//: Create a smart-pointer to a dbmrf_bmrf_storage.
struct dbmrf_bmrf_storage_new : public dbmrf_bmrf_storage_sptr
{
  typedef dbmrf_bmrf_storage_sptr base;

  //: Constructor - creates a default dbmrf_bmrf_storage_sptr.
  dbmrf_bmrf_storage_new() : base(new dbmrf_bmrf_storage()) { }

  //: Constructor - creates a dbmrf_bmrf_storage_sptr with the network and frame number.
  dbmrf_bmrf_storage_new(const bmrf_network_sptr& network)
   : base(new dbmrf_bmrf_storage( network )) { }
};

#endif // dbmrf_bmrf_storage_h_
