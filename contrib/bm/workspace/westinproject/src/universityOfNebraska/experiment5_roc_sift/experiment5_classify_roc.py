'''
Created on Aug 9, 2011

@author: bm
'''
import dsmpy
import os

img_glob = "C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/rgb/*.png";
result_dir = "C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/results/experiment5_roc_sift"
reduced_pixel_time_series_map_bin = "C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/results/experiment2/experiment2_reduced_pixel_time_series_map_sptr.bin"
gt_video_glob="C:/smw/universityOfNebraska/60frames/change/change_maps/change_map_*.tiff";

manager_bin_names = result_dir + "/manager_%03d.bin"
prediction_names = result_dir + "/prediction_maps_%03d"
confusion_matrix_names = result_dir + "/confusion_matrix_%03d.txt"
roc_file = result_dir + "/python_roc.txt"

init_covar = 0.7;
min_covar = 0.4;
width=320;
height=240;

#mahalan_factors = range(1,21,1);
mahalan_factors = [0.001,.01,.1,.5,1,2,5,10]

sensitivity = [];
specificity = [];

for i in range(len(mahalan_factors)):
    prediction_dir = prediction_names % (i + 1);
    
    if( not os.path.exists(prediction_dir)):
        os.mkdir(prediction_dir)
        
    prediction_glob = prediction_dir + "/*.tiff"    
    
    manager_bin = manager_bin_names % (i+1);   
    confusion_matrix_txt = confusion_matrix_names % (i+1);     
        
    reduced_pixel_time_series_map_sptr = dsmpy.read_pixel_time_series_map_bin(reduced_pixel_time_series_map_bin);
    dsm_manager_sptr = dsmpy.classify_pixel_time_series_map(result_dir, reduced_pixel_time_series_map_sptr, mahalan_factor=mahalan_factors[i], init_covar=init_covar, min_covar=min_covar,verbose=False)
    #dsmpy.write_manager_bin(dsm_manager_sptr, manager_bin)
    dsmpy.dsm_manager_write_prediction_maps(dsm_manager_sptr, width, height, prediction_dir)
    dsmpy.dsm_confusion_matrix_from_globs(gt_video_glob, prediction_glob, confusion_matrix_txt)
    dsmpy.remove_data(dsm_manager_sptr.id);
    f = open(confusion_matrix_txt)
    lines = f.readlines();
    f.close();
    
    sensitivity.append(lines[3]);
    specificity.append(lines[4]);

for j in range(len(sensitivity)):
    sensitivity[j].rstrip('\n');
    specificity[j].rstrip('\n');
    
f = open(roc_file,"w") 
f.write(str(sensitivity) + '\n' + str(specificity));
f.close()   
    
    
    
