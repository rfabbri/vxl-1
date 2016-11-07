'''
Created on Jun 1, 2011

@author: bm
'''
import dsmpy
result_dir="C:/Users/bm/Documents/progressReview/westin/results/sift_experiment1";
reduced_pixel_time_series_map_bin=result_dir + "/reduced_pixel_time_series_map_sptr.bin";

pixel_time_series_map_sptr = dsmpy.read_pixel_time_series_map_bin(reduced_pixel_time_series_map_bin);
dsmpy.classify_pixel_time_series_map(result_dir,pixel_time_series_map_sptr,init_covar = 1.0,min_covar=0.05);
