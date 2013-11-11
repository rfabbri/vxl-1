% This is /lemsvxl/contrib/firat/matlab/curve_matching_based_object_detection/compute_log_likelihoods.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 22, 2011

function log_likelihoods = compute_log_likelihoods(ccm_model_file, ccm_param_file, image_name, edgemap_folder, edgemap_ext, edgeorient_ext, edgemap_log2_scale_ratio, image_width, edgmap_selection_precision, cut_off_target_pyramid_scale, hypothesis_file)
    alphabet = char([0:25] + 'a');
    randoutfile = [alphabet(unidrnd(26,1,10)) '.like'];
    
    cmd = sprintf('!~/lemsvxl/bin/contrib/firat/dbsks_xgraph_likelihood_calculator/dbsks_xgraph_likelihood_calculator %s %s %s %s %s %s %f %d %d %f %s %s', ccm_model_file, ccm_param_file, image_name, edgemap_folder, edgemap_ext, edgeorient_ext, edgemap_log2_scale_ratio, image_width, edgmap_selection_precision, cut_off_target_pyramid_scale, hypothesis_file, randoutfile);
    
    eval(cmd);
    log_likelihoods = load(randoutfile);
    eval(sprintf('!rm -f %s', randoutfile));
end
