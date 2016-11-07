// This is brl/vidpro/bpro_mview_dbutils.cxx
#include "bpro_mview_dbutils.h"
#include <brdb/brdb_database_manager.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_relation.h>
#include <bpro/bpro_storage_registry.h>
#include <bpro/bpro_storage.h>

void bpro_mview_dbutils::create_new_database()
{
  vcl_set<vcl_string> types = bpro_storage_registry::types();
  for( vcl_set<vcl_string>::iterator 
         type_itr = types.begin();
       type_itr != types.end();  ++type_itr )
    {      
      vcl_vector<vcl_string> r_names;
      vcl_vector<vcl_string> r_types;
      r_names.push_back("view_id");
      r_names.push_back("name");
      r_names.push_back("sptr");
      r_types.push_back(brdb_value_t<unsigned>::type()); // "unsigned"
      r_types.push_back(brdb_value_t<vcl_string>::type()); // "vcl_string"
      r_types.push_back(brdb_value_t<bpro_storage_sptr>::type()); // "bpro_storage_sptr"
      brdb_relation_sptr r = new brdb_relation(r_names, r_types);
      brdb_database_manager::instance()->add_relation(*type_itr,r);
    }
  {
    vcl_vector<vcl_string> r_names;
    vcl_vector<vcl_string> r_types;
    r_names.push_back("name");
    r_names.push_back("type");
    r_names.push_back("sptr");
    r_types.push_back(brdb_value_t<vcl_string>::type()); //"vcl_string"
    r_types.push_back(brdb_value_t<vcl_string>::type()); // "vcl_string"
    r_types.push_back(brdb_value_t<bpro_storage_sptr>::type()); // "bpro_storage_sptr"
    brdb_relation_sptr r = new brdb_relation(r_names, r_types);
    brdb_database_manager::instance()->add_relation("global_data",r);
  }
  {
    vcl_vector<vcl_string> r_names;
    vcl_vector<vcl_string> r_types;
    r_names.push_back("view_id");
    r_names.push_back("view_name");
    r_types.push_back(brdb_value_t<unsigned>::type()); // "unsigned"
    r_types.push_back(brdb_value_t<vcl_string>::type()); // "vcl_string"
    brdb_relation_sptr r = new brdb_relation(r_names, r_types);
    brdb_database_manager::instance()->add_relation("existing_views",r);
  }
}

bpro_storage_sptr 
bpro_mview_dbutils::get_view_data_by_name_at( const vcl_string& name,
                                               const unsigned view_id)
{
  vcl_set<vcl_string> types = bpro_storage_registry::types();
  for( vcl_set<vcl_string>::iterator type_itr = types.begin(); 
       type_itr != types.end();  ++type_itr )
    {
      brdb_query_aptr Q = brdb_query_comp_new("view_id", brdb_query::EQ, view_id)
                        & brdb_query_comp_new("name",    brdb_query::EQ, name);
      brdb_selection_sptr selec = brdb_database_manager::instance()->select(*type_itr, Q);

      if(!selec->empty())
        {
          if (selec->size() > 1) {
            vcl_cerr << "\nWarning: multiple storage have the same name and frame\n";
            return NULL;
          }
          else {
            bpro_storage_sptr storage;
            selec->get("sptr", 0, storage);
            return storage;
          }
        }
    }
  return NULL; // nothing found
}

bool bpro_mview_dbutils::store_data_at(const bpro_storage_sptr& storage,
                                        const unsigned view_id)
{
  if( !storage.ptr() )
    return false;

  brdb_database_manager::instance()->
    add_tuple(storage->type(), new brdb_tuple(view_id,storage->name(),storage));
  return true;
}

vcl_set < bpro_storage_sptr > bpro_mview_dbutils::get_all_storage_classes()
{
  vcl_set< bpro_storage_sptr > whole_set;
  vcl_set<vcl_string> types = bpro_storage_registry::types();  
  for( vcl_set<vcl_string>::iterator type_itr = types.begin();
       type_itr != types.end();  ++type_itr )
    {
      vcl_set< bpro_storage_sptr > local_type;
      bpro_mview_dbutils::get_all_storage_classes(*type_itr);
      whole_set.insert(local_type.begin(),local_type.end());
    }
  return whole_set;
}


vcl_set < bpro_storage_sptr > 
bpro_mview_dbutils::get_all_storage_classes(const vcl_string& type)
{
  vcl_set < bpro_storage_sptr > storage_set;

  // Local data
  brdb_query_aptr Q = brdb_query_comp_new("view_id", brdb_query::ALL, 0);
  brdb_selection_sptr selec = brdb_database_manager::instance()->select(type, Q);
  bpro_storage_sptr storage;

  if (!selec->empty())  {
    for(unsigned int i=0; i<selec->size(); i++)   {
      selec->get("sptr", i, storage);
      storage_set.insert(storage);
    }
  }

  // Global data relation
  Q = brdb_query_comp_new("type", brdb_query::EQ, type);
  selec = brdb_database_manager::instance()->select("global_data", Q);
  if (!selec->empty())  {
    for(unsigned int i=0; i<selec->size(); i++)  {
      selec->get("sptr", i, storage);
      storage_set.insert(storage);
    }
  }
  return storage_set;
}


