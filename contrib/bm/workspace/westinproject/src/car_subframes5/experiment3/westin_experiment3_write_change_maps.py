'''
Created on Jun 11, 2011

@author: bm
'''
import dsmpy
result_dir = "C:/Users/bm/Documents/progressReview/westin/results/sift_subframes5/experiment3_results"
img_glob = "C:/Users/bm/Documents/progressReview/westin/westinCars/subframes5/grey/*.tif";
manager_bin = result_dir +"/manager_sptr.bin"
change_map_dir = result_dir+"/change_maps"
manager_sptr = dsmpy.read_manager_bin(manager_bin)
dsmpy.write_change_maps(manager_sptr, img_glob, change_map_dir)
