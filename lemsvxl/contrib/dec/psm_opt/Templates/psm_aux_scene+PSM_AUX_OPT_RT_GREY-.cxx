#include <psm/psm_aux_traits.h>
#include "../psm_opt_rt_sample.h"
#include <psm/psm_aux_scene.hxx>
#include <hsds/hsds_fd_tree_incremental_reader.hxx>

PSM_AUX_SCENE_INSTANTIATE(PSM_AUX_OPT_RT_GREY);
HSDS_FD_TREE_INCREMENTAL_READER_INSTANTIATE(psm_aux_traits<PSM_AUX_OPT_RT_GREY>::sample_datatype,3);

