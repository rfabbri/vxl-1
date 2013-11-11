// This is brl/vidpro/storage/vidpro_vsol3D_storage.h
#ifndef vidpro_vsol3D_storage_h_
#define vidpro_vsol3D_storage_h_


//:
// \file
// \brief The vidpro storage class for an image
// \author Mark Johnson (mrj@lems.brown.edu)
// \date 7/22/03
//
//
// \verbatim
//  Modifications
//   5/11/2004  Matt Leotta  Added binary I/O support
//   7/22/2004  Matt Leotta  Modified interface for easier data access
// \endverbatim

#include <bpro/bpro_storage.h>
#include <vidpro/storage/vidpro_vsol3D_storage_sptr.h>
#include <vsol/vsol_spatial_object_3d_sptr.h>

#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_string.h>

class vidpro_vsol3D_storage : public bpro_storage {

public:
  typedef vcl_map< vcl_string, vcl_vector<vsol_spatial_object_3d_sptr> > data_map;

  //: Constructor
  vidpro_vsol3D_storage();

  //: Destructor
  virtual ~vidpro_vsol3D_storage();

  //: Return the type identifier string
  virtual vcl_string type() const { return "vsol3D"; }
    
  //: Register vsol_spatial_object_3d types for I/O
//  virtual void register_binary_io() const;
    
  //: Return IO version number;
//  short version() const;
  
  //: Binary save self to stream.
//  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
 // void b_read(vsl_b_istream &is);

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "vidpro_vsol3D_storage"; }

  //: Add a vector of vsol3D objects to the group named \p which
  void add_objects( const vcl_vector< vsol_spatial_object_3d_sptr >& object , 
                    const vcl_string& which = "default" );

  //: Add an object to the group named \p which
  void add_object( const vsol_spatial_object_3d_sptr& object, 
                   const vcl_string& which = "default" );

  //: Search for the object in all groups and remove it
  // \retval The name of the group removed from of the empty string if not found
  vcl_string remove_object( const vsol_spatial_object_3d_sptr& object );

  //: Search for the object in the given group only and remove it
  // \return true if the object was successfully removed
  // \return false if the object was not found
  bool remove_object( const vsol_spatial_object_3d_sptr& object, const vcl_string& group );

  //: Return a vector of all vsol objects in the group named \p which
  vcl_vector< vsol_spatial_object_3d_sptr > data_named( const vcl_string& which) const;

  //: Return a vector of all vsol objects in all groups
  vcl_vector< vsol_spatial_object_3d_sptr > all_data() const;

  //: Return the names of all groups 
  vcl_vector< vcl_string > groups() const;
  
  //: Erase all objects in all groups
  void clear_all();

protected:

private:

  //: The data
  data_map vsol_map_; 

  //: Is the binary I/O registered
  static bool registered_; 
};


//: Create a smart-pointer to a vidpro_vsol3D_storage.
struct vidpro_vsol3D_storage_new : public vidpro_vsol3D_storage_sptr
{
  typedef vidpro_vsol3D_storage_sptr base;

  //: Constructor - creates a default vidpro_vsol3D_storage_sptr.
  vidpro_vsol3D_storage_new() : base(new vidpro_vsol3D_storage()) { }
};


#endif //vidpro_vsol3D_storage_h_
