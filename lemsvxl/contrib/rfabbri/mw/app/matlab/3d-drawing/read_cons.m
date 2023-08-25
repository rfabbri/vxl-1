function [cons]=read_cons(filename,img_filename,lengthThreshold,ID)

addpath export_fig
base_dir='/vision/scratch/maruthi/DataSets/cats_dogs/trainval_bp/';
[path,name,str]=fileparts(filename);
cons=read_cem_file(filename);
