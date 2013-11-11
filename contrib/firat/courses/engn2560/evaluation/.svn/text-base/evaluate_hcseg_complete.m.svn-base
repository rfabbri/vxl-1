function [fc_results, oc_results] = evaluate_hcseg_complete(exp_folder, gt_folder, gt_ext, nbins_values, npad_values, ov_thres_values)
        LNB = length(nbins_values); 
        LNP = length(npad_values);
        LM = length(ov_thres_values);
        oc_results = zeros(LNB, LNP, LM);
        fc_results = zeros(LNB, LNP, LM);

        for i = 1:LNB
                for j = 1:LNP
                        for k = 1:LM      
                                fprintf('Processing %d %d %d...\n',i, j, k);                  
                                nbins = nbins_values(i);
                                npad = npad_values(j);
                                ov_thres = ov_thres_values(k);
                                oc_results(i,j,k) = hcseg_overall_consistency([exp_folder '/hcseg_' num2str(nbins) '_' num2str(npad) '_' num2str(ov_thres)], gt_folder, gt_ext);
                                fc_results(i,j,k) = hcseg_figure_consistency([exp_folder '/hcseg_' num2str(nbins) '_' num2str(npad) '_' num2str(ov_thres)], gt_folder, gt_ext);
                        end
                end     
        end
       
end


