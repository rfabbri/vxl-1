% This script compute an image pyramid for a dataset
% (c) Nhon Trinh
% Date: June 27, 2009

%Edits nhon script
clear all
close all
warning off

%Read in file of all directories
fid=fopen('/users/mnarayan/data/mnarayan/weizman_segmentation/weizman_segmentation_object_list.txt');
line=1;
while 1
    tline = fgetl(fid);
    if ~ischar(tline)
        break
    end
    input_dir{line}=tline;
    output_dir{line}=[tline '/image_pyramid'];
    line=line+1;
end
fclose(fid);
 
regexp = '*.png';

step = 2 ^ (1/4);
num_steps = 10;

%% Process

% Make some annoucement
fprintf(1, '-------------------------------------------\n');
fprintf(1, 'Building image pyramid for the Hoofed Animals dataset\n');
fprintf(1, 'step = %g\n', step);
fprintf(1, 'num_steps = %d\n', num_steps);
fprintf(1, '-------------------------------------------\n');

for dirs=1:length(input_dir)
    
    directory = input_dir{dirs};
    file_list = dir(fullfile(directory, regexp));
    fprintf(1, 'input_dir = %s\n', directory);
    
    for k = 1 : length(file_list)
        % name of input image
        input_filename = file_list(k).name;
        input_image_file = fullfile(directory, input_filename);
        
        fprintf('Building a pyramid for %s ...', input_filename);
        
        % load input image
        im = imread(input_image_file);
        
        % remove extension of the image
        [pathstr, image_name, ext, versn] = fileparts(input_image_file);
        
        % create a folder for each image
        output_image_dir = output_dir{dirs};
        if (~exist(output_image_dir, 'dir'))
            mkdir(output_image_dir);
        end;
        
        % create multiple images, each is a scaled version of the original image
        for kk = 0 : num_steps
            scale = 1 / step ^ (kk);
            im_scaled = imresize(im, scale, 'bicubic');
            
            im_scaled_filename = [image_name, '_', num2str(kk, '%02d'), '.png'];
            
            % save to image's folder
            imwrite(im_scaled, fullfile(output_image_dir, im_scaled_filename));
        end;
        
        fprintf(1, ' done.\n');
    end
end
