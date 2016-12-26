#This is a sample script for:
# testing the hierarchy created by the learning procedure
# created by Ozge C. Ozcanli - July 15, 09

class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string

import dbrec_batch
dbrec_batch.register_processes();
dbrec_batch.register_datatypes();

python_path = "C:\\projects\\vehicles\\";

import os;
import shutil;

learning_dir = "output_learning";
input_learning_path = python_path+learning_dir+"/";
model_path = input_learning_path+"models/";

# list of the images
f=open(python_path + "images.txt", 'r')
image_fnames=f.readlines();
f.close();
image_size = "128";
images_path = "Y:\\vehicles\\database\\vehicles\\"+image_size+"x"+image_size+"\\";
masks_path = "Y:\\vehicles\\database\\vehicles\\"+image_size+"x"+image_size+"\\mask\\";
train_suffix = "_055-135-no_sun.png";

import array;
#test_dbs_cnt = 8;
test_dbs_cnt = 1;
test_dbs_arr = [" " for col in range(test_dbs_cnt)];
test_dbs_arr[0] = "_055-135-068-030.png";  # for now test with only a single database
#test_dbs_arr[0] = "_055-135-024-090.png";
#test_dbs_arr[1] = "_055-135-024-180.png";
#test_dbs_arr[2] = "_055-135-024-270.png";
#test_dbs_arr[3] = "_055-135-045-045.png";
#test_dbs_arr[4] = "_055-135-045-135.png";
#test_dbs_arr[5] = "_055-135-045-225.png";
#test_dbs_arr[6] = "_055-135-068-030.png";
#test_dbs_arr[7] = "_055-135-068-150.png";

number_of_classes = 3;
f2=open(python_path + "classes_"+str(number_of_classes)+".txt", 'r')
class_names=f2.readlines();
f2.close();
class_id_file="classes_and_ids_"+str(number_of_classes)+".txt";
class_id_file_path=python_path + "classes_and_ids_"+str(number_of_classes)+".txt";

hierarchy_name = "hierarchy";
output_parse_images=0;     # make this 1 if want to output the parse images (posterior maps of class and non-class)
max_type_id = 20;
output_path = python_path+"output_detection_"+str(number_of_classes)+"_classes_"+image_size+"_"+image_size+"_learning_dir_"+learning_dir+"/";
output_path_images = output_path + "out_images/";
output_path_parses = output_path + "out_parse_images/";

if os.path.exists(output_path) != 1:
  os.mkdir(output_path);
if os.path.exists(output_path_images) != 1:
  os.mkdir(output_path_images);
if os.path.exists(output_path_parses) != 1:
  os.mkdir(output_path_parses);

class_prior = 0.1;



print("Load the learned hierarchy");
dbrec_batch.init_process("dbrecParseHierarchyXMLProcess");
dbrec_batch.set_input_string(0, input_learning_path + hierarchy_name + ".xml");
dbrec_batch.run_process();
(id, type) = dbrec_batch.commit_output(0);
hierarchy = dbvalue(id, type);

print("visualize the hierarchy");
dbrec_batch.init_process("dbrecHierarchyVisualizeProcess");
dbrec_batch.set_input_from_db(0, hierarchy);
dbrec_batch.set_input_string(1, output_path + hierarchy_name + ".svg");
dbrec_batch.run_process();

#create an experiment statistics object to keep track of TPs and FPs overall.
print("Create exp stat instance");
dbrec_batch.init_process("dborlExpStatInitializeProcess");
dbrec_batch.run_process();
(id,type)=dbrec_batch.commit_output(0);
exp_stat = dbvalue(id, type);

#create the category info file
print("Create category info set instance");
dbrec_batch.init_process("dborlCategoryInfoSetCreateProcess");
dbrec_batch.set_input_string(0, class_id_file_path);
dbrec_batch.run_process();
(id,type)=dbrec_batch.commit_output(0);
cat_info = dbvalue(id, type);

print("write category info set instance as an xml file");
dbrec_batch.init_process("dborlCategoryInfoSetWriteXMLProcess");
dbrec_batch.set_input_from_db(0, cat_info);
dbrec_batch.set_input_string(1, output_path+class_id_file+"_output.xml");
dbrec_batch.run_process();

