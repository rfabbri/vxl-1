#include "dbrec3d_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include "dbrec3d_processes.h"


void dbrec3d_register::register_datatype()
{
}

void dbrec3d_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec3d_register_managers_process, "dbrec3dRegisterManagersProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec3d_kernels_to_parts_process, "dbrec3dKernelsToPartsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec3d_get_response_process, "dbrec3dGetResponseProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec3d_save_vrml_process, "dbrec3dSaveVRMLProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec3d_non_max_suppression_process, "dbrec3dNonMaxSuppressionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec3d_find_pairs_process, "dbrec3dFindPairsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec3d_create_scene_process, "dbrec3dCreateSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec3d_xml_write_process, "dbrec3dXmlWriteProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec3d_xml_parse_process, "dbrec3dXmlParseProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec3d_save_vrml_process, "dbrec3dSaveVrmlProcess");

}
