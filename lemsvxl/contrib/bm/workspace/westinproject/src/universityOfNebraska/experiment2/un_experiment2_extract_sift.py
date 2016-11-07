'''
Created on Jun 11, 2011

@author: bm
'''
import dsmpy
img_glob = "C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/grey/*.tif";
result_dir = "C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/results/experiment2"
reduced_pixel_time_series_map_bin = result_dir + "/experiment2_reduced_pixel_time_series_map_sptr.bin"

ndims2keep=2;
xmin = 110;
xmax = 187;
ymin = 120;
ymax = 207;

reduced_pixel_time_series_map_sptr = dsmpy.pixel_time_series_map_extract_sift_box_and_reduce(img_glob=img_glob, xmin=xmin, xmax=xmax, ymin=ymin, ymax=ymax, ndims2keep=ndims2keep)
dsmpy.write_pixel_time_series_map_sptr_bin(reduced_pixel_time_series_map_sptr, reduced_pixel_time_series_map_bin )
