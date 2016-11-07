#include "pcl_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "pcl_processes.h"

void pcl_register::register_datatype()
{
//  REGISTER_DATATYPE(bof_feature_vector_sptr);

}

void pcl_register::register_process()
{
  REG_PROCESS_FUNC_CONS2(pcl_convert_scene_to_pc_process);
  REG_PROCESS_FUNC_CONS2(pcl_compute_fphf_process);
  REG_PROCESS_FUNC_CONS2(pcl_k_means_assign_process);
  REG_PROCESS_FUNC_CONS2(pcl_k_means_learning_process);
  REG_PROCESS_FUNC_CONS2(pcl_convert_id_to_rgb_process);
  REG_PROCESS_FUNC_CONS2(pcl_trasform_xy_align_process);
  REG_PROCESS_FUNC_CONS2(pcl_read_and_filter_ply_normals_process);

} 

