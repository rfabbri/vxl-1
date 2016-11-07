'''
Created on Jun 9, 2011

@author: bm
'''
import dsmpy
img_glob = "C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/rgb/*.png";
result_dir = "C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/results/experiment1"
reduced_pixel_time_series_map_bin = result_dir + "/reduced_pixel_time_series_map_sptr.bin"
manager_bin = result_dir + "/manager_sptr.bin"
change_dir = result_dir +"/change_maps"

mahalan_factor = 3;
init_covar = 0.7;
min_covar = 0.5;
x = 146
y = 188

reduced_pixel_time_series_map_sptr = dsmpy.read_pixel_time_series_map_bin(reduced_pixel_time_series_map_bin)
state_machine_manager_sptr = dsmpy.classify_pixel_time_series_map(result_dir, reduced_pixel_time_series_map_sptr, mahalan_factor=mahalan_factor, init_covar=init_covar, min_covar=min_covar,verbose=False)
dsmpy.write_manager_bin(state_machine_manager_sptr, manager_bin)
dsmpy.write_manager_output(state_machine_manager_sptr, reduced_pixel_time_series_map_sptr, x, y, result_dir)
dsmpy.write_change_maps(dsm_manager_sptr=state_machine_manager_sptr, original_img_glob=img_glob, change_map_directory=change_dir)