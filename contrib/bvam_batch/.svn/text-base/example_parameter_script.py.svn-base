"""
This is a sample script by Ozge C Ozcanli
02/19/2008
"""

import bvam_batch
bvam_batch.register_processes();
bvam_batch.register_datatypes();

print("Creating Voxel World");
# clon CreateVoxelWorldProcess and make it the current process
bvam_batch.init_process("CreateVoxelWorldProcess");
# call the parse method of the current process
bvam_batch.set_params_process("D:/projects/lockheed-voxel-world/CreateVoxelWorldProcess.xml");
# run the current process
bvam_batch.run_process();
world_id = bvam_batch.commit_output(0);

import glob
image_fnames = glob.glob("Z:/video/ieden/image_pvd_helicopter/seq2/images/*.png");
camera_fnames = glob.glob("Z:/video/ieden/image_pvd_helicopter/seq2/cameras/*.txt");

print("Loading Camera");
bvam_batch.init_process("LoadProjCameraProcess");
bvam_batch.set_input_string(0,camera_fnames[1]);
bvam_batch.run_process();
cam_id = bvam_batch.commit_output(0);
 
print("Loading Image");
bvam_batch.init_process("LoadImageViewProcess");
bvam_batch.set_input_string(0,image_fnames[1]);
bvam_batch.run_process();
image_id = bvam_batch.commit_output(0);
 
print("Normalize Image");
bvam_batch.init_process("BvamNormalizeImageProcess");
bvam_batch.set_params_process("D:/projects/lockheed-voxel-world/BvamNormalizeImageProcess.xml");
bvam_batch.set_input_from_db(0,image_id);
bvam_batch.set_input_from_db(1,cam_id);
bvam_batch.set_input_from_db(2,world_id);
bvam_batch.run_process();
prob_img_id = bvam_batch.commit_output(0);

print("Done");

 