% This script compute an image pyramid for a dataset
% (c) Nhon Trinh
% Date: June 27, 2009

%Change directories

path(path,'/home/ausumezbas/Desktop/maruthi_research/gpb_code');
path(path,'/home/ausumezbas/Desktop/maruthi_research/gpb_code/gpb_sp_to');
path(path,'/home/ausumezbas/Desktop/maruthi_research/gpb_code/BSR/grouping/lib');

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

for dirs=1:length(input_dir)
    
    directory = input_dir{173};
    file_list = dir(fullfile(directory, regexp));
    fprintf(1, 'input_dir = %s\n', directory);
    
    tstart=tic;
    for k = 1 : length(file_list)
        
        % name of input image
        input_filename = file_list(k).name;
        input_image_file = fullfile(directory, input_filename);
        
        fprintf('Computing Edges for %s ...', input_filename);
        
        %Start computing edges
        [gPb_map, Ts_edge_map, gen_edge_map] = globalPb_subpixel_TO(input_image_file, '', rsz, threshold);
        
        %Convert to c++
        Ts_edge_map(:,1:2) = Ts_edge_map(:,1:2)-1;
        
        %Write out edge map
        [h, w, n] = size(imread(input_image_file));
        
        % remove extension of the image
        [pathstr, image_name, ext, versn] = fileparts(input_image_file);
        temp_name = [image_name '.edg'];
        edg_filename = fullfile(directory, temp_name);
        
        %Writing out edge name
        save_edg(edg_filename, Ts_edge_map, [w h]);
            

    end
    v=toc(tstart);
    disp(['Elapsed Time: ' num2str(v)]);
    fprintf(1, ' done.\n');
end


