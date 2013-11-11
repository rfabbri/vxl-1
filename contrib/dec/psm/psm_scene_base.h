#ifndef psm_scene_base_h_
#define psm_scene_base_h_

#include <vcl_set.h>
#include <vbl/vbl_bounding_box.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

#include <vgl/vgl_point_3d.h>

#include <bgeo/bgeo_lvcs.h>
#include <bgeo/bgeo_lvcs_sptr.h>

#include "psm_cell_id.h"
#include "psm_apm_traits.h"


//: The untemplated psm_scene base class.
class psm_scene_base : public vbl_ref_count
{
public:

  //: return the block index of a 3-d point
  vgl_point_3d<int> block_index(vgl_point_3d<double> pt) const;

  //: return true if the specified block should be populated.
  bool valid_block(vgl_point_3d<int> block_idx) const;

  //: return the bounding box of a block
  vbl_bounding_box<double,3> block_bounding_box(vgl_point_3d<int> block_idx) const;

  //: return the bounding box of a cell
  vbl_bounding_box<double,3> cell_bounding_box(psm_cell_id const& cell_id) const;

  //: access origin
  vgl_point_3d<double> origin() const {return origin_;}

  //: access block len
  double block_len() const {return block_len_;}

  //: access valid blocks
  vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> > valid_blocks(){return valid_blocks_;}

  // Operators that allow psm_scene's to be placed in a brdb database

  //: equality operator
  bool operator == (psm_scene_base const& that) const;

  //: less than operator
  bool operator < (psm_scene_base const& that) const;

  //: return the appearance model type
  virtual psm_apm_type appearance_model_type() const = 0;

  typedef vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> >::iterator block_index_iterator;
  block_index_iterator block_index_begin() { return valid_blocks_.begin(); }
  block_index_iterator block_index_end() { return valid_blocks_.end(); }

protected:

  //: set a particular block to valid or invalid
  void set_block_valid(vgl_point_3d<int> block_idx, bool isvalid);
  
  //: default constuctor
  psm_scene_base() {}

  //: standard constructor
  psm_scene_base(vgl_point_3d<double> origin, double block_len, bgeo_lvcs_sptr lvcs = bgeo_lvcs_sptr()) : origin_(origin), block_len_(block_len), lvcs_(lvcs) {}

  vgl_point_3d<double> origin_;
  double block_len_;
  bgeo_lvcs_sptr lvcs_;

  vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> > valid_blocks_;

};

typedef vbl_smart_ptr<psm_scene_base> psm_scene_base_sptr;

//: output description of voxel world to stream.
vcl_ostream&  operator<<(vcl_ostream& s, psm_scene_base const& scene);


#endif
