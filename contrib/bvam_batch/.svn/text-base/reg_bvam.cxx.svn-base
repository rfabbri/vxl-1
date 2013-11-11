#include "batch_bvam.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
// processes
#include <vil_pro/vil_load_image_view_process.h>
#include <vil_pro/vil_save_image_view_process.h>
#include <vpgl_pro/vpgl_load_proj_camera_process.h>
#include <vpgl_pro/vpgl_load_perspective_camera_process.h>
#include <vpgl_pro/vpgl_save_perspective_camera_process.h>
#include <vpgl_pro/vpgl_load_rational_camera_process.h>
#include <vpgl_pro/vpgl_load_rational_camera_nitf_process.h>
#include <bvam/pro/bvam_create_voxel_world_process.h>
#include <bvam/pro/bvam_update_process.h>
#include <bvam/pro/bvam_detect_changes_process.h>
#include <bvam/pro/bvam_render_virtual_view_process.h>
#include <bvam/pro/bvam_render_expected_image_process.h>
#include <bvam/pro/bvam_roi_init_process.h>
#include <bvam/pro/bvam_normalize_image_process.h>
#include <bvam/pro/bvam_illum_index_process.h>
#include <bvam/pro/bvam_clean_world_process.h>

#include <bvam/algo/pro/bvam_ekf_camera_optimize_process.h>
#include <bvam/algo/pro/bvam_init_ekf_camera_optimizer_process.h>
#include <bvam/pro/bvam_rpc_registration_process.h>
#include <bvam/pro/bvam_generate_edge_map_process.h>
#include <bvam/pro/bvam_save_occupancy_raw_process.h>

// datatypes
#include <vcl_string.h>
#include <vil/vil_image_view_base.h>
#include <bvam/bvam_voxel_world.h>
#include <vpgl/vpgl_camera.h>
#include <bvam/algo/bvam_ekf_camera_optimizer_state.h>


PyObject *
register_processes(PyObject *self, PyObject *args)
{
  REG_PROCESS(vil_load_image_view_process, bprb_batch_process_manager);
  REG_PROCESS(vil_save_image_view_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_rational_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_rational_camera_nitf_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_proj_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_perspective_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_save_perspective_camera_process, bprb_batch_process_manager);
  REG_PROCESS(bvam_create_voxel_world_process, bprb_batch_process_manager);
  REG_PROCESS(bvam_update_process, bprb_batch_process_manager);
  REG_PROCESS(bvam_detect_changes_process, bprb_batch_process_manager);
  REG_PROCESS(bvam_render_virtual_view_process, bprb_batch_process_manager);
  REG_PROCESS(bvam_render_expected_image_process,bprb_batch_process_manager);
  REG_PROCESS(bvam_roi_init_process,bprb_batch_process_manager);
  REG_PROCESS(bvam_normalize_image_process, bprb_batch_process_manager);
  REG_PROCESS(bvam_illum_index_process, bprb_batch_process_manager);
  REG_PROCESS(bvam_rpc_registration_process, bprb_batch_process_manager);
  REG_PROCESS(bvam_generate_edge_map_process, bprb_batch_process_manager);
  REG_PROCESS(bvam_ekf_camera_optimize_process, bprb_batch_process_manager);
  REG_PROCESS(bvam_init_ekf_camera_optimizer_process, bprb_batch_process_manager);
  REG_PROCESS(bvam_save_occupancy_raw_process, bprb_batch_process_manager);
  REG_PROCESS(bvam_clean_world_process, bprb_batch_process_manager);
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  REGISTER_DATATYPE(bool);
  REGISTER_DATATYPE(vcl_string);
  REGISTER_DATATYPE(int);
  REGISTER_DATATYPE(long);
  REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(double);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  typedef vbl_smart_ptr<vpgl_camera<double> > vpgl_camera_double_sptr;
  REGISTER_DATATYPE( vpgl_camera_double_sptr );
  REGISTER_DATATYPE( bvam_voxel_world_sptr );
  REGISTER_DATATYPE( bvam_ekf_camera_optimizer_state );
  Py_INCREF(Py_None);
  return Py_None;
}
