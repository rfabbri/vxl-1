import psm_batch
psm_batch.register_processes();
psm_batch.register_datatypes();

#### parameters ####

do_init = 1;
do_refine = 1;
num_its = 30;
init_alpha = 0.746;

refine_prob = 0.2;
damping_factor = 0.70;


model_dir = "c:/research/psm/models/capitol_batch";
image_id_fname = model_dir + "/image_list.txt";
image_fname = "c:/research/data/capitol/video_grey/frame_%05d.png";
camera_fname = "c:/research/data/capitol/cameras_KRT/camera_%05d.txt";
camera_idx = range(0,255,8);

##################


class dbvalue:
  def __init__(self, index, type):
    self.id = index
    self.type = type

image_ids = [];
# write camera indices to file
fd = open(image_id_fname,"w");
print >>fd, len(camera_idx);
for c in camera_idx:
  img_id = "frame_%05d" % c;
  image_ids.append(img_id);
  print >>fd, img_id;
fd.close();

# load scene 
print("Loading Scene");
psm_batch.init_process("psmLoadSceneProcess");
psm_batch.set_params_process(model_dir + "/scene_params.xml");
psm_batch.run_process();
(id,type) = psm_batch.commit_output(0);
scene = dbvalue(id,type);

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
      psm_batch.set_input_float(5,init_alpha);
      psm_batch.run_process();


print("Loading Virtual Camera");
psm_batch.init_process("vpglLoadPerspectiveCameraProcess");
psm_batch.set_input_string(0,"C:/research/psm/output/capitol_views/camera_high.txt");
psm_batch.run_process();
(id,type) = psm_batch.commit_output(0);
vcam = dbvalue(id,type);

expected_fname = "c:/research/psm/output/expected_image_capitol_batch_%03d.tiff";

for it in range(0,num_its,1):
  for c in range(0,len(camera_idx),1):
    
    print("Loading Camera");
    psm_batch.init_process("vpglLoadPerspectiveCameraProcess");
    psm_batch.set_input_string(0,camera_fname % camera_idx[c]);
    psm_batch.run_process();
    (id,type) = psm_batch.commit_output(0);
    cam = dbvalue(id,type);
    
    print("Loading Image");
    psm_batch.init_process("vilLoadImageViewProcess");
    psm_batch.set_input_string(0,image_fname % camera_idx[c]);
    psm_batch.run_process();
    (id,type) = psm_batch.commit_output(0);
    image = dbvalue(id,type);
    
    print "Generating opt_samples for camera ", camera_idx[c];
    psm_batch.init_process("psmOptGenerateOptSamplesProcess");
    psm_batch.set_input_from_db(0,image);
    psm_batch.set_input_from_db(1,cam);
    psm_batch.set_input_string(2,image_ids[c]);
    psm_batch.set_input_from_db(3,scene);
    psm_batch.run_process();

  # Do the optimization
  psm_batch.init_process("psmOptBayesianUpdateProcess");
  psm_batch.set_input_from_db(0,scene);
  psm_batch.set_input_float(1,damping_factor);
  psm_batch.set_input_string(2,image_id_fname);
  psm_batch.run_process();
  
  # Generate Expected Image 
  print("Generating Expected Image");
  psm_batch.init_process("psmRenderExpectedProcess");
  psm_batch.set_input_from_db(0,vcam); 
  psm_batch.set_input_unsigned(1,1280);
  psm_batch.set_input_unsigned(2,720);
  psm_batch.set_input_from_db(3,scene);
  psm_batch.run_process();
  (id,type) = psm_batch.commit_output(0);
  expected = dbvalue(id,type);
  (id,type) = psm_batch.commit_output(1);
  mask = dbvalue(id,type);
  
  print("saving expected image");
  psm_batch.init_process("vilSaveImageViewProcess");
  psm_batch.set_input_from_db(0,expected);
  psm_batch.set_input_string(1,expected_fname % it);
  psm_batch.run_process();
  
  psm_batch.remove_data(expected.id);
  psm_batch.remove_data(mask.id);
    
  if ( (do_refine) & (it+1 < num_its) ):    
    print("Refining Scene");
    psm_batch.init_process("psmRefineSceneProcess");
    psm_batch.set_input_from_db(0,scene);
    psm_batch.set_input_float(1,refine_prob);
    psm_batch.set_input_bool(2,False);
    psm_batch.run_process();
  
  psm_batch.remove_data(cam.id);
  psm_batch.remove_data(image.id);

psm_batch.remove_data(scene.id);

# switch processes to make sure scene destructor is called
psm_batch.init_process("vilLoadImageViewProcess");

print("Done.");