//: Returns the set of all storage classes (all types) at the given view id
vcl_set < bpro_storage_sptr > 
bpro_mview_dbutils::get_all_storage_classes(const unsigned view_id)
{
  vcl_set < bpro_storage_sptr > storage_set;
  vcl_set<vcl_string> types = bpro_storage_registry::types();  
  for( vcl_set<vcl_string>::iterator type_itr = types.begin();
       type_itr != types.end();  ++type_itr )
    {
      brdb_query_aptr Q = brdb_query_comp_new("view_id", brdb_query::EQ, view_id);
      brdb_selection_sptr selec = brdb_database_manager::instance()->select(*type_itr, Q);
      if (!selec->empty()) 
        {
          bpro_storage_sptr storage;
          for(unsigned int i=0; i<selec->size(); i++)
            {
              selec->get("sptr", i, storage);
              storage_set.insert(storage);
            }

        }
    }
  return storage_set;
}


bpro_storage_sptr bpro_mview_dbutils::
get_data_by_name_at(const vcl_string& name, const unsigned view_id)
{
  brdb_query_aptr Q = brdb_query_comp_new("name", brdb_query::EQ, name);
  brdb_selection_sptr selec = brdb_database_manager::instance()->select("global_data", Q);

  if(!selec->empty())
    {
      if (selec->size() > 1){
        vcl_cerr << "\nWarning: multiple global storage have the same name\n";
        return NULL;
      }
      else {
        bpro_storage_sptr storage;
        selec->get("sptr", 0, storage);
        return storage;
      }
    }
  
  vcl_set<vcl_string> types = bpro_storage_registry::types();  
  for( vcl_set<vcl_string>::iterator type_itr = types.begin();
       type_itr != types.end();  ++type_itr )
    {
      Q = brdb_query_comp_new( "view_id", brdb_query::EQ, view_id)
        & brdb_query_comp_new( "name",    brdb_query::EQ, name);
      selec = brdb_database_manager::instance()->select(*type_itr,Q);

      if(!selec->empty())
        {
          if (selec->size() > 1) {
            vcl_cerr << "\nWarning: multiple storage have the same name and frame\n";
            return NULL;
          }
          else {
            bpro_storage_sptr storage;
            selec->get("sptr", 0, storage);
            return storage;
          }
        }
    }
  return NULL; // nothing found
}


vcl_vector < vcl_string > 
bpro_mview_dbutils::get_all_storage_class_names(const vcl_string& type,
                                                 const unsigned view_id)
{
  vcl_vector<vcl_string> names;
  brdb_query_aptr Q = brdb_query_comp_new("view_id", brdb_query::EQ, view_id);
  brdb_selection_sptr selec = brdb_database_manager::instance()->select(type, Q);
  if (!selec->empty()) 
    {
      for(unsigned i=0; i<selec->size(); i++)
        {
          vcl_string name;
          selec->get("name", i, name);
          names.push_back(name);
        }
    }
  return names;
}


vcl_vector< vcl_vector < vcl_string > >
bpro_mview_dbutils::get_all_storage_class_names(const vcl_string& type)
{
  vcl_vector <vcl_vector < vcl_string > > result;
  vcl_set<unsigned> vids = bpro_mview_dbutils::existing_view_ids();
  for(vcl_set<unsigned>::iterator vit = vids.begin();
      vit != vids.end(); ++vit)
    {
      vcl_vector < vcl_string > names =
        bpro_mview_dbutils::get_all_storage_class_names(type,*vit);
      result.push_back(names);
    }
  return result;
}

bpro_storage_sptr bpro_mview_dbutils::new_data_at(const vcl_string& type, 
                                                    const vcl_string& name, 
                                                    const unsigned view_id)
{
  if( name == "" )
    return NULL;
  vcl_set<vcl_string> types = bpro_storage_registry::types();  
  vcl_set<vcl_string>::iterator result = types.find(type);
  if( result == types.end() )
    return NULL;
  bpro_storage_sptr temp = bpro_storage_registry::storage(type);
  bpro_storage_sptr blank = temp->clone();
  blank->set_name(name);
  if(!bpro_mview_dbutils::store_data_at(blank, view_id))
    return NULL;
  return blank;
}

//Get the current set of view_ids
vcl_set<unsigned> bpro_mview_dbutils::existing_view_ids()
{
  vcl_set<unsigned> view_ids;
  brdb_query_aptr Q = brdb_query_comp_new("view_id", brdb_query::ALL, 0);
  brdb_selection_sptr selec = brdb_database_manager::instance()->select("existing_views", Q);
  if (!selec->empty()) 
    {
      for(unsigned int i=0; i<selec->size(); i++)
        {
          unsigned vid = 0;
          selec->get("view_id", i, vid);
          view_ids.insert(vid);
        }
    }
  return view_ids;
}


//will invoke name later
bool bpro_mview_dbutils::add_view_id(const unsigned view_id,
                                          vcl_string const& name)
{
  return brdb_database_manager::instance()->
    add_tuple("existing_views", new brdb_tuple(view_id, name));
}


vcl_vector < vcl_string > bpro_mview_dbutils::
get_global_storage_names(const vcl_string& type)
{

  vcl_vector<vcl_string> names;
  brdb_query_aptr Q = brdb_query_comp_new("type", brdb_query::EQ, type);
  brdb_selection_sptr selec = brdb_database_manager::instance()->select("global_data", Q);
  if (!selec->empty()) 
    {
      for(unsigned i=0; i<selec->size(); i++)
        {
          vcl_string name;
          selec->get("name", i, name);
          names.push_back(name);
        }
    }
  return names;
}
