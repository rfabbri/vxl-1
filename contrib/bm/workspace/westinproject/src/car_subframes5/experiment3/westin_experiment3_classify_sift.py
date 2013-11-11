'''
Created on Jun 11, 2011

@author: bm
'''
import dsmpy
result_dir = "C:/Users/bm/Documents/progressReview/westin/results/sift_subframes5/experiment3_results"
reduced_pixel_time_series_map_sptr_bin = result_dir + "/reduced_pixel_time_series_map_sptr.bin"
manager_sptr_bin = result_dir + "/manager_sptr.bin"

mahalan_factor = 2;
init_covar = 0.8;
min_covar = 0.2;

x=646
y=176

reduced_pixel_time_series_map_sptr = dsmpy.read_pixel_time_series_map_bin(reduced_pixel_time_series_map_sptr_bin)
manager_sptr = dsmpy.classify_pixel_time_series_map(result_directory=result_dir, pixel_time_series_map=reduced_pixel_time_series_map_sptr, mahalan_factor=mahalan_factor, init_covar=init_covar, min_covar=min_covar, verbose=False)
dsmpy.write_manager_bin(manager_sptr, manager_sptr_bin)
dsmpy.write_manager_output(manager_sptr, reduced_pixel_time_series_map_sptr, x, y, result_dir)
