#include "psm_algo_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "psm_algo_processes.h"


void psm_algo_register::register_datatype()
{
}

void psm_algo_register::register_process()
{
  const bool null_func = 0;
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_condense_scene_process, "psmCondenseSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_convert_vis_implicit_process, "psmConvertVisImplicitProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_pixel_probability_density_process, "psmPixelProbabilityDensityProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_pixel_probability_range_process, "psmPixelProbabilityRangeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_refine_scene_process, "psmRefineSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_render_expected_process, "psmRenderExpectedProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_update_process, "psmUpdateProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_update_vis_implicit_process, "psmUpdateVisImplicitProcess");
  //REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_convert_to_boct_process, "psmConvertToBoctProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_clean_unobserved_process, "psmCleanUnobservedProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_compute_expected_depth_process, "psmComputeExpectedDepthProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_ray_probe_process, "psmRayProbeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_sample_backprojection_process, "psmSampleBackprojectionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_update_dcdf_implicit_process, "psmUpdateDcdfImplicitProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_convert_dcdf_implicit_process, "psmConvertDcdfImplicitProcess");

  
}
