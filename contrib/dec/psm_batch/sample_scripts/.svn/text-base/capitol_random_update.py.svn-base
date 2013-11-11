import random
import psm_batch
psm_batch.register_processes();
psm_batch.register_datatypes();

do_init = 0;
init_prob = 0.005;
refine_prob = 0.4;

model_dir = "c:/research/psm/models/capitol_online";
image_id_fname = model_dir + "/image_list.txt";
image_fname = "c:/research/data/capitol/video_grey/frame_%05d.png";
camera_fname = "c:/research/data/capitol/cameras_KRT/camera_%05d.txt";
probe_fname = "c:/research/psm/output/capitol_online_update_probe_%03d.txt";

class dbvalue:
  def __init__(self, index, type):
    self.id = index
    self.type = type

print("Loading Scene");
psm_batch.init_process("psmLoadSceneProcess");
psm_batch.set_params_process(model_dir + "/scene_params.xml");
psm_batch.run_process();
(id,type) = psm_batch.commit_output(0);
scene = dbvalue(id,type);

print("Loading Virtual Camera");
psm_batch.init_process("vpglLoadPerspectiveCameraProcess");
psm_batch.set_input_string(0,camera_fname % 50);
psm_batch.run_process();
(id,type) = psm_batch.commit_output(0);
cam50 = dbvalue(id,type);

print("Loading image for ray probe");
psm_batch.init_process("vilLoadImageViewProcess");
psm_batch.set_input_string(0,image_fname % 50);
psm_batch.run_process();
(id,type) = psm_batch.commit_output(0);
img50 = dbvalue(id,type);

if (do_init):
  #initialize the scene
  print("Initializing the Scene");
  for x in range(-1,2,1):
    for y in range(-1,2,1):
      psm_batch.init_process("psmInitBlockProcess");
      psm_batch.set_input_from_db(0,scene);
      psm_batch.set_input_int(1,x);
      psm_batch.set_input_int(2,y);
      psm_batch.set_input_int(3,0);
      psm_batch.set_input_unsigned(4,6);
      psm_batch.set_input_float(5,init_prob);
      psm_batch.run_process();


expected_fname = "c:/research/psm/output/expected_image_capitol_online_%03d.tiff";

min_range = 10;
last_i = -min_range;
nframes = 120;

for x in range(26,nframes,1):
  i = random.randint(0,254);
  # try, try again if this frame is too close to the last
  while (abs(i - last_i) < min_range):
    i = random.randint(0,254);
  last_i = i;

  print("Loading Camera");
  psm_batch.init_process("vpglLoadPerspectiveCameraProcess");
  psm_batch.set_input_string(0,camera_fname % i);
  psm_batch.run_process();
  (id,type) = psm_batch.commit_output(0);
  cam = dbvalue(id,type);
  
  print("Loading Image");
  psm_batch.init_process("vilLoadImageViewProcess");
  psm_batch.set_input_string(0,image_fname % i);
  psm_batch.run_process();
  (id,type) = psm_batch.commit_output(0);
  img = dbvalue(id,type);
  
  print("Updating Scene");
  psm_batch.init_process("psmUpdateProcess");
  psm_batch.set_input_from_db(0,img);
  psm_batch.set_input_from_db(1,cam);
  psm_batch.set_input_from_db(2,scene);
  psm_batch.run_process();
  
  # generate ray probe
  print("Generating ray probe");
  psm_batch.init_process("psmRayProbeProcess");
  psm_batch.set_input_from_db(0,img50);
  psm_batch.set_input_from_db(1,cam50);
  psm_batch.set_input_float(2,853);
  psm_batch.set_input_float(3,239);
  psm_batch.set_input_from_db(4,scene);
  psm_batch.set_input_string(5,probe_fname % x);
  psm_batch.run_process();
  
  if ((x + 1) % 5 == 0):
    
    print("Generating Expected Image");
    psm_batch.init_process("psmRenderExpectedProcess");
    psm_batch.set_input_from_db(0,cam50); 
    psm_batch.set_input_unsigned(1,1280);
    psm_batch.set_input_unsigned(2,720);
    psm_batch.set_input_from_db(3,scene);
    psm_batch.run_process();
    (id,type) = psm_batch.commit_output(0);
    expected = dbvalue(id,type);
    
    print("saving expected image");
    psm_batch.init_process("vilSaveImageViewProcess");
    psm_batch.set_input_from_db(0,expected);
    psm_batch.set_input_string(1,expected_fname % x);
    psm_batch.run_process();
    
    psm_batch.remove_data(expected.id);
    
  if ( ((x+1) % 1 == 0) & (x+1 != nframes) ):    
    
    print("Refining Scene");
    psm_batch.init_process("psmRefineSceneProcess");
    psm_batch.set_input_from_db(0,scene);
    psm_batch.set_input_float(1,refine_prob);
    psm_batch.set_input_bool(2,False);
    psm_batch.run_process();
  
  
  psm_batch.remove_data(cam.id);
  psm_batch.remove_data(img.id);


psm_batch.remove_data(scene.id);

# switch processes to make sure scene destructor is called
psm_batch.init_process("vilLoadImageViewProcess");

print("Done.");
