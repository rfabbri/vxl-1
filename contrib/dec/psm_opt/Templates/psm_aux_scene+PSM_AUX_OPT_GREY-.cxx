#include <psm/psm_aux_traits.h>
#include "../psm_opt_sample.h"
#include <psm/psm_aux_scene.txx>
#include <hsds/hsds_fd_tree_incremental_reader.txx>

PSM_AUX_SCENE_INSTANTIATE(PSM_AUX_OPT_GREY);
HSDS_FD_TREE_INCREMENTAL_READER_INSTANTIATE(psm_aux_traits<PSM_AUX_OPT_GREY>::sample_datatype,3);

