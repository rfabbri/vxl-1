# Script that thresholds an existing occupancy grid, then generates the l2 distances using such mask
# used with a  - bvxm voxel world of SteepleStreet Providence video
# Author : Isabel Restrepo
# 3-30-2009

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
param_dir = "C:/Scripts/python_voxel/SteepleStreet/params/thresh";
world_dir = "C:/Experiments/object_recognition/bseg3d/SteepleStreet/world_for_thresh"
results_dir = "C:/Experiments/object_recognition/bseg3d/SteepleStreet/results_for_thresh";
 
#size of grids --- caution this should be the same as in world_model_params
nx = 500;
ny = 425;
nz = 145;
thresh_values = [0.4]#, 0.5, 0.6, 0.7, 0.8, 0.9 ];

threshold = 0;
take_distances = 0;
save_raw = 0;
render = 1;


if threshold:


  for thresh in thresh_values:
    print("------------------------------------------")
    print("Thresholding World");
    bseg3d_batch.init_process("bseg3dThresholdWorldProcess");
    bseg3d_batch.set_input_string(0, world_dir + "/ocp_bin_0_scale_0.vox");
    bseg3d_batch.set_input_string(1, world_dir +"/" + str(thresh) +"/ocp_bin_0_scale_0.vox");
    bseg3d_batch.set_input_string(2, world_dir +"/" + str(thresh) +"/mask.vox");
    bseg3d_batch.set_input_float(3, thresh);  
    bseg3d_batch.set_input_unsigned(4, nx);
    bseg3d_batch.set_input_unsigned(5, ny);
    bseg3d_batch.set_input_unsigned(6, nz);
    bseg3d_batch.run_process();
    
    #if save_raw:
    print("------------------------------------------")
    print("Saving ocp grid");
    bseg3d_batch.init_process("bseg3dSaveFloatGridProcess");
    bseg3d_batch.set_input_string(0, world_dir +"/" + str(thresh) + "/ocp_bin_0_scale_0.vox");
    bseg3d_batch.set_input_string(1,world_dir +"/" + str(thresh) + "/ocp.raw");
    bseg3d_batch.set_input_unsigned(2,nx);
    bseg3d_batch.set_input_unsigned(3,ny);
    bseg3d_batch.set_input_unsigned(4,nz);
    bseg3d_batch.run_process();
    
    print("------------------------------------------")
    print("Saving mask grid");
    bseg3d_batch.init_process("bseg3dSaveFloatGridProcess");
    bseg3d_batch.set_input_string(0, world_dir +"/" + str(thresh) + "/mask.vox");
    bseg3d_batch.set_input_string(1,world_dir +"/" + str(thresh) + "/mask.raw");
    bseg3d_batch.set_input_unsigned(2,nx);
    bseg3d_batch.set_input_unsigned(3,ny);
    bseg3d_batch.set_input_unsigned(4,nz);
    bseg3d_batch.run_process();
    print "Done Thresholding";
  
  
  
if take_distances:
  
  for thresh in thresh_values:
    print("------------------------------------------")
    print("Calculating l2-distances");
    bseg3d_batch.init_process("bseg3dL2DistanceProcess");
    bseg3d_batch.set_input_string(0,world_dir + "/apm_mog_grey_bin_0_scale_0.vox");
    bseg3d_batch.set_input_string(1,world_dir +"/" + str(thresh) + "/mask.vox");
    bseg3d_batch.set_input_string(2,world_dir +"/" + str(thresh) + "/dist.vox");
    bseg3d_batch.set_input_unsigned(3,nx);
    bseg3d_batch.set_input_unsigned(4,ny);
    bseg3d_batch.set_input_unsigned(5,nz);
    bseg3d_batch.run_process();
    
    print("------------------------------------------")
    print("Saving ocp grid");
    bseg3d_batch.init_process("bseg3dSaveFloatGridProcess");
    bseg3d_batch.set_input_string(0, world_dir +"/" + str(thresh) + "/dist.vox");
    bseg3d_batch.set_input_string(1,world_dir +"/" + str(thresh) + "/dist.raw");
    bseg3d_batch.set_input_unsigned(2,nx);
    bseg3d_batch.set_input_unsigned(3,ny);
    bseg3d_batch.set_input_unsigned(4,nz);
    bseg3d_batch.run_process();


if save_raw:
  print("------------------------------------------")
  print("Saving grid");
  bseg3d_batch.init_process("bseg3dSaveFloatGridProcess");
  bseg3d_batch.set_input_string(0, world_dir +"/apm_float_bin_0_scale_0.vox");
  bseg3d_batch.set_input_string(1,world_dir +"/dist1.raw");
  bseg3d_batch.set_input_unsigned(2, 500);
  bseg3d_batch.set_input_unsigned(3,425);
  bseg3d_batch.set_input_unsigned(4,145);
  bseg3d_batch.run_process();
  
  
  
if render:
  for thresh in thresh_values:
    print("1.------------------------------------------")
    print("Creating Voxel World");
    bseg3d_batch.init_process("bvxmCreateVoxelWorldProcess");
    bseg3d_batch.set_params_process(param_dir + "/params" + str(int(thresh*10)) +".xml");
    bseg3d_batch.run_process();
    (id,type) = bseg3d_batch.commit_output(0);
    world = dbvalue(id,type);
    print(str(id))

    cam_name = "C:/Data/SteepleSite/cameras_KRT_opt/frame_%05d.txt";
    i = 5;
    print("------------------------------------------")
    print("Loading Camera");
    bseg3d_batch.init_process("vpglLoadPerspectiveCameraProcess");
    bseg3d_batch.set_input_string(0,cam_name % i);
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
    bseg3d_batch.set_input_string(1,results_dir + "/distance_" + str(int(thresh*10)) + ".tiff");
    bseg3d_batch.run_process();
      

    bseg3d_batch.remove_data(distance_img.id);
    bseg3d_batch.remove_data(cam.id);
    bseg3d_batch.remove_data(world.id);

