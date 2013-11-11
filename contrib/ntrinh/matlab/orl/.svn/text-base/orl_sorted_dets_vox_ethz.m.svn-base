function [gt, det] = orl_sorted_dets_vox_ethz(category, groundtruth_folder, ...
  objectlist_file, detection_result_folder, wcm_idx, required_min_overlap,...
  excluded_images)
% This constructs the Precision-Recall curve for a VOX experiment to detect
% objects in the ETHZ dataset
% (c) Nhon Trinh
% Date: April 13, 2009

%% parameters

if (~exist('excluded_images'))
  excluded_images = [];
end;

if (nargin < 5)
  wcm_idx = 0;
end;

%% Print input data
fprintf(1, 'Evaluate Object Detection Result\n');
fprintf(1, '  groundtruth folder= %s\n', groundtruth_folder);
fprintf(1, '  objectlist_file = %s\n', objectlist_file);
fprintf(1, '  detection_result_folder = %s\n', detection_result_folder);
fprintf(1, '  category = %s\n', category);
fprintf(1, '  required_min_overlap = %f\n', required_min_overlap);


%% Load groundtruth data
[gt, gt_ids] = load_ethz_groundtruth(groundtruth_folder, objectlist_file, category);

% remove ground truth that have to do with excluded_images
num_gt_ids = length(gt_ids);
keep = ones(num_gt_ids, 1);
for i = 1 : num_gt_ids
  if (~isempty(strmatch(gt_ids{i}, excluded_images)))
    keep(i) = 0;
  end;
end;
keep_idx = find(keep > 0);
gt = gt(keep_idx);
gt_ids = gt_ids(keep_idx);


%% Load all detection
[det_list, image_list] = load_vox_ethz_detection(detection_result_folder, excluded_images);

%% Parse detection results
[det_list, gt, gt_ids] = orl_eval_det_pascal_criteria(det_list, gt, gt_ids, required_min_overlap, wcm_idx);


% struc to return
det.det_list = det_list;
det.image_list = image_list;


%%
function [new_det_list, new_gt, new_gt_ids] = orl_eval_det_pascal_criteria(det_list, gt, gt_ids, required_min_overlap, wcm_idx)
% parse data from xml tree
num_dets = length(det_list);
ids{num_dets} = [];
confidence = zeros(num_dets, 1);
bbox_xmin = zeros(num_dets, 1);
bbox_ymin = zeros(num_dets, 1);
bbox_xmax = zeros(num_dets, 1);
bbox_ymax = zeros(num_dets, 1);


for i = 1 : num_dets
  ids{i} = det_list(i).object_name;
  if (wcm_idx < 1)
    confidence(i) = det_list(i).confidence; % ccm cost
  else
    confidence(i) = det_list(i).wcm_confidence(wcm_idx);
  end;
  bbox_xmin(i) = det_list(i).bbox_xmin;
  bbox_ymin(i) = det_list(i).bbox_ymin;
  bbox_xmax(i) = det_list(i).bbox_xmax;
  bbox_ymax(i) = det_list(i).bbox_ymax;
end;
bbox = [bbox_xmin, bbox_ymin, bbox_xmax, bbox_ymax];



%% Sort detection results to decreasing confidence level

[sorted_mconfidence, sort_idx] = sort(-confidence);
confidence = -sorted_mconfidence;
ids = ids(sort_idx);
bbox = bbox(sort_idx, :);
det_list = det_list(sort_idx);

%% Assign detection to groundtruth objects

tp = zeros(num_dets, 1);
fp = zeros(num_dets, 1);
gt_bbox_idx = zeros(num_dets, 1); % index of bbox assigned to the detection

for d = 1 : num_dets
  % display progress
  fprintf(1, 'tp/fp : compute %d/%d\n', d, num_dets);
  
  % find groundtruth image
  idx = strmatch(ids{d}, gt_ids, 'exact');
  if (isempty(idx))
    error('Unrecognized image "%s"', ids{d});
  elseif (length(idx) > 1)
    error('Multiple image "%s"', ids{d});
  end;
  
  % assign detection to groundtruth object if any
  det_bbox = bbox(d, :);
  
  overlap_max = -inf;
  for j = 1 : size(gt(idx).bbox, 1)
    % find overlapping between groundtruth bbox and detection bbox
    gt_bbox = gt(idx).bbox(j, :);
    
    bbox_intersect = [max(det_bbox(1), gt_bbox(1)), ...
      max(det_bbox(2), gt_bbox(2)), ...
      min(det_bbox(3), gt_bbox(3)), ...
      min(det_bbox(4), gt_bbox(4))];
    
    % width and height of the intersection box
    iw = bbox_intersect(3) - bbox_intersect(1) + 1;
    ih = bbox_intersect(4) - bbox_intersect(2) + 1;
    
    if (iw > 0 && ih > 0)
      % compute overlap area of intersection / area of union
      det_bbox_area = (det_bbox(3)-det_bbox(1)+1)*(det_bbox(4)-det_bbox(2)+1);
      gt_bbox_area = (gt_bbox(3)-gt_bbox(1)+1)*(gt_bbox(4)-gt_bbox(2)+1);
      union_area = det_bbox_area + gt_bbox_area - iw*ih;
      
      overlap = iw*ih / union_area;
      if (overlap > overlap_max)
        overlap_max = overlap;
        jmax = j;
      end;
    end;
  end;
  
  % assign detection as true positive/don't care/false positive
  if (overlap_max >= required_min_overlap)
    if (~gt(idx).detected(jmax))
      tp(d)=1;            % true positive
      gt_bbox_idx(d) = jmax;   % index of assigned bbox
      
      gt(idx).detected(jmax)=true;
    else
      fp(d)=1;            % false positive (multiple detection)   
    end
  else
      fp(d)=1;                    % false positive
  end
end;

% update evaluation status of each detection
num_dets = length(det_list)
for i = 1 : num_dets
  det_list(i).fp = fp(i);
  det_list(i).tp = tp(i);
  det_list(i).gt_bbox_idx = gt_bbox_idx(i);
end;

new_det_list = det_list;
new_gt = gt;
new_gt_ids = gt_ids;
