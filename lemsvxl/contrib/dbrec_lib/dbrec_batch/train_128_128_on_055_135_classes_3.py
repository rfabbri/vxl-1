#This is a sample script for:
# training appearance models for class recognition using dbrec
# created by Ozge C. Ozcanli - June 23, 09

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

output_path = python_path+"output_learning/";
model_path = output_path+"models/";

# list of the images
f=open("images.txt", 'r')
image_fnames=f.readlines();
f.close();
images_path = "Y:\\vehicles\\database\\vehicles\\128x128\\";
masks_path = "Y:\\vehicles\\database\\vehicles\\128x128\\mask\\";
train_suffix = "_055-135-no_sun.png";

number_of_classes = 3;

f2=open(python_path + "classes_"+str(number_of_classes)+".txt", 'r')
class_names=f2.readlines();
f2.close();

class_id_file="classes_and_ids_"+str(number_of_classes)+".txt";
class_id_file_path=python_path + "classes_and_ids_"+str(number_of_classes)+".txt";

if os.path.exists(output_path) != 1:
  os.mkdir(output_path);
if os.path.exists(model_path) != 1:
  os.mkdir(model_path);

N = 5; # number of parts to be selected to construct a new hierarchy
prim_radius = 5;   # the new OR nodes's radius will be depth of the new node*prim_radius

class_prior = 0.1;
ndirs = 4;
lambda_min = 1.0;
lambda_max = 2.0;
lambda_inc = 1.0;

print("Create the random hierarchy");
dbrec_batch.init_process("dbrecImgCreateRndHierarchyProcess");
dbrec_batch.set_input_int(0, ndirs);
dbrec_batch.set_input_float(1, lambda_min);
dbrec_batch.set_input_float(2, lambda_max);
dbrec_batch.set_input_float(3, lambda_inc);
dbrec_batch.set_input_int(4, number_of_classes);
dbrec_batch.run_process();
(id, type) = dbrec_batch.commit_output(0);
random_hierarchy = dbvalue(id, type);

print("visualize the hierarchy");
dbrec_batch.init_process("dbrecHierarchyVisualizeProcess");
dbrec_batch.set_input_from_db(0, random_hierarchy);
dbrec_batch.set_input_string(1, output_path + "random_hierarchy.svg");
dbrec_batch.run_process();
'''
for type_id in range(0,13,1):
  dbrec_batch.init_process("dbrecHierarchyVisualizeSampledClass");
  dbrec_batch.set_input_from_db(0, random_hierarchy);
  dbrec_batch.set_input_int(1, -1);  # pass class id -1 if just wanna draw the composition
  dbrec_batch.set_input_int(2, type_id);
  dbrec_batch.set_input_string(3, output_path + "random_hierarchy_primitive_" + str(type_id) + ".svg");
  dbrec_batch.run_process();
'''
print("write the hierarchy as xml");
dbrec_batch.init_process("dbrecWriteHierarchyXMLProcess");
dbrec_batch.set_input_from_db(0, random_hierarchy);
dbrec_batch.set_input_string(1, output_path + "random_hierarchy.xml");
dbrec_batch.run_process();

#create the category info file
print("Create category info set instance");
dbrec_batch.init_process("dborlCategoryInfoSetCreateProcess");
dbrec_batch.set_input_string(0, class_id_file_path);
dbrec_batch.run_process();
(id,type)=dbrec_batch.commit_output(0);
cat_info = dbvalue(id, type);

print("Initialize the appearance learner");
dbrec_batch.init_process("dbrecImgWeibullLearnerInitProcess");
dbrec_batch.set_input_from_db(0, random_hierarchy);
dbrec_batch.run_process();
(id, type) = dbrec_batch.commit_output(0);
learner = dbvalue(id, type);

for i in range(0,len(image_fnames),1):
  print(str(i));

  image_name=image_fnames[i];
  image_name=image_name[:-1];
  image_filename=images_path + image_name + train_suffix;
  image_mask_filename=masks_path + image_name + train_suffix;

  class_name=class_names[i];
  class_name=class_name[:-1];

  print("Load Image");
  dbrec_batch.init_process("vilLoadImageViewProcess");
  dbrec_batch.set_input_string(0,image_filename);
  dbrec_batch.run_process();
  (id,type)=dbrec_batch.commit_output(0);    
  frame = dbvalue(id, type);
  
  print("Load the gt map to be used as the fg map during training");
  dbrec_batch.init_process("vilLoadImageViewProcess");
  dbrec_batch.set_input_string(0,image_mask_filename);
  dbrec_batch.run_process();
  (id,type)=dbrec_batch.commit_output(0);
  gt_map = dbvalue(id, type);
  
  # the class instances in these gt maps are black so threshold below 128 to get the gt masks
  dbrec_batch.init_process("vilThresholdImageProcess");
  dbrec_batch.set_input_from_db(0, gt_map);
  dbrec_batch.set_input_float(1, 128);
  dbrec_batch.set_input_bool(2, 0);
  dbrec_batch.run_process();
  (id,type)=dbrec_batch.commit_output(0);
  gt_mask = dbvalue(id, type);

  print("Parse the image");
  dbrec_batch.init_process("dbrecImgParseProcess");
  dbrec_batch.set_input_from_db(0, frame);  # the process converts rgb images to grey and scales to [0,1]
  dbrec_batch.set_input_from_db(1, random_hierarchy);
  dbrec_batch.set_input_float(2, class_prior);
  dbrec_batch.set_input_string(3, model_path);
  dbrec_batch.run_process();
  (id,type)=dbrec_batch.commit_output(0);
  cf = dbvalue(id, type);
  
  print("Update the appearance learner");
  dbrec_batch.init_process("dbrecImgWeibullLearnerUpdateProcess");
  dbrec_batch.set_input_from_db(0, learner);
  dbrec_batch.set_input_from_db(1, cf);
  dbrec_batch.set_input_from_db(2, gt_mask);
  dbrec_batch.process_init();                   # initializes the third input
  dbrec_batch.run_process();
  
  dbrec_batch.remove_data(frame.id);
  dbrec_batch.remove_data(gt_map.id);
  dbrec_batch.remove_data(gt_mask.id);
  dbrec_batch.remove_data(cf.id);

