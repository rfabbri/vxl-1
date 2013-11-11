% This script prepares ETHZ dataset to upload to VOX
% (c) Nhon Trinh
% Date: Jan 28, 2009

%% Task 2 - Prepare ETHZ data for VOX
if (1)
  clear all;
  % inputs
  orig_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\all_originals';
  edgemap_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\all_edgemaps';
  edgeorient_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\all_edgeorients-txt';
  groundtruth_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\all_groundtruths_patched2';
  screenshot_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\screenshot_all_groundtruths_patched2';
  imagelist_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\list_all_images.txt';
  
  % outputs
  output_folder = 'D:\vision\data\ETHZ-shape\vox-upload';
  
  
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
    orig_image_filename = [objectname, '.jpg'];
    orig_image_file = fullfile(orig_folder, orig_image_filename);

    % edgemap
    edgemap_filename = [objectname, '_edges.tif'];
    edgemap_file = fullfile(edgemap_folder, edgemap_filename);
    
    % edge orientation
    edgeorient_filename = [objectname, '_orient.txt'];
    edgeorient_file = fullfile(edgeorient_folder, edgeorient_filename);
    
    % groundtruth file
    groundtruth_filename = [objectname, '.groundtruth'];
    groundtruth_file = fullfile(groundtruth_folder, groundtruth_filename);
    
    % screenshot file
    screenshot_filename = [objectname, '.groundtruth.png'];
    screenshot_file = fullfile(screenshot_folder, screenshot_filename);
    
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
    fprintf(1, '  edgemap_filename = %s\n', edgemap_filename);
    fprintf(1, '  edgeorient_filename = %s\n', edgeorient_filename);
    fprintf(1, '  groundtruth_filename = %s\n', groundtruth_filename);
    fprintf(1, '  screenshot groundtruth filename = %s\n', screenshot_filename);
    fprintf(1, '  output object_folder = %s\n', out_obj_folder);
    fprintf(1, '  object category = %s\n', category);
    
    % d) Create XML output file
    xml_filename = [objectname, '.xml'];
    xml_file = fullfile(out_obj_folder, xml_filename); 
    vox_ethz_gt2xml(groundtruth_file, category, xml_file);
    
    % e) Copy other files over
    % original image
    copyfile(orig_image_file, fullfile(out_obj_folder, orig_image_filename));

    % edge map
    copyfile(edgemap_file, fullfile(out_obj_folder, 'edges.tif'));
    
    % edge orientation
    copyfile(edgeorient_file, fullfile(out_obj_folder, 'orient.txt'));
    
    % screenshot
    copyfile(screenshot_file, fullfile(out_obj_folder, 'screenshot_groundtruth.png'));
    
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


%% Task 1
% This script take all images in a folder and create a folder for each of
% them. Then copy the image to that folder. This is to prepare for
% uploading data to VOX
% (c) Nhon Trinh
% Date: Oct 22, 2008

if (0)
  clear all;
  close all;


  % input data

  source_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\groundtruth-manual-trace\applelogos-bw';
  target_folder = 'V:\projects\kimia\categorization\ethz-shape-gt';

  a = dir(source_folder);
  for i = 1 : length(a)
    if (a(i).isdir==1)
      continue;
    end;
    source_filename = a(i).name;
    source_file = fullfile(source_folder, source_filename);

    % target file
    [pathstr, name, ext, versn] = fileparts(source_filename);
    target = fullfile(target_folder, name);
    if (exist(target, 'dir'))
      rmdir(target, 's');
    end;
    mkdir(target);
    copyfile(source_file, target);
  end;
end;