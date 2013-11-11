// This is brl/vidpro/bpro_storage_registry.h
#ifndef bpro_storage_registry_h_
#define bpro_storage_registry_h_

//:
// \file
// \brief Adapted from vidpro/repository to be free of the repository

// \author J.L. Mundy 
// \date April 20, 2007
//
// \verbatim
//  Modifications
//  
// \endverbatim

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_fwd.h>
#include <bpro/bpro_storage_sptr.h>
#include <bpro/bpro_storage.h>

class bpro_storage_registry : public vbl_ref_count {

public:

  //: Constructor
  bpro_storage_registry();
  //: Destructor
  ~bpro_storage_registry(){}

  //: Registers a storage type with the storage_registry
  //  Call this function with a dummy instance of each type you plan to use
  //  Returns false if this type is already registered
  static bool register_type(const bpro_storage_sptr& dummy_storage);

  //: Returns the set of strings representing all registered types
  static vcl_set< vcl_string > types();

  //: Clear all data from the storage_registry
  static void remove_all(){registered_types_.clear();}

  //: Remove data from the storage_registry except those with the given names
  static void remove_all_except(const vcl_set<vcl_string>& retain);

  //: Print a summary of the storage_registry structure to cout
  static void print_summary();

  //: Return the dummy storage class
  static bpro_storage_sptr storage(vcl_string const& type)
    {return registered_types_[type];}

protected:

  //: A vector of registered types
  static vcl_map<vcl_string, bpro_storage_sptr> registered_types_;

};



#endif // bpro_storage_registry_h_
