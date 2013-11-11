#ifndef ddbetl_track_storage_h
#define ddbetl_track_storage_h

// \file
// \brief A storage class for dbetl data
// \author  mleotta
// \date  11/22/2004

#include <bpro1/bpro1_storage.h>
#include <dbetl/dbetl_point_track_sptr.h>
#include <dbetl/dbetl_camera_sptr.h>
#include "dbetl_track_storage_sptr.h"

//: Storage class for dbetl data
class dbetl_track_storage : public bpro1_storage {

public:
  //: Constructor
  dbetl_track_storage() : camera_(NULL) {}
  //: Destructor
  virtual ~dbetl_track_storage() {}
  virtual vcl_string type() const { return "betl"; }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbetl_track_storage"; }

  //: Set the point tracks
  void set_tracks(const vcl_vector<vcl_vector<dbetl_point_track_sptr> >& tracks);

  //: Access the tracks
  vcl_vector<vcl_vector<dbetl_point_track_sptr> > tracks() const;
    
  //: Set the angles
  void set_angles(const vcl_vector<double>& angles);

  //: Access the angles
  vcl_vector<double> angles() const;

  //: Set the camera
  void set_camera(const dbetl_camera_sptr& camera);

  //: Access the camera
  dbetl_camera_sptr camera() const;
  
private:
  //: The tracks
  vcl_vector<vcl_vector<dbetl_point_track_sptr> > tracks_;

  //: The angles
  vcl_vector<double> angles_;

  //: The camera
  dbetl_camera_sptr camera_;

};


//: Create a smart-pointer to a dbetl_track_storage.
struct dbetl_track_storage_new : public dbetl_track_storage_sptr
{
  typedef dbetl_track_storage_sptr base;

  //: Constructor - creates a default dbetl_track_storage_sptr.
  dbetl_track_storage_new() : base(new dbetl_track_storage()) { }
};

#endif
