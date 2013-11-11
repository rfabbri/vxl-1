'''
Created on Aug 8, 2011

@author: bm
'''

import dsmpy

vg="C:/smw/universityOfNebraska/60frames/grey/frame_*.tif"
pixel_time_series_bin="C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/results/experiment4/experiment4_intensity_time_series_map_sptr.bin"
pixel_time_series_dat="C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/results/experiment4/experiment4_intensity_time_series_map_sptr.txt"
xmin = 110;
xmax = 187;
ymin = 120;
ymax = 207;

pixel_time_series_map_sptr=dsmpy.pixel_time_series_map_extract_intensity_box(vg, xmin, xmax, ymin, ymax);
dsmpy.write_pixel_time_series_map_sptr_bin(pixel_time_series_map_sptr, pixel_time_series_bin)
#dsmpy.write_pixel_time_series_map_sptr_dat(pixel_time_series_map_sptr, pixel_time_series_dat)

