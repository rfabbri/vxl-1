function [prec, rec, gt, det] = orl_eval_vox_detect_ethz_prec_rec(category, ...
  groundtruth_folder, objectlist_file, detection_result_folder, wcm_idx,...
  required_min_overlap)
% This constructs the Precision-Recall curve for a VOX experiment to detect
% objects in the ETHZ dataset
% (c) Nhon Trinh
% Date: April 13, 2010

prec = [];
rec = [];

%parameters

[gt, det] = orl_sorted_dets_vox_ethz(category, ...
  groundtruth_folder, ...
  objectlist_file, ...
  detection_result_folder, ...
  wcm_idx,...
  required_min_overlap)

dets = det.det_list;
num_dets = length(dets);
fp = zeros(num_dets, 1);
tp = zeros(num_dets, 1);
for i = 1: num_dets
  fp(i) = dets(i).fp;
  tp(i) = dets(i).tp;
end;



% compute number of positive
npos = 0;
for i = 1 : length(gt) 
  % increment the total number of positive objects
  npos = npos + size(gt(i).bbox, 1);
end;

% compute precision/recall
% cache copy of detection classification
det_fp = fp;
det_tp = tp;

fp=cumsum(fp);
tp=cumsum(tp);
rec=tp/npos;
prec=tp./(fp+tp);

%% Display images that no objects were detected
fprintf(1, 'Images with missed detection \n');
for i = 1 : length(gt)
  detected = gt(i).detected;
  num_missing = length(detected) - sum(detected);
  if (num_missing > 0)
    fprintf(1, '  %s : missing %d objects:\n', gt(i).id, num_missing);
    for m = 1 : length(detected)
      if (~detected(m))
        bbox_w = gt(i).bbox(m, 3) - gt(i).bbox(m, 1);
        bbox_h = gt(i).bbox(m, 4) - gt(i).bbox(m, 2);
        fprintf(1, '    bbox %d: %d x %d, scale = %g\n', m, bbox_w, bbox_h,...
          sqrt(bbox_w*bbox_h));
      end
    end;
  end;
end;









