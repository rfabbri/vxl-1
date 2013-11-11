#include "psm_opt_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "psm_opt_processes.h"


void psm_opt_register::register_datatype()
{
}

void psm_opt_register::register_process()
{
  const bool null_func = 0;
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_opt_generate_opt_samples_process, "psmOptGenerateOptSamplesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_opt_lm_update_process, "psmOptLmUpdateProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_opt_bayesian_update_process, "psmOptBayesianUpdateProcess");
  
}
