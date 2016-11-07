// This is brl/vidpro/vidpro_repository.cxx
#include <vidpro/vidpro_repository.h>
#include <bpro/bpro_storage.h>
#include <bpro/bpro_process.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_cassert.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_map_io.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_set_io.h>

#include <brdb/brdb_relation.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_database_manager.h>

//:
// \file


//: Constructor
vidpro_repository::vidpro_repository()
: bpro_storage_registry(), current_frame_(0)
{}

//: Return the current frame
int 
vidpro_repository::current_frame() const
{ 
  return current_frame_;
}

//: Clear all data from the repository
void
vidpro_repository::remove_all()
{
  DATABASE->clear();
  current_frame_ = 0;
}


//: Remove all data from the repository except those with the given names
void 
vidpro_repository::remove_all_except(const vcl_set<vcl_string>& retain)
{

  // Local data
  for( vcl_map<vcl_string, bpro_storage_sptr>::const_iterator 
    type_itr = registered_types_.begin();
    type_itr != registered_types_.end();  ++type_itr )
  {
    brdb_query_aptr Q(NULL);
    for (vcl_set<vcl_string>::const_iterator keep_it = retain.begin();
      keep_it != retain.end();keep_it++)
    {
      Q = Q | brdb_query_comp_new("name", brdb_query::NEQ, *keep_it);
    }
    DATABASE->select(type_itr->first, Q)->delete_tuples();
  }

  // Global data
  brdb_selection_sptr selec, total;
  for (vcl_set<vcl_string>::const_iterator keep_it = retain.begin();
    keep_it != retain.end();keep_it++)
  {
    // Not-Equal operator not defined for vcl_string: using brdb_query::EQ and NOT the selection
    brdb_query_aptr Q = brdb_query_comp_new("name", brdb_query::EQ, *keep_it);
    selec = DATABASE->select("global_data", Q);
    if (keep_it == retain.begin())
      total = selec;
    else
      total = total | selec;
  }
  total = total->selection_not();
  total->delete_tuples();
}



//: Clear the repository and replace with a new one
// \note the registered types in new_rep must be a subset of types registered here
bool 
vidpro_repository::replace_data(const vcl_string& path)
{
  this->remove_all();
  return brdb_database_manager::load_database(path);
}

bool
vidpro_repository::remove_frame(int frame)
{
  for( vcl_map<vcl_string, bpro_storage_sptr>::const_iterator 
    type_itr = registered_types_.begin();
    type_itr != registered_types_.end();  ++type_itr )
  {
    brdb_query_aptr Q = brdb_query_comp_new("frame", brdb_query::EQ, frame);
    brdb_selection_sptr selec = DATABASE->select(type_itr->first, Q);


    if(selec->size()>0)
    {
      selec->delete_tuples();
    }

  }
  return true;

}

//refresh the frame numbers in the database
bool
vidpro_repository::decrement_frame_number()
{
  for( vcl_map<vcl_string, bpro_storage_sptr>::const_iterator 
    type_itr = registered_types_.begin();
    type_itr != registered_types_.end();  ++type_itr )
  {

    for (int i=0; i< this->num_frames(); i++)
    {
      brdb_query_aptr Q = brdb_query_comp_new("frame", brdb_query::EQ, i+1);
      brdb_selection_sptr s_all = DATABASE->select(type_itr->first, Q);
      vcl_vector<vcl_string> names = this->get_all_storage_class_names((type_itr->first), i+1);

      for(vcl_vector<vcl_string>::iterator it = names.begin(); it!=names.end(); it++)
      {
        Q = brdb_query_comp_new("name", brdb_query::EQ, *it);
        brdb_selection_sptr s = new brdb_selection(s_all, Q);
        s->update_selected_tuple_value("frame", i);
      }

    }
  }
  return true;
}

