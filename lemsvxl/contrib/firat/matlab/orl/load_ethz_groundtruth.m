function [gt, gt_ids] = load_ethz_groundtruth(groundtruth_folder, objectlist_file, category)
% Load groundtruth for a category in ETHZ dataset

% load list of groundtruth files
fid = fopen(objectlist_file, 'r');
C = textscan(fid, '%s');
fclose(fid);

objectlist = C{1};
gt_ids = objectlist;

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
  
  if strcmp('bottle', object_category)
    object_category = 'bottles';
  end
  
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
return;
