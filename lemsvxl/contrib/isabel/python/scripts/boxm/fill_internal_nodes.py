import boxm_batch;
boxm_batch.register_processes();
boxm_batch.register_datatypes();

class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string


model_dir = "/Users/isa/Experiments/CapitolBOXMSmall";
output_dir = "/Users/isa/Experiments/CapitolBOXMSmall/filled/appearance";
output_drishti = output_dir + "/drishti"

import os
if not os.path.isdir( output_dir + "/"):
  os.mkdir( output_dir + "/");

if not os.path.isdir( output_drishti + "/" ):
	os.mkdir( output_drishti + "/" );

scene_prefix = "gaussf1";

print("Creating a Scene");
boxm_batch.init_process("boxmCreateSceneProcess");
boxm_batch.set_input_string(0,  model_dir +"/" + scene_prefix + "_scene.xml");
boxm_batch.run_process();
(scene_id, scene_type) = boxm_batch.commit_output(0);
filled_scene = dbvalue(scene_id, scene_type);

print("*************************************");


#print("Filling internal nodes");
#boxm_batch.init_process("boxmFillInternalCellsProcess");
#boxm_batch.set_input_from_db(0, scene);
#boxm_batch.run_process();
#(scene_id, scene_type) = boxm_batch.commit_output(0);
#filled_scene = dbvalue(scene_id, scene_type);


resolution=0;
enforce_level = 0;

print("Convert to regular grid");
boxm_batch.init_process("boxmSceneToBvxmGridProcess");
boxm_batch.set_input_from_db(0,filled_scene);
boxm_batch.set_input_string(1, output_dir +  "/" + scene_prefix + ".vox");
boxm_batch.set_input_unsigned(2, resolution);
boxm_batch.set_input_bool(3,enforce_level);
boxm_batch.run_process();
(grid_id, grid_type) = boxm_batch.commit_output(0);
grid = dbvalue(grid_id, grid_type);

print("Save Grid");
boxm_batch.init_process("bvxmSaveGridRawProcess");
boxm_batch.set_input_from_db(0,grid);
boxm_batch.set_input_string(1,output_drishti + "/" + scene_prefix + ".raw");
boxm_batch.run_process();


resolution=0;
enforce_level = 1;

print("Convert to regular grid");
boxm_batch.init_process("boxmSceneToBvxmGridProcess");
boxm_batch.set_input_from_db(0,filled_scene);
boxm_batch.set_input_string(1, output_dir +  "/" + scene_prefix + str(resolution) +".vox");
boxm_batch.set_input_unsigned(2, resolution);
boxm_batch.set_input_bool(3,enforce_level);
boxm_batch.run_process();
(grid_id, grid_type) = boxm_batch.commit_output(0);
grid = dbvalue(grid_id, grid_type);

print("Save Grid");
boxm_batch.init_process("bvxmSaveGridRawProcess");
boxm_batch.set_input_from_db(0,grid);
boxm_batch.set_input_string(1,output_drishti + "/" + scene_prefix + str(resolution) +".raw");
boxm_batch.run_process();

#resolution=1;
#enforce_level = 1;
#
#print("Convert to regular grid");
#boxm_batch.init_process("boxmSceneToBvxmGridProcess");
#boxm_batch.set_input_from_db(0,filled_scene);
#boxm_batch.set_input_string(1, output_dir + "/alpha1.vox");
#boxm_batch.set_input_unsigned(2, resolution);
#boxm_batch.set_input_bool(3,enforce_level);
#boxm_batch.run_process();
#(grid_id, grid_type) = boxm_batch.commit_output(0);
#grid = dbvalue(grid_id, grid_type);
#
#print("Save Grid");
#boxm_batch.init_process("bvxmSaveGridRawProcess");
#boxm_batch.set_input_from_db(0,grid);
#boxm_batch.set_input_string(1,output_drishti + "/alpha1.raw");
#boxm_batch.run_process();


