#include "dbru_register.h"
#include "dbru_processes.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <dbru/dbru_osl_sptr.h>

void dbru_register::register_datatype()
{
  REGISTER_DATATYPE( dbru_osl_sptr );
}

void dbru_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbru_load_objects_process, "dbruLoadObjectsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbru_create_change_object_process, "dbruCreateChangeObjectProcess");
}
