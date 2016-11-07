'''
Created on Jun 9, 2011

@author: bm
'''
import dsmpy
img_glob = "C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/grey/*.tif";
result_dir = "C:/Users/bm/Documents/progressReview/universityOfNebraska/60subframes/results/experiment1"
pixel_time_series_map_bin = result_dir + "/pixel_time_series_map_sptr.bin"
reduced_pixel_time_series_map_bin = result_dir + "/reduced_pixel_time_series_map_sptr.bin"

xmin = 109;
xmax = 184;
ymin = 165;
ymax = 209;

pixel_time_series_map_sptr = dsmpy.extract_sift_time_series_box(xmin=xmin, xmax=xmax, ymin=ymin, ymax=ymax, image_glob=img_glob);
dsmpy.write_pixel_time_series_map_sptr_bin(pixel_time_series_map_sptr,pixel_time_series_map_bin);

reduced_pixel_time_series_map_sptr = dsmpy.reduce_pixel_time_series_map_dimension(pixel_time_series_map_sptr, ndims_to_keep=2)
dsmpy.write_pixel_time_series_map_sptr_bin(reduced_pixel_time_series_map_sptr,reduced_pixel_time_series_map_bin)