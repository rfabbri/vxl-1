% This script compute an image pyramid for a dataset
% (c) Nhon Trinh
% Date: June 27, 2009

%Change directories

% path(path,'/home/ausumezbas/Desktop/maruthi_research/gpb_code');
% path(path,'/home/ausumezbas/Desktop/maruthi_research/gpb_code/gpb_sp_to');
% path(path,'/home/ausumezbas/Desktop/maruthi_research/gpb_code/BSR/grouping/lib');
path(path,'/home/ausumezbas/Desktop/maruthi_research/gpb_code/TO-edge-detectorToolbox');

%Edits nhon script
clear all
close all

warning off

filename='image_pyramid_data.txt';
%Read in file of all directories
fid=fopen(filename);
line=1;
while 1
    tline = fgetl(fid);
    if ~ischar(tline)
        break
    end
    input_dir{line}=tline;
    line=line+1;
end
fclose(fid); 
regexp = '*.png';

rsz=1;
threshold=0.1;

%% Process

% Make some annoucement
fprintf(1, '-------------------------------------------\n');
fprintf(1, 'Computing gpb Edges for the ETHZ dataset   \n');
fprintf(1, 'threshold = %g\n', threshold);
fprintf(1, '-------------------------------------------\n');

sigma = 1;
n = 1; % interpolate 2-fold
threshold = 5;
QaD_flag = 1;

for dirs=1:length(input_dir)
    
    directory = input_dir{dirs};
    file_list = dir(fullfile(directory, regexp));
    fprintf(1, 'input_dir = %s\n', directory);
    
    tstart=tic;
    for k = 1 : length(file_list)
        
        % name of input image
        input_filename = file_list(k).name;
        input_image_file = fullfile(directory, input_filename);
        
        fprintf('Computing Edges for %s ...\n', input_filename);
        
        img=imread(input_image_file);
        
        %Start computing edges
        [TO_edgemap, gen_edgemap] = third_order_edge_detector(img, sigma, n, threshold, QaD_flag);
        
        %Convert to c++
        TO_edgemap(:,1:2) = TO_edgemap(:,1:2)-1;
        
        %Write out edge map
        [h, w, n] = size(imread(input_image_file));
        
        % remove extension of the image
        [pathstr, image_name, ext, versn] = fileparts(input_image_file);
        temp_name = [image_name '_to.edg'];
        edg_filename = fullfile(directory, temp_name);
        
        %Writing out edge name
        save_edg(edg_filename, TO_edgemap, [w h]);
            

    end
    v=toc(tstart);
    disp(['Elapsed Time: ' num2str(v)]);
    fprintf(1, ' done.\n');
end