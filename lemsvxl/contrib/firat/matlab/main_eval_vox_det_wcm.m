% This script evaluates and plot the Precision-Recall curve for a set of
% object detection
% (c) Nhon Trinh
% Date: Feb 1, 2009

addpath ~/lemsvxl/src/contrib/firat/matlab/orl
addpath ~/lemsvxl/src/contrib/ntrinh/matlab/xml_io_tools

clear all;
close all;

%% Input data



groundtruth_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_groundtruths_patched2';
%manual_trace_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/groundtruth-manual-trace';
objectlist_file = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/objectlist.txt';

%groundtruth_folder = '/vision/projects/kimia/shockshape/symseg/results/ethz-bb/ethz_bb_groundtruth';
%objectlist_file = '/vision/projects/kimia/shockshape/symseg/results/ethz-bb/objectlist.txt';

%groundtruth_folder = '/vision/projects/kimia/shockshape/symseg/results/VOC07/bottles/gt';
%objectlist_file = '/vision/projects/kimia/shockshape/symseg/results/VOC07/bottles/objectlist.txt';

%groundtruth_folder = '/vision/projects/kimia/shockshape/symseg/results/UIUC-cars/cars_ms_gt';
%objectlist_file = '/vision/projects/kimia/shockshape/symseg/results/UIUC-cars/objectlist.txt';




% %------------------------------------------------------------------------
% Boundary accuracy evaluation
%exp_folder = 'D:\vision\projects\symseg\xshock\xshock-experiments';
%exp_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments';

% %------------------------------------------------------------------------
category = 'giraffes';
%detection_result_folder = fullfile(exp_folder,'exp_40b-giraffes-all_images-v0_69-copied_exp_40');
%detection_result_folder = fullfile(exp_folder,'exp_96-giraffes_on_ethz-v1-fixed_bugs');
%detection_result_folder = fullfile(exp_folder,'exp_97-giraffes_on_ethz-v1-changed_ignored_list');
%detection_result_folder =  '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/giraffes_exp1_v1.07_orient_45_dist_4';
%detection_result_folder =  '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/giraffes_exp3_v1.09_orient_45_dist_4';
detection_result_folder =  '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/giraffes_exp5_nc_v1.3_orient_30';
pos_training_images_file = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/giraffes/list_giraffes_training_pos_images.txt';
wcm_idx = 0;



% %-----------------------------------------ex---------------------------------
%category = 'mugs';
%pos_training_images_file = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/mugs/list_mugs_training_pos_image_one_half.txt';
% % detection_result_folder = fullfile(exp_folder,'exp_56b-mugs-all_images-v0_78-copied_exp_56');
% %detection_result_folder = fullfile(exp_folder, 'exp_85-mugs-on_mugs-v0_90-bg_ccm_cost-chamfer_8');
% %detection_result_folder = fullfile(exp_folder, 'exp_86-mugs-on_mugs-v0_90-bg_ccm_cost-chamfer_4');
% %detection_result_folder = fullfile(exp_folder, 'exp_87-mugs-on_mugs-v0_92-nkdiff_constraint-chamfer_4');
% %detection_result_folder = fullfile(exp_folder, 'exp_88-mugs-on_mugs-v0_93-efficient-nkdiff_constraint-chamfer_4');
% %detection_result_folder = fullfile(exp_folder, 'exp_92-mugs_on_mugs-v0_97-chamfer_4-fixed_0phi_bug2');
%detection_result_folder = fullfile(exp_folder, 'exp_93-mugs_on_ethz-v0_99-chamfer_4-denser_edges');
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/mugs_exp2_v0.99_orient_30';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/mugs_exp4_v1.09_rv3_orient_45_dist_4';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/mugs_exp5_new_code_v1.2_full_orient_30';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/mugs_exp7_nc_v1.3_orient_45';
%wcm_idx = 0;

