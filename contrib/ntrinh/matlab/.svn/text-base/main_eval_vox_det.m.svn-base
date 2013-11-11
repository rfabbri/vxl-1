% This script evaluates and plot the Precision-Recall curve for a set of
% object detection
% (c) Nhon Trinh
% Date: Feb 1, 2009

clear all;
close all;

%% input data

%category = 'bottles';
%category = 'giraffes';
category = 'swans';
plot_pr_curve = true;
required_min_overlap = 0.5;

groundtruth_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\all_groundtruths_patched2';
%groundtruth_folder = 'D:\vision\data\ETHZ-shape\giraffes-groundtruth-short-legs';

objectlist_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\objectlist.txt';

exp_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-experiments';
%exp_folder = 'D:\vision\projects\symseg\xshock\xshock-experiments';

%detection_result_folder = fullfile(exp_folder, 'all_giraffes-20-use-ccm-model');
%detection_result_folder = fullfile(exp_folder, 'all_images-21-use-ccm-model');
%detection_result_folder = fullfile(exp_folder, 'all_giraffes-22-add-nms-polygon');
%detection_result_folder = fullfile(exp_folder, 'exp_23b-all_images-add-nms-polygon');
%detection_result_folder = fullfile(exp_folder, 'all_giraffes-24-clean-edges-kovesi-thresh-15-len-10');
%detection_result_folder = fullfile(exp_folder, 'exp_24b-all_images-clean-edges-kovesi-thresh-15-len-10');

%detection_result_folder = fullfile(exp_folder, 'exp_25-all_giraffes-matching-using-oriented-edges');
%detection_result_folder = fullfile(exp_folder, 'exp_26-all_giraffes-manual_override_bnd_dist');
%detection_result_folder = fullfile(exp_folder, 'exp_27-all_giraffes-use_wcm');
%detection_result_folder = fullfile(exp_folder, 'exp_28-all_giraffes-use_wcm-2_scales');
%detection_result_folder = fullfile(exp_folder, 'exp_30-all_giraffes-v0_63-add_nms_polygon-kovesi-I_10-len_4');
%detection_result_folder = fullfile(exp_folder, 'exp_31-all_giraffes-v0_66-use_wcm-kovesi-I_10-len_4');
%detection_result_folder = fullfile(exp_folder, 'exp_32-all_giraffes-v0_63_repeat_exp_24');
%detection_result_folder = fullfile(exp_folder, 'exp_34-all_giraffes-v0_63_repeat_exp_24-I_10-len_4');
%detection_result_folder = fullfile(exp_folder, 'exp_36-all_giraffes-v0_69-fixed-log0-I_15-len_10');
%detection_result_folder = fullfile(exp_folder, 'exp_40-all_images-v0_69-fixed-log0-I_15-len_10');


%detection_result_folder = fullfile(exp_folder, 'exp_41-bottles-v0_70-cfrag_list_ignore');
%detection_result_folder = fullfile(exp_folder, 'exp_43-all_images-v0_70-cfrag_list_ignore');
%detection_result_folder = fullfile(exp_folder, 'exp_44-all_images-v0_71-window_400x800');

%detection_result_folder = fullfile(exp_folder, 'exp_47-all_images-combine_exp_45_46');
detection_result_folder = fullfile(exp_folder, 'exp_49-swans-all_images-v0_75-window_512x512');
%detection_result_folder = fullfile(exp_folder, 'exp_50-bottles-all_images-v0_75-window_400x700');




%% Print input data
fprintf(1, 'Evaluate Object Detection Result\n');
fprintf(1, '  groundtruth folder= %s\n', groundtruth_folder);
fprintf(1, '  objectlist_file = %s\n', objectlist_file);
fprintf(1, '  detection_result_folder = %s\n', detection_result_folder);
fprintf(1, '  category = %s\n', category);
fprintf(1, '  required_min_overlap = %f\n', required_min_overlap);



