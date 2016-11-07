#ifndef psm_cell_id_h_
#define psm_cell_id_h_

#include <vgl/vgl_point_3d.h>
#include <hsds/hsds_fd_tree_node_index.h>

//: A comparison functor for vgl_point_3d's. Needed to create a vcl_set of vgl_point_3d<int>'s.
template <class T>
class vgl_point_3d_cmp : public vcl_binary_function<vgl_point_3d<T>, vgl_point_3d<T>, bool>
{
public:
  vgl_point_3d_cmp(){}

  bool operator()(vgl_point_3d<T> const& v0, vgl_point_3d<T> const& v1) const
  {
    if (v0.z() != v1.z())
      return (v0.z() < v1.z());
    else if (v0.y() != v1.y())
      return (v0.y() < v1.y());
    else
      return (v0.x() < v1.x());
  }
};


class psm_cell_id
{
public:
  psm_cell_id(vgl_point_3d<int> const& block_idx, hsds_fd_tree_node_index<3> const& cell_idx) 
    : block_idx_(block_idx), cell_idx_(cell_idx) {}

  vgl_point_3d<int> block_idx_;
  hsds_fd_tree_node_index<3> cell_idx_;

  //: equality operator
  bool operator == (psm_cell_id const& that) const { 
    return( (this->block_idx_ == that.block_idx_) && (this->cell_idx_ == that.cell_idx_) ); 
  }

  //: less than operator
  bool operator < (psm_cell_id const& that) const {   
    if (this->block_idx_ == that.block_idx_) {
      return(this->cell_idx_ < that.cell_idx_);
    }
    else {
      vgl_point_3d_cmp<int> p3d_cmp;
      return p3d_cmp(this->block_idx_, that.block_idx_);
    }
  }

};


#endif