print("Construct the models and print");
dbrec_batch.init_process("dbrecImgWeibullLearnerPrintProcess");
dbrec_batch.set_input_from_db(0, learner);
dbrec_batch.set_input_string(1, model_path);
dbrec_batch.run_process();
  
print("Initialize the part learner for depth 1"); # pick some parts from the primitive layer
dbrec_batch.init_process("dbrecImgHierarchyLearnerInitProcess");
dbrec_batch.set_input_from_db(0, random_hierarchy);
dbrec_batch.set_input_int(1, 1);            # primitive layers depth is 1
dbrec_batch.run_process();
(id, type) = dbrec_batch.commit_output(0);
hlearner = dbvalue(id, type);

for i in range(0,len(image_fnames),1):
  print(str(i));

  image_name=image_fnames[i];
  image_name=image_name[:-1];
  image_filename=images_path + image_name + train_suffix;
  image_mask_filename=masks_path + image_name + train_suffix;

  class_name=class_names[i];
  class_name=class_name[:-1];
  
  dbrec_batch.init_process("dborlCategoryInfoGetIDProcess");
  dbrec_batch.set_input_from_db(0, cat_info);
  dbrec_batch.set_input_string(1, class_name);
  val=dbrec_batch.run_process();
  (id,type)=dbrec_batch.commit_output(0);
  cat_id_db = dbvalue(id,type);
  if val==1:
    cat_id = dbrec_batch.get_input_int(cat_id_db.id);
    print("returned category id: "+str(cat_id)+" for category: "+class_name+"!\n");

  print("Load Image");
  dbrec_batch.init_process("vilLoadImageViewProcess");
  dbrec_batch.set_input_string(0,image_filename);
  dbrec_batch.run_process();
  (id,type)=dbrec_batch.commit_output(0);
  frame = dbvalue(id, type);

  print("Load the gt map to be used as the fg map during training");
  dbrec_batch.init_process("vilLoadImageViewProcess");
  dbrec_batch.set_input_string(0,image_mask_filename);
  dbrec_batch.run_process();
  (id,type)=dbrec_batch.commit_output(0);
  gt_map = dbvalue(id, type);
  
  # the class instances in these gt maps are black so threshold below 128 to get the gt masks
  dbrec_batch.init_process("vilThresholdImageProcess");
  dbrec_batch.set_input_from_db(0, gt_map);
  dbrec_batch.set_input_float(1, 128);
  dbrec_batch.set_input_bool(2, 0);
  dbrec_batch.run_process();
  (id,type)=dbrec_batch.commit_output(0);
  gt_mask = dbvalue(id, type);

  print("Parse the image");
  dbrec_batch.init_process("dbrecImgParseProcess");
  dbrec_batch.set_input_from_db(0, frame);  # the process converts rgb images to grey and scales to [0,1]
  dbrec_batch.set_input_from_db(1, random_hierarchy);
  dbrec_batch.set_input_float(2, class_prior);
  dbrec_batch.set_input_string(3, model_path);
  dbrec_batch.run_process();
  (id,type)=dbrec_batch.commit_output(0);
  cf = dbvalue(id, type);

  print("Update the part learner");
  dbrec_batch.init_process("dbrecImgHierarchyLearnerUpdateProcess");
  dbrec_batch.set_input_from_db(0, hlearner);
  dbrec_batch.set_input_from_db(1, cf);
  dbrec_batch.set_input_from_db(2, gt_mask);
  dbrec_batch.set_input_from_db(4, cat_id_db);
  dbrec_batch.process_init();                   # initializes the third input
  dbrec_batch.run_process();
  
  dbrec_batch.remove_data(frame.id);
  dbrec_batch.remove_data(gt_map.id);
  dbrec_batch.remove_data(gt_mask.id);
  dbrec_batch.remove_data(cf.id);
  
# select top N best parts for each class and create a new OR node with its selected parts for each class
print("Construct a new hierarchy and print");
dbrec_batch.init_process("dbrecImgHierarchyLearnerConstructProcess");
dbrec_batch.set_input_from_db(0, hlearner);
dbrec_batch.set_input_from_db(1, random_hierarchy);  # needs the old hierarchy to create new unique ids for the new parts
dbrec_batch.set_input_int(2, N);
dbrec_batch.set_input_float(3, prim_radius);
dbrec_batch.run_process();
(id,type)=dbrec_batch.commit_output(0);
new_hierarchy = dbvalue(id, type);

print("write the hierarchy as xml");
dbrec_batch.init_process("dbrecWriteHierarchyXMLProcess");
dbrec_batch.set_input_from_db(0, new_hierarchy);
dbrec_batch.set_input_string(1, output_path + "hierarchy.xml");
dbrec_batch.run_process();

print("visualize the hierarchy");
dbrec_batch.init_process("dbrecHierarchyVisualizeProcess");
dbrec_batch.set_input_from_db(0, new_hierarchy);
dbrec_batch.set_input_string(1, output_path + "hierarchy.svg");
dbrec_batch.run_process();

 

