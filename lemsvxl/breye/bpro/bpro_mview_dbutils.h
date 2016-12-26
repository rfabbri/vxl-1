// This is brl/vidpro/bpro_mview_dbutils.h
#ifndef bpro_mview_dbutils_h_
#define bpro_mview_dbutils_h_

//:
// \file
// \brief Database utility functions to support multi-view processing

// \author J.L. Mundy 
// \date April 21, 2007
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

class bpro_mview_dbutils 
{
 public:
  //: Erase all old data and create a new database with empty relations
  static void create_new_database();
  
  static bpro_storage_sptr new_data_at(const vcl_string& type, 
                                        const vcl_string& name, 
                                        const unsigned view_id);

  //: Store view data at the specified view id
  static bool store_data_at(const bpro_storage_sptr& storage,
                            const unsigned view_id);
  //: get all storage classes in the database
  static vcl_set < bpro_storage_sptr > get_all_storage_classes();
  //: get all storage classes in the database with the given type
  static vcl_set < bpro_storage_sptr > get_all_storage_classes(const vcl_string& type);
  //: Returns the set of all storage classes (all types) at the given view
  static vcl_set < bpro_storage_sptr > get_all_storage_classes(unsigned view_id);
  //: Get all data by name (camera view and world)
  static bpro_storage_sptr get_data_by_name_at(const vcl_string& name,
                                                const unsigned view_id);

  //: Get a storage class at the specified view id by name
  static bpro_storage_sptr get_view_data_by_name_at( const vcl_string& name,
                                                      const unsigned view_id);
  //:All means all view storage names for a type
  static vcl_vector< vcl_vector < vcl_string > > get_all_storage_class_names(const vcl_string& type);
  //:All means all of a type at the given view id 
  static vcl_vector < vcl_string > get_all_storage_class_names(const vcl_string& type, const unsigned view_id);

  //: Names by type in the global storage
  static vcl_vector < vcl_string > get_global_storage_names(const vcl_string& type);
  //: Add a new camera_view to the database to assign storage later
  static bool add_view_id(const unsigned view_id,
                          vcl_string const& name="No Name");

  //: Get the existing view_ids 
  static  vcl_set<unsigned> existing_view_ids();
  
};
#endif //bpro_mview_dbutils
