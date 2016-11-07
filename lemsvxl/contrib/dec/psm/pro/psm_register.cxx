#include "psm_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <psm/psm_scene_base.h>

#include "psm_processes.h"


void psm_register::register_datatype()
{
  REGISTER_DATATYPE( psm_scene_base_sptr );
}

void psm_register::register_process()
{
  const bool null_func = 0;
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_load_scene_process, "psmLoadSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_init_block_process, "psmInitBlockProcess");  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_init_vis_implicit_aux_scene_process, "psmInitVisImplicitAuxSceneProcess"); 
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_save_alpha_raw_process, "psmSaveAlphaRawProcess");  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_init_dcdf_implicit_aux_scene_process, "psmInitDcdfImplicitAuxSceneProcess"); 
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_mog_to_simple_process, "psmMogToSimpleProcess");
  
}
