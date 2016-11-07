#include "../psm_aux_scene.hxx"
#include "../psm_vis_implicit_sample.h"
#include "../psm_aux_traits.h"
#include <hsds/hsds_fd_tree_incremental_reader.hxx>

PSM_AUX_SCENE_INSTANTIATE(PSM_AUX_VIS_IMPLICIT);
HSDS_FD_TREE_INCREMENTAL_READER_INSTANTIATE(psm_aux_traits<PSM_AUX_VIS_IMPLICIT>::sample_datatype,3);

