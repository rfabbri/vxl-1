clear all
close all

voc2009_files=textread('weiz_seg_list.txt','%s');
voc2010_files=textread('/vision/scratch/maruthi/Voc2010_TrainVal/objectlist.txt','%s');

all_files=voc2009_files; % ; voc2010_files];

for k=1:length(all_files)
    file_name=[all_files{k} '/src_bw'];
    parallel_compute_image_pyramid(file_name);
    file_name=[all_files{k} '/src_color'];
    parallel_compute_image_pyramid(file_name);
end