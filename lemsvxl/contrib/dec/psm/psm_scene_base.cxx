#include <vcl_set.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>

#include <hsds/hsds_fd_tree.h>

#include <vgl/vgl_point_3d.h>
#include <vbl/vbl_bounding_box.h>

#include "psm_cell_id.h"
#include "psm_scene_base.h"


//: set a particular block to valid or invalid
void psm_scene_base::set_block_valid(vgl_point_3d<int> block_idx, bool isvalid)
{
  if (isvalid) {
    valid_blocks_.insert(block_idx);
  }
  else {
    valid_blocks_.erase(block_idx);
  }
}


vgl_point_3d<int> psm_scene_base::block_index(vgl_point_3d<double> pt) const
{
  vgl_vector_3d<double> pt_norm = (pt - origin_) / block_len_;
  return vgl_point_3d<int>((int)vcl_floor(pt_norm.x()),(int)vcl_floor(pt_norm.y()),(int)vcl_floor(pt_norm.z()));
}

//: return true if the specified block should be populated
bool psm_scene_base::valid_block(vgl_point_3d<int> block_idx) const
{
  vcl_set<vgl_point_3d<int>,vgl_point_3d_cmp<int> >::const_iterator vbit = valid_blocks_.find(block_idx);
  return (vbit != valid_blocks_.end());
}

//: return the bounding box of a block
vbl_bounding_box<double,3> psm_scene_base::block_bounding_box(vgl_point_3d<int> block_idx) const
{
  vgl_point_3d<double> block_origin = origin_ + (vgl_vector_3d<double>(block_idx.x(),block_idx.y(),block_idx.z())*block_len_);
  vbl_bounding_box<double,3> block_bb;
  block_bb.update(block_origin.x(),block_origin.y(),block_origin.z());
  block_bb.update(block_origin.x()+block_len_, block_origin.y()+block_len_, block_origin.z()+block_len_);
  
  return block_bb;
}

//: return the bounding box of a cell
vbl_bounding_box<double,3> psm_scene_base::cell_bounding_box(psm_cell_id const& cell_id) const
{
  // use float as datatype of tree since we know it exists (does not affect bounding box computation)
  return hsds_fd_tree<float,3>::cell_bounding_box(block_bounding_box(cell_id.block_idx_), cell_id.cell_idx_);
}


  //: equality operator
bool psm_scene_base::operator == (psm_scene_base const& that) const
{
  bool equal = true;
  equal &= (this->block_len_ == that.block_len());
  equal &= (this->origin_.x() == that.origin().x());
  equal &= (this->origin_.y() == that.origin().y());
  equal &= (this->origin_.z() == that.origin().z());
  return equal;

}

  //: less than operator
bool psm_scene_base::operator < (psm_scene_base const& that) const
{
  vgl_point_3d_cmp<double> p3d_cmp;
  return p3d_cmp(this->origin_, that.origin());
}

//: output description of voxel world to stream.
vcl_ostream&  operator<<(vcl_ostream& s, psm_scene_base const& scene)
{
  s << "psm_scene_base: origin = " << scene.origin() <<", block_len = " << scene.block_len();

  return s;
}

