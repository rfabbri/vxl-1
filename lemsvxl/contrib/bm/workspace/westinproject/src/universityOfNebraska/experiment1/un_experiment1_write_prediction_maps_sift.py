'''
Created on Aug 7, 2011

@author: bm
'''

import dsmpy
result_dir = "C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/results/experiment1"
manager_bin = result_dir + "/manager_sptr.bin"
prediction_maps = result_dir + "/prediction_maps"
avg_num_states_per_frame=result_dir+"/avg_num_states_per_frame.txt"
width=320;
height=240;

dsm_manager_sptr = dsmpy.read_manager_bin(manager_bin);

dsmpy.dsm_manager_write_prediction_maps(dsm_manager_sptr, width, height, prediction_maps)

dsmpy.manager_write_average_num_states_per_frame(dsm_manager_sptr, avg_num_states_per_frame)