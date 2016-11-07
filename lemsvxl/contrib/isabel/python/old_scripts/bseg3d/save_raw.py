# Script generate l2 distances of an already trained bvxm voxel world of CapitolSiteHigh Providence video
# Author : Isabel Restrepo
# 3-26-2009

import bseg3d_batch
import os
bseg3d_batch.register_processes();
bseg3d_batch.register_datatypes();

#Delay for debugging
import time
#time.sleep(30);


class dbvalue:
  def __init__(self, index, type):
    self.id = index
    self.type = type

  
#set directory names
results_dir = "C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/cropped_world";
  
#grid dimensions
# nx = 475;
# ny = 475;
# nz = 185
nx = 219;
ny = 215;
nz = 81;



print("------------------------------------------")
print("Saving ocp grid");
bseg3d_batch.init_process("bseg3dSaveFloatGridProcess");
bseg3d_batch.set_input_string(0, results_dir + "/ocp_cropped.vox");
bseg3d_batch.set_input_string(1, results_dir + "/ocp_cropped.raw");
bseg3d_batch.set_input_unsigned(2,nx);
bseg3d_batch.set_input_unsigned(3,ny);
bseg3d_batch.set_input_unsigned(4,nz);
bseg3d_batch.run_process();


