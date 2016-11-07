import bvpl_batch
bvpl_batch.register_processes();
bvpl_batch.register_datatypes();


class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string

input_dir = "/Volumes/Experiments/object_recognition/bvpl/CapitolSiteHigh/world_dan"
output_dir = "/Volumes/Experiments/object_recognition/bvpl/CapitolSiteHigh/cropped_world"
#first creat an empty world.
print("Load Grid World");
bvpl_batch.init_process("bvxmLoadGridProcess");
bvpl_batch.set_input_string(0,input_dir + "/apm_gauss.vox");
bvpl_batch.set_input_string(1,"bsta_gauss_f1");
bvpl_batch.run_process();
(world_id,world_type)= bvpl_batch.commit_output(0);
world = dbvalue(world_id,world_type);

print("Crop Voxel World");
bvpl_batch.init_process("bvxmCropGridProcess");
bvpl_batch.set_params_process(output_dir +"/crop_grid_params.xml");
bvpl_batch.set_input_from_db(0,world);
bvpl_batch.run_process();
(crop_world_id,crop_world_type)= bvpl_batch.commit_output(0);
crop_world = dbvalue(crop_world_id,crop_world_type);

# print("Writing World");
# bvpl_batch.init_process("bvxmSaveGridRawProcess");
# bvpl_batch.set_input_from_db(0,crop_world);
# bvpl_batch.set_input_string(1,output_dir + "/gcrop_world.raw");
# bvpl_batch.set_input_string(2,"float");
# bvpl_batch.run_process();



