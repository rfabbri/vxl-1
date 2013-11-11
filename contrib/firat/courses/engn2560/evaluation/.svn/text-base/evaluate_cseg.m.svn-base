function [fc_results, oc_results] = evaluate_cseg(exp_folder, gt_folder, gt_ext, bulevel_values, ov_thres_values)
%function [fc_results, oc_results, p_results, r_results] = evaluate_cseg(exp_folder, gt_folder, gt_ext, bulevel_values, ov_thres_values)
        LC = length(bulevel_values);
        LM = length(ov_thres_values);
        oc_results = zeros(LC, LM);
        fc_results = zeros(LC, LM);
        %p_results = zeros(LC, LM);
        %r_results = zeros(LC, LM);
        for i = 1:LC                
                bulevel = bulevel_values(i);
                for j = 1:LM      
                        fprintf('Processing %d %d...\n', i, j);                  
                        ov_thres = ov_thres_values(j);
                        oc_results(i,j) = cseg_overall_consistency([exp_folder '/cseg_' num2str(bulevel) '_' num2str(ov_thres)], gt_folder, gt_ext);
                        fc_results(i,j) = cseg_figure_consistency([exp_folder '/cseg_' num2str(bulevel) '_' num2str(ov_thres)], gt_folder, gt_ext);
                        %temp_results = cseg_pr([exp_folder '/cseg_' num2str(bulevel) '_' num2str(ov_thres)], gt_folder, gt_ext);
                        %p_results(i,j) = temp_results(1);
                        %r_results(i,j) = temp_results(2);
                end
        end
end


