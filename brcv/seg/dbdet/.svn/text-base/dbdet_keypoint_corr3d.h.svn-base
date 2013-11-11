// This is brcv/seg/dbdet/dbdet_keypoint_corr3d.h
#ifndef dbdet_keypoint_corr3d_h_
#define dbdet_keypoint_corr3d_h_
//:
// \file
// \brief A correspondence between a keypoint in multiple views and a 3D point
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 4/25/05
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgl/vgl_point_3d.h>
#include <vbl/vbl_ref_count.h>
#include <dbdet/dbdet_keypoint_sptr.h>
#include <vsl/vsl_fwd.h>
#include <vcl_string.h>
#include <vcl_map.h>


//: A 3D point corresponding to multiple 2D keypoint from several views
class dbdet_keypoint_corr3d : public vbl_ref_count, public vgl_point_3d<double>
{
 public:
  //: Constructor
  dbdet_keypoint_corr3d() {}
  //: Constructor
  dbdet_keypoint_corr3d(double x, double y, double z) : vgl_point_3d<double>(x,y,z) {}
  //: Destructor
  virtual ~dbdet_keypoint_corr3d() {}

  //: Return the keypoint that corresponds in the give view
  // returns a NULL keypoint if there is no correspondence
  dbdet_keypoint_sptr in_view(int view_index) const;

  //: Add a correspondence to the given keypoint in the given view
  //  if a corresponding point already exist in that view it is overwritten
  void add_correspondence(const dbdet_keypoint_sptr& kp, int view);
  
  //: Remove the correspondence in the given view if it exists
  // return true if successful
  bool remove_correspondence(int view);
  
  //: Merge correspondence from another keypoint with this one
  // only succeed if all existing correspondence agree
  // returns true if successful 
  bool merge(const dbdet_keypoint_corr3d& other);

  //: Return the number of correspondences in the map
  unsigned int size() const { return view_map_.size(); }

 
  //=============== Binary I/O Methods ========================
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbdet_keypoint_corr3d"; }

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual dbdet_keypoint_corr3d* clone() const;

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  virtual void print_summary(vcl_ostream &os) const;

 private:

  //: The map from each view (index) to the keypoint in that view
  vcl_map<int, dbdet_keypoint_sptr> view_map_;

};


//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, const dbdet_keypoint_corr3d* k);

//: Read a dbdet_keypoint_corr3d point from the stream
void vsl_b_read(vsl_b_istream &is, dbdet_keypoint_corr3d* &k);

//: Write a dbdet_keypoint_corr3d point to the stream
void vsl_b_write(vsl_b_ostream &os, const dbdet_keypoint_corr3d* k);



#endif // dbdet_keypoint_corr3d_h_
