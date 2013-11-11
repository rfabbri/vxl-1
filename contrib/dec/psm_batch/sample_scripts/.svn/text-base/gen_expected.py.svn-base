import psm_batch
psm_batch.register_processes();
psm_batch.register_datatypes();

class dbvalue:
  def __init__(self, index, type):
    self.id = index
    self.type = type


print("Loading Scene");
psm_batch.init_process("psmLoadSceneProcess");
psm_batch.set_params_process("c:/research/psm/models/capitol_high_bundle/scene_params.xml");

psm_batch.run_process();
(id,type) = psm_batch.commit_output(0);
scene = dbvalue(id,type);

import glob

camera_fnames = glob.glob("C:/research/registration/output/capitol_high_train/flyover/cameras_KRT/*.txt");
expected_fname = "c:/research/psm/output/expected_image.tiff";
mask_fname = "c:/research/psm/output/expected_mask.tiff";

i = 116;

print("Loading Virtual Camera");
psm_batch.init_process("vpglLoadPerspectiveCameraProcess");
psm_batch.set_input_string(0,camera_fnames[i]);
psm_batch.run_process();
(id,type) = psm_batch.commit_output(0);
vcam = dbvalue(id,type);

print("Generating Expected Image");
psm_batch.init_process("psmRenderExpectedProcess");
psm_batch.set_input_from_db(0,vcam);
psm_batch.set_input_unsigned(1,1280);
psm_batch.set_input_unsigned(2,720);
psm_batch.set_input_from_db(3,scene);
psm_batch.run_process();
(id,type) = psm_batch.commit_output(0);
expected_img = dbvalue(id,type);
(id,type) = psm_batch.commit_output(1);
mask_img = dbvalue(id,type);

print("saving expected image");
psm_batch.init_process("vilSaveImageViewProcess");
psm_batch.set_input_from_db(0,expected_img);
psm_batch.set_input_string(1,expected_fname);
psm_batch.run_process();

print("saving mask");
psm_batch.init_process("vilSaveImageViewProcess");
psm_batch.set_input_from_db(0,mask_img);
psm_batch.set_input_string(1,mask_fname);
psm_batch.run_process();

