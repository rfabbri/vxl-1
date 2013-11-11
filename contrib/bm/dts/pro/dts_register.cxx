//this is /contrib/bm/dts/pro/dts_register.cxx
#include<bprb/bprb_macros.h>
#include<bprb/bprb_batch_process_manager.h>
#include<bprb/bprb_func_process.h>

#include<dts/pro/dts_register.h>
#include<dts/pro/dts_processes.h>

#include<dts/dts_time_series_base_sptr.h>
#include<dts/dts_pixel_time_series_base_sptr.h>

void dts_register::register_datatype()
{
    REGISTER_DATATYPE(dts_time_series_base_sptr);
    REGISTER_DATATYPE(dts_pixel_time_series_base_sptr);
    return;
}

void dts_register::register_process()
{
	REG_PROCESS_FUNC_CONS( bprb_func_process,
                           bprb_batch_process_manager,
                           dts_pixel_time_series_create_sptr_process,
                           "dtsPixelTimeSeriesCreateSptrProcess");

    REG_PROCESS_FUNC_CONS( bprb_func_process,
                           bprb_batch_process_manager,
                           dts_pixel_time_series_extract_sift_video_box_process,
                           "dtsPixelTimeSeriesExtractSiftVideoBoxProcess");

	REG_PROCESS_FUNC_CONS( bprb_func_process,
                           bprb_batch_process_manager,
                           dts_pixel_time_series_extract_sift_video_pca_process,
                           "dtsPixelTimeSeriesExtractSiftVideoPcaProcess");

	REG_PROCESS_FUNC_CONS( bprb_func_process,
                           bprb_batch_process_manager,
                           dts_pixel_time_series_pca_process,
                           "dtsPixelTimeSeriesPcaProcess");

    REG_PROCESS_FUNC_CONS( bprb_func_process,
                           bprb_batch_process_manager,
                           dts_pixel_time_series_write_bin_process,
                           "dtsPixelTimeSeriesWriteBinProcess");

    REG_PROCESS_FUNC_CONS( bprb_func_process,
                           bprb_batch_process_manager,
                           dts_pixel_time_series_read_bin_process,
                           "dtsPixelTimeSeriesReadBinProcess");

	REG_PROCESS_FUNC_CONS( bprb_func_process,
                           bprb_batch_process_manager,
                           dts_pixel_time_series_write_txt_process,
                           "dtsPixelTimeSeriesWriteTxtProcess");

    return;
}
