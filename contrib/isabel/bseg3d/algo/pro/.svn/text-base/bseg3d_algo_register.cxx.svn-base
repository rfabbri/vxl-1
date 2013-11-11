#include "bseg3d_algo_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>


#include "bseg3d_algo_processes.h"


void bseg3d_algo_register::register_datatype()
{
  
}

void bseg3d_algo_register::register_process()
{
  const bool null_func = 0;
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bseg3d_threshold_world_process, "bseg3dThresholdWorldProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bseg3d_l2distance_process, "bseg3dL2DistanceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bseg3d_save_float_grid_process, "bseg3dSaveFloatGridProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bseg3d_merge_mixtures_process, "bseg3dMergeMixturesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bseg3d_multiply_grids_process, "bseg3dMultiplyGridsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bseg3d_crop_grid_process,"bseg3dCropGridProcess");
}
