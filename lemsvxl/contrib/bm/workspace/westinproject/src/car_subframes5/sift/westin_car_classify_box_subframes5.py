'''
Created on Jun 7, 2011

@author: bm
'''
import dsmpy
img_glob = "C:/Users/bm/Documents/progressReview/westin/westinCars/subframes5/grey/*.tif";
result_dir = "C:/Users/bm/Documents/progressReview/westin/results/sift_subframes5/box"
reduced_pixel_time_series_map_bin = result_dir + "/reduced_pixel_time_series_map_sptr.bin";
manager_bin = result_dir + "/manager_sptr.bin";
change_dir = result_dir + "/change_maps2"


mahalan_factor = 3;
init_covar = 2.0;
min_covar = 0.5;

x = 646;
y = 176;
x2 = 660
y2 = 226


dat2_file = result_dir + "/%d_%d.txt" % (x2,y2)

reduced_pixel_time_series_map_sptr = dsmpy.read_pixel_time_series_map_bin(reduced_pixel_time_series_map_bin)
dsmpy.write_pixel_dat_file(reduced_pixel_time_series_map_sptr,x2,y2,dat2_file)
state_machine_manager_sptr = dsmpy.classify_pixel_time_series_map(result_dir, reduced_pixel_time_series_map_sptr, mahalan_factor=mahalan_factor, init_covar=init_covar, min_covar=min_covar,verbose=False)
dsmpy.write_manager_bin(state_machine_manager_sptr,manager_bin)
#state_machine_manager_sptr=dsmpy.read_manager_bin(manager_bin)
dsmpy.write_manager_output(state_machine_manager_sptr,reduced_pixel_time_series_map_sptr,x,y,result_dir);
dsmpy.write_manager_output(state_machine_manager_sptr,reduced_pixel_time_series_map_sptr,x2,y2,result_dir);
dsmpy.write_change_maps(state_machine_manager_sptr, img_glob, change_dir)