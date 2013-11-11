'''
Created on Jun 11, 2011

@author: bm
'''
import dsmpy
img_glob = "C:/Users/bm/Documents/progressReview/westin/westinCars/subframes5/grey/*.tif";
result_dir = "C:/Users/bm/Documents/progressReview/westin/results/sift_subframes5/experiment3_results"

reduced_pixel_time_series_map_sptr_bin = result_dir + "/reduced_pixel_time_series_map_sptr.bin"

xmin = 490
xmax = 737
ymin = 121
ymax = 274

ndims2keep = 2

reduced_pixel_time_series_map_sptr = dsmpy.pixel_time_series_map_extract_sift_box_and_reduce(img_glob, xmin, xmax, ymin, ymax, ndims2keep)
dsmpy.write_pixel_time_series_map_sptr_bin(reduced_pixel_time_series_map_sptr, reduced_pixel_time_series_map_sptr_bin)