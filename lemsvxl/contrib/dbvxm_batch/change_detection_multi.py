#import bvxm_batch
import dbvxm_batch
import glob
##bvxm_batch.register_processes();
##bvxm_batch.register_datatypes();
dbvxm_batch.register_processes();
dbvxm_batch.register_datatypes();

dbvxm_batch.print_db();

print("Creating Voxel World");
dbvxm_batch.init_process("dbvxmCreateVoxelWorldProcess");
dbvxm_batch.set_params_process("./world_multi_model_params.xml");
dbvxm_batch.run_process();
multi_voxel_world_id = dbvxm_batch.commit_output(0);

print("Creating Voxel World");
dbvxm_batch.init_process("bvxmCreateVoxelWorldProcess");
dbvxm_batch.set_params_process("./world_model_params.xml");
dbvxm_batch.run_process();
voxel_world_id = dbvxm_batch.commit_output(0);

##
# list of the images
f=open('./full_hiafa_images_mul.txt', 'r')
mul_image_fnames=f.readlines();
f.close();

for i in range(0,10,1):
  mul_image_filename=mul_image_fnames[i];
  mul_image_filename=mul_image_filename[:-1];

  print mul_image_filename;

  index= mul_image_filename.rfind('\\');

  dir_name=mul_image_filename[:index];
  dir_name=dir_name.replace('\\','/');
  print dir_name;

  cam_filename=glob.glob(dir_name+"/*_v*.RPB");

  map_type="10bins_1d_radial";
  print("Illumination Index");
  dbvxm_batch.init_process("bvxmIllumIndexProcess");
  dbvxm_batch.set_input_string(0,map_type);
  dbvxm_batch.set_input_string(1,mul_image_filename);
  dbvxm_batch.set_input_unsigned(2,8);
  dbvxm_batch.set_input_unsigned(3,0);
  dbvxm_batch.run_process();
  bin_id = dbvxm_batch.commit_output(0);


  dbvxm_batch.init_process("LoadRationalCameraProcess");
  dbvxm_batch.set_input_string(0,cam_filename[0]);
  dbvxm_batch.run_process();
  cam_id = dbvxm_batch.commit_output(0);

  # get a roi from the image 
  dbvxm_batch.init_process("bvxmRoiInitProcess");
  dbvxm_batch.set_input_string(0,mul_image_filename);
  dbvxm_batch.set_input_from_db(1,cam_id);
  dbvxm_batch.set_input_from_db(2,voxel_world_id);
  dbvxm_batch.set_params_process("./corrected_roi_params.xml");
  statuscode=dbvxm_batch.run_process();
  cropped_cam_id = dbvxm_batch.commit_output(0);
  cropped_image_id = dbvxm_batch.commit_output(1);

  print("Converting to Grey Image");
  dbvxm_batch.init_process("RGBItoGreyProcess");
  dbvxm_batch.set_input_from_db(0,cropped_image_id);
  dbvxm_batch.run_process();
  grey_image_id = dbvxm_batch.commit_output(0);
  print "grey_image_id ", grey_image_id;
  
  print("Saving Grey Image");
  dbvxm_batch.init_process("SaveImageViewProcess");
  dbvxm_batch.set_input_from_db(0,grey_image_id);
  dbvxm_batch.set_input_string(1,"./gry"+str(i)+".png");
  dbvxm_batch.run_process();

 
  
  print("Detect Scale");
  dbvxm_batch.init_process("dbvxmDetectScaleProcess");
  dbvxm_batch.set_input_from_db(0,voxel_world_id);
  dbvxm_batch.set_input_from_db(1,cropped_cam_id);
  dbvxm_batch.set_input_from_db(2,grey_image_id);
  dbvxm_batch.run_process();  
  curr_scale_id = dbvxm_batch.commit_output(0);


  
  app_type="apm_mog_grey";
  print("Update Model");
  dbvxm_batch.init_process("dbvxmUpdateMultiScaleProcess");
  dbvxm_batch.set_input_from_db(0,grey_image_id);
  dbvxm_batch.set_input_from_db(1,cropped_cam_id);
  dbvxm_batch.set_input_from_db(2,multi_voxel_world_id);
  dbvxm_batch.set_input_string(3,app_type);
  dbvxm_batch.set_input_from_db(4,bin_id);
  dbvxm_batch.set_input_from_db(5,curr_scale_id);
  dbvxm_batch.set_input_unsigned(6,3);
  dbvxm_batch.run_process();  
  out_img_id = dbvxm_batch.commit_output(0);
  mask_img_id = dbvxm_batch.commit_output(1);


  print("Writing World");
  bvxm_batch.init_process("bvxmSaveOccupancyRaw");
  bvxm_batch.set_input_from_db(0,voxel_world_id);
  bvxm_batch.set_input_string(1,"./world.raw");
  bvxm_batch.run_process();
##
##  print cam_id;bvxm_batch.print_db();
##  # get a roi from the image 
##  bvxm_batch.init_process("bvxmRoiInitProcess");
##  bvxm_batch.set_input_string(0,image_filename);
##  bvxm_batch.set_input_from_db(1,cam_id);
##  bvxm_batch.set_input_from_db(2,voxel_world_id);
##  bvxm_batch.set_params_process("./corrected_roi_params.xml");
##  statuscode=bvxm_batch.run_process();
##  if statuscode:
##    cropped_cam_id = bvxm_batch.commit_output(0);
##    cropped_image_id = bvxm_batch.commit_output(1);  
##    print("Saving Image");
##    bvxm_batch.init_process("SaveImageViewProcess");
##    bvxm_batch.set_input_from_db(0,cropped_image_id);
##    bvxm_batch.set_input_string(1,"./ini"+str(i)+".png");
##    bvxm_batch.run_process();
        ##  if statuscode:
