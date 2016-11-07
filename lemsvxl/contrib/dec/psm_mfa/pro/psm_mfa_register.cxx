#include "psm_mfa_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "psm_mfa_processes.h"


void psm_mfa_register::register_datatype()
{
}

void psm_mfa_register::register_process()
{
  const bool null_func = 0;
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_mfa_optimizer_process, "psmMfaOptimizerProcess");

}