//refresh the frame numbers in the database
bool
vidpro_repository::increment_frame_number()
{
  for( vcl_map<vcl_string, bpro_storage_sptr>::const_iterator 
    type_itr = registered_types_.begin();
    type_itr != registered_types_.end();  ++type_itr )
  {

    for (int i=this->num_frames()-1; i>=0 ; i--)
    {
      brdb_query_aptr Q=brdb_query_comp_new("frame", brdb_query::EQ, i);
      brdb_selection_sptr s_all = DATABASE->select(type_itr->first, Q);
      vcl_vector<vcl_string> names = this->get_all_storage_class_names((type_itr->first), i);

      for(vcl_vector<vcl_string>::iterator it = names.begin(); it!=names.end(); it++)
      {
        Q = brdb_query_comp_new("name", brdb_query::EQ, *it);
        brdb_selection_sptr s = new brdb_selection(s_all, Q);
        s->update_selected_tuple_value("frame", i+1);
      }

    }
  }
  return true;
}


//: Write repository to a file
void 
vidpro_repository::save_data(const vcl_string& path)
{
  brdb_database_manager::save_database(path);
}

//: Add in a new repository to the existing storage elements
bool vidpro_repository::add_repository(const vcl_string& path)
{
  return brdb_database_manager::merge_database(path);
}

void
vidpro_repository::initialize( int num_frames )
{
  this->remove_all();

  // create a template map of NULL smart pointers
  //storage_map <==> vcl_map< vcl_string, vcl_vector< bpro_storage_sptr > >
  vidpro_repository::storage_map temp_map;

  for( vcl_map<vcl_string, bpro_storage_sptr>::const_iterator 
    type_itr = registered_types_.begin();
    type_itr != registered_types_.end();  ++type_itr )
  {      
    vcl_vector<vcl_string> r_names;
    vcl_vector<vcl_string> r_types;
    r_names.push_back("frame");
    r_names.push_back("name");
    r_names.push_back("sptr");
    r_types.push_back(brdb_value_t<int>::type()); // "int"
    r_types.push_back(brdb_value_t<vcl_string>::type()); // "vcl_string"
    r_types.push_back(brdb_value_t<bpro_storage_sptr>::type()); // "bpro_storage_sptr"
    brdb_relation_sptr r = new brdb_relation(r_names, r_types);
    DATABASE->add_relation(type_itr->first,r);
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
    DATABASE->add_relation("global_data",r);
  }

  {
    vcl_vector<vcl_string> r_names;
    vcl_vector<vcl_string> r_types;
    r_names.push_back("process");
    r_names.push_back("ostream");
    r_names.push_back("type");
    r_types.push_back(brdb_value_t<vcl_string>::type()); 
    r_types.push_back(brdb_value_t<vcl_string>::type()); 
    r_types.push_back(brdb_value_t<vcl_string>::type());
    brdb_relation_sptr r = new brdb_relation(r_names, r_types);
    DATABASE->add_relation("ostream-process",r);
  }

}

// obsolete: but part of the fa�ade
void vidpro_repository::initialize_global()
{}

void
vidpro_repository::add_new_frame()
{
  //FIXME: it is hard to adapt this function to the database
  //since one cannot add undefined values to the tuples
  //whereas if one try to add clones of dummy empty storages
  //they will all have the same defeult name
  //In the old repository,there was no problem
  //in adding empty  vector<bpro_storage_sptr>

  // NEW: need some work
  //vcl_map<vcl_string, bpro_storage_sptr>::iterator 
  //  result = registered_types_.begin();
  //for(;result != registered_types_.end();  ++result){

  //  bpro_storage_sptr blank = result->second->clone();
  //  brdb_tuple_sptr new_tuple = 
  //    new brdb_tuple(this->num_frames(),blank->name(),blank);
  //  //stores at a new last frame
  //  DATABASE->add_tuple(blank->type(),new_tuple);
  //}

  // OLD:
  //vidpro_repository::storage_map temp_map;
  //for( vcl_map<vcl_string, bpro_storage_sptr>::const_iterator 
  //     type_itr = registered_types_.begin();
  //     type_itr != registered_types_.end();  ++type_itr ){
  //  temp_map[type_itr->first] = vcl_vector< bpro_storage_sptr >();
  //}
  //data_.push_back( temp_map );

  current_frame_ += 1; 
}


//: Increment the current frame number if not at the end
bool
vidpro_repository::go_to_next_frame()
{
  ////Check if the frame is already in the database
  if( valid_frame(current_frame_ + 1) ) {
    current_frame_++;
    return true;
  }
  return false;
}

bool
vidpro_repository::go_to_next_n_frame(int n)
{
  if( valid_frame(current_frame_ + n) ) {
    current_frame_+=n;
    return true;
  }
  return false;
}