##    cropped_cam_id = bvxm_batch.commit_output(0);
##    cropped_image_id = bvxm_batch.commit_output(1);  
##
##    # RPC camera correction 
##    bvxm_batch.init_process("bvxmGenerateEdgeMapProcess");
##    bvxm_batch.set_input_from_db(0,cropped_image_id);
##    bvxm_batch.set_params_process(python_path + "edge_map_params.xml");
##    bvxm_batch.run_process();
##    cropped_edge_image_id = bvxm_batch.commit_output(0);
##      
##    bvxm_batch.init_process("bvxmRpcRegistrationProcess");
##    bvxm_batch.set_input_from_db(0,voxel_world_id);
##    bvxm_batch.set_input_from_db(1,cropped_cam_id);
##    bvxm_batch.set_input_from_db(2,cropped_edge_image_id);
##    if i<num_train:
##      bvxm_batch.set_input_bool(3,0);
##    else:
##      bvxm_batch.set_input_bool(3,1);
##    bvxm_batch.set_params_process(python_path + "rpc_registration_parameters.xml");
##    bvxm_batch.run_process();
##    cam_id = bvxm_batch.commit_output(0);
##    voxel_image_id = bvxm_batch.commit_output(1);
##
##
##    print("Saving Image");
##    bvxm_batch.init_process("SaveImageViewProcess");
##    bvxm_batch.set_input_from_db(0,cropped_image_id);
##    bvxm_batch.set_input_string(1,"./ini"+str(i)+".png");
##    bvxm_batch.run_process();
##    
##    
##    map_type="10bins_1d_radial";
##    print("Illumination Index");
##    bvxm_batch.init_process("bvxmIllumIndexProcess");
##    bvxm_batch.set_input_string(0,map_type);
##    bvxm_batch.set_input_string(1,image_filename);
##    bvxm_batch.set_input_unsigned(2,8);
##    bvxm_batch.set_input_unsigned(3,0);
##    bvxm_batch.run_process();
##    bin_id = bvxm_batch.commit_output(0);
##
##    app_type="apm_mog_grey";
##
##    print voxel_world_id;
##    # Normalizing images
##    print(" Normalizing Image ");
##    bvxm_batch.init_process("bvxmNormalizeImageProcess");
##    bvxm_batch.set_params_process("./normalize.xml");
##    bvxm_batch.set_input_from_db(0,cropped_image_id);
##    bvxm_batch.set_input_from_db(1,cam_id);
##    bvxm_batch.set_input_from_db(2,voxel_world_id);
##    bvxm_batch.set_input_string(3,app_type);
##    bvxm_batch.set_input_from_db(4,bin_id);
##    bvxm_batch.run_process();
##    normalized_img_id = bvxm_batch.commit_output(0);
##    float1_id = bvxm_batch.commit_output(1);
##    float2_id = bvxm_batch.commit_output(2);
##
##    print("Saving Image");
##    bvxm_batch.init_process("SaveImageViewProcess");
##    bvxm_batch.set_input_from_db(0,normalized_img_id);
##    bvxm_batch.set_input_string(1,"./normalized"+str(i)+".png");
##    bvxm_batch.run_process();
##    curr_image_id=normalized_img_id;
##
##    print("Updating World");
##    bvxm_batch.init_process("bvxmUpdateProcess");
##    bvxm_batch.set_input_from_db(0,normalized_img_id);
##    bvxm_batch.set_input_from_db(1,cam_id);
##    bvxm_batch.set_input_from_db(2,voxel_world_id);
##    bvxm_batch.set_input_string(3,app_type);
##    bvxm_batch.set_input_from_db(4,bin_id);
##    bvxm_batch.run_process();
##    out_img_id = bvxm_batch.commit_output(0);
##    mask_img_id = bvxm_batch.commit_output(1);
##
##    print("Display changes");
##    bvxm_batch.init_process("bvxmChangeDetectionDisplayProcess");
##    bvxm_batch.set_params_process("./change_display_params.xml");
##    bvxm_batch.set_input_from_db(0,curr_image_id);
##    bvxm_batch.set_input_from_db(1,out_img_id);
##    bvxm_batch.set_input_from_db(2,mask_img_id);
##    bvxm_batch.run_process();
##    change_img_id = bvxm_batch.commit_output(0);
##
##    print("Saving Image");
##    bvxm_batch.init_process("SaveImageViewProcess");
##    bvxm_batch.set_input_from_db(0,change_img_id);
##    bvxm_batch.set_input_string(1,"./change"+str(i)+".png");
##    bvxm_batch.run_process();
##
##    print("Writing World");
##    bvxm_batch.init_process("bvxmSaveOccupancyRaw");
##    bvxm_batch.set_input_from_db(0,voxel_world_id);
##    bvxm_batch.set_input_string(1,"./world.raw");
##    bvxm_batch.run_process();



