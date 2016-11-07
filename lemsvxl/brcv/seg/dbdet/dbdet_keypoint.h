// This is brcv/seg/dbdet/dbdet_keypoint.h
#ifndef dbdet_keypoint_h_
#define dbdet_keypoint_h_
//:
// \file
// \brief A detected image keypoint
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 2/22/05
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_fwd.h>
#include <vsl/vsl_binary_loader.h>


//: A detected keypoint (base class)
class dbdet_keypoint : public vbl_ref_count, public vgl_point_2d<double>
{
 public:
  //: Constructor
  dbdet_keypoint() {}
  //: Constructor
  dbdet_keypoint(double x, double y) : vgl_point_2d<double>(x,y) {}
  //: Destructor
  virtual ~dbdet_keypoint() {}
  
  virtual bool has_scale() const { return false; }
  virtual double scale() const { return 1.0; }
  virtual bool set_scale(double /*s*/) { return false; }
  
  virtual bool has_orientation() const { return false; }
  virtual double orientation() const { return 0.0; }
  virtual bool set_orientation(double /*o*/) { return false; }
  
  virtual unsigned int descriptor_size() const { return 0; }
  virtual const vnl_vector<double>& descriptor() const { static vnl_vector<double> empty; return empty; }
  virtual bool set_descriptor(const vnl_vector<double>& /*d*/) { return false; }

  //=============== Binary I/O Methods ========================
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbdet_keypoint"; }

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual dbdet_keypoint* clone() const;

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  virtual void print_summary(vcl_ostream &os) const;

};


//: Allows derived class to be loaded by base-class pointer
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(os,base_ptr)".  This loads derived class
//  objects from the disk, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(const dbdet_keypoint& k);


//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, const dbdet_keypoint* k);



#endif // dbdet_keypoint_h_
