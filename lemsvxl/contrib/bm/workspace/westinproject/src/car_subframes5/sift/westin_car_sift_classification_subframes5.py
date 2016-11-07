'''
Created on Jun 6, 2011

@author: bm
'''
import dsmpy

img_glob = "C:/Users/bm/Documents/progressReview/westin/westinCars/subframes5/grey/*.tif";
result_dir= "C:/Users/bm/Documents/progressReview/westin/results/sift_subframes5";
reduced_pixel_time_series_map_bin = result_dir + "/reduced_pixel_time_series_map_sptr.bin";
manager_bin = result_dir +"/manager_sptr.bin"
change_dir = result_dir +"/change_maps"
mahalan_factor = 2;
init_covar = 0.7;
min_covar = 0.1;

x=646
y=176

reduced_pixel_time_series_map_sptr = dsmpy.read_pixel_time_series_map_bin(reduced_pixel_time_series_map_bin)
#state_machine_manager_sptr = dsmpy.classify_pixel_time_series_map(result_dir, reduced_pixel_time_series_map_sptr, mahalan_factor=mahalan_factor, init_covar=init_covar, min_covar=min_covar,verbose=False)
#dsmpy.write_manager_bin(state_machine_manager_sptr, manager_bin)
manager_sptr=dsmpy.read_manager_bin(manager_bin)
dsmpy.write_manager_output(manager_sptr, reduced_pixel_time_series_map_sptr, x, y, result_dir)
