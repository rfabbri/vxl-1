#include "dbfs_register.h"
#include "dbfs_processes.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <dbfs/dbfs_rect_feature_sptr.h>

void dbfs_register::register_datatype()
{
  REGISTER_DATATYPE( dbfs_rect_feature_set_sptr );
}

void dbfs_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbfs_create_rect_features_process, "dbfsCreateRectFeatureSetProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbfs_populate_rect_features_process, "dbfsPopulateRectFeaturesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbfs_display_rect_features_process, "dbfsDisplayRectFeaturesProcess");
}
