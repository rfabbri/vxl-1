// This is bpro/bpro_storage_registry.cxx
#include "bpro_storage_registry.h"


//Declare initial value for static member
vcl_map<vcl_string, bpro_storage_sptr> 
bpro_storage_registry::registered_types_= vcl_map<vcl_string, bpro_storage_sptr>();




//: Constructor
bpro_storage_registry::bpro_storage_registry()
{}


//: Registers a storage type with the storage_registry
bool 
bpro_storage_registry::register_type(const bpro_storage_sptr& dummy_storage)
{
  if(!dummy_storage)
    return false;
  registered_types_[dummy_storage->type()]=dummy_storage;
  vsl_add_to_binary_loader(*dummy_storage);
  return true;
}


//: Returns the set of strings representing all registered types
vcl_set< vcl_string > 
bpro_storage_registry::types()
{
  vcl_set<vcl_string> types;
  for(vcl_map<vcl_string, bpro_storage_sptr>::iterator sit = registered_types_.begin(); sit!= registered_types_.end(); ++sit)
    types.insert((*sit).first);
  return types;
}


//: Remove data from the storage_registry except those with the given names
void bpro_storage_registry::
remove_all_except(const vcl_set<vcl_string>& retain)
{
  vcl_set<vcl_string> types = bpro_storage_registry::types();
  for(vcl_set<vcl_string>::iterator sit = types.begin(); 
      sit != types.end(); ++sit)
    if(retain.count(*sit)>0)
      continue;
    else
      registered_types_.erase(*sit);
}


//: Print a summary of the storage_registry structure to cout
void 
bpro_storage_registry::print_summary()
{
  vcl_set<vcl_string> types = bpro_storage_registry::types();
  vcl_cout << "Registered Types: \n";
  for(vcl_set<vcl_string>::iterator sit = types.begin();
      sit != types.end(); ++sit)
    vcl_cout << *sit << '\n';
}