% %-------------------------------------------------------------------------
%category = 'swans';
%pos_training_images_file = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/swans/list_swans_training_pos_images.txt';
%detection_result_folder = fullfile(exp_folder,'exp_49b-swans-all_images-v0_75-copied_exp_49');
%detection_result_folder = fullfile(exp_folder,'exp_103-swans_on_ethz-v1_05');
%detection_result_folder = fullfile(exp_folder,'exp_105-swans_on_ethz-v1_06');
%detection_result_folder = fullfile(exp_folder,'exp_106-swans_on_ethz-v1_06-root-22');
%detection_result_folder = fullfile(exp_folder, 'exp_107-swans_on_ethz-v1_06-root-22-overlap-0_3');
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/swans_exp1_v1.07_orient_45_dist_4';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/swans_exp2_v1.09_orient_45_dist_4_conf_0';
%wcm_idx = 0; %4;

% % %%--------------------------------------------------------------------------
%category = 'applelogos';
% % detection_result_folder = fullfile(exp_folder,'exp_66b-applelogos_prototype1_and_2-all_images-copied_exp_66');
% % %detection_result_folder = fullfile(exp_folder,'exp_63b-applelogos_prototype1_and_2-all_images-copied_exp_63');
% detection_result_folder = fullfile(exp_folder, 'exp_84-applelogos_prototype1-applelogos-v0_90-repeat_exp_82_long_range_ccm');
% detection_result_folder = fullfile(exp_folder, 'exp_84b-reduce_overlap_ratio_0_3');
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/applelogos_exp1_v0.90_orient_30';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/applelogos_exp2_v_0.90_orient_15';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments/exp_111-applelogos_on_ethz-v1_09';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/applelogos_exp5_v1.09_full_ethz_conf_0';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/applelogos_exp6_new_code_orient_45_100';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/applelogos_exp8_nc_v1.1_default';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/applelogos_exp10_nc_v1.3_orient45_nkd_100';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/applelogos_exp12_nc_v1.3_orient30_nkd_100';
%wcm_idx = 0;
%pos_training_images_file = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/applelogos/list_applelogos_training_pos_image_one_half.txt';

% %-------------------------------------------------------------------------
%category = 'bottles';
%detection_result_folder = fullfile(exp_folder,'exp_50b-bottles-all_images-v0_75-copied_exp_50');
%detection_result_folder = fullfile(exp_folder,'exp_100-bottles_on_ethz_bottles-v1_03');
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/bottles_exp1_v1.07_orient_45_dist_4';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/bottles_exp3_nc_v1.3_45';
%pos_training_images_file = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/bottles/list_bottles_training_pos_image_one_half.txt';
%wcm_idx = 0;

% %-------------------------------------------------------------------------
%category = 'bottles';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ethz-bb/experiments/bottles_exp2';
%pos_training_images_file = '/vision/projects/kimia/shockshape/symseg/results/ethz-bb/train/bottles/list_bottles_training_pos_image_one_half.txt';
%wcm_idx = 0;

%category = 'mugs';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ethz-bb/experiments/mugs_exp3_fixed';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ethz-bb/experiments/mugs_exp4_less_scales';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/ethz-bb/experiments/mugs_exp5_more_scales';
%pos_training_images_file = '/vision/projects/kimia/shockshape/symseg/results/ethz-bb/train/mugs/list_mugs_training_pos_image_one_half.txt';
%wcm_idx = 0;

% %-------------------------------------------------------------------------
%category = 'bottles';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/VOC07/bottles/experiments/exp2_v1.03';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/VOC07/bottles/experiments/exp3_v1.03_minsize_25';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/VOC07/bottles/experiments/exp3_v1.03_minsize_25_ratio_20';
%wcm_idx = 0;

%category = 'cars';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/UIUC-cars/experiments/exp1_no-neg-ccm_v1.04';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/UIUC-cars/experiments/exp2_no-neg-ccm_ratio_20_inc_0.25_min_25';
%pos_training_images_file = '';
%wcm_idx = 0;

%%%%%%%%%%%------------------------------------------------------------Extended/Brown ETHZ
%groundtruth_folder = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/groundtruth/all_bb';

