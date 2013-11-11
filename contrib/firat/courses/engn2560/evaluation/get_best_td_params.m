function [coeff, minscore, max_val] = get_best_td_params(fc_results, coeff_values, minscore_values)
        [r,c]=find(fc_results == max(max(fc_results)));
        max_val = fc_results(r,c);
        coeff = coeff_values(r);
        minscore = minscore_values(c);
end
