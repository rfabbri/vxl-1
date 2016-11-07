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

world_dir = "C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/world"
results_dir = "C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/results";
#grid dimensions
nx = 475;
ny = 475;
nz = 185

print("------------------------------------------")
print("Merging Mixtures");
bseg3d_batch.init_process("bseg3dMergeMixturesProcess");
bseg3d_batch.set_input_string(0,world_dir + "/apm_mog_grey_bin_0_scale_0.vox");
bseg3d_batch.set_input_string(1,world_dir + "/apm_gauss.vox");
bseg3d_batch.set_input_unsigned(2,nx);
bseg3d_batch.set_input_unsigned(3,ny);
bseg3d_batch.set_input_unsigned(4,nz);
bseg3d_batch.run_process();