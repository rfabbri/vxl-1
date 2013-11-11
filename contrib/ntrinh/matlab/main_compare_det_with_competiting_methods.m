% This script compares our detection results with our competitors
% (c) Nhon Trinh
% March 31, 2009

clear all;
close all;

%% % What to do
plot_prec_rec = 1;
plot_bnd_accuracy = 0;

category_id = 1;
categories = {'Applelogos', 'Bottles', 'Giraffes', 'Mugs', 'Swans'};




%% Plot properties
marker_size = 5;
line_width = 3;
  
%% Global data
zhu_eccv08_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\competing-methods\zhu_eccv08\eccv_pr';
zhu_eccv08_filenames = {'applelogos_regenerated.mat', ...
                           'bottles_regenerated.mat', ... 
                           'giraffes_regenerated.mat', ...
                           'mugs_regenerated.mat', ...
                           'swans_regenerated.mat' ...
                           };

ferrari_tr08_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\competing-methods\ferrari_tr08';
ferrari_tr08_handdrawn_filenames = {'ferrari_TR08_handdrawn_applelogos_dr_vs_fppi.csv', ...
                                    'ferrari_TR08_handdrawn_bottles_dr_vs_fppi.csv', ...
                                    'ferrari_TR08_handdrawn_giraffes_dr_vs_fppi.csv', ...
                                    'ferrari_TR08_handdrawn_mugs_dr_vs_fppi.csv', ...
                                    'ferrari_TR08_handdrawn_swans_dr_vs_fppi.csv', ...
                                    };

symseg_eval_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-experiments\xshock-prec-rec';
symseg_eval_filenames = {'applelogos_exp_63b_eval.mat', ...
                         'bottles_exp_50b_eval.mat', ...
                         'giraffes_exp_40b_eval.mat', ...
                         'mugs_exp_56b_eval.mat', ...
                         'swans_exp_49b_eval.mat' ...
                         };

