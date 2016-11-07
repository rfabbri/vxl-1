//this is /contrib/bm/dts/dts_pixel_time_series_base_sptr.h
#ifndef DTS_PIXEL_TIME_SERIES_BASE_SPTR_H_
#define DTS_PIXEL_TIME_SERIES_BASE_SPTR_H_

#include<dts/dts_pixel_time_series_base.h>
#include<vbl/vbl_smart_ptr.h>

typedef 
    vbl_smart_ptr<dts_pixel_time_series_base>
        dts_pixel_time_series_base_sptr;

#endif //DTS_PIXEL_TIME_SERIES_BASE_SPTR_H_