clear all;

%function [] = plot_pr_mcs(file)
% Plots precision-recal curve from multiview curve stereo evaluation
% experiments.

%file='/home/rfabbri/tmp/stats_tau_distance_tau_dtheta_eval_results_cfrags_30.xml';
%file='/home/rfabbri/tmp/stats_tau_distance_tau_dtheta_eval_results_cfrags_30_2conf.xml';

% Capitol:
%file='/home/rfabbri/tmp/stats_tau_distance_tau_dtheta_eval_results_cfrags_100_4conf.xml';

%file='/home/rfabbri/tmp/stats_tau_distance_tau_dtheta_eval_results_cfrags_len40_4conf-nothresh.xml';

%file='/home/rfabbri/tmp/30samples_4views/stats_tau_distance_tau_dtheta_eval_results.xml'


%file='/home/rfabbri/tmp/length40_near_optimal_no_lowe/stats_tau_distance_tau_dtheta_eval_results.xml'

%file='/home/rfabbri/tmp/length20_near_optimal_with_lowe_5conf/stats_tau_distance_tau_dtheta_eval_results.xml'

%file='/home/rfabbri/tmp/downtown/length40_noother_4conf/stats_tau_distance_tau_dtheta_eval_results.xml'

%file='/home/rfabbri/tmp/downtown/length40_noother_4conf/stats_tau_distance_tau_dtheta_eval_results.xml'

%file='/home/rfabbri/tmp/capitol-full/length40_near_optimal_with_lowe_4conf/stats_tau_distance_tau_dtheta_eval_results.xml'
%file='/home/rfabbri/tmp/capitol-full/length40_near_optimal_no_lowe_4conf/stats_tau_distance_tau_dtheta_eval_results.xml'
file='/home/rfabbri/tmp/capitol-full/length40_noother_4conf/stats_tau_distance_tau_dtheta_eval_results.xml'



%Read in xml file
tree=xml_read(file);

%Grab positive and negative
numbPositive=tree.dataset.ATTRIBUTE.num_positive;
numbNegative=tree.dataset.ATTRIBUTE.num_negative;


%Numb pr points
prPoints = length(tree.dataset.stats);
precision = zeros(1,prPoints);
recall    = zeros(1,prPoints);

for i=1:prPoints
    
    %Grab stats
    stat = tree.dataset.stats(i);
    TP   = stat.TP;
    FP   = stat.FP;
    TN   = stat.TN;
    FN   = stat.FN;

    %Compute Precision
    precision(i)=TP/(TP+FP);
    
    %Compute Recall
    recall(i)=TP/numbPositive;
end
