// This is shp/dbmsh3d/algo/dbmsh3d_bucketing.h
#ifndef dbmsh3d_bucketing_h_
#define dbmsh3d_bucketing_h_

//:
// \file
// \brief Classes for putting mesh elements in buckets for easy neighborhood check
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Feb 14, 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include <vgl/vgl_box_3d.h>
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_ref_count.h>
#include <dbmsh3d/dbmsh3d_mesh.h>

#include <dbmsh3d/algo/dbmsh3d_bucketing_sptr.h>

//: Base class for bucketting mesh elements
class dbmsh3d_bucketing_base
{
public:
  dbmsh3d_bucketing_base(){}
  ~dbmsh3d_bucketing_base(){}
protected:

};



// ============================================================================
// dbmsh3d_face_bucket
// ============================================================================
//: A bucket containing faces
class dbmsh3d_face_bucket : public vbl_ref_count
{
public:
  dbmsh3d_face_bucket(int col, int row, int slab)
  {
    this->set_index(col, row, slab);
  };
  ~dbmsh3d_face_bucket(){};

  // --------------------------------------------------------------------------
  // Utilities
  // --------------------------------------------------------------------------
  
  //: Add a face to ``this" bucket
  bool add_face(dbmsh3d_face* face);

  //: Remove a face from ``this" bucket
  bool remove_face(dbmsh3d_face* face);

  //: Clear the face list
  void clear()
  { this->face_list_.clear(); }

  //: column index
  int col() const { return this->index_[0]; }

  //: row index
  int row() const { return this->index_[1]; }
  
  //: slab index
  int slab() const { return this->index_[2]; }

  //: set index
  void set_index(int col, int row, int slab)
  { this->index_[0] = col; this->index_[1] = row; this->index_[2] = slab; }

  const vcl_map<int, dbmsh3d_face* >& face_list() const
  {return this->face_list_; }
  


protected:
  int index_[3];
  vcl_map<int, dbmsh3d_face* > face_list_;

};


//: Class to put faces into buckets and retrieve neighborhood faces
class dbmsh3d_bucketing_face
{
  // --------------------------------------------------------------------------
  // Constructors / Destructors
  // --------------------------------------------------------------------------
public:
  //: Constructor
  dbmsh3d_bucketing_face(int ncols, int nrows, int nslabs,
    const vgl_box_3d<double >& bbox);

  //: Destructor
  ~dbmsh3d_bucketing_face();

  //: Clean up all the faces (the grid structure remains intact)
  void clear(){}

  // --------------------------------------------------------------------------
  // Data Access
  // --------------------------------------------------------------------------
  
  // grid size
  int ncols() const { return this->bucket_array_.get_row1_count(); }
  int nrows() const { return this->bucket_array_.get_row2_count(); }
  int nslabs() const { return this->bucket_array_.get_row3_count(); }
  
  // voxel size
  double row_spacing() const { return this->row_spacing_; }
  double col_spacing() const { return this->col_spacing_; }
  double slab_spacing() const { return this->slab_spacing_; }

  // origin
  vgl_point_3d<double > origin() const {return this->bbox_.min_point(); }

  //: set the grid structure
  void set_grid(int ncols, int nrows, int nslabs,
    const vgl_box_3d<double >& bbox);

  //: Bucket given index
  dbmsh3d_face_bucket_sptr get_bucket(int col, int row, int slab);

  //: Bucket given a point
  dbmsh3d_face_bucket_sptr get_bucket(const vgl_point_3d<double >& pt);

  


  // --------------------------------------------------------------------------
  // Utilities
  // --------------------------------------------------------------------------

  //: Add a face to the buckets
  // Add ``face" to every bucket that its vertices belongs to
  void add(dbmsh3d_face* face);

  //: Take the faces from a mesh and put into the buckets
  void add(dbmsh3d_mesh& mesh);


  
protected:
   double col_spacing_;
   double row_spacing_;//cell dimensions
   double slab_spacing_;
   vgl_box_3d<double > bbox_; //bounding box for the array

   //: the buckets containg the faces
   vbl_array_3d<dbmsh3d_face_bucket_sptr > bucket_array_;
};




#endif // shp/dbmsh3d/algo/dbmsh3d_bucketing.h