for tk in range(0,test_dbs_cnt, 1):
  #create an experiment statistics object to keep track of TPs and FPs for this db
  print("Create exp stat instance");
  dbrec_batch.init_process("dborlExpStatInitializeProcess");
  dbrec_batch.run_process();
  (id,type)=dbrec_batch.commit_output(0);
  exp_stat_tk = dbvalue(id, type);

  for i in range(0,len(image_fnames),1):
    print(str(i));

    image_name=image_fnames[i];
    image_name=image_name[:-1];
    image_filename=images_path + image_name + test_dbs_arr[tk];
    #image_mask_filename=masks_path + image_name + test_dbs_arr[tk];

    # the order of the test images in the test database are the same as the training database, so use the same class names
    class_name=class_names[i];
    class_name=class_name[:-1];

    # load the image
    print("Load Image");
    dbrec_batch.init_process("vilLoadImageViewProcess");
    dbrec_batch.set_input_string(0,image_filename);
    dbrec_batch.run_process();
    (id,type)=dbrec_batch.commit_output(0);
    frame = dbvalue(id, type);
    
    print("Parse the image");
    dbrec_batch.init_process("dbrecImgParseProcess");
    dbrec_batch.set_input_from_db(0, frame);  # the process converts rgb images to grey and scales to [0,1]
    dbrec_batch.set_input_from_db(1, hierarchy);
    dbrec_batch.set_input_float(2, class_prior);
    dbrec_batch.set_input_string(3, model_path);
    dbrec_batch.run_process();
    (id,type)=dbrec_batch.commit_output(0);
    cf = dbvalue(id, type);
    
    if output_parse_images==1:
      print("Retrieve result maps from context factory");
      for display_type_id in range(1,max_type_id,1):
        dbrec_batch.init_process("dbrecContextFactoryGetMapProcess");
        dbrec_batch.set_input_from_db(0,cf);
        dbrec_batch.set_input_from_db(1,hierarchy);
        dbrec_batch.set_input_from_db(2,frame);
        dbrec_batch.set_input_int(3,display_type_id);  #type id
        dbrec_batch.set_input_int(4,0);  # map id
        statuscode=dbrec_batch.run_process();
        if statuscode:
          (id, type) = dbrec_batch.commit_output(0);
          out_map = dbvalue(id, type);
          (id, type) = dbrec_batch.commit_output(1);
          out_map_byte = dbvalue(id, type);

          dbrec_batch.init_process("vilSaveImageViewProcess");
          dbrec_batch.set_input_from_db(0, out_map_byte);
          dbrec_batch.set_input_string(1,output_path_parses+image_name+"_class_posterior_"+class_name+"_part_id_"+str(display_type_id)+test_dbs_arr[tk]);
          dbrec_batch.run_process();

          dbrec_batch.init_process("dbrecContextFactoryGetMapProcess");
          dbrec_batch.set_input_from_db(0,cf);
          dbrec_batch.set_input_from_db(1,hierarchy);
          dbrec_batch.set_input_from_db(2,frame);
          dbrec_batch.set_input_int(3,display_type_id);  #type id
          dbrec_batch.set_input_int(4,1);  # map id
          dbrec_batch.run_process();
          (id, type) = dbrec_batch.commit_output(0);
          out_map2 = dbvalue(id, type);
          (id, type) = dbrec_batch.commit_output(1);
          out_map_byte2 = dbvalue(id, type);

          dbrec_batch.init_process("vilSaveImageViewProcess");
          dbrec_batch.set_input_from_db(0, out_map_byte2);
          dbrec_batch.set_input_string(1,output_path_parses+image_name+"_non_class_posterior_"+class_name+"_part_id_"+str(display_type_id)+test_dbs_arr[tk]);
          dbrec_batch.run_process();
        
          dbrec_batch.remove_data(out_map.id);
          dbrec_batch.remove_data(out_map_byte.id);
          dbrec_batch.remove_data(out_map2.id);
          dbrec_batch.remove_data(out_map_byte2.id);

    print("Classify Image");
    dbrec_batch.init_process("dbrecClassifyImageProcess");
    dbrec_batch.set_input_from_db(0, cf);
    dbrec_batch.set_input_from_db(1, hierarchy);
    dbrec_batch.set_input_from_db(2, frame);
    dbrec_batch.run_process();
    (id,type)=dbrec_batch.commit_output(0);
    output_class_id = dbvalue(id, type);
    output_class = dbrec_batch.get_input_int(id);
    (id,type)=dbrec_batch.commit_output(1);
    output_img = dbvalue(id, type);
    
    dbrec_batch.init_process("vilSaveImageViewProcess");
    dbrec_batch.set_input_from_db(0, output_img);
    dbrec_batch.set_input_string(1, output_path_images + "class_"+class_name+"_classified_"+str(output_class)+"_"+image_name+test_dbs_arr[tk]);
    dbrec_batch.run_process();

    print("Evaluate this classification");
    dbrec_batch.init_process("dborlExpStatEvalClassificationWIDProcess");
    dbrec_batch.set_input_from_db(0, exp_stat);
    dbrec_batch.set_input_from_db(1, cat_info);
    dbrec_batch.set_input_string(2, class_name);
    dbrec_batch.set_input_from_db(3, output_class_id);
    dbrec_batch.run_process();

    dbrec_batch.init_process("dborlExpStatEvalClassificationWIDProcess");
    dbrec_batch.set_input_from_db(0, exp_stat_tk);
    dbrec_batch.set_input_from_db(1, cat_info);
    dbrec_batch.set_input_string(2, class_name);
    dbrec_batch.set_input_from_db(3, output_class_id);
    dbrec_batch.run_process();
    
    dbrec_batch.remove_data(frame.id);
    dbrec_batch.remove_data(cf.id);
    dbrec_batch.remove_data(output_img.id);
    dbrec_batch.remove_data(output_class_id.id);

  #print the accumulated result of the experiment for these test db
  dbrec_batch.init_process("dborlExpStatPrintProcess");
  dbrec_batch.set_input_from_db(0, exp_stat_tk);
  dbrec_batch.set_input_string(1, output_path + "out" + test_dbs_arr[tk] + ".txt");
  dbrec_batch.run_process();

#print the accumulated result of the experiment for these test db
dbrec_batch.init_process("dborlExpStatPrintProcess");
dbrec_batch.set_input_from_db(0, exp_stat);
dbrec_batch.set_input_string(1, output_path + "out_all"+train_suffix+".txt");
dbrec_batch.run_process();



  

 

