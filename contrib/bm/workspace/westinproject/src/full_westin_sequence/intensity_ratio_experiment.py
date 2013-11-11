'''
Created on May 24, 2011

@author: bm
'''

import dsmpy

img_glob="C:/Users/bm/Documents/progressReview/westin/westinCars/grey/*.tif";
result_dir="C:/Users/bm/Documents/progressReview/westin/results/intensity_ratio_experiment1";
ncn_sptr_bin=result_dir+"/ncn_sptr.bin"
pivot_pixel_candidates_dat=result_dir+"/pivot_pixel_candidates.dat"
temporal_entropy_bin=result_dir+"/temporal_entropy.bin"
temporal_entropy_dat=result_dir+"/temporal_entropy.dat"
neighborhood_dat=result_dir+"/neighborhoods.dat";
target_neighborhood_map_dat=result_dir+"/target_neighborhood_map.dat"
pixel_time_series_map_sptr_bin=result_dir+"/pixel_time_series_map_sptr.bin";
pixel_time_series_map_sptr_dat=result_dir+"/pixel_time_series_map";
reduced_pixel_time_series_map_sptr_bin=result_dir+"/reduced_pixel_time_series_map_sptr.bin"
reduced_pixel_time_series_map_sptr_dat=result_dir+"/reduced_pixel_time_series_map"
pixels = [ [646, 646, 645, 645, 891],
           [176, 177, 176, 177, 327] ];
           
#ncn_sptr = dsmpy.dsm_ncn_sptr(pixels,img_glob,num_pivot_pixels=2000);
#dsmpy.ncn_calculate_temporal_entropy(ncn_sptr);
#dsmpy.ncn_write_temporal_entropy_bin(ncn_sptr,temporal_entropy_bin);
#dsmpy.ncn_write_temporal_entropy_dat(ncn_sptr,temporal_entropy_dat);

#dsmpy.ncn_read_temporal_entropy_bin(ncn_sptr,temporal_entropy_bin);
#dsmpy.build_neighborhood(ncn_sptr);
#dsmpy.write_ncn_sptr_bin(ncn_sptr, ncn_sptr_bin);
#dsmpy.ncn_write_neighborhood_dat(ncn_sptr, neighborhood_dat)

ncn_sptr = dsmpy.read_ncn_sptr_bin(ncn_sptr_bin);

#ncn_sptr = dsmpy.read_ncn_sptr_bin(ncn_sptr_bin);
#dsmpy.ncn_write_pivot_pixel_candidates_dat(ncn_sptr,pivot_pixel_candidates_dat);
target_neighborhood_map_sptr=dsmpy.target_neighborhood_map_sptr_from_ncn(ncn_sptr);
dsmpy.target_neighborhood_map_write_dat(target_neighborhood_map_sptr, target_neighborhood_map_dat);
pixel_time_series_map_sptr = dsmpy.extract_intensity_ratio_pixel_time_series_map(target_neighborhood_map_sptr,img_glob);
dsmpy.write_pixel_time_series_map_sptr_bin(pixel_time_series_map_sptr, pixel_time_series_map_sptr_bin)
dsmpy.write_pixel_time_series_map_sptr_dat(pixel_time_series_map_sptr, pixel_time_series_map_sptr_dat);
reduced_pixel_time_series_map_sptr=dsmpy.reduce_pixel_time_series_map_dimension(pixel_time_series_map_sptr,2);
dsmpy.write_pixel_time_series_map_sptr_bin(reduced_pixel_time_series_map_sptr,reduced_pixel_time_series_map_sptr_bin);
dsmpy.write_pixel_time_series_map_sptr_dat(reduced_pixel_time_series_map_sptr,reduced_pixel_time_series_map_sptr_dat);

