#This is a sample script for:
# creating a bar plot to compare to the PCA
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

image_size = "128";
number_of_classes = 3;
pca_path = "C:\\projects\\vehicles_pca\\output_detection_"+str(number_of_classes)+"_classes_"+image_size+"_"+image_size+"_0.98/";

learning_dir = "output_learning";
comparison_path = "C:\\projects\\vehicles\\output_detection_"+str(number_of_classes)+"_classes_"+image_size+"_"+image_size+"_learning_dir_"+learning_dir+"/";

train_db = "_055-135";
test_db = "_055-135-068-030.png";  # for now test with only a single database

width = 1200;
height = 600;
# create the bar plot instance and add a bar from each output file
dbrec_batch.init_process("bsvgBarPlotInitializeProcess");
dbrec_batch.set_input_float(0, width);
dbrec_batch.set_input_float(1, height);
dbrec_batch.set_input_string(2, image_size + "x" + image_size + " " + str(number_of_classes) + "-classes test_db: " + test_db);
dbrec_batch.run_process();
(id, type) = dbrec_batch.commit_output(0);
bar_plot = dbvalue(id, type);

# create an exp instance to read the stats from output files of the experiments
print("Create exp stat instance");
dbrec_batch.init_process("dborlExpStatInitializeProcess");
dbrec_batch.run_process();
(id,type)=dbrec_batch.commit_output(0);
exp_stat = dbvalue(id, type);

# add the bar for PCA
dbrec_batch.init_process("dborlExpStatReadProcess");
dbrec_batch.set_input_from_db(0, exp_stat);
dbrec_batch.set_input_string(1, pca_path + "out" + test_db + ".txt");
dbrec_batch.run_process();

dbrec_batch.init_process("dborlExpStatGetValuesProcess");
dbrec_batch.set_input_from_db(0, exp_stat);
dbrec_batch.run_process();
(id,type)=dbrec_batch.commit_output(0);
tpr = dbvalue(id, type);

dbrec_batch.init_process("bsvgBarPlotAddProcess");
dbrec_batch.set_input_from_db(0, bar_plot);
dbrec_batch.set_input_from_db(1, tpr);     # height
dbrec_batch.set_input_string(2, "pca");  # bar title
dbrec_batch.set_input_string(3, "red");  # color
dbrec_batch.run_process();

# add the bar for dbrec
dbrec_batch.init_process("dborlExpStatReadProcess");
dbrec_batch.set_input_from_db(0, exp_stat);
dbrec_batch.set_input_string(1, comparison_path + "out" + test_db + ".txt");
dbrec_batch.run_process();

dbrec_batch.init_process("dborlExpStatGetValuesProcess");
dbrec_batch.set_input_from_db(0, exp_stat);
dbrec_batch.run_process();
(id,type)=dbrec_batch.commit_output(0);
tpr2 = dbvalue(id, type);

dbrec_batch.init_process("bsvgBarPlotAddProcess");
dbrec_batch.set_input_from_db(0, bar_plot);
dbrec_batch.set_input_from_db(1, tpr2);     # height
dbrec_batch.set_input_string(2, "dbrec");  # bar title
dbrec_batch.set_input_string(3, "blue");  # color
dbrec_batch.run_process();

dbrec_batch.init_process("bsvgBarPlotWriteProcess");
dbrec_batch.set_input_from_db(0, bar_plot);
dbrec_batch.set_input_string(1, comparison_path + "plot" + test_db + ".svg");
dbrec_batch.run_process();



  

 