%% Concatenate output xml record files
if (1)
cmd = ['cat ', ...
  fullfile(detection_result_folder, '..', 'xml_det_header.txt'), ' ', ... % header
  fullfile(detection_result_folder, 'xml_det_body.txt'), ' ', ... % body
  fullfile(detection_result_folder, '..', 'xml_det_footer.txt'), ' > ', ... % footer
  fullfile(detection_result_folder, 'xml_det.xml')]; % output file
system(cmd);

end;


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

if (1)

%% Load all detection
[tree, treename] = xml_read(fullfile(detection_result_folder, 'xml_det.xml'));

% parse data from xml tree
ids{length(tree.xshock_det)} = [];
confidence = zeros(length(tree.xshock_det), 1);
bbox_xmin = zeros(length(tree.xshock_det), 1);
bbox_ymin = zeros(length(tree.xshock_det), 1);
bbox_xmax = zeros(length(tree.xshock_det), 1);
bbox_ymax = zeros(length(tree.xshock_det), 1);


for i = 1 : length(tree.xshock_det)
  ids{i} = tree.xshock_det(i).object_name;
  confidence(i) = tree.xshock_det(i).confidence;
  bbox_xmin(i) = tree.xshock_det(i).bbox_xmin;
  bbox_ymin(i) = tree.xshock_det(i).bbox_ymin;
  bbox_xmax(i) = tree.xshock_det(i).bbox_xmax;
  bbox_ymax(i) = tree.xshock_det(i).bbox_ymax;
end;
bbox = [bbox_xmin, bbox_ymin, bbox_xmax, bbox_ymax];



%% Sort detection results to decreasing confidence level

[sorted_confidence, sort_idx] = sort(-confidence);
ids = ids(sort_idx);
bbox = bbox(sort_idx, :);
tree.xshock_det = tree.xshock_det(sort_idx);

%% Assign detection to groundtruth objects

num_det = length(confidence);
tp = zeros(num_det, 1);
fp = zeros(num_det, 1);

for d = 1 : num_det
  % display progress
  fprintf(1, '%s : pr : compute %d/%d\n', category, d, num_det);
  
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
      gt(idx).detected(jmax)=true;
    else
      fp(d)=1;            % false positive (multiple detection)   
    end
  else
      fp(d)=1;                    % false positive
  end
end;

% compute precision/recall

% cache copy of detection classification
det_fp = fp;
det_tp = tp;

fp=cumsum(fp);
tp=cumsum(tp);
rec=tp/npos;
prec=tp./(fp+tp);

%% Save precision-recall data
det_pr_filename = ['ethz_', category, '_pr.mat'];
det_pr_file = fullfile(detection_result_folder, det_pr_filename);
save(det_pr_file, 'prec', 'rec');


%% Compute average precision
ap=0;
for t=0:0.1:1
    p=max(prec(rec>=t));
    if isempty(p)
        p=0;
    end
    ap=ap+p/11;
end

if (plot_pr_curve)
    % plot precision/recall
    plot(rec,prec,'-', 'linewidth', 2);
    axis([0 1 0 1]);
    grid;
    xlabel('recall');
    ylabel('precision');
    num_images = length(dir(fullfile(detection_result_folder, 'xml_det_record+*.txt')));
    title(sprintf('class: %s, #images=%d, AP = %.3f', category, num_images, ap));
end


%% Display images that no objects were detected
fprintf(1, 'Images with missed detection \n');
for i = 1 : length(gt)
  detected = gt(i).detected;
  num_missing = length(detected) - sum(detected);
  if (num_missing > 0)
    fprintf(1, '  %s : missing %d objects\n', gt(i).id, num_missing);
  end;
end;

%% Output xml file of top detection

for i = 1 : length(tree.xshock_det)
  tree.xshock_det(i).fp = det_fp(i);
  tree.xshock_det(i).tp = det_tp(i);
end;

num_det_to_save = min(150, length(tree.xshock_det));

top_tree.xshock_det = tree.xshock_det(1:num_det_to_save);
output_xml_file = fullfile(detection_result_folder, 'xml_top_det.xml');
wPref.StructItem = false;
xml_write(output_xml_file, top_tree, treename{1}, wPref);



end; % load detection