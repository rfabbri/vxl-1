import bvam_batch
bvam_batch.register_processes();
bvam_batch.register_datatypes();

print("Creating Voxel World");
bvam_batch.init_process("CreateVoxelWorldProcess");
bvam_batch.set_input_string(0,"d:/dec/matlab/reg3d/batch_test");
bvam_batch.run_process();
world_id = bvam_batch.commit_output(0);


import glob
image_fnames = glob.glob("f:/dec/helicopter/sequences/seq2/images/*.png");
camera_fnames = glob.glob("f:/dec/helicopter/sequences/seq2/cameras/*.txt");

for i in range(0,100,25):
  print("Loading Camera");
  bvam_batch.init_process("LoadProjCameraProcess");
  bvam_batch.set_input_string(0,camera_fnames[i]);
  bvam_batch.run_process();
  cam_id = bvam_batch.commit_output(0);
 
  print("Loading Image");
  bvam_batch.init_process("LoadImageViewProcess");
  bvam_batch.set_input_string(0,image_fnames[i]);
  bvam_batch.run_process();
  image_id = bvam_batch.commit_output(0);
 
  print("Updating World");
  bvam_batch.init_process("BvamUpdateProcess");
  bvam_batch.set_input_from_db(0,image_id);
  bvam_batch.set_input_from_db(1,cam_id);
  bvam_batch.set_input_from_db(2,world_id);
  bvam_batch.run_process();
  prob_img_id = bvam_batch.commit_output(0);

print("Done");

 