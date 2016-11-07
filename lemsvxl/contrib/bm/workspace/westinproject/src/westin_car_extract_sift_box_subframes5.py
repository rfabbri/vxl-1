'''
Created on Jun 6, 2011

@author: bm
'''
import dsmpy
img_glob = "C:/Users/bm/Documents/progressReview/westin/westinCars/subframes5/grey/*.tif";
result_dir= "C:/Users/bm/Documents/progressReview/westin/results/sift_subframes5/box";
pixel_time_series_map_bin = result_dir+"/pixel_time_series_map_sptr.bin";
reduced_pixel_time_series_map_bin = result_dir + "/reduced_pixel_time_series_map_sptr.bin";

xmin = 583;
xmax = 683;
ymin = 140;
ymax = 235;

pixel_time_series_map_sptr = dsmpy.extract_sift_time_series_box(xmin=xmin,xmax=xmax,ymin=ymin,ymax=ymax,image_glob=img_glob)
dsmpy.write_pixel_time_series_map_sptr_bin(pixel_time_series_map_sptr, pixel_time_series_map_bin);
#dsmpy.write_pixel_time_series_map_sptr_dat(pixel_time_series_map_sptr, result_dir);

reduced_pixel_time_series_map_sptr = dsmpy.reduce_pixel_time_series_map_dimension(pixel_time_series_map_sptr, 2);
#dsmpy.write_pixel_time_series_map_sptr_dat(reduced_pixel_time_series_map_sptr, result_dir);
dsmpy.write_pixel_time_series_map_sptr_bin(reduced_pixel_time_series_map_sptr, reduced_pixel_time_series_map_bin);