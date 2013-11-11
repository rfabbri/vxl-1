'''
Created on Aug 7, 2011


@author: bm
'''
import dsmpy
change_file="C:/smw/universityOfNebraska/60frames/change/bwm_binary/universityOfNebraska60Frames_gt.bin";
result_dir="C:/smw/universityOfNebraska/60frames/change/change_maps"
classical_change_dir="C:/smw/universityOfNebraska/60frames/change/classical_change_maps"
width=320;
height=240;
dsm_ground_truth_sptr=dsmpy.dsm_ground_truth_read_bwm_bin(change_file);
dsmpy.dsm_ground_truth_build_change_maps(dsm_ground_truth_sptr, width, height);
dsmpy.dsm_ground_truth_save_change_map_tiff(dsm_ground_truth_sptr, result_dir);
dsmpy.dsm_ground_truth_save_change_map_classical_tiff(dsm_ground_truth_sptr, width, height, classical_change_dir)