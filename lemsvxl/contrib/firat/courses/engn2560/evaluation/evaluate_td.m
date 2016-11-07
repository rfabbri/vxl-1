function [fc_results, oc_results] = evaluate_td(exp_folder, gt_folder, gt_ext, coeff_values, minscore_values)
%[fc_results, oc_results, p_results, r_results] = evaluate_td(exp_folder, gt_folder, gt_ext, coeff_values, minscore_values)
        LC = length(coeff_values);
        LM = length(minscore_values);
        oc_results = zeros(LC, LM);
        fc_results = zeros(LC, LM);
        for i = 1:LC                
                coeff = coeff_values(i);
                for j = 1:LM      
                        fprintf('Processing %d %d...\n', i, j);                  
                        minscore = minscore_values(j);
                        oc_results(i,j) = td_overall_consistency([exp_folder '/td_' num2str(coeff) '_' num2str(minscore)], gt_folder, gt_ext);
                        fc_results(i,j) = td_figure_consistency([exp_folder '/td_' num2str(coeff) '_' num2str(minscore)], gt_folder, gt_ext);            
                end
        end
end


