'''
Created on Aug 8, 2011

@author: bm
'''
import dsmpy
img_glob = "C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/rgb/*.png";
result_dir = "C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/results/experiment2"
reduced_pixel_time_series_map_bin = result_dir + "/experiment2_reduced_pixel_time_series_map_sptr.bin"
manager_bin = result_dir + "/experiment2_mahalan2_manager_sptr.bin"
change_dir = result_dir + "/mahalan2_change_maps"
prediction_maps = result_dir + "/mahalan2_prediction_maps"
avg_num_states_per_frame=result_dir+"/avg_num_states_per_frame_mahalan2.txt"
prediction_video_glob = prediction_maps+"/predicted_change_map_*.tiff"
gt_video_glob="C:/smw/universityOfNebraska/60frames/change/change_maps/change_map_*.tiff"
confusion_matrix_txt=result_dir+"/mahalan2_confusion_matrix.txt"

mahalan_factor = 2;
init_covar = 0.2;
min_covar = 0.1;
x = 146
y = 188
width=320;
height=240;
x2 = 160
y2 = 185

reduced_pixel_time_series_map_sptr = dsmpy.read_pixel_time_series_map_bin(reduced_pixel_time_series_map_bin)
state_machine_manager_sptr = dsmpy.classify_pixel_time_series_map(result_dir, reduced_pixel_time_series_map_sptr, mahalan_factor=mahalan_factor, init_covar=init_covar, min_covar=min_covar,verbose=False)
dsmpy.write_manager_bin(state_machine_manager_sptr, manager_bin)
dsmpy.write_change_maps(dsm_manager_sptr=state_machine_manager_sptr, original_img_glob=img_glob, change_map_directory=change_dir)
dsmpy.dsm_manager_write_prediction_maps(state_machine_manager_sptr , width, height, prediction_maps)
dsmpy.manager_write_average_num_states_per_frame(state_machine_manager_sptr, avg_num_states_per_frame)
dsmpy.dsm_confusion_matrix_from_globs(gt_video_glob,prediction_video_glob,confusion_matrix_txt);

