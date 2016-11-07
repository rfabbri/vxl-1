# Script generate expected images of an already trained bvxm voxel world of SteepleStreet Providence video
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

#options for this script
train = 1; #Train the voxel world
calculate_distances = 0; #compute l2-distances in the voxel world
save_raw = 0; #save drishti raw file of distance-grid
render = 0;   #ecpected images of distance grid


    
#set directory names
param_dir = "C:/Scripts/python_voxel/SteepleStreet/params";


# create a directory to save results
results_dir = "C:/Experiments/object_recognition/bseg3d/SteepleStreet/results";
if not os.path.isdir( results_dir + "/"):
  os.mkdir( results_dir + "/");

img_names = "C:/Data/SteepleSite/video_grey/frame_%05d.png";
cam_names = "C:/Data/SteepleSite/cameras_KRT_opt/frame_%05d.txt";
  

#create world.
print("1.------------------------------------------")
print("Creating Voxel World");
bseg3d_batch.init_process("bvxmCreateVoxelWorldProcess");
bseg3d_batch.set_params_process(param_dir + "/voxel_world_params.xml");
bseg3d_batch.run_process();
(id,type) = bseg3d_batch.commit_output(0);
world = dbvalue(id,type);
print(str(id))

nframes = 99;
for i in range(0,nframes,1):

  print("------------------------------------------")
  print("Loading Camera");
  bseg3d_batch.init_process("vpglLoadPerspectiveCameraProcess");
  bseg3d_batch.set_input_string(0,cam_names % i);
  bseg3d_batch.run_process();
  (id,type) = bseg3d_batch.commit_output(0);
  cam = dbvalue(id,type);
 
  print("------------------------------------------")
  print("Generating Expected Image");
  bseg3d_batch.init_process("bvxmRenderExpectedImageProcess");
  bseg3d_batch.set_input_from_db(0,cam); 
  bseg3d_batch.set_input_unsigned(1,1280);
  bseg3d_batch.set_input_unsigned(2,720);
  bseg3d_batch.set_input_from_db(3,world);
  bseg3d_batch.set_input_string(4,"apm_mog_grey");
  bseg3d_batch.set_input_unsigned(5,0);
  bseg3d_batch.set_input_unsigned(6,0);
  bseg3d_batch.run_process();
  (id,type) = bseg3d_batch.commit_output(0);
  expected = dbvalue(id,type);

  print("------------------------------------------")
  print("saving expected image");
  bseg3d_batch.init_process("vilSaveImageViewProcess");
  bseg3d_batch.set_input_from_db(0,expected);
  bseg3d_batch.set_input_string(1, results_dir + "/expected_" + str(i) + ".png");
  bseg3d_batch.run_process();