% Data from Ravishankar etal (ECCV'08) paper
num_objects = [44, 55, 92, 66, 33];
num_images = 255;
ferrari_eccv06.fppi = [0.4, 0.3];
ferrari_eccv06.dr = [ 0.727, 0.568; ... % applelogos
                            0.909, 0.891; ... % bottles
                            0.681, 0.626; ... % giraffes
                            0.818, 0.682; ... % mugs
                            0.939, 0.758; ... % swans
                            ];
ferrari_cvpr07.fppi = [0.4, 0.3];
ferrari_cvpr07.dr = [ 0.864, 0.841; ... % applelogos
                            0.927, 0.909; ... % bottles
                            0.703, 0.659; ... % giraffes
                            0.834, 0.803; ... % mugs
                            0.939, 0.909; ... % swans
                            ];
                          
ravishankar_eccv08.fppi = [0.4, 0.3];
ravishankar_eccv08.dr = [ 0.977, .955; ... % applelogos
                                .927, .909; ... % bottles
                                .934, .912; ... % giraffes
                                .953, .937; ... % mugs
                                .969, .939; ... % swans
                              ];

% Boundary accuracy are computed at FPPI = 0.4
ferrari_tr08_learntmodel.bnd_coverage = [ 0.916; ... % applelogos
                              0.836; ... % bottles
                              0.685; ... % giraffes
                              0.844; ... % mugs
                              0.777; ... % swans
                            ];
                          
ferrari_tr08_learntmodel.bnd_precision = [ 0.939; ... % applelogos
                               0.845; ... % bottles
                              0.773; ... % giraffes
                              0.776; ... % mugs
                              0.772; ... % swans
                            ];
% These data are approximated from the paper's graph, not original data                         
ferrari_tr08_learntmodel.fppi = [0.4];
ferrari_tr08_learntmodel.dr = [0.843; ... % applelogos
                               0.818376; ... % bottles
                               0.446389; ... % giraffes
                               0.798701; ... % mugs
                               0.699571; ... % swans
  ];

% Baseline for comparison: groundtruth BBox
gt_bbox.bnd_coverage = [ 0.425; ... % applelogos
                         0.712; ... % bottles
                         0.267; ... % giraffes
                         0.551; ... % mugs
                         0.368; ... % swans
                        ];
                     
gt_bbox.bnd_precision = [ 0.408; ... % applelogos
                          0.677; ... % bottles
                          0.298; ... % giraffes
                          0.623; ... % mugs
                          0.393; ... % swans
                        ];          
%return;
close all;

%% Plot Boundary accuracy

if (plot_bnd_accuracy)
  figure;
  hold on;
  curve_names = {};
  
  % Ferrari TR08 - learnt model
  plot(ferrari_tr08_learntmodel.dr(category_id, :), ...
    ferrari_tr08_learntmodel.bnd_coverage(category_id, :), ...
    'rs-', 'MarkerSize',marker_size,'LineWidth',line_width);
  plot(ferrari_tr08_learntmodel.dr(category_id, :), ...
    ferrari_tr08_learntmodel.bnd_precision(category_id, :), ...
    'bs--', 'MarkerSize',marker_size,'LineWidth',line_width);
  curve_names{end+1} = 'Ferrari TR08 - average coverage';
  curve_names{end+1} = 'Ferrari TR08 - average precision';

  % symseg
  symseg_eval_file = fullfile(symseg_eval_folder, ...
    symseg_eval_filenames{category_id});
  symseg_eval = load(symseg_eval_file);
  symseg.rec = symseg_eval.rec;
  symseg.avg_bnd_coverage = symseg_eval.avg_bnd_coverage;
  symseg.avg_bnd_precision = symseg_eval.avg_bnd_precision;
  
  plot(symseg.rec, symseg.avg_bnd_coverage, ...
    'r-', 'MarkerSize',marker_size,'LineWidth',line_width);
  plot(symseg.rec, symseg.avg_bnd_precision, ...
    'b-', 'MarkerSize',marker_size,'LineWidth',line_width);    
  curve_names{end+1} = 'Our method - average coverage';
  curve_names{end+1} = 'Our method - average precision';
  
  hold off;
  axis([0 1 0 1]);
  xlabel('recall');
  ylabel('boundary accuracy');
  legend(curve_names);
  title(['Boundary accuracy - ', categories{category_id}]);
  grid off;

  
  
end;

%% Plot precision-recall
if (plot_prec_rec)
  figure;
  hold on;
  methods = {};
  
  % zhu_eccv08 --------------
  zhu_eccv08_filename = zhu_eccv08_filenames{category_id};
  zhu_eccv08_file = fullfile(zhu_eccv08_folder, zhu_eccv08_filename);
  pr_data = load(zhu_eccv08_file);
  
  % zhu_eccv08_cs
  zhu_eccv08_cs.prec = pr_data.cont_sel(1:end,1);
  zhu_eccv08_cs.rec = pr_data.cont_sel(1:end,2);
  
  plot(zhu_eccv08_cs.rec, zhu_eccv08_cs.prec, '-b', ...
    'MarkerSize',marker_size,'LineWidth', line_width);
  methods{end+1} = 'Zhu ECCV08 - Contour Selection';
  
%   % zhu_eccv08_spfgl
%   zhu_eccv08_spfgl.precision = pr_data.sp_fg_label(1:end,1);
%   zhu_eccv08_spfgl.recall = pr_data.sp_fg_label(1:end,2);
%   plot(zhu_eccv08_spfgl.recall, zhu_eccv08_spfgl.precision, '-g', ...
%     'MarkerSize',marker_size,'LineWidth',line_width);
%   methods{end+1} = 'Sinle Point Figure/Ground Labeling (Zhu ECCV08)';


  % ferrari cvpr 2007
  [temp_prec, temp_rec] = det_convert_dr_fppi_to_prec_rec(...
      ferrari_cvpr07.dr(category_id, :), ...
      ferrari_cvpr07.fppi, ...
      num_images, num_objects(category_id));
  ferrari_cvpr07.prec = temp_prec;
  ferrari_cvpr07.rec = temp_rec;
  
%   plot(ferrari_cvpr07.rec, ferrari_cvpr07.prec, '-gs', ...
%      'MarkerSize',marker_size,'LineWidth',line_width);
%   methods{end+1} = 'Ferrari CVPR07';

  
  % ferrari TR08
  ferrari_tr08_handdrawn_filename = ferrari_tr08_handdrawn_filenames{category_id};
  ferrari_tr08_handdrawn_file = fullfile(ferrari_tr08_folder, ferrari_tr08_handdrawn_filename);
  data = dlmread(ferrari_tr08_handdrawn_file, ',', 1, 0);
  ferrari_tr08_handdrawn.fppi = data(:, 1);
  ferrari_tr08_handdrawn.dr = data(:, 2);
  
  [temp_prec, temp_rec] = det_convert_dr_fppi_to_prec_rec(...
      ferrari_tr08_handdrawn.dr, ...
      ferrari_tr08_handdrawn.fppi, ...
      num_images, num_objects(category_id));
  ferrari_tr08_handdrawn.prec = temp_prec;
  ferrari_tr08_handdrawn.rec = temp_rec;
  
  plot(ferrari_tr08_handdrawn.rec, ferrari_tr08_handdrawn.prec, '-g', ...
    'MarkerSize',marker_size,'LineWidth',line_width);
  methods{end+1} = 'Ferrari TR08 - using hand drawn model';
  
  % Ravishankar ECCV08
  [temp_prec, temp_rec] = det_convert_dr_fppi_to_prec_rec(...
      ravishankar_eccv08.dr(category_id, :), ...
      ravishankar_eccv08.fppi, ...
      num_images, num_objects(category_id));
  ravishankar_eccv08.prec = temp_prec;
  ravishankar_eccv08.rec = temp_rec;
  plot(ravishankar_eccv08.rec, ravishankar_eccv08.prec, '-cs', ...
     'MarkerSize',marker_size,'LineWidth',line_width);
  methods{end+1} = 'Ravishankar ECCV08';

  % symseg
  symseg_eval_filename = symseg_eval_filenames{category_id};
  symseg_eval_file = fullfile(symseg_eval_folder, symseg_eval_filename);
  
  symseg_eval = load(symseg_eval_file);
  symseg.prec = symseg_eval.prec;
  symseg.rec = symseg_eval.rec;
    
  plot(symseg_eval.rec, symseg_eval.prec, '-r', ...
    'MarkerSize',marker_size,'LineWidth',line_width);
  methods{end+1} = 'Our method';
  
  hold off;
  axis([0 1 0 1]);
  xlabel('recall');
  ylabel('precision');
  title(categories{category_id});
  legend(methods);
  grid off;
end;

return;

