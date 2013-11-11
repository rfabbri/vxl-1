import bseg3d_batch
bseg3d_batch.register_processes();
bseg3d_batch.register_datatypes();

#Delay for debugging
import time
#time.sleep(30);


class dbvalue:
  def __init__(self, index, type):
    self.id = index
    self.type = type

results_dir = "C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/results";

param_dir = "C:/Scripts/python_voxel/CapitolSiteHigh/params";

#grid dimensions
nx = 475;
ny = 475;
nz = 185

img_names = "Y:/video/dec/capitol_sfm/video_grey/frame_%05d.png";
cam_names = "Y:/video/dec/CapitolSiteHigh/cameras_KRT/camera_%05d.txt";

i = 224;
 
#create world.
print("------------------------------------------")
print("Creating Voxel World");
bseg3d_batch.init_process("bvxmCreateVoxelWorldProcess");
bseg3d_batch.set_params_process(param_dir + "/voxel_world_params.xml");
bseg3d_batch.run_process();
(id,type) = bseg3d_batch.commit_output(0);
world = dbvalue(id,type);
print(str(id))

print("------------------------------------------")
print("Loading Camera");
bseg3d_batch.init_process("vpglLoadPerspectiveCameraProcess");
bseg3d_batch.set_input_string(0,cam_names % i);
bseg3d_batch.run_process();
(id,type) = bseg3d_batch.commit_output(0);
cam = dbvalue(id,type);

print("------------------------------------------")
print("Loading Image");
bseg3d_batch.init_process("vilLoadImageViewProcess");
bseg3d_batch.set_input_string(0,img_names % i);
bseg3d_batch.run_process();
(id,type) = bseg3d_batch.commit_output(0);
image = dbvalue(id,type);

print("------------------------------------------")
print("Loading Mask");
bseg3d_batch.init_process("vilLoadImageViewProcess");
bseg3d_batch.set_input_string(0,results_dir +"/224_mask.png");
bseg3d_batch.run_process();
(id,type) = bseg3d_batch.commit_output(0);
mask = dbvalue(id,type);

print("------------------------------------------")
print("Locating Region");
bseg3d_batch.init_process("bvxmLocateRegionProcess");
bseg3d_batch.set_input_from_db(0,image);
bseg3d_batch.set_input_from_db(1,mask);
bseg3d_batch.set_input_from_db(2,cam);
bseg3d_batch.set_input_from_db(3,world);
bseg3d_batch.set_input_string(4,results_dir +"/locate.vox");
bseg3d_batch.set_input_string(5,"apm_mog_grey");
bseg3d_batch.run_process();


print("------------------------------------------")
print("Saving ocp grid");
bseg3d_batch.init_process("bseg3dSaveFloatGridProcess");
bseg3d_batch.set_input_string(0, results_dir + "/locate.vox");
bseg3d_batch.set_input_string(1, results_dir + "/locate.raw");
bseg3d_batch.set_input_unsigned(2,nx);
bseg3d_batch.set_input_unsigned(3,ny);
bseg3d_batch.set_input_unsigned(4,nz);
bseg3d_batch.run_process();

print("------------------------------------------")
print("Saving ocp grid");
bseg3d_batch.init_process("bseg3dSaveFloatGridProcess");
bseg3d_batch.set_input_string(0, results_dir + "/ocp_bin_0_scale_0.vox");
bseg3d_batch.set_input_string(1, results_dir + "/ocp_bin_0_scale_0.raw");
bseg3d_batch.set_input_unsigned(2,nx);
bseg3d_batch.set_input_unsigned(3,ny);
bseg3d_batch.set_input_unsigned(4,nz);
bseg3d_batch.run_process();
