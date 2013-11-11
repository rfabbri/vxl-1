// This is brl/vidpro/storage/vidpro_vtol_storage.h
#ifndef vidpro_vtol_storage_h_
#define vidpro_vtol_storage_h_

//:
// \file
// \brief The vidpro storage class for vtol objects
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/30/04
//
//
// \verbatim
//  Modifications
// \endverbatim



#include <bpro/bpro_storage.h>
#include <vidpro/storage/vidpro_vtol_storage_sptr.h>
#include <vtol/vtol_topology_object_sptr.h>
#include <vcl_set.h>


class vidpro_vtol_storage : public bpro_storage {

public:

  vidpro_vtol_storage();
  virtual ~vidpro_vtol_storage();
  virtual vcl_string type() const { return "vtol"; }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro_storage* clone() const;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "vidpro_vtol_storage"; }

  //: Clear all vtol topology objects
  void clear();
  //: Add a vtol topology object to the storage class
  void add_vtol( const vtol_topology_object_sptr& top_obj );
  //: Remove a vtol topology object from the storage class if it exists
  // \return false if the object was not in the storage class
  bool remove_vtol( const vtol_topology_object_sptr& top_obj );
  //: The number of topology objects stored
  int size() const;

  //: Return a const iterator to the beginning of the data
  vcl_set<vtol_topology_object_sptr>::const_iterator begin() const;
  //: Return a const iterator to the end of the data
  vcl_set<vtol_topology_object_sptr>::const_iterator end() const;
  
protected:

private:

  vcl_set<vtol_topology_object_sptr> data_set_;
  
};

//: Create a smart-pointer to a vidpro_vtol_storage.
struct vidpro_vtol_storage_new : public vidpro_vtol_storage_sptr
{
  typedef vidpro_vtol_storage_sptr base;

  //: Constructor - creates a default vidpro_vtol_storage_sptr.
  vidpro_vtol_storage_new() : base(new vidpro_vtol_storage()) { }
};

#endif // vidpro_vtol_storage_h_
