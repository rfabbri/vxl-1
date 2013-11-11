'''
Created on May 24, 2011

@author: bm
'''

import dsmpy

img_glob="C:/Users/bm/Documents/progressReview/westin/westinCars/grey/*.tif";
result_dir="C:/Users/bm/Documents/progressReview/westin/results/intensity_ratio_experiment1";
ncn_sptr_bin=result_dir+"/ncn_sptr.bin"
temporal_entropy_bin=result_dir+"/temporal_entropy.bin"
temporal_entropy_dat=result_dir+"/temporal_entropy.dat"
neighborhood_dat=result_dir+"/neighborhoods.dat";
pixels = [ [646, 646, 645, 645],
           [176, 177, 176, 177] ];
           
ncn_sptr = dsmpy.dsm_ncn_sptr(pixels,img_glob);
#dsmpy.ncn_calculate_temporal_entropy(ncn_sptr);
#dsmpy.ncn_write_temporal_entropy_bin(ncn_sptr,temporal_entropy_bin);
#dsmpy.ncn_write_temporal_entropy_dat(ncn_sptr,temporal_entropy_dat);

dsmpy.ncn_read_temporal_entropy_bin(ncn_sptr,temporal_entropy_bin);
dsmpy.build_neighborhood(ncn_sptr);
dsmpy.write_ncn_sptr_bin(ncn_sptr, ncn_sptr_bin);
dsmpy.ncn_write_neighborhood_dat(ncn_sptr, neighborhood_dat)