%category = 'applelogos';
%objectlist_file = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/vox/applelogos-vox-upload-1/objectlist.txt';
%objectlist_file = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/vox/eapplelogos-vox-upload/objectlist.txt';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/experiments/exp4_bapplelogos_v0.90_orient_45';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/experiments/exp3_eapplelogos_v0.90_orient_45';
%pos_training_images_file = '';
%wcm_idx = 0;

%category = 'bottles';
%objectlist_file = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/vox/bottles-vox-upload-1/objectlist.txt';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/experiments/exp6_bottles_v1.03_orient_45';
%pos_training_images_file = '';
%wcm_idx = 0;

%category = 'mugs';
%objectlist_file = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/vox/mugs-vox-upload-1/objectlist.txt';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/experiments/exp7_mugs_v0.99_orient_45';
%pos_training_images_file = '';
%wcm_idx = 0;

%category = 'swans';
%objectlist_file = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/vox/swans-vox-upload-1/objectlist.txt';
%objectlist_file = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/vox/eswans-vox-upload/objectlist.txt';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/experiments/exp9_swans_v1.06_orient_45';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/experiments/exp8_eswans_v1.06_orient_45';
%pos_training_images_file = '';
%wcm_idx = 0;

%category = 'giraffes';
%objectlist_file = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/vox/giraffes-vox-upload-1/objectlist.txt';
%detection_result_folder = '/vision/projects/kimia/shockshape/symseg/results/Brown-ETHZ/experiments/exp10_giraffes_v1.00';
%pos_training_images_file = '';
%wcm_idx = 0;

%training_images = {'swans_aal.jpg'};

% load list of training images
if (1)
    fid = fopen(pos_training_images_file, 'r');
    C = textscan(fid, '%s');
    fclose(fid);
    training_images = C{1};
else
    training_images = {};
end

% What to do
eval_pr = 0;
plot_pr_curve = 0;

eval_rec_fppi = 1;
plot_rec_fppi_curve = 1;

save_top_det_xml = 0;

eval_bnd_accuracy = 0;
plot_bnd_accuracy = 0;

use_existing_eval_data = 0;

required_min_overlap = 0.5;


%% Evaluate detection results in a folder
if (eval_pr)
  output_mat_file = fullfile(detection_result_folder, 'eval.mat');

  if (~use_existing_eval_data)
    [prec, rec, gt, det] = orl_eval_vox_detect_ethz_prec_rec(category, ...
      groundtruth_folder, objectlist_file, detection_result_folder, wcm_idx,...
      required_min_overlap, training_images);
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

%% Rec vs. FPPI
if (eval_rec_fppi)
  output_mat_file = fullfile(detection_result_folder, 'eval.mat');

  if (~use_existing_eval_data)
     [fppi, rec, gt, det] = orl_eval_vox_detect_ethz_fppi_rec(category, ...
      groundtruth_folder, objectlist_file, detection_result_folder, wcm_idx,...
      required_min_overlap, training_images);
  else
    load(output_mat_file);
  end;
  
  
  if (plot_rec_fppi_curve)
    % plot FPPI vs. Detection rate (recall)
    figure;
    plot(fppi, rec,'r--', 'linewidth', 3);
    axis([0 1.48 0 1]);
    grid on;
    xlabel('FPPI');
    ylabel('DR');
   
    
    num_objects = 0;
    for i = 1 : length(gt)
      num_objects = num_objects + size(gt(i).bbox, 1);
    end;

    num_images = length(det.image_list);
    title(sprintf('class: %s, #images=%d, #objects=%d, #detections=%d', ...
      category, num_images, num_objects, length(fppi)));
      
      
    urec = unique(rec);
    argmax_rec = find(rec == urec(end));
    first_max_rec_ind = argmax_rec(1);
    fprintf('Min confidence threshold at %d = %f\n', first_max_rec_ind, det.det_list(first_max_rec_ind).confidence );     
  end;

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
% save(output_mat_file);
return;

