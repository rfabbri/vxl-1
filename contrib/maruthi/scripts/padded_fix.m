% This script compute an image pyramid for a dataset
% (c) Nhon Trinh
% Date: June 27, 2009

%Change directories
function []=parallel_compute_gpb_edges(directory)

path(path,'/home/ausumezbas/Desktop/maruthi_research/gpb_code');
path(path,'/home/ausumezbas/Desktop/maruthi_research/gpb_code/gpb_sp_to');
path(path,'/home/ausumezbas/Desktop/maruthi_research/gpb_code/BSR/grouping/lib');


warning off

regexp = '*.png';

rsz=1;
threshold=0.1;
padding=10;

%% Process

% Make some annoucement
fprintf(1, '-------------------------------------------\n');
fprintf(1, 'Computing gpb Edges for the Hoofed Animals dataset   \n');
fprintf(1, 'threshold = %g\n', threshold);
fprintf(1, '-------------------------------------------\n');


file_list = dir(fullfile(directory, regexp));
fprintf(1, 'input_dir = %s\n', directory);

tstart=tic;
for k = 5 : 5% length(file_list)
    
    % name of input image
    input_filename = file_list(k).name;
    input_image_file = fullfile(directory, input_filename);
    [pathstr, image_name, ext, versn] = fileparts(input_image_file);
    
    fprintf('Computing Edges for %s ...', input_filename);
    
    %Create padded image
    padded_image=imread(input_image_file);
    padded_img=padarray(padded_image,[padding padding],'replicate');
    file_name_padded=[image_name '_padded' ext];
    imwrite(padded_img,file_name_padded);
    
    %Start computing edges
    [gPb_map, Ts_edge_map, gen_edge_map] = globalPb_subpixel_TO(file_name_padded, '', rsz, threshold);
    
    
    %Removing padding of image
    img=imread(input_image_file);
    Ts_edge_map(:,1:2) = Ts_edge_map(:,1:2)-padding;
    [bad_x_index]=find(Ts_edge_map(:,1)<1 | Ts_edge_map(:,1)>size(img,2));
    [bad_y_index]=find(Ts_edge_map(:,2)<1 | Ts_edge_map(:,2)>size(img,1));
    out_of_bounds=unique([bad_x_index ;  bad_y_index]);
    Ts_edge_map(out_of_bounds,:)=[];

    %Convert to c++
    Ts_edge_map(:,1:2) = Ts_edge_map(:,1:2)-1;
    
    %Write out edge map
    [h, w, n] = size(imread(input_image_file));
    
    % remove extension of the image
    temp_name = [image_name '_gpb.edg'];
    edg_filename = fullfile(directory, temp_name);
    
    %Writing out edge name
    save_edg(edg_filename, Ts_edge_map, [w h]);
    
    delete(file_name_padded);
    
end
v=toc(tstart);
disp(['Elapsed Time: ' num2str(v)]);
fprintf(1, ' done.\n');



