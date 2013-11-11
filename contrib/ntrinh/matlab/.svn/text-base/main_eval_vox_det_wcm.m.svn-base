% This script evaluates and plot the Precision-Recall curve for a set of
% object detection
% (c) Nhon Trinh
% Date: Feb 1, 2009

clear all;
close all;

%% Input data



groundtruth_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\all_groundtruths_patched2';
%groundtruth_folder = 'v:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\giraffe-groundtruth-short-legs';
manual_trace_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\groundtruth-manual-trace';
objectlist_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\objectlist.txt';

% %------------------------------------------------------------------------
% Boundary accuracy evaluation
%exp_folder = 'D:\vision\projects\symseg\xshock\xshock-experiments';
exp_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-experiments';


% %--------------------------------------------------------------------------
% category = 'applelogos';
% pos_training_images_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-train\applelogos\list_applelogos_training_pos_image_one_half.txt';
% %detection_result_folder = fullfile(exp_folder, 'exp_84-applelogos_prototype1-applelogos-v0_90-repeat_exp_82_long_range_ccm');
% detection_result_folder = fullfile(exp_folder, 'exp_84b-reduce_overlap_ratio_0_3');
% wcm_idx = 0;



% %-------------------------------------------------------------------------
% category = 'bottles';
% pos_training_images_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-train\bottles\list_bottles_training_pos_image_one_half.txt';
% detection_result_folder = fullfile(exp_folder,'exp_50b-bottles-all_images-v0_75-copied_exp_50');
% %detection_result_folder = fullfile(exp_folder,'exp_100-bottles_on_ethz_bottles-v1_03');
% wcm_idx = 0;

% %--------------------------------------------------------------------------
% category = 'mugs';
% pos_training_images_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-train\mugs\list_mugs_training_pos_image_one_half.txt';
% % % detection_result_folder = fullfile(exp_folder,'exp_56b-mugs-all_images-v0_78-copied_exp_56');
% % %detection_result_folder = fullfile(exp_folder, 'exp_92-mugs_on_mugs-v0_97-chamfer_4-fixed_0phi_bug2');
% detection_result_folder = fullfile(exp_folder, 'exp_93-mugs_on_ethz-v0_99-chamfer_4-denser_edges');
% wcm_idx = 0;


% %------------------------------------------------------------------------
% category = 'giraffes';
% pos_training_images_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-train\giraffes\list_giraffes_training_pos_images.txt';
% detection_result_folder = fullfile(exp_folder,'exp_40b-giraffes-all_images-v0_69-copied_exp_40');
% %detection_result_folder = fullfile(exp_folder,'exp_96-giraffes_on_ethz-v1-fixed_bugs');
% %detection_result_folder = fullfile(exp_folder,'exp_97-giraffes_on_ethz-v1-changed_ignored_list');
% wcm_idx = 0;




%-------------------------------------------------------------------------
category = 'swans';
pos_training_images_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-train\swans\list_swans_training_pos_images.txt';
detection_result_folder = fullfile(exp_folder,'exp_49b-swans-all_images-v0_75-copied_exp_49');
%detection_result_folder = fullfile(exp_folder, 'exp_107-swans_on_ethz-v1_06-root-22-overlap-0_3');
detection_result_folder = fullfile(exp_folder, 'exp_108-swans_on_ethz-v1_06-root-22-overlap-0_2-edge_0_5');
wcm_idx = 0 %4;





% load list of training images
fid = fopen(pos_training_images_file, 'r');
C = textscan(fid, '%s');
fclose(fid);
training_images = C{1};



% What to do
eval_pr = 0;
plot_pr_curve = 0;

eval_rec_fppi = 1;
plot_rec_fppi_curve = 1;

save_top_det_xml = 1;

eval_bnd_accuracy = 0;
plot_bnd_accuracy = 0;

use_existing_eval_data = 0;

required_min_overlap = 0.2;

