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
param_dir = "C:/Scripts/python_voxel/CapitolSiteHigh/params";
world_dir = "C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/world"
results_dir = "C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/results";
  

#create world.
take_distances = 0;
save_raw = 0;
render = 1;
#grid dimensions
nx = 475;
ny = 475;
nz = 185
if take_distances:
  print("1.------------------------------------------")
  print("Creating Voxel World");
  bseg3d_batch.init_process("bvxmCreateVoxelWorldProcess");
  bseg3d_batch.set_params_process(param_dir + "/voxel_world_params.xml");
  bseg3d_batch.run_process();
  (id,type) = bseg3d_batch.commit_output(0);
  world = dbvalue(id,type);
  
  print("------------------------------------------")
  print("Calculating l2-distances");
  bseg3d_batch.init_process("bseg3dL2DistanceProcess");
  bseg3d_batch.set_input_string(0,world_dir + "/apm_mog_grey_bin_0_scale_0.vox");
  bseg3d_batch.set_input_string(1,world_dir + "/mask.vox");
  bseg3d_batch.set_input_string(2,world_dir + "/dist.vox");
  bseg3d_batch.set_input_unsigned(3,nx);
  bseg3d_batch.set_input_unsigned(4,ny);
  bseg3d_batch.set_input_unsigned(5,nz);
  bseg3d_batch.run_process();
  
  print("------------------------------------------")
  print("Saving ocp grid");
  bseg3d_batch.init_process("bseg3dSaveFloatGridProcess");
  bseg3d_batch.set_input_string(0, world_dir + "/dist.vox");
  bseg3d_batch.set_input_string(1, world_dir + "/dist.raw");
  bseg3d_batch.set_input_unsigned(2,nx);
  bseg3d_batch.set_input_unsigned(3,ny);
  bseg3d_batch.set_input_unsigned(4,nz);
  bseg3d_batch.run_process();


if save_raw:
  print("------------------------------------------")
  print("Saving grid");
  bseg3d_batch.init_process("bseg3dSaveFloatGridProcess");
  bseg3d_batch.set_input_string(0, world_dir +"/ocp_bin_0_scale_0.vox");
  bseg3d_batch.set_input_string(1,world_dir +"/ocp.raw");
  bseg3d_batch.set_input_unsigned(2, nx);
  bseg3d_batch.set_input_unsigned(3, ny);
  bseg3d_batch.set_input_unsigned(4, nz);
  bseg3d_batch.run_process();
  
  
  
if render:
  print("1.------------------------------------------")
  print("Creating Voxel World");
  bseg3d_batch.init_process("bvxmCreateVoxelWorldProcess");
  bseg3d_batch.set_params_process(param_dir + "/voxel_world_params.xml");
  bseg3d_batch.run_process();
  (id,type) = bseg3d_batch.commit_output(0);
  world = dbvalue(id,type);
  print(str(id))

  cams = [36,136,226,254]
  for c in cams:
    cam_name = "Y:/video/dec/CapitolSiteHigh/cameras_KRT/camera_%05d.txt";
    img_out = results_dir + "/distance_%05d.tiff";
    print("------------------------------------------")
    print("Loading Camera");
    bseg3d_batch.init_process("vpglLoadPerspectiveCameraProcess");
    bseg3d_batch.set_input_string(0,cam_name % c);
    bseg3d_batch.run_process();
    (id,type) = bseg3d_batch.commit_output(0);
    cam = dbvalue(id,type);
     
    print("---------------------------");
    print("Generating Expected Image");
    bseg3d_batch.init_process("bvxmRenderExpectedImageProcess");
    bseg3d_batch.set_input_from_db(0,cam);
    bseg3d_batch.set_input_unsigned(1,1280);
    bseg3d_batch.set_input_unsigned(2,720);
    bseg3d_batch.set_input_from_db(3,world);
    bseg3d_batch.set_input_string(4,"apm_float");
    bseg3d_batch.set_input_unsigned(5,0);
    bseg3d_batch.set_input_unsigned(6,0);
    bseg3d_batch.run_process();
    (id,type) = bseg3d_batch.commit_output(0);
    distance_img= dbvalue(id,type);
    
    print("---------------------------");
    print("Saving Image");
    bseg3d_batch.init_process("vilSaveImageViewProcess");
    bseg3d_batch.set_input_from_db(0,distance_img);
    bseg3d_batch.set_input_string(1,img_out % c);
    bseg3d_batch.run_process();
      

    bseg3d_batch.remove_data(distance_img.id);
    bseg3d_batch.remove_data(cam.id);
      
  bseg3d_batch.remove_data(world.id);