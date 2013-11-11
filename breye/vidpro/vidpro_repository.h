// This is brl/vidpro/vidpro_repository.h
#ifndef vidpro_repository_h_
#define vidpro_repository_h_

//:
// \file
// \brief This file defines a class that stores smart pointers 
//        to various types of data associated with the frames
//        of a video sequence in a database design.
// \author Eduardo Almeida
// \date 05/2007
//
// \verbatim
//  Modifications
//    05/01/07 - Initial version.
// \endverbatim

#include <bpro/bpro_storage_registry.h>
#include <bpro/bpro_storage.h>
#include <bpro/bpro_process_sptr.h>
#include <vidpro/vidpro_repository_sptr.h>

class vidpro_repository : public bpro_storage_registry {

public:
  typedef vcl_map< vcl_string, vcl_vector< bpro_storage_sptr > > storage_map;

  //: Constructor
  vidpro_repository();
  //: Destructor
  ~vidpro_repository(){}

  //: Clear all data from the repository
  void remove_all();

  //: Remove all data from the repository except those with the given names
  void remove_all_except(const vcl_set<vcl_string>& retain);

  //: Clear the repository and replace with a new one
  // \note the registered types in new_rep must be a subset of types registered here
  bool replace_data(const vcl_string& path);

  //:Decrement the frame numbers in the database
  bool decrement_frame_number();

  //:Increment the frame numbers in the database
  bool increment_frame_number();

  //: Removes all data at the given frame of all types
  bool remove_frame(int frame);

  //: Write repository to a file
  void save_data(const vcl_string& path);

  //: Add new repository to *this repository (maintain original storage units)
  bool add_repository(const vcl_string& path);

  //: Clear all data and resize the repository for the given number of frames
  void initialize( int num_frames = 0);

  //: Intialize the map of registered types for global data
  void initialize_global();

  //: Add a new frame at the end
  void add_new_frame();

  //: Return the current frame
  int current_frame() const; 

  //: Return the number of frames in the sequence
  int num_frames() const;

  //: Returns true if the specified frame is valid
  bool valid_frame( int frame ) const;

  //: Increment the current frame number if not at the end
  bool go_to_next_frame();
  //: Increment the current frame by n number if not at the end
  bool go_to_next_n_frame(int n);
  //: Decrement the current frame number if not at the beginning
  bool go_to_prev_frame();
  //: Decrement the current frame by n number if not at the beginning
  bool go_to_prev_n_frame(int n);
  //: Jump the the specified frame number if valid
  bool go_to_frame( int num );

  //: Retrieve a vector of names that describe the storage classes of a given type
  //  at the current frame
  vcl_vector < vcl_string > get_all_storage_class_names(const vcl_string& type);
  //: Retrieve a vector of names that describe the storage classes of a given type
  //  \param frame_offset indicates the frame number
  vcl_vector < vcl_string > get_all_storage_class_names(const vcl_string& type, int frame);
  //: Returns the set of all storage classes (all types) at the given frame
  vcl_set < bpro_storage_sptr > get_all_storage_classes(int frame) const;
  //: Returns the set of all storage classes sptrs of a given type (at all frames)
  vcl_set < bpro_storage_sptr > get_all_storage_classes(const vcl_string& type) const;
  //: Returns complete set of all storage classes sptr's (global and local)
  vcl_set < bpro_storage_sptr > get_all_storage_classes() const;
  //: Returns the name that describe the a storage class
  bool get_storage_name(const bpro_storage_sptr& storage, vcl_string& name);
  //: Returns the type that describe the a storage class
  bool get_storage_type(const bpro_storage_sptr& storage, vcl_string& type);
  //: Returns the frame where a not global storage class is stored at
  bool get_storage_frame(const bpro_storage_sptr& storage, int &frame);

  //: Returns true if the given storage exists at the given frame
  bool exist(vcl_string const & name , int frame);

  bool exist_in_global(vcl_string const & name, vcl_string const & type);

  bool update_storage(vcl_string const name , int frame, bpro_storage_sptr const& sto);
  bool remove_storage(vcl_string const name , int frame);


  //: Returns the number of storage classes of a given type at the current frame
  unsigned int get_storage_class_size(const vcl_string& type) const;

  //: Check whether a storage class is stored or not
  bool is_stored(const bpro_storage_sptr& storage);

  //: Retrieve a storage smart pointer to the data named \p name at the current frame
  //  The optional frame_offset is added to the current frame number
  bpro_storage_sptr get_data_by_name(const vcl_string& name, int frame_offset=0 );
  //: Retrieve a storage smart pointer to the data named \p name at the given frame
  bpro_storage_sptr get_data_by_name_at( const vcl_string& name, int frame);
  //: Retrieve a storage smart pointer to the data indexed by ind of a given type at the current frame
  //  The optional frame_offset is added to the current frame number
  bpro_storage_sptr get_data(const vcl_string& type, int frame_offset=0, int ind=0);
  //: Retrieve a storage smart pointer to the data indexed by ind of a given type at the given frame
  bpro_storage_sptr get_data_at(const vcl_string& type, int frame, int ind=0);
  //: Retrieve a global storage smart pointer to the data indexed by ind of a given type
  bpro_storage_sptr get_global_data(const vcl_string& type, int ind);
  //: Retrieve a storage smart pointer to the global data named \p name
  bpro_storage_sptr get_global_storage_by_name(const vcl_string& name);
  //: Remove a global storage named \p name
  bool remove_global_storage_by_name(const vcl_string& name);

  //: Store the storage smart pointer to the data at the current frame
  //  The optional frame_offset is added to the current frame number
  //  Returns false if no storage of this type has been defined
  bool store_data(const bpro_storage_sptr& storage, int frame_offset=0);
  //: Store the storage smart pointer to the data at the given frame
  //  Returns false if no storage of this type has been defined
  bool store_data_at(const bpro_storage_sptr& storage, int frame);
  //: Store the storage smart pointer to the global data
  bool store_global_data(const bpro_storage_sptr& storage);

  vcl_map<vcl_string, vcl_string> get_process_ostreams(bpro_process_sptr const & pro);

  void clear_process_ostreams(bpro_process_sptr const & pro);
  vcl_vector<vcl_string> get_object_stream_types();
  vcl_vector<vcl_string> get_object_stream_names();
  vcl_string get_object_stream_name( vcl_string const &path );
   vcl_vector<vcl_string> get_object_stream_paths();


  //: Create a new empty storage class
  //  The optional frame_offset is added to the current frame number
  //  \return NULL if this data type is not registered
  bpro_storage_sptr new_data(const vcl_string& type, const vcl_string& name, int frame_offset=0);
  //: Create a new empty storage class
  //  \return NULL if this data type is not registered
  bpro_storage_sptr new_data_at(const vcl_string& type, const vcl_string& name, int frame);
  //: Create a new empty global storage class
  //  \return false if this data type is not registered
  bpro_storage_sptr new_global_data( const vcl_string& type, const vcl_string& name);


  //: Print a summary of the repository structure to cout
  void print_summary();

protected:

private:
  //: The current active frame (starting with 0)
  int current_frame_;
};


// \todo Need proper implementation for the following functions.

//: Binary save vidpro1_repository* to stream.
void vsl_b_write(vsl_b_ostream &os, const vidpro_repository* n);

//: Binary load vidpro1_repository* from stream.
void vsl_b_read(vsl_b_istream &is, vidpro_repository* &n);

//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, const vidpro_repository* n);

#endif // vidpro_repository_h_
