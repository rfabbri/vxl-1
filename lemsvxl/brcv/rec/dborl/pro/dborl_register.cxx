#include "dborl_register.h"
#include "dborl_processes.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <dborl/dborl_exp_stat_sptr.h>
#include <dborl/dborl_category_info_sptr.h>

void dborl_register::register_datatype()
{
  REGISTER_DATATYPE( dborl_exp_stat_sptr );
  REGISTER_DATATYPE( dborl_category_info_set_sptr );
}

void dborl_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dborl_exp_stat_initialize_process, "dborlExpStatInitializeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dborl_exp_stat_print_process, "dborlExpStatPrintProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dborl_exp_stat_read_process, "dborlExpStatReadProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dborl_exp_stat_get_values_process, "dborlExpStatGetValuesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dborl_exp_stat_eval_classification_process, "dborlExpStatEvalClassificationProcess");
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dborl_category_info_set_create_process, "dborlCategoryInfoSetCreateProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dborl_category_info_set_write_xml_process, "dborlCategoryInfoSetWriteXMLProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dborl_exp_stat_eval_classification_using_id_process, "dborlExpStatEvalClassificationWIDProcess");  
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dborl_category_info_set_get_id_process, "dborlCategoryInfoGetIDProcess");
}
