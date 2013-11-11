'''
Created on Jun 11, 2011

@author: bm
'''
import dsmpy
img_glob = "C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/rgb/*.png";
result_dir = "C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/results/experiment2"
reduced_pixel_time_series_map_bin = result_dir + "/experiment2_reduced_pixel_time_series_map_sptr.bin"
manager_bin = result_dir + "/experiment2_manager_sptr.bin"
change_dir = result_dir +"/change_maps"

mahalan_factor = 3;
init_covar = 0.5;
min_covar = 0.3;
x = 146
y = 188
x2 = 160
y2 = 185


reduced_pixel_time_series_map_sptr = dsmpy.read_pixel_time_series_map_bin(reduced_pixel_time_series_map_bin)
state_machine_manager_sptr = dsmpy.classify_pixel_time_series_map(result_dir, reduced_pixel_time_series_map_sptr, mahalan_factor=mahalan_factor, init_covar=init_covar, min_covar=min_covar,verbose=False)
dsmpy.write_manager_bin(state_machine_manager_sptr, manager_bin)
dsmpy.write_manager_output(state_machine_manager_sptr, reduced_pixel_time_series_map_sptr, x, y, result_dir)
dsmpy.write_manager_bin(state_machine_manager_sptr, manager_bin)
dsmpy.write_manager_output(state_machine_manager_sptr, reduced_pixel_time_series_map_sptr, x2, y2, result_dir)
dsmpy.write_change_maps(dsm_manager_sptr=state_machine_manager_sptr, original_img_glob=img_glob, change_map_directory=change_dir)