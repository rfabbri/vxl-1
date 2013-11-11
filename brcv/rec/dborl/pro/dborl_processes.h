#ifndef dborl_processes_h_
#define dborl_processes_h_

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

DECLARE_FUNC_CONS(dborl_exp_stat_initialize_process);
DECLARE_FUNC_CONS(dborl_exp_stat_print_process);
DECLARE_FUNC_CONS(dborl_exp_stat_read_process);
DECLARE_FUNC_CONS(dborl_exp_stat_eval_classification_process);

DECLARE_FUNC_CONS(dborl_category_info_set_create_process);
DECLARE_FUNC_CONS(dborl_category_info_set_write_xml_process);
DECLARE_FUNC_CONS(dborl_exp_stat_eval_classification_using_id_process);  
DECLARE_FUNC_CONS(dborl_exp_stat_get_values_process);

DECLARE_FUNC_CONS(dborl_category_info_set_get_id_process);


#endif  // dborl_processes_h_

