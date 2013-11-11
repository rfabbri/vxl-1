'''
Created on Jun 8, 2011

@author: bm
'''
import dsmpy
result_dir = "C:/Users/bm/Documents/progressReview/westin/results/sift_subframes5/box"
manager_bin = result_dir + "/manager_sptr.bin";
average_num_states_per_frame_txt = result_dir + "/average_num_states_per_frame.txt"

manager_sptr=dsmpy.read_manager_bin(manager_bin)
dsmpy.manager_write_average_num_states_per_frame(manager_sptr,average_num_states_per_frame_txt)