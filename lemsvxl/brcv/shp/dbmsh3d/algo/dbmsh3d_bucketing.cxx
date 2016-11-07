// This is file shp/dbmsh3d/algo/dbmsh3d_bucketing.cxx

//:
// \file

#include "dbmsh3d_bucketing.h"
#include <vnl/vnl_math.h>


// ============================================================================
// dbmsh3d_face_bucket
// ============================================================================

// ----------------------------------------------------------------------------
// Utilities
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//: Add a face to ``this" bucket
bool dbmsh3d_face_bucket::
add_face(dbmsh3d_face* face)
{
  if (!face) return false;
  this->face_list_.insert(vcl_make_pair(face->id(), face)); 
  return true;
}


// ----------------------------------------------------------------------------
//: Remove a face from ``this" bucket
bool dbmsh3d_face_bucket::
remove_face(dbmsh3d_face* face)
{
  if (!face) return false;
  if (this->face_list_.erase(face->id()) > 0)
    return true;
  else
    return false;
}





// ============================================================================
// dbmsh3d_bucketing_face              
// ============================================================================


// ----------------------------------------------------------------------------
// Constructors / Destructors
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//: Constructor
dbmsh3d_bucketing_face::
dbmsh3d_bucketing_face(int ncols, int nrows, int nslabs,
                       const vgl_box_3d<double >& bbox)
{
  this->set_grid(ncols, nrows, nslabs, bbox);
}


// ----------------------------------------------------------------------------
//: Destructor
dbmsh3d_bucketing_face::
~dbmsh3d_bucketing_face()
{
}


// ----------------------------------------------------------------------------
//: set the grid structure
void dbmsh3d_bucketing_face::
set_grid(int ncols, int nrows, int nslabs,
         const vgl_box_3d<double >& bbox)
{
  this->bbox_ = bbox;
  this->bucket_array_.resize(ncols, nrows, nslabs);
  this->col_spacing_ = bbox.width() / ncols;
  this->row_spacing_ = bbox.height() / nrows;
  this->slab_spacing_ = bbox.depth() / nslabs;

  // locate memory for the buckets
  for (int col=0; col < ncols; ++col)
    for (int row=0; row < nrows; ++row)
      for (int slab=0; slab < nslabs; ++slab)
      {
        this->bucket_array_(col, row, slab) = new dbmsh3d_face_bucket(col, row, slab);
      
      }
  return;
}


// ----------------------------------------------------------------------------
//: Bucket given index
dbmsh3d_face_bucket_sptr dbmsh3d_bucketing_face::
get_bucket(int col, int row, int slab)
{
  // range check
  if (col < 0 || col >= this->ncols() ||
    row < 0 || row >= this->nrows() ||
    slab < 0 || slab >= this->nslabs()
    )
    return 0;

  return this->bucket_array_(col, row, slab);
}


// ----------------------------------------------------------------------------
//: Bucket given a point
dbmsh3d_face_bucket_sptr dbmsh3d_bucketing_face::
get_bucket(const vgl_point_3d<double >& pt)
{
  vgl_vector_3d<double > v = pt - this->origin();
  int col = (int) (v.x() / this->col_spacing());
  int row = (int) (v.y() / this->row_spacing());
  int slab = (int) (v.z() / this->slab_spacing());

  return this->get_bucket(col, row, slab);
}



// --------------------------------------------------------------------------
// Utilities
// --------------------------------------------------------------------------

//: Add a face to the buckets
void dbmsh3d_bucketing_face::
add(dbmsh3d_face* face)
{
  vgl_point_3d<double > origin = this->origin();
  for (unsigned i=0; i<face->vertices().size(); ++i)
  {
    const dbmsh3d_vertex* vertex = static_cast<const dbmsh3d_vertex* >
      (face->vertices(i));
    vgl_vector_3d<double > v = vertex->pt() - origin;
    if ( v.x() < 0 || v.y() < 0 || v.z() < 0)
      continue;

    int col = (int) (v.x() / this->col_spacing());
    int row = (int) (v.y() / this->row_spacing());
    int slab = (int) (v.z() / this->slab_spacing());

    if (col >= this->ncols() || row >= this->nrows() || slab >= this->nslabs())
      continue;

    this->get_bucket(col, row, slab)->add_face(face);  
  }
}


// ----------------------------------------------------------------------------
//: Take the faces from a mesh and put into the buckets
void dbmsh3d_bucketing_face::
add(dbmsh3d_mesh& mesh)
{
  mesh.reset_face_traversal();
  for (dbmsh3d_face* face = 0; mesh.next_face(face); )
  {
    this->add(face);
  }
}

