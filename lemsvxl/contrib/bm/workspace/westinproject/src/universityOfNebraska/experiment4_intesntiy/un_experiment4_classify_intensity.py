'''
Created on Aug 8, 2011

@author: bm
'''
import dsmpy
result_dir="C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/results/experiment4"
pixel_time_series_intensity_bin=result_dir+"/experiment4_intensity_time_series_map_sptr.bin"
manager_bin = result_dir + "/experiment4_manager_sptr.bin"
prediction_dir = result_dir + "/prediction_maps"
gt_video_glob="C:/smw/universityOfNebraska/60frames/change/change_maps/change_map_*.tiff"
prediction_video_glob = prediction_dir + "/predicted_change_map_*.tiff"
confusion_matrix_txt = result_dir + "/experiment4_confusion_matrix.txt"
avg_states_per_frame_txt = result_dir + "/experiment4_avg_num_states_per_frame.txt"

mahalan_factor = 2.5;
init_covar = 75;
min_covar = 20;
width=320;
height=240;

intensity_pixel_time_series_map_sptr = dsmpy.read_pixel_time_series_map_bin(pixel_time_series_intensity_bin);

state_machine_manager_sptr = dsmpy.classify_pixel_time_series_map(result_dir, intensity_pixel_time_series_map_sptr, mahalan_factor=mahalan_factor, ndims = 1, init_covar=init_covar, min_covar=min_covar,verbose=False)

dsmpy.write_manager_bin(state_machine_manager_sptr, manager_bin);

dsmpy.dsm_manager_write_prediction_maps(state_machine_manager_sptr , width, height, prediction_dir)

dsmpy.manager_write_average_num_states_per_frame(state_machine_manager_sptr, avg_states_per_frame_txt)

dsmpy.dsm_confusion_matrix_from_globs(gt_video_glob,prediction_video_glob,confusion_matrix_txt);
 