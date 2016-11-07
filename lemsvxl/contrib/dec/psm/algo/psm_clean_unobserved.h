//This is lemsvxl/contrib/dec/psm/algo/psm_clean_unobserved.cxx
//:
// \file
// \brief A function which sets the probability to zero of cells not observed by enough cameras
// \author Daniel Crispell
// \date 04/30/2009
//
// \verbatim
//  Modifications
//    
// \endverbatim


#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_point_3d.h>

#include <hsds/hsds_fd_tree.h>
#include <psm/psm_scene_base.h>
#include <psm/psm_scene.h>
#include <psm/psm_aux_scene_base.h>
#include <psm/psm_aux_scene.h>
#include <psm/psm_vis_implicit_sample.h>



//: clean the scene by removing cells with not enough observations
template<psm_apm_type APM>
bool psm_clean_unobserved(psm_scene<APM> &scene, unsigned int min_obs, float min_maxvis)
{   

  psm_aux_scene_base_sptr aux_scene_base;

  aux_scene_base = scene.template get_aux_scene<PSM_AUX_VIS_IMPLICIT>();

  psm_aux_scene<PSM_AUX_VIS_IMPLICIT> *aux_scene = dynamic_cast<psm_aux_scene<PSM_AUX_VIS_IMPLICIT>*>(aux_scene_base.ptr());
  if (!aux_scene) {
    vcl_cerr << "error casting aux_scene to appropriate type. " << vcl_endl;
    return false;
  }
  vcl_cout << "cleaning scene.. min obs = " << min_obs << ", min_maxvis = " << min_maxvis << vcl_endl;

  vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> > valid_blocks = scene.valid_blocks();
  vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> >::iterator vbit = valid_blocks.begin();
  for (; vbit != valid_blocks.end(); ++vbit) {

    hsds_fd_tree<psm_vis_implicit_sample,3> &aux_block = aux_scene->get_block(*vbit);
    hsds_fd_tree<psm_sample<APM>,3> &block = scene.get_block(*vbit);

    hsds_fd_tree<psm_vis_implicit_sample,3>::const_iterator aux_block_it = aux_block.begin();
    typename hsds_fd_tree<psm_sample<APM>,3>::iterator block_it = block.begin();

    for (; block_it != block.end(); ++block_it, ++aux_block_it) {
      if ( (aux_block_it->second.n_obs < min_obs) || (aux_block_it->second.max_vis < min_maxvis) ) {
        block_it->second.alpha = 0;
      }
    }
  }

  return true;
}

