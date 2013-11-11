# Script to crop a voxel grid
# Author : Isabel Restrepo
# 6-14-2009

import bseg3d_batch
import os
bseg3d_batch.register_processes();
bseg3d_batch.register_datatypes();

#Delay for debugging
import time
time.sleep(15);

  
param_dir = "C:/Projects/vxl/vxl/contrib/brl/lemsvxlsrc/contrib/isabel/params";

#create world.
print("1.------------------------------------------")
print("Creating Voxel World");
bseg3d_batch.init_process("bseg3dCropGridProcess");
bseg3d_batch.set_params_process(param_dir + "/crop_grid_params.xml");
bseg3d_batch.run_process();