//: Decrement the current frame number if not at the beginning
bool
vidpro_repository::go_to_prev_frame()
{
  if( current_frame_ > 0 ) {
    current_frame_--;
    return true;
  }
  return false;
}
bool
vidpro_repository::go_to_prev_n_frame(int n)
{
  if( current_frame_ - n >=0 ) {
    current_frame_-=n;
    return true;
  }
  return false;
}

//: Jump the the specified frame number if valid
bool
vidpro_repository::go_to_frame( int num )
{
  if( valid_frame( num ) ){
    current_frame_ = num;
    return true;
  }
  return false;
}


//: Returns true if the specified frame is valid
bool
vidpro_repository::valid_frame( int frame ) const
{ 

  if(frame < 0){
    return false;
  }

  for( vcl_map<vcl_string, bpro_storage_sptr>::iterator 
    type_itr = registered_types_.begin();
    type_itr != registered_types_.end();  ++type_itr )
  {
    brdb_query_aptr Q = brdb_query_comp_new("frame", brdb_query::EQ, frame);
    brdb_selection_sptr selec = DATABASE->select(type_itr->first, Q);
    if (!selec->empty()) return true;
  }
  return false;
}



//: Retrieve a vector of names that describe the storage classes of a given type
vcl_vector < vcl_string > 
vidpro_repository::get_all_storage_class_names(const vcl_string& type)
{
  return get_all_storage_class_names(type, current_frame_);
}

//: Retrieve a storage smart pointer to the global data named \p name
bpro_storage_sptr
vidpro_repository::get_global_storage_by_name(const vcl_string& name)
{
  brdb_query_aptr Q = brdb_query_comp_new("name", brdb_query::EQ, name);
  brdb_selection_sptr selec = DATABASE->select("global_data", Q);
  if(!(selec->size()==1))
    return NULL;
  bpro_storage_sptr sto;
  selec->get("sptr",sto);
  return sto;
}

//: Remove a global storage named \p name
bool
vidpro_repository::remove_global_storage_by_name(const vcl_string& name)
{
  brdb_query_aptr Q = brdb_query_comp_new("name", brdb_query::EQ, name);
  brdb_selection_sptr selec = DATABASE->select("global_data", Q);
  if(!(selec->size()==1))
    return false;
  selec->delete_tuples();
  return true;
}

//: Retrieve a vector of names that describe the storage classes of a given type
vcl_vector < vcl_string > 
vidpro_repository::get_all_storage_class_names(const vcl_string& type, int frame)
{

  vcl_vector<vcl_string> names;

  brdb_query_aptr Q = brdb_query_comp_new("type", brdb_query::EQ, type);
  brdb_selection_sptr selec = DATABASE->select("global_data", Q);

  for(unsigned i=0; i<selec->size(); i++)
  {
    vcl_string name;
    selec->get("name", i, name);
    names.push_back(name);
  }

  if(DATABASE->exists(type)){
    Q = brdb_query_comp_new("frame", brdb_query::EQ, frame);
    selec = DATABASE->select(type, Q);

    for(unsigned i=0; i<selec->size(); i++)
    {
      vcl_string name;
      selec->get("name", i, name);
      names.push_back(name);
    }
  }

  return names;
}


