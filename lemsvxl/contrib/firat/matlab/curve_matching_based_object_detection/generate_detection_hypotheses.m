% This is /lemsvxl/contrib/firat/matlab/curve_matching_based_object_detection/generate_detection_hypotheses.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 22, 2011

function [hypotheses, match_scores] = generate_detection_hypotheses(img_contours, model_contour, num_samples, num_scales, num_detections_per_img_contour, hypo_out_file, xgraph_file)
    hypotheses = cell(length(img_contours), 1);
    match_scores = cell(length(img_contours), 1);
    sampled_model_contour = sample_curve(model_contour,num_samples,1);
    for it = 1:length(img_contours)
        F = img_contours{it};
        [cf_center, cf_len, cf_rev, match_scores{it}] = get_best_matches_multi_scale(sampled_model_contour, F, num_detections_per_img_contour, 9:6:num_samples, 'chord', [1,9]);
        %[cf_center, cf_len, cf_rev, match_scores{it}] = match_model_query_multi_scale(model_contour, F, num_samples, 3, num_scales, num_detections_per_img_contour);
        %[cf_center, cf_len, cf_rev] = srlifcm(model_contour, F, num_samples, 3, num_scales, num_detections_per_img_contour);
        hypotheses{it} = zeros(length(cf_center), 4);
        for i = 1:length(cf_center)
            j = cf_center(i)/num_samples*size(model_contour,1);
            l = cf_len(i)/num_samples*size(model_contour,1);
            
            MF = mod((round(j-l/2):round(j+l/2))-1,   size(model_contour, 1)) + 1;
            
            F_sampled = sample_curve(F, length(MF), 0); %resampled query contour
            CF = model_contour(MF,:);  %corresponding contour fragment 
            
            if cf_rev(i)
                T = cp2tform(CF(end:-1:1,:), F_sampled, 'nonreflective similarity');
            else
                T = cp2tform(CF, F_sampled, 'nonreflective similarity');
            end
                        
            Tmatrix = T.tdata.T;
            sc = Tmatrix(1,1);
            ss = Tmatrix(2,1);
            scale = sqrt(sc^2+ss^2);
            angle = atan2(ss,sc);
            tx = Tmatrix(3,1);
            ty = Tmatrix(3,2);
            hypotheses{it}(i,:) = [tx ty angle scale];
        end
    end 
    hypotheses = cell2mat(hypotheses);  
    match_scores = cell2mat(match_scores); 
    if exist('hypo_out_file', 'var') == 1 && exist('xgraph_file', 'var') == 1
        disp('Saving...')
        write_hypothesis_file(hypotheses, xgraph_file, hypo_out_file);    
    end
end
