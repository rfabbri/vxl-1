#include "bbgm_register.h"
#include "bbgm_processes.h"
#include <dbbgm/bbgm_image_sptr.h>
#include <dbbgm/bbgm_wavelet_sptr.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>


void bbgm_register::register_datatype()
{
  REGISTER_DATATYPE( bbgm_image_sptr );
  REGISTER_DATATYPE( bbgm_wavelet_sptr );
}

void bbgm_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_load_image_of_process, "bbgmLoadImageOfProcess");
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_save_image_of_process, "bbgmSaveImageOfProcess");

  REG_PROCESS_FUNC_CONS_INIT(bprb_func_process, bprb_batch_process_manager, bbgm_update_dist_image_process, "bbgmUpdateDistImageProcess");

  REG_PROCESS_FUNC_CONS_INIT(bprb_func_process, bprb_batch_process_manager, bbgm_update_dist_image_stream_process, "bbgmUpdateDistImageStreamProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_display_dist_image_process, "bbgmDisplayDistImageProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_local_frame_trans_process, "bbgmLocalFrameTransProcess");

  REG_PROCESS_FUNC_CONS_INIT(bprb_func_process, bprb_batch_process_manager, bbgm_update_parzen_dist_image_process, "bbgmUpdateParzenDistImageProcess");
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_measure_process, "bbgmMeasureProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_measure_wvlookup_process, "bbgmMeasureWvLookupProcess");

  REG_PROCESS_FUNC(bprb_func_process, bprb_batch_process_manager, bbgm_wavelet_xform_process, "bbgmWaveletXformProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_wavelet_utils_process, "bbgmWaveletUtilsProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_invoke_gui_process, "bbgmInvokeGUIProcess");
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_load_wavelet_process, "bbgmLoadWaveletProcess");
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_save_wavelet_process, "bbgmSaveWaveletProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_ROC_process, "bbgmROCProcess");
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_select_mixture_averaging_process, "bbgmSelectMixtureAveragingProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_thresh_and_KDU_encode_process, "bbgmThreshAndEncodeProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,  bbgm_KDU_decompress_and_glue_process, "bbgmDecompressAndGlueProcess");

  	
  
}