//: Returns the set of all storage classes sptrs (all types) at the given frame
vcl_set < bpro_storage_sptr > 
vidpro_repository::get_all_storage_classes(int frame) const
{

  vcl_set < bpro_storage_sptr > storage_set;
  for( vcl_map<vcl_string, bpro_storage_sptr>::iterator 
    type_itr = registered_types_.begin();
    type_itr != registered_types_.end();  ++type_itr )
  {
    brdb_query_aptr Q = brdb_query_comp_new("frame", brdb_query::EQ, frame);
    brdb_selection_sptr selec = DATABASE->select(type_itr->first, Q);
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

  // This is a dummy query just to get the global_data relation
  brdb_query_aptr Q = brdb_query_comp_new("name", brdb_query::ALL, vcl_string(""));
  brdb_selection_sptr selec = DATABASE->select("global_data", Q);
  if (!selec->empty()) 
  {
    bpro_storage_sptr storage;
    for(unsigned int i=0; i<selec->size(); i++)
    {
      selec->get("sptr", i, storage);
      storage_set.insert(storage);
    }

  }

  return storage_set;
}

//: Returns the set of all storage classes sptrs of a given type (at all frames)
vcl_set < bpro_storage_sptr > 
vidpro_repository::get_all_storage_classes(const vcl_string& type) const
{
  vcl_set < bpro_storage_sptr > storage_set;

  // Local data
  brdb_query_aptr Q = brdb_query_comp_new("frame", brdb_query::ALL, 0);
  brdb_selection_sptr selec = DATABASE->select(type, Q);
  bpro_storage_sptr storage;
  if(selec){
  if (!selec->empty())  {
    for(unsigned int i=0; i<selec->size(); i++)   {
      selec->get( "sptr", i, storage);
      storage_set.insert(storage);
    }
  }
  }

  // Global data relation
  Q = brdb_query_comp_new("type", brdb_query::EQ, type);
  selec = DATABASE->select("global_data", Q);
  if (!selec->empty())  {
    for(unsigned int i=0; i<selec->size(); i++)  {
      selec->get( "sptr", i, storage);
      storage_set.insert(storage);
    }
  }
  return storage_set;
}

//: Returns complete set of all storage classes sptr's (global and local)
vcl_set < bpro_storage_sptr > 
vidpro_repository::get_all_storage_classes() const
{
  vcl_set< bpro_storage_sptr > whole_set;

  for( vcl_map<vcl_string, bpro_storage_sptr>::iterator 
    type_itr = registered_types_.begin();
    type_itr != registered_types_.end();  ++type_itr )
  {
    vcl_set< bpro_storage_sptr > local_type;
    local_type = get_all_storage_classes(type_itr->first);
    whole_set.insert(local_type.begin(),local_type.end());
  }
  return whole_set;
}

//: Returns the name that describe the a storage class
bool 
vidpro_repository::get_storage_name(const bpro_storage_sptr& storage, vcl_string& name)
{
  brdb_query_aptr Q = brdb_query_comp_new("sptr", brdb_query::EQ, storage);
  brdb_selection_sptr selec = DATABASE->select("global_data", Q);
  if (!selec->empty()){
    selec->get("name", 0, name);
    return true;
  }

  for ( vcl_map<vcl_string, bpro_storage_sptr>::iterator 
    type_itr = registered_types_.begin(); 
    type_itr != registered_types_.end();  ++type_itr){
      Q = brdb_query_comp_new("sptr", brdb_query::EQ, storage);
      selec = DATABASE->select(type_itr->first, Q);
      if (!selec->empty()){
        selec->get("name", 0, name);
        return true;
      }
  }
  // Storage not at the repository!
  return false;
}

//: Returns the type that describe the a storage class
bool
vidpro_repository::get_storage_type(const bpro_storage_sptr& storage, vcl_string& type)
{
  brdb_query_aptr Q = brdb_query_comp_new("sptr", brdb_query::EQ, storage);
  brdb_selection_sptr selec = DATABASE->select("global_data", Q);
  if (!selec->empty()){
    selec->get("type", 0, type);
    return true;
  }

  for ( vcl_map<vcl_string, bpro_storage_sptr>::iterator 
    type_itr = registered_types_.begin(); 
    type_itr != registered_types_.end();  ++type_itr){
      Q = brdb_query_comp_new("sptr", brdb_query::EQ, storage);
      selec = DATABASE->select(type_itr->first, Q);
      if (!selec->empty()){
        type = type_itr->first;
        return true;
      }
  }
  // Storage not at the repository!
  type = "";
  return false;
}

//: Returns the frame where a not global storage class is stored at
bool 
vidpro_repository::get_storage_frame(const bpro_storage_sptr& storage, int &frame)
{
  for ( vcl_map<vcl_string, bpro_storage_sptr>::iterator 
    type_itr = registered_types_.begin(); 
    type_itr != registered_types_.end();  ++type_itr){
      brdb_query_aptr Q = brdb_query_comp_new("sptr", brdb_query::EQ, storage);
      brdb_selection_sptr selec = DATABASE->select(type_itr->first, Q);
      if (!selec->empty()){
        selec->get( "frame", 0, frame);
        return true;
      }
  }
  return false;
}


//: Checks whether not global storage class is stored at
bool 
vidpro_repository::exist(vcl_string const & name , int frame)
{
  for ( vcl_map<vcl_string, bpro_storage_sptr>::iterator 
    type_itr = registered_types_.begin(); 
    type_itr != registered_types_.end();  ++type_itr){
      brdb_query_aptr Q = brdb_query_comp_new("name", brdb_query::EQ, name);
      brdb_selection_sptr selec = DATABASE->select(type_itr->first, Q);
      if (!selec->empty()){
        return true;
      }
  }
  return false;
}

//: Checks whether not global storage class is stored at
bool 
vidpro_repository::exist_in_global(vcl_string const & name , vcl_string const & type)
{

  brdb_query_aptr Q = brdb_query_comp_new("type", brdb_query::EQ, type)
                    & brdb_query_comp_new("name", brdb_query::EQ, name);
  brdb_selection_sptr selec = DATABASE->select("global_data", Q);
  if (!selec->empty())
    return true;

  return false;
}

bool 
vidpro_repository::update_storage(vcl_string const name , int frame, bpro_storage_sptr const& sto)
{
  brdb_query_aptr Q = brdb_query_comp_new("name",  brdb_query::EQ, name)
                    & brdb_query_comp_new("frame", brdb_query::EQ, frame);
  brdb_selection_sptr s = DATABASE->select("vsol2D", Q);
  if(!(s->size()==1))
    return false;

  s->update_selected_tuple_value("sptr", sto);
  return true;
}

bool 
vidpro_repository::remove_storage(vcl_string const name , int frame)
{
  brdb_query_aptr Q = brdb_query_comp_new("name",  brdb_query::EQ, name)
                    & brdb_query_comp_new("frame", brdb_query::EQ, frame);
  brdb_selection_sptr s = DATABASE->select("vsol2D", Q);

  if(!(s->size()==1))
    return false;

  s->delete_tuples();
  return true;
}

//: Returns the number of storage classes of a given type at the current frame
unsigned int 
vidpro_repository::get_storage_class_size(const vcl_string& type) const
{
  if( valid_frame( current_frame_ ) ) {
    brdb_query_aptr Q = brdb_query_comp_new("frame", brdb_query::EQ, current_frame_);
    brdb_selection_sptr selec = DATABASE->select(type, Q);
    return selec->size();
  }
  return 0;
}

//: Check whether a storage class is stored or not
bool 
vidpro_repository::is_stored(const bpro_storage_sptr& storage)
{
  brdb_query_aptr Q = brdb_query_comp_new("sptr", brdb_query::EQ, storage);
  brdb_selection_sptr selec = DATABASE->select("global_data", Q);
  if (!selec->empty()){
    return true;
  }

  for ( vcl_map<vcl_string, bpro_storage_sptr>::iterator 
    type_itr = registered_types_.begin(); 
    type_itr != registered_types_.end();  ++type_itr){
      Q = brdb_query_comp_new("sptr", brdb_query::EQ, storage);
      selec = DATABASE->select(type_itr->first, Q);
      if (!selec->empty()){
        return true;
      }
  }
  // Storage not at the repository!
  return false;
}


//: Retrieve a storage smart pointer to the data named \p name at the current frame
//  The optional frame_offset is added to the current frame number
bpro_storage_sptr 
vidpro_repository::get_data_by_name(const vcl_string& name, int frame_offset )
{
  return get_data_by_name_at( name, current_frame_ + frame_offset);
}


//: Retrieve a storage smart pointer to the data named \p name at the given frame
bpro_storage_sptr 
vidpro_repository::get_data_by_name_at( const vcl_string& name, int frame)
{
  brdb_query_aptr Q = brdb_query_comp_new("name", brdb_query::EQ, name);
  brdb_selection_sptr selec = DATABASE->select("global_data", Q);

  if(!selec->empty())
  {
    if (selec->size() > 1)
      vcl_cerr << "\nWarning: multiple global storage have the same name:\n"
      << name << "\n";

    bpro_storage_sptr storage;
    selec->get( "sptr", 0, storage);
    return storage;
  }

  for( vcl_map<vcl_string, bpro_storage_sptr>::iterator 
    type_itr = registered_types_.begin();
    type_itr != registered_types_.end();  ++type_itr )
  {
    Q = brdb_query_comp_new("frame", brdb_query::EQ, frame)
      & brdb_query_comp_new("name",  brdb_query::EQ, name);
    selec = DATABASE->select(type_itr->first, Q);

    if(!selec->empty())
    {
      if (selec->size() > 1)
        vcl_cerr << "\nWarning: multiple storage have the same name and frame:\n"
        << name <<" @ frame "<<frame<<"\n";

      bpro_storage_sptr storage;
      selec->get( "sptr", 0, storage);
      return storage;
    }
  }

  return NULL; // nothing found
}


//: Retrieve a storage smart pointer to the data indexed by ind of a given type at the current frame
//  The optional frame_offset is added to the current frame number
bpro_storage_sptr 
vidpro_repository::get_data(const vcl_string& type, int frame_offset, int ind)
{
  return get_data_at( type, current_frame_ + frame_offset, ind);
}



//: Retrieve a storage smart pointer to the data indexed by ind of a given type at the given frame
bpro_storage_sptr 
vidpro_repository::get_data_at(const vcl_string& type, int frame, int ind)
{
  if( valid_frame( frame ) ) {

    brdb_query_aptr Q = brdb_query_comp_new("frame", brdb_query::EQ, frame);
    brdb_selection_sptr selec = DATABASE->select(type, Q);
    bpro_storage_sptr storage;
    selec->get( "sptr", ind, storage);
    return storage;
  }
  return NULL;
}

//: Retrieve a storage smart pointer to the data indexed by ind of a given type at the given frame
bpro_storage_sptr 
vidpro_repository::get_global_data(const vcl_string& type, int ind)
{
  brdb_query_aptr Q = brdb_query_comp_new("type", brdb_query::EQ, type);
  brdb_selection_sptr selec = DATABASE->select("global_data", Q);

  bpro_storage_sptr storage;
  if (!selec->empty()){
    selec->get( "sptr", ind, storage);
    return storage;
  }
  return NULL;
}

//: Store the storage smart pointer to the data at the current frame
//  The optional frame_offset is added to the current frame number
//  Returns false if no storage of this type has been defined
bool 
vidpro_repository::store_data(const bpro_storage_sptr& storage, int frame_offset)
{
  return store_data_at( storage, current_frame_ + frame_offset );
}


//: Store the storage smart pointer to the global data
//  Returns false if no storage of this type has been defined
bool 
vidpro_repository::store_global_data(const bpro_storage_sptr& storage)
{
  return DATABASE->
    add_tuple("global_data",new brdb_tuple(storage->name(),storage->type(),storage));
}


//: Store the storage smart pointer to the data at the given frame
//  Returns false if no storage of this type has been defined
bool 
vidpro_repository::store_data_at(const bpro_storage_sptr& storage, int frame)
{
  if( !storage.ptr() )
    return false;

  int sto_frame; vcl_string name;
  if( get_storage_name(storage,name) ){
    vcl_cerr << __FILE__ << ":\nWarning: storage \""<<name<< "\" is already stored ";
    if (get_storage_frame(storage,sto_frame))
      vcl_cerr << "at frame " << sto_frame << ".\n";
    else
      vcl_cerr << "in global relation.\n";
    return false;
  }

  // Construct the new tuple
  brdb_tuple_sptr new_tuple = new brdb_tuple(frame,storage->name(),storage);
  name = storage->name();
  vcl_string type = storage->type();

  // Search for existing global storage with the same name
  brdb_query_aptr Q = brdb_query_comp_new("name", brdb_query::EQ, name);
  brdb_selection_sptr selec_global = DATABASE->select("global_data", Q);

  // Also search in the local frame
  brdb_query_aptr Q2 = brdb_query_comp_new("frame", brdb_query::EQ, frame)
                     & brdb_query_comp_new("name",  brdb_query::EQ, name);
  brdb_selection_sptr selec_local = DATABASE->select(type, Q2);

  if ( (!selec_global->empty() && !selec_local->empty()) ||
    selec_global->size() > 1  ||  selec_local->size() > 1 )
    vcl_cerr << "\nWarning: Multiple storage have the same name: "<<name<<"!\n";

  // Remove found tuples to insure the storage's name/frame is unique
  selec_global->delete_tuples();
  selec_local->delete_tuples();
 

  // store local data

  if(
    DATABASE->
    add_tuple(storage->type(),new_tuple))
  {
     return true;
  }
  return false;

}


//: Create a new empty storage class
//  The optional frame_offset is added to the current frame number
//  \return false if this data type is not registered
bpro_storage_sptr  
vidpro_repository::new_data( const vcl_string& type, 
                             const vcl_string& name, 
                             int frame_offset )
{
  return new_data_at( type, name, current_frame_ + frame_offset );
}


//: Create a new empty storage class
//  \return false if this data type is not registered
bpro_storage_sptr  
vidpro_repository::new_data_at( const vcl_string& type, 
                                const vcl_string& name, 
                                int frame )
{
  if( name == "" )
    return NULL;
  vcl_map<vcl_string, bpro_storage_sptr>::iterator 
    result = registered_types_.find(type);
  if( result == registered_types_.end() )
    return NULL;

  bpro_storage_sptr blank = result->second->clone();
  blank->set_name(name);
  if(!this->store_data_at(blank, frame))
    return NULL;

  return blank;
}

//: Create a new empty global storage class
//  \return false if this data type is not registered
bpro_storage_sptr  
vidpro_repository::new_global_data( const vcl_string& type, 
                                    const vcl_string& name)
{
  if( name == "" )
    return NULL;
  vcl_map<vcl_string, bpro_storage_sptr>::iterator 
    result = registered_types_.find(type);
  if( result == registered_types_.end() )
    return NULL;

  bpro_storage_sptr blank = result->second->clone();
  blank->set_name(name);
  if(!this->store_global_data(blank))
    return NULL;

  return blank;
}

//:Returns a map with names and types of output streams of a given process
vcl_map<vcl_string, vcl_string> 
vidpro_repository::get_process_ostreams(bpro_process_sptr const & pro)
{
  vcl_map<vcl_string, vcl_string> ostream_names;
  brdb_query_aptr Q = brdb_query_comp_new("process", brdb_query::EQ, pro->name());
  brdb_selection_sptr selec = DATABASE->select("ostream-process", Q);
  for(unsigned i=0; i<selec->size(); i++)
  {
    vcl_string name;
    vcl_string type;
    selec->get( "ostream", i, name);
    selec->get( "type", i, type);
    ostream_names.insert(vcl_pair<vcl_string,vcl_string>(type,name));
  }

  return ostream_names;
}

void
vidpro_repository::clear_process_ostreams(bpro_process_sptr const & pro)
{
  vcl_map<vcl_string, vcl_string> ostream_names;
  brdb_query_aptr Q = brdb_query_comp_new("process", brdb_query::EQ, pro->name());
  brdb_selection_sptr selec = DATABASE->select("ostream-process", Q);
  if(selec->empty())
    return;

  selec->delete_tuples();
  return;
}

vcl_vector<vcl_string> 
vidpro_repository::get_object_stream_types()
{
  vcl_vector<vcl_string> types;

  brdb_query_aptr Q = brdb_query_comp_new("name", brdb_query::ALL, vcl_string(""));
  brdb_selection_sptr selec = DATABASE->select("object ostreams", Q);
  for(unsigned i=0; i<selec->size(); i++)
  {
    vcl_string type;
    selec->get( "type", i, type);
    types.push_back(type);
  }

  return types;
}

vcl_vector<vcl_string> 
vidpro_repository::get_object_stream_names()
{
  vcl_vector<vcl_string> names;

  brdb_query_aptr Q = brdb_query_comp_new("name", brdb_query::ALL, vcl_string(""));
  brdb_selection_sptr selec = DATABASE->select("object ostreams", Q);
  for(unsigned i=0; i<selec->size(); i++)
  {
    vcl_string name;
    selec->get( "name", i, name);
    names.push_back(name);
  }

  return names;
}

vcl_string
vidpro_repository::get_object_stream_name( vcl_string const &path )
{
  vcl_string name;

  brdb_query_aptr Q = brdb_query_comp_new("path", brdb_query::EQ, path);
  brdb_selection_sptr selec = DATABASE->select("object ostreams", Q);
  if(selec->size()==1)
    selec->get("name", name);
  else
    vcl_cerr<< "Warning: Couldn't find requested object stream name\n"; 

  return name;
}

vcl_vector<vcl_string> 
vidpro_repository::get_object_stream_paths()
{
  vcl_vector<vcl_string> paths;

  if(!DATABASE->exists("object ostreams"))
    return paths;

  brdb_query_aptr Q = brdb_query_comp_new("path", brdb_query::ALL, vcl_string(""));
  brdb_selection_sptr selec = DATABASE->select("object ostreams", Q);
  for(unsigned i=0; i<selec->size(); i++)
  {
    vcl_string path;
    selec->get( "path", i, path);
    paths.push_back(path);
  }

  return paths;
}


//: Print a summary of the repository structure to cout
void 
vidpro_repository::print_summary()
{
  DATABASE->print();

  vcl_cout << "\n**** vidpro_repository summary: ****\n\n";
  vcl_cout << "Number of frames: " << num_frames() << vcl_endl;

  vcl_cout << "Registered Types: " ;

  //Registered Types
  for ( vcl_map<vcl_string, bpro_storage_sptr>::iterator 
    type_itr = registered_types_.begin(); 
    type_itr != registered_types_.end();  ++type_itr)
    vcl_cout << (type_itr->first) << ", " ;
  vcl_cout << "\b\b.";

  if (num_frames()) {
    // Local data
    vcl_cout << "\n\nCurrent frame:";
    for( vcl_map<vcl_string, bpro_storage_sptr>::iterator 
      type_itr = registered_types_.begin();
      type_itr != registered_types_.end();  ++type_itr )
    {
      brdb_query_aptr Q = brdb_query_comp_new("frame", brdb_query::EQ, current_frame_);
      brdb_selection_sptr selec = DATABASE->select(type_itr->first, Q);
      if (!selec->empty()){
        vcl_cout << "\n  " <<type_itr->first << ": ";
      }

      for(unsigned i=0; i<selec->size(); i++)
      {
        vcl_string name;
        selec->get( "name", i, name);
        vcl_cout << name << " ";
      }
    }
  }

  // Global data
  vcl_stringstream global_summary;
  brdb_query_aptr Q = brdb_query_comp_new("type", brdb_query::ALL, vcl_string(""));
  brdb_selection_sptr selec = DATABASE->select("global_data", Q);

  if(!selec->empty()) {
    // if the global relation is not empty:
    global_summary << "\n\nGlobal:";
    for( vcl_map<vcl_string, bpro_storage_sptr>::iterator 
      type_itr = registered_types_.begin();
      type_itr != registered_types_.end();  ++type_itr )
    {
      Q = brdb_query_comp_new("type", brdb_query::EQ, type_itr->first);
      selec = DATABASE->select("global_data", Q);
      if (!selec->empty())
        global_summary << "\n  " << type_itr->first + ": ";

      for(unsigned i=0; i<selec->size(); i++)
      {
        vcl_string name;
        selec->get( "name", i, name);
        global_summary << name << " ";
      }
    }
    vcl_cout << global_summary.str();
  }

  vcl_cout << "\n\n************************************\n";

}

//: Return the number of frames in the sequence
int 
vidpro_repository::num_frames() const
{
  int maxf=-1;
  for( vcl_map<vcl_string, bpro_storage_sptr>::iterator 
    type_itr = registered_types_.begin();
    type_itr != registered_types_.end();  ++type_itr )
  {
    brdb_query_aptr Q = brdb_query_comp_new("frame", brdb_query::ALL, 0);
    brdb_selection_sptr selec = DATABASE->select(type_itr->first, Q);
    if (!selec->empty())
    {
      int frame;
      for(unsigned int i=0; i<selec->size(); i++)
      {
        selec->get( "frame", i, frame);
        if (i==0) 
          maxf = frame;
        else if(maxf<frame) 
          maxf = frame;
      }
    }
  }
  return maxf+1;
}




//------------------------------------------------------------------------------
// \todo Need proper implementation for the following functions.

//: Binary save vidpro1_repository* to stream.
void vsl_b_write(vsl_b_ostream &os, const vidpro_repository* n)
{
};

//: Binary load vidpro1_repository* from stream.
void vsl_b_read(vsl_b_istream &is, vidpro_repository* &n)
{
};

//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, const vidpro_repository* n)
{
};
