// This is brcv/trk/dbmrf/pro/dbmrf_curvel_3d_storage.h
#ifndef dbmrf_curvel_3d_storage_h_
#define dbmrf_curvel_3d_storage_h_

//:
// \file
// \brief The curvel_3d storage class
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 3/23/04
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include "dbmrf_curvel_3d_storage_sptr.h"
#include <vcl_set.h>
#include <vcl_vector.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_4x4.h>
#include <vgl/vgl_vector_3d.h>
#include <bmrf/bmrf_curve_3d_sptr.h>

//: Storage class for bmrf_curvel_3d
class dbmrf_curvel_3d_storage : public bpro1_storage {
public:

  //: Constructor
  dbmrf_curvel_3d_storage();
  //: Constructor
  dbmrf_curvel_3d_storage( const vcl_set<bmrf_curve_3d_sptr>& curves,
                            const vnl_double_3x4& camera,
                            const vgl_vector_3d<double>& dir,
                            const vnl_double_4x4& xform );
  //: Destructor
  virtual ~dbmrf_curvel_3d_storage(){}

  //: Returns the type string "curvel_3d"
  virtual vcl_string type() const { return "curvel_3d"; }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbmrf_curvel_3d_storage"; }

  //: Set the curvel_3d storage set
  void set_curvel_3d( const vcl_set<bmrf_curve_3d_sptr>& curves);

  //: Retrieve the curvel_3d storage set
  void get_curvel_3d( vcl_set<bmrf_curve_3d_sptr>& curves ) const;

  //: Set the cameras
  void set_camera( const vnl_double_3x4& camera );

  //: Retrieve the cameras
  vnl_double_3x4 camera() const;

  //: Set the direction of motion
  void set_direction( const vgl_vector_3d<double>& dir );

  //: Retrieve the direction of motion
  vgl_vector_3d<double> direction() const;
  
  //: Set the bounding box transform
  void set_bb_xform( const vnl_double_4x4& xform );

  //: Retrieve the bounding box transform
  vnl_double_4x4 bb_xform() const;

  //: Return IO version number;
  short version() const;
  
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

private:
  //: The vector of bmrf_curve_3d smart pointers
  vcl_set<bmrf_curve_3d_sptr> curvel_3d_;
  
  //: Vector of cameras
  vnl_double_3x4 camera_;

  //: The direction of motion
  vgl_vector_3d<double> direction_;

  //: This transform maps the unit cube into the vehicle aligned bounding box
  vnl_double_4x4 bb_xform_;

};


//: Create a smart-pointer to a dbmrf_curvel_3d_storage.
struct dbmrf_curvel_3d_storage_new : public dbmrf_curvel_3d_storage_sptr
{
  typedef dbmrf_curvel_3d_storage_sptr base;

  //: Constructor - creates a default dbmrf_curvel_3d_storage_sptr.
  dbmrf_curvel_3d_storage_new() : base(new dbmrf_curvel_3d_storage()) { }

  //: Constructor - creates a dbmrf_curvel_3d_storage_sptr with curvel_3ds.
  dbmrf_curvel_3d_storage_new(const vcl_set<bmrf_curve_3d_sptr>& curves,
                               const vnl_double_3x4& camera,
                               const vgl_vector_3d<double>& dir,
                               const vnl_double_4x4& xform )
   : base(new dbmrf_curvel_3d_storage( curves, camera, dir, xform )) { }
};


#endif //dbmrf_curvel_3d_storage_h_
