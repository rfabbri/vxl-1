% This script prepares ETHZ dataset to upload to VOX
% (c) Nhon Trinh
% Modified by Firat Kalaycilar
% Date: Jan 28, 2009

%% Task 2 - Prepare ETHZ data for VOX

function main_prepare_vox_data(orig_folder, img_ext, groundtruth_folder, imagelist_file, output_folder)

addpath ~/lemsvxl/src/contrib/ntrinh/matlab/vox
addpath ~/lemsvxl/src/contrib/ntrinh/matlab/xml_io_tools
if (1)
  
  % inputs
  %orig_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\all_originals';
  %groundtruth_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\all_groundtruths_patched2';  
  %imagelist_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\list_all_images.txt';
  
  % outputs
  %output_folder = 'D:\vision\data\ETHZ-shape\vox-upload';
  
  
  % load the image list
  fid = fopen(imagelist_file, 'r');
  C = textscan(fid, '%s\n');
  imagelist = C{1};
  
  % Collect list of object names
  objectlist = {};
  
  % iterate thru the image list
  for i = 1 : length(imagelist)
    imagename = char(imagelist(i));
    
    % retrieve the object name
    [pathstr, objectname, ext, versn] = fileparts(imagename);
    
    % a) form names for related files
    
    % original image
    orig_image_filename = [objectname, '.' img_ext];
    orig_image_file = fullfile(orig_folder, orig_image_filename);

    % groundtruth file
    groundtruth_filename = [objectname, '.groundtruth'];
    groundtruth_file = fullfile(groundtruth_folder, groundtruth_filename);    
    
    % output object folder
    out_obj_folder = fullfile(output_folder, objectname);
 
    
    % b) Create output folder
    
    % use this with caution !!!!!
    % clean up any existing object directory
    if (exist(out_obj_folder, 'dir'))
    %  rmdir(out_obj_dir, 's');
    else
      % create the folder if it doesn't exist
      [success, message, messageid] = mkdir(output_folder, objectname);
      if (success ~= 1)
        fprintf(2, 'ERROR: could not create folder %s\n', out_obj_folder);
        continue;
      end;
    end;
    
    % c) parse objectname to get category
    idx = strfind(objectname, '_');
    if (isempty(idx))
      category = 'unknown';
    else
      category = objectname(1: (idx-1));
    end;

       
    % c) print out the file names that will be used
    fprintf(1, 'object name = %s\n', objectname);
    fprintf(1, '  orig_image_filename = %s\n', orig_image_filename);
    fprintf(1, '  groundtruth_filename = %s\n', groundtruth_filename);   
    fprintf(1, '  output object_folder = %s\n', out_obj_folder);
    fprintf(1, '  object category = %s\n', category);
    
    % d) Create XML output file
    xml_filename = [objectname, '.xml'];
    xml_file = fullfile(out_obj_folder, xml_filename); 
    vox_ethz_gt2xml(groundtruth_file, category, xml_file);
    
    % e) Copy other files over
    % original image
    copyfile(orig_image_file, fullfile(out_obj_folder, orig_image_filename));    
    
    % f) Record the object list
    objectlist{end+1} = objectname;
  end;
end;

% the object list to file
fid = fopen(fullfile(output_folder, 'objectlist.txt'), 'w');
for i = 1 : length(objectlist)
  fprintf(fid, '%s\n', char(objectlist{i}));
end;
fclose(fid);
end


