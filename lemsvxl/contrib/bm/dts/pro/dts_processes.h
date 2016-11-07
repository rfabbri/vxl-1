// this is /contrib/bm/dts/pro/dts_processes.h
#ifndef DTS_PROCESSES_H_
#define DTS_PROCESSES_H_

#include<bprb/bprb_func_process.h>
#include<bprb/bprb_macros.h>

//INIT FUNCTIONS
DECLARE_FUNC_CONS( dts_pixel_time_series_create_sptr_process );
DECLARE_FUNC_CONS( dts_pixel_time_series_extract_sift_video_box_process );
DECLARE_FUNC_CONS( dts_pixel_time_series_pca_process );
DECLARE_FUNC_CONS( dts_pixel_time_series_extract_sift_video_pca_process );

DECLARE_FUNC_CONS( dts_pixel_time_series_write_bin_process );
DECLARE_FUNC_CONS( dts_pixel_time_series_read_bin_process );
DECLARE_FUNC_CONS( dts_pixel_time_series_write_txt_process );




#endif //DTS_PROCESSES_H_