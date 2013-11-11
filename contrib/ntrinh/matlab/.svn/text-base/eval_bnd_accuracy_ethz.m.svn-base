function [avg_coverage, avg_precision] = eval_bnd_accuracy_ethz(gt_ext, det_list, exp_folder)
% 
% (c) Nhon Trinh
% April 30, 2009

%% Hard-coded parameters
tol_ratio = 0.04; % ratio between tolerance and diagonal of gt bounding box

%% Collect of name of objects correspond to each list

num_dets = length(det_list);
det_ids{num_dets} = [];
for i = 1 : num_dets
  det_ids{i} = det_list(i).object_name;
end;

num_gts = length(gt_ext);
gt_ids{num_gts} = [];
for i = 1 : num_gts
  gt_ids{i} = gt_ext(i).id;
end;
  



%% Determine converage and precision for each true detection (TP)
bnd_avail(num_dets) = 0;
bnd_coverage(num_dets) = 0;
bnd_precision(num_dets) = 0;

for d = 1 : num_dets  
  % only compute for true positive detection
  if (~det_list(d).tp)
    bnd_avail(d) = 0;
    bnd_coverage(d) = 0;
    bnd_precision(d) = 0;
    continue;
  end;
    
  %>> find groundtruth mask
  gt_ids_idx = strmatch(det_ids{d}, gt_ids, 'exact');
  if (isempty(gt_ids_idx))
    error('Unrecognized image "%s"', det_ids{d});
  elseif (length(gt_ids_idx) > 1)
    error('Multiple image "%s"', det_ids{d});
  end;
  
  % index of bounding box
  gt_bbox_idx = det_list(d).gt_bbox_idx;
  
  % get the groundtruth mask
  gt_mask = gt_ext(gt_ids_idx).mask{gt_bbox_idx};
  
  % if mask is not available, move on
  if (isempty(gt_mask))
    bnd_avail(d) = 0;
    bnd_coverage(d) = 0;
    bnd_precision(d) = 0;
    continue;
  end;
  
  % get the grondtruth bbox
  gt_bbox = gt_ext(gt_ids_idx).bbox(gt_bbox_idx, 1:4);
  
  %>> find detection mask
  det_mask_filename = det_list(d).bnd_screenshot;
  det_mask_file = fullfile(exp_folder, det_mask_filename);
  
  det_mask = imread(det_mask_file);
  % conver to bw
  det_mask = im2bw(det_mask, 0.5);
  
  %% Now we are ready to compute coverage and precision
  bbox_diag = sqrt((gt_bbox(3)-gt_bbox(1)).^2 + (gt_bbox(4)-gt_bbox(2)).^2);
  tol = tol_ratio * bbox_diag;
  
  % Converage = percentage of gt points covered
  dt_det_mask = bwdist(det_mask);
  
  % any gt point in the following mask is "covered"
  coverage_mask = (abs(dt_det_mask) <= tol);
  
  % computing coverage
  gt_covered = gt_mask .* coverage_mask;
  bnd_avail(d) = true;
  bnd_coverage(d) = sum(sum(gt_covered)) / sum(sum(gt_mask));
  
  % Precision = percentage of det points near some gt points
  dt_gt_mask = bwdist(gt_mask);
  
  % any det points in the following mask is on target
  precision_mask = (abs(dt_gt_mask) <= tol);
  
  % compute precision
  det_precise = det_mask .* precision_mask;
  bnd_precision(d) = sum(sum(det_precise)) / sum(sum(det_mask));
  
  fprintf(1, 'det_idx = %d, avail =%d, bnd_prec = %g, bnd_cov =%g\n', ...
    d, bnd_avail(d), bnd_precision(d), bnd_coverage(d));
end;


% now find average for each threshold step
bnd_coverage =cumsum(bnd_coverage);
bnd_precision = cumsum(bnd_precision);
bnd_avail = cumsum(bnd_avail);

avg_coverage = bnd_coverage ./ bnd_avail;
avg_precision = bnd_precision ./ bnd_avail;

