'''
Created on Jan 25, 2011

@author: bm
'''
#Example script to calculate Temporal Entropy

import dsm_batch;
dsm_batch.register_processes();
dsm_batch.register_datatypes();

class dbvalue:
    def __init__(self, index, type):
        self.id = index #unsigned integer
        self.type = type #string
    
class id_pair:
    def __init__(self, first, second):
        self.first = first;
        self.second = second;   
        

#python libraries for file and utility functions
import os;
import shutil;
        
#variables for temporal entropy calculation      
python_path = "./"
westin_experiment_path = python_path + "westin_experiments/"
if os.path.exists(westin_experiment_path) != 1:
    os.mkdir(westin_experiment_path);
  
data_path = "C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\data\\westin1\\";
video_glob = data_path+"*.jpg";
nbins = 16;
temporal_entropy_dat = westin_experiment_path + "temporal_entropy.dat"
temporal_entropy_bin = westin_experiment_path + "temporal_entropy.bin"
database = westin_experiment_path + "database.bin"
neighborhood_mfile = westin_experiment_path + "neighborhood.m"
neighborhood_xml = westin_experiment_path + "neighborhood.xml"
ncn_sptr_bin = westin_experiment_path + "ncn_sptr.bin"

dsm_batch.clear();

dsm_batch.init_process("dsmCreateNcnSptrProcess")
dsm_batch.set_params_process("./westin_experiments/dsm_parameters.xml")
dsm_batch.run_process();
(id,type) = dsm_batch.commit_output(0);
ncn_sptr = dbvalue(id,type);

dsm_batch.init_process("dsmReadTemporalEntropyBinProcess")
dsm_batch.set_input_string(0,temporal_entropy_bin);
dsm_batch.set_input_from_db(1,ncn_sptr)
dsm_batch.run_process();


dsm_batch.init_process("dsmBuildNcnProcess")
dsm_batch.set_input_from_db(0,ncn_sptr)
dsm_batch.run_process();

dsm_batch.init_process("dsmWriteNeighborhoodMfile")
dsm_batch.set_input_string(0,neighborhood_mfile);
dsm_batch.set_input_from_db(1,ncn_sptr);
dsm_batch.run_process();

dsm_batch.init_process("dsmWriteNeighborhoodXml")
dsm_batch.set_input_string(0,neighborhood_xml)
dsm_batch.set_input_from_db(1,ncn_sptr)
dsm_batch.run_process()

dsm_batch.init_process("dsmWriteNcnSptrBinProcess")
dsm_batch.set_input_string(0,ncn_sptr_bin)
dsm_batch.set_input_from_db(1,ncn_sptr)
dsm_batch.run_process();


dsm_batch.print_db();




