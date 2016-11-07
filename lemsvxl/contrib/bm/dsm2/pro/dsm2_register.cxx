//this is /contrib/bm/dsm2/pro/dsm2_register.cxx
#include<bprb/bprb_macros.h>
#include<bprb/bprb_batch_process_manager.h>
#include<bprb/bprb_func_process.h>

#include <dsm2/pro/dsm2_register.h>
#include <dsm2/pro/dsm2_processes.h>

#include <dsm2/dsm2_manager_base_sptr.h>


void dsm2_register::register_datatype()
{
	REGISTER_DATATYPE(dsm2_manager_base_sptr);
}//end dsm2_register::register_datatype

void dsm2_register::register_process()
{

}//end dsm2_register::register_process
