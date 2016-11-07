# Script to merge the mixture of gaussians inside a grid into single gaussians. The result is a grid with univariate gaussians
# Author : Isabel Restrepo
# 3-26-2009

import bseg3d_batch
import os
bseg3d_batch.register_processes();
bseg3d_batch.register_datatypes();

#Delay for debugging
import time
#time.sleep(30);

  
#set directory names
results_dir = "C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/results";
  
#grid dimensions
nx = 475;
ny = 475;
nz = 185


print("------------------------------------------")
print("Saving ocp grid");
bseg3d_batch.init_process("bseg3dMultiplyGridsProcess");
bseg3d_batch.set_input_string(0, results_dir + "/ocp_bin_0_scale_0.vox");
bseg3d_batch.set_input_string(1, results_dir + "/locate.vox");
bseg3d_batch.set_input_string(2, results_dir + "/prod_ocp_locate.vox");
bseg3d_batch.set_input_unsigned(3,nx);
bseg3d_batch.set_input_unsigned(4,ny);
bseg3d_batch.set_input_unsigned(5,nz);
bseg3d_batch.run_process();

print("------------------------------------------")
print("Saving ocp grid");
bseg3d_batch.init_process("bseg3dSaveFloatGridProcess");
bseg3d_batch.set_input_string(0, results_dir + "/prod_ocp_locate.vox");
bseg3d_batch.set_input_string(1, results_dir + "/prod_ocp_locate.raw");
bseg3d_batch.set_input_unsigned(2,nx);
bseg3d_batch.set_input_unsigned(3,ny);
bseg3d_batch.set_input_unsigned(4,nz);
bseg3d_batch.run_process();