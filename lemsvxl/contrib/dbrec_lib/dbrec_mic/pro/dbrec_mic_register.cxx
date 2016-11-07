#include "dbrec_mic_register.h"
#include "dbrec_mic_processes.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <dbrec_mic/dbrec_pca_utils.h>
#include <dbrec_mic/dbrec_pca_utils_sptr.h>

void dbrec_mic_register::register_datatype()
{
  REGISTER_DATATYPE( dbrec_pca_sptr );
  REGISTER_DATATYPE( dbrec_bayesian_pca_sptr );
  REGISTER_DATATYPE( dbrec_template_matching_sptr );
}

void dbrec_mic_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_pca_initialize_process, "dbrecPCAInitializeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_pca_add_data_process, "dbrecPCAAddDataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_pca_construct_process, "dbrecPCAConstructProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_pca_classify_process, "dbrecPCAClassifyProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_bayesian_pca_initialize_process, "dbrecBayesianPCAInitializeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_bayesian_pca_add_data_process, "dbrecBayesianPCAAddDataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_bayesian_pca_add_data_process2, "dbrecBayesianPCAAddDataProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_bayesian_pca_construct_process, "dbrecBayesianPCAConstructProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_bayesian_pca_visualize_process, "dbrecBayesianPCAVisualizeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_bayesian_pca_classify_process, "dbrecBayesianPCAClassifyProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_template_matching_classify_process, "dbrecTemplateMatchingClassifyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_template_matching_add_data_process2, "dbrecTemplateMatchingAddDataProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_template_matching_add_data_process, "dbrecTemplateMatchingAddDataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_template_matching_initialize_process, "dbrecTemplateMatchingInitializeProcess");
}
