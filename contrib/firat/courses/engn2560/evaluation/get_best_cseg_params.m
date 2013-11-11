function [bulevel, ov_thres, max_val] = get_best_cseg_params(fc_results, bulevel_values, ov_thres_values)
        [r,c]=find(fc_results == max(max(fc_results)));
        max_val = fc_results(r,c);
        bulevel = bulevel_values(r);
        ov_thres = ov_thres_values(c);
end
