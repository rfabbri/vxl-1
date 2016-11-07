#ifndef dbrec3d_processes_h_
#define dbrec3d_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

DECLARE_FUNC_CONS(dbrec3d_register_managers_process);
DECLARE_FUNC_CONS(dbrec3d_kernels_to_parts_process);
DECLARE_FUNC_CONS(dbrec3d_get_response_process);
DECLARE_FUNC_CONS(dbrec3d_save_vrml_process);
DECLARE_FUNC_CONS(dbrec3d_non_max_suppression_process);
DECLARE_FUNC_CONS(dbrec3d_find_pairs_process);
DECLARE_FUNC_CONS(dbrec3d_create_scene_process);
DECLARE_FUNC_CONS(dbrec3d_xml_write_process);
DECLARE_FUNC_CONS(dbrec3d_xml_parse_process);
DECLARE_FUNC_CONS(dbrec3d_save_vrml_process);

#endif
