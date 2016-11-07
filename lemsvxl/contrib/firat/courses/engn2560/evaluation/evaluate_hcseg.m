function [fc_results, oc_results] = evaluate_hcseg(exp_folder, gt_folder, gt_ext, ov_thres_values)
        LM = length(ov_thres_values);
        oc_results = zeros(1, LM);
        fc_results = zeros(1, LM);

        for j = 1:LM      
                fprintf('Processing %d...\n', j);                  
                ov_thres = ov_thres_values(j);
                oc_results(j) = hcseg_overall_consistency([exp_folder '/hcseg_' num2str(ov_thres)], gt_folder, gt_ext);
                fc_results(j) = hcseg_figure_consistency([exp_folder '/hcseg_' num2str(ov_thres)], gt_folder, gt_ext);
               
        end
       
end


