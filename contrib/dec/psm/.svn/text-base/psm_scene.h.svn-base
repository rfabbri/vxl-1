#ifndef psm_scene_h_
#define psm_scene_h_

#include <vcl_string.h>
#include <vcl_set.h>
#include <vul/vul_file.h>
#include <vgl/vgl_point_3d.h>

#include <bgeo/bgeo_lvcs.h>
#include <bgeo/bgeo_lvcs_sptr.h>

#include <hsds/hsds_fd_tree.h>

#include "psm_scene_base.h"
#include "psm_apm_traits.h"
#include "psm_aux_traits.h"
#include "psm_sample.h"
#include "psm_cell_id.h"

#include "psm_scene_block_storage.h"
#include "psm_scene_block_storage_single.h"
#include "psm_scene_block_storage_cached.h"
#include "psm_scene_block_storage_null.h"

#include "psm_aux_scene_base.h"
#include "psm_aux_scene.h"


template <psm_apm_type APM>
class psm_scene : public psm_scene_base
{
public:

  //: default constructor
  psm_scene() : block_storage_(new psm_scene_block_storage_null<psm_sample<APM> >()) {}

  //: destructor
  ~psm_scene() { delete block_storage_; }

  //: constructor
  psm_scene(vgl_point_3d<double> origin, double block_len, vcl_string storage_dir, bgeo_lvcs_sptr lvcs, unsigned int max_subdivision_levels = hsds_fd_tree_node_index<3>::MAX_LEVELS)
    : psm_scene_base(origin, block_len, lvcs), max_subdivision_levels_(max_subdivision_levels), block_storage_(new psm_scene_block_storage_single<psm_sample<APM> >(storage_dir)) 
  { 
    // discover blocks already on disk
    discover_blocks(); 
  }


  //: get a scene block
  hsds_fd_tree<psm_sample<APM> ,3>& get_block(vgl_point_3d<int> block_idx);

  //: get a cell
  psm_sample<APM>& get_cell(vgl_point_3d<int> block_idx, hsds_fd_tree_node_index<3> cell_idx);
  //: get a cell
  inline psm_sample<APM> &get_cell(psm_cell_id const& cell_id) { return get_cell(cell_id.block_idx_, cell_id.cell_idx_); }

  //: return the number of cells in the scene
  unsigned int num_cells();

  //: return the number of cells in the given block
  unsigned int num_cells(vgl_point_3d<int> block_idx);

  //: set a particular block to the specified data
  void set_block(vgl_point_3d<int> block_idx, psm_sample<APM>  data);

  //: set a particular block to the specified block
  void set_block(vgl_point_3d<int> block_idx, hsds_fd_tree<psm_sample<APM> ,3> &data);

  void init_block(vgl_point_3d<int> block_idx, unsigned int block_level, float init_alpha = 0.001);

  //: access storage dir
  vcl_string storage_dir(){return block_storage_->storage_directory();}

  //: return appearance model type
  virtual psm_apm_type appearance_model_type() const {return psm_sample<APM> ::apm_type;}

  template <psm_aux_type AUX_T> 
  psm_aux_scene_base_sptr get_aux_scene(vcl_string storage_suffix = "")
  {
    vcl_string aux_storage_dir(storage_dir() + "/" + psm_aux_traits<AUX_T>::storage_subdir() + storage_suffix);
    psm_aux_scene_base_sptr aux_sptr = new psm_aux_scene<AUX_T>(origin(), block_len(), aux_storage_dir);
    return aux_sptr;
  }

  template<psm_aux_type AUX_T>
  void init_aux_scene(vcl_string storage_suffix = "") 
  {
    vcl_string aux_storage_dir(storage_dir() + "/" + psm_aux_traits<AUX_T>::storage_subdir() + storage_suffix);
    vul_file::make_directory(aux_storage_dir);
    psm_aux_scene_base_sptr aux_scene_base = get_aux_scene<AUX_T>(storage_suffix);
    psm_aux_scene<AUX_T> *aux_scene = dynamic_cast<psm_aux_scene<AUX_T>*>(aux_scene_base.ptr());
    // loop through valid blocks and init same blocks in aux scene
    vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> >::iterator vbit = valid_blocks_.begin();
    for (; vbit != valid_blocks_.end(); ++vbit) {
      hsds_fd_tree<psm_sample<APM>,3> &block = this->get_block(*vbit);
      hsds_fd_tree<typename psm_aux_traits<AUX_T>::sample_datatype,3> aux_block = block.twin_tree(typename psm_aux_traits<AUX_T>::sample_datatype());
      aux_scene->set_block(*vbit,aux_block);
    }
    return;
  }

  void save_alpha_raw(vcl_string filename, vgl_point_3d<int> block_idx, unsigned int resolution_level);

private:
  //: discover already existing blocks on disk
  void discover_blocks();

  unsigned int max_subdivision_levels_;

  psm_scene_block_storage<psm_sample<APM> > *block_storage_;


};


#endif
