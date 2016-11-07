% This script plot the distribution of groundtruth data
% (c) Nhon Trinh
% March 2, 2009

clear all;
close all;

%% input data

category = 'bottles';
groundtruth_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\all_groundtruths_patched2';
objectlist_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\objectlist.txt';

%exp_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-experiments';
%exp_folder = 'D:\vision\projects\symseg\xshock\xshock-experiments';





%% Print input data
fprintf(1, 'Plot scales of groudtruth data\n');
fprintf(1, '  groundtruth folder= %s\n', groundtruth_folder);
fprintf(1, '  objectlist_file = %s\n', objectlist_file);
fprintf(1, '  category = %s\n', category);

%% Load groundtruth data

% load list of groundtruth files
fid = fopen(objectlist_file, 'r');
C = textscan(fid, '%s');
fclose(fid);

objectlist = C{1};

% initialize groundtruth data structure
gt(length(objectlist)) = struct('id', [], 'bbox', [], 'detected', []);

% load data for images with matched category
npos = 0;
for i = 1 : length(objectlist)
  objectname = objectlist{i};
  gt(i).id = objectname;
  
  % check category of this image
  idx = strfind(objectname, '_');
  
  if (isempty(idx))
    error('Objectname does not include category in front,\n');
  end;
  object_category = objectname(1:idx-1);
  
  % if category match, load groundtruth file
  if (strcmp(object_category, category))
    gt_filename = [objectname, '.groundtruth'];
    
    % read bounding boxes from groundtruth files
    gt_bbox = dlmread(fullfile(groundtruth_folder, gt_filename), ' ');
    if (isempty(gt_bbox))
      error('No bbox in groundtruth file.\n');
    end;

    % record info
    gt(i).bbox = gt_bbox;
    gt(i).id = objectname;
    gt(i).detected = false(size(gt_bbox, 1), 1);

    % increment the total number of positive objects
    npos = npos + size(gt_bbox, 1);
  end;
end;

gt_ids = objectlist;

%% Compute scales of each box and plot it out
gt_scales = [];
aspect_ratios = [];
for i = 1 : length(objectlist)
  objectname = objectlist{i};
  gt(i).id = objectname;
  bbox = gt(i).bbox;
  
  if (~isempty(bbox))
    area = (bbox(:, 3) - bbox(:, 1)) .* (bbox(:, 4)-bbox(:, 2));
    scales = sqrt(area);
    gt_scales = [gt_scales; scales];
    
    ratio = (bbox(:, 3) - bbox(:, 1)) ./ (bbox(:, 4)-bbox(:, 2));
    aspect_ratios = [aspect_ratios; ratio];
  end;
end;

figure; plot(gt_scales);
title(['Scales of category = ', category]);

figure; plot(aspect_ratios);
title(['Aspect ratios of category = ', category]);