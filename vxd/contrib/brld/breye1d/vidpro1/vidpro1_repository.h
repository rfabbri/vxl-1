// This is brl/vidpro1/vidpro1_repository.h
#ifndef vidpro1_repository_h_
#define vidpro1_repository_h_

//:
// \file
// \brief This file defines a class that stores smart pointers 
//        to various types of data associated with the frames
//        of a video sequence.
// \author Based on original code by Mark Johnson (mrj)
// \date 7/15/03
//
// \verbatim
//  Modifications
//    10/28/03 - Matt Leotta - Redesigned for abstract types
//                           - Converted comments to Doxygen style 
//    05/27/04 - Amir Tamrakar - Added the print_summary function
//    07/22/04 - Matt Leotta - Added functions to clone empty storage
//                           - Changed internal representation of registered types
// \endverbatim

#include <vector>
#include <map>
#include <set>
#include <string>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_fwd.h>
#include <bpro1/bpro1_storage_sptr.h>
#include <vidpro1/vidpro1_repository_sptr.h>

class vidpro1_repository : public vbl_ref_count {

public:
  typedef std::map< std::string, std::vector< bpro1_storage_sptr > > storage_map;

  //: Constructor
  vidpro1_repository();
  //: Destructor
  ~vidpro1_repository(){}

  //: Registers a storage type with the repository
  //  Call this function with a dummy instance of each type you plan to use
  //  Returns false if this type is already registered
  static bool register_type(const bpro1_storage_sptr& dummy_storage);
  //: Returns the set of strings representing all registered types
  std::set< std::string > types() const;
  //: Clear all data from the repository
  void remove_all();
  //: Remove all data from the repository except those with the given names
  void remove_all_except(const std::set<std::string>& retain);
  //: Clear the repository and replace with the data in new_rep;
  // \note the registered types in new_rep must be a subset of types registered here
  void replace_data(const vidpro1_repository_sptr& new_rep);

  //: Add new repository to *this repository (maintain original storage units)
  bool add_repository(const vidpro1_repository_sptr& new_rep);

  //: Clear all data and resize the repository for the given number of frames
  void initialize( int num_frames );

  //: Intialize the map of registered types for global data
  void initialize_global();

  //: Add a new frame at the end
  void add_new_frame();

  //: Return the current frame
  int current_frame() const { return current_frame_; }
  //: Return the number of frames in the sequence
  int num_frames() const { return data_.size(); }
  
  //: Returns true if the specified frame is valid
  bool valid_frame( int frame ) const;
  
  //: Increment the current frame number if not at the end
  bool go_to_next_frame();
  //: Increment the current frame by n number if not at the end
  bool go_to_next_n_frame(int n);
  //: Decrement the current frame number if not at the beginning
  bool go_to_prev_frame();
  bool go_to_prev_n_frame(int n);
  //: Jump the the specified frame number if valid

  bool go_to_frame( int num );

  //: Return an iterator to the beginning of the data at the current frame
  storage_map::iterator map_begin();
  //: Return an iterator to the end of the data at the current frame
  storage_map::iterator map_end();
  
  //: Return an iterator to the beginning of the global data
  storage_map::iterator global_map_begin();
  //: Return an iterator to the end of the global data
  storage_map::iterator global_map_end();

  //: Retrieve a vector of names that describe the storage classes of a given type
  //  at the current frame
  std::vector < std::string > get_all_storage_class_names(const std::string& type);
  //: Retrieve a vector of names that describe the storage classes of a given type
  //  \param frame_offset indicates the frame number
  std::vector < std::string > get_all_storage_class_names(const std::string& type, int frame);
  
  //: Returns the set of all storage classes (all types) at the given frame
  std::set < bpro1_storage_sptr > get_all_storage_classes(int frame) const;
  
  //: Returns the number of storage classes of a given type at the current frame
  int get_storage_class_size(const std::string& type) const;

  //: Retrieve a storage smart pointer to the data named \p name at the current frame
  //  The optional frame_offset is added to the current frame number
  bpro1_storage_sptr get_data_by_name(const std::string& name, int frame_offset=0 );
  //: Retrieve a storage smart pointer to the data named \p name at the given frame
  bpro1_storage_sptr get_data_by_name_at( const std::string& name, int frame);
  //: Retrieve a storage smart pointer to the data indexed by ind of a given type at the current frame
  //  The optional frame_offset is added to the current frame number
  bpro1_storage_sptr get_data(const std::string& type, int frame_offset=0, int ind=0);
  //: Retrieve a storage smart pointer to the data indexed by ind of a given type at the given frame
  bpro1_storage_sptr get_data_at(const std::string& type, int frame, int ind=0);

  //: Store the storage smart pointer to the data at the current frame
  //  The optional frame_offset is added to the current frame number
  //  Returns false if no storage of this type has been defined
  bool store_data(const bpro1_storage_sptr& storage, int frame_offset=0);
  //: Store the storage smart pointer to the data at the given frame
  //  Returns false if no storage of this type has been defined
  bool store_data_at(const bpro1_storage_sptr& storage, int frame);

  //: Pop back the storage smart pointer to the data at the current frame with the given type
  //  The optional frame_offset is added to the current frame number
  //  Returns false if no storage of this type has been defined
  bool pop_data(const std::string& type, int frame_offset=0);
  //: Pop back the storage smart pointer to the data at the given frame with the given type
  //  Returns false if no storage of this type has been defined
  bool pop_data_at(const std::string& type, int frame);

  //: Create a new empty storage class
  //  The optional frame_offset is added to the current frame number
  //  \return NULL if this data type is not registered
  bpro1_storage_sptr new_data(const std::string& type, const std::string& name, int frame_offset=0);
  //: Create a new empty storage class
  //  \return NULL if this data type is not registered
  bpro1_storage_sptr new_data_at(const std::string& type, const std::string& name, int frame);

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);
 
  //: Return IO version number;
  short version() const;

  //: Print a summary of the repository structure to cout
  void print_summary();
  
protected:

private:
  //: The current active frame (starting with 0)
  int current_frame_;
  //: A vector of registered types
  static std::map<std::string, bpro1_storage_sptr> registered_types_;
  //: Vector(indexed by frame number) of a map of type name to vector of data smart pointers
  std::vector< storage_map > data_;
  //: Data associated with all frames - a map of type name to vector of data smart pointers
  storage_map global_data_;
};


//: Binary save vidpro1_repository* to stream.
void vsl_b_write(vsl_b_ostream &os, const vidpro1_repository* n);

//: Binary load vidpro1_repository* from stream.
void vsl_b_read(vsl_b_istream &is, vidpro1_repository* &n);

//: Print an ASCII summary to the stream
void vsl_print_summary(std::ostream &os, const vidpro1_repository* n);


#endif // vidpro1_repository_h_
