function [nbins, npad, ov_thres, max_val] = get_best_hcseg_params(fc_results, nbins_values, npad_values, ov_thres_values)
        ind = find(fc_results == max(fc_results(:)));
        [r,c,b] = ind2sub(size(fc_results), ind);
        max_val = fc_results(r,c,b);
        nbins = nbins_values(r);
        npad = npad_values(c);
        ov_thres = ov_thres_values(b);
end
