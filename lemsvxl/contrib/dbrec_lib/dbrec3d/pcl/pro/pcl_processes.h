#ifndef bof_processes_h_
#define bof_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

DECLARE_FUNC_CONS(pcl_convert_scene_to_pc_process);
DECLARE_FUNC_CONS(pcl_compute_fphf_process);
DECLARE_FUNC_CONS(pcl_k_means_assign_process);
DECLARE_FUNC_CONS(pcl_k_means_learning_process);
DECLARE_FUNC_CONS(pcl_convert_id_to_rgb_process);
DECLARE_FUNC_CONS(pcl_trasform_xy_align_process);
DECLARE_FUNC_CONS(pcl_read_and_filter_ply_normals_process);

#endif