%% DR vs. FPPI
if (eval_rec_fppi)
  
  output_mat_file = fullfile(detection_result_folder, 'eval.mat');
  [fppi, dr, gt, det] = orl_eval_vox_detect_ethz_fppi_rec(category, ...
      groundtruth_folder, objectlist_file, detection_result_folder, wcm_idx,...
      required_min_overlap, training_images);
  
  if (plot_rec_fppi_curve)
    % plot FPPI vs. Detection rate (recall)
    figure;
    plot(fppi, dr,'-', 'linewidth', 2);
    axis([0 1.5 0 1]);
    set(gca, 'xtick', [0:0.2:1.5]);
    grid on;
    xlabel('FPPI');
    ylabel('DR');
    
    num_objects = 0;
    for i = 1 : length(gt)
      num_objects = num_objects + size(gt(i).bbox, 1);
    end;

    num_images = length(det.image_list);
    title(sprintf('class: %s, #images=%d, #objects=%d IoU=%g', ...
      category, num_images, num_objects, required_min_overlap));
  end;
  
  exp_prefix = '';
  
  [pathstr, name, ext, versn] = fileparts(detection_result_folder);
  first_dash = strfind(name, '-');
  if (~isempty(first_dash))
    first_dash = first_dash(1);
    prefix = name(1:first_dash-1);
    prefix = [prefix, '-'];
  end;
  
  out_fname = [prefix, category, '-dr_fppi', '-IoU20', '.mat'];
  out_fpath = fullfile(detection_result_folder, out_fname);
  save(out_fpath, 'dr', 'fppi', 'gt', 'det');
end;

%% Evaluate detection results in a folder
if (eval_pr)
  output_mat_file = fullfile(detection_result_folder, 'eval.mat');

  if (~use_existing_eval_data)
    [prec, rec, gt, det] = orl_eval_vox_detect_ethz_prec_rec(category, ...
      groundtruth_folder, objectlist_file, detection_result_folder, wcm_idx,...
      required_min_overlap);
  else
    load(output_mat_file);
  end;

  % compute number of images and objects
  num_images = length(dir(fullfile(detection_result_folder, 'VOC_det_result+*.txt')));
  num_objects = 0;
  for i = 1 : length(gt)
    num_objects = num_objects + size(gt(i).bbox, 1);
  end;

  % Compute average precision
  ap=0;
  for t=0:0.1:1
      p=max(prec(rec>=t));
      if isempty(p)
          p=0;
      end
      ap=ap+p/11;
  end

  %% Generate plot
  if (plot_pr_curve)
      % plot precision/recall
      figure;
      plot(rec,prec,'-', 'linewidth', 2);
      axis([0 1 0 1]);
      grid;
      xlabel('recall');
      ylabel('precision');

      title(sprintf('class: %s, #images=%d, #objects=%d, AP = %.3f', ...
        category, num_images, num_objects, ap));
  end
end;



%% Output xml file of top detection

if (save_top_det_xml)
  num_det_to_save = min(300, length(det.det_list));

  top_tree.xshock_det = det.det_list(1:num_det_to_save);
  output_xml_file = fullfile(detection_result_folder, 'xml_top_det.xml');
  wPref.StructItem = false;
  xml_write(output_xml_file, top_tree, 'xgraph_det_list', wPref);
  
  % create a folder to save the top detections
  top_det_dirname = 'top_det';
  top_det_folder = fullfile(detection_result_folder, top_det_dirname);
  if (~exist(top_det_folder, 'dir'))
    mkdir(top_det_folder);
  end;
  
  % copy screenshots of the top detection to the top_det_folder
  for i = 1 : length(top_tree.xshock_det)
    d = top_tree.xshock_det(i);
    screenshot_html = d.screenshot;
    screenshot_file = screenshot_html(11:(end-2));
    
    new_screenshot_fname = ['top_det_', num2str(i, '%04d'), '.png'];
    new_screenshot_fpath = fullfile(top_det_folder, new_screenshot_fname);
    
    copyfile(fullfile(detection_result_folder, screenshot_file), new_screenshot_fpath);
  end;
end;



%% Compute Boundary accuracies 

if (eval_bnd_accuracy)
  
  % Load the manual trace
  category_bw_name = [category, '-bw'];
  gt_ext = load_ethz_manual_trace(gt, fullfile(manual_trace_folder, category_bw_name));

  [avg_bnd_coverage, avg_bnd_precision] = eval_bnd_accuracy_ethz(gt_ext, ...
    det.det_list, detection_result_folder);

  %% Generate plots

  if (plot_bnd_accuracy)
    figure;
    plot(rec, avg_bnd_coverage, 'b-', 'linewidth', 2);
    hold on;
    plot(rec, avg_bnd_precision, 'r-', 'linewidth', 2);
    hold off;
    axis([0 1 0 1]);
    grid;
    legend('average coverage', 'average precision');
    title(sprintf('boundary accuracy - class: %s, #objects=%d', ...
          category, num_objects));

    xlabel('recall');
  end;
end;

% save everything to a .mat file
%save(output_mat_file);
return;

