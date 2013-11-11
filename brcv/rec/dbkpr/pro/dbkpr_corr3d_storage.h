// This is brcv/rec/dbkpr/pro/dbkpr_corr3d_storage.h
#ifndef dbkpr_corr3d_storage_h_
#define dbkpr_corr3d_storage_h_

//:
// \file
// \brief The vidpro1 storage class for keypoint correspondences.
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 4/25/05
//
//
// \verbatim
//  Modifications 12/17/06 Ricardo Fabbri     Changed storage to pointer to projective camera 
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include <vcl_vector.h>
#include <dbdet/dbdet_keypoint_corr3d_sptr.h>
#include <dbkpr/pro/dbkpr_corr3d_storage_sptr.h>
#include <dbkpr/dbkpr_view_span_tree_sptr.h>
#include <dvpgl/pro/dvpgl_camera_storage.h>

//: Storage class for keypoints 3d correspondences, 3D points, cameras, and image sizes.
//
// \remarks Perhaps some of this should be a global storage. The only frame-specific things here are the
// number of rows/cols and the camera.
//
class dbkpr_corr3d_storage : public dvpgl_camera_storage {
public:

  //: Constructor
  dbkpr_corr3d_storage(){}
  //: Constructor
  dbkpr_corr3d_storage( const vcl_vector< dbdet_keypoint_corr3d_sptr >& corr, int ni, int nj);
  //: Destructor
  virtual ~dbkpr_corr3d_storage() {}

  //: Returns the type string "keypoint_corr3d"
  virtual vcl_string type() const { return "keypoints_corr3d"; }

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbkpr_corr3d_storage"; }

  //: Set the correspondences storage vector
  void set_correspondences( const vcl_vector< dbdet_keypoint_corr3d_sptr >& corr);

  //: Retrieve the correspondences storage vector
  const vcl_vector< dbdet_keypoint_corr3d_sptr >& correspondences() const { return keypoints_3d_; }

  //: Return the width
  int ni() const { return ni_; }

  //: Return the height
  int nj() const { return nj_; }

  //: Has view spanning tree?
  bool has_tree() const {return tree_ != 0; }

  //: Set view spanning tree. User must alloc it; we will delete it
  void set_tree(dbkpr_view_span_tree_sptr &ptr) {tree_ = ptr;}

  dbkpr_view_span_tree_sptr tree() {return tree_;}

private:
  //: The vector of lowe keypoint smart pointers
  vcl_vector< dbdet_keypoint_corr3d_sptr > keypoints_3d_;

  //: The width of the image that contains the keypoints
  int ni_;
  //: The height of the image that contains the keypoints
  int nj_;

  dbkpr_view_span_tree_sptr tree_;
};


//: Create a smart-pointer to a dbkpr_corr3d_storage.
struct dbkpr_corr3d_storage_new : public dbkpr_corr3d_storage_sptr
{
  typedef dbkpr_corr3d_storage_sptr base;

  //: Constructor - creates a default dbkpr_corr3d_storage_sptr.
  dbkpr_corr3d_storage_new() : base(new dbkpr_corr3d_storage()) { }

  //: Constructor - creates a dbkpr_corr3d_storage_sptr with keypoints.
  dbkpr_corr3d_storage_new(const vcl_vector< dbdet_keypoint_corr3d_sptr >& corr, int ni, int nj)
   : base(new dbkpr_corr3d_storage( corr, ni, nj )) { }
};


#endif //dbkpr_corr3d_storage_h_
