#include <psm/psm_apm_traits.h>
#include <vil/vil_rgb.h>
#include <psm/psm_scene_block_storage_single.txx>
#include "../psm_opt_rt_sample.h"

PSM_SCENE_BLOCK_STORAGE_SINGLE_INSTANTIATE(psm_opt_rt_sample<vil_rgb<float> >);
