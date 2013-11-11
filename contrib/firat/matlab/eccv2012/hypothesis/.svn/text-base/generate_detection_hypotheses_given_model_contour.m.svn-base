% This is generate_detection_hypotheses_given_model_contour.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Jan 19, 2012

% default max_samples = 81, min_samples = 9, inc_samples = 6

function [hypotheses, match_scores] = generate_detection_hypotheses_given_model_contour(img_contours, model_contour, max_samples, min_samples, inc_samples, num_detections_per_img_contour)
    hypotheses = cell(length(img_contours), 1);
    match_scores = cell(length(img_contours), 1);
    sampled_model_contour = sample_curve(model_contour,max_samples,1);
    for it = 1:length(img_contours)
        F = img_contours{it};
        [cf_center, cf_len, cf_rev, match_scores{it}] = get_best_matches_multi_scale(sampled_model_contour, F, num_detections_per_img_contour, min_samples:inc_samples:max_samples, 'chord', [1,5]);        
        tempS(length(cf_center)) = struct;
        hypothesis{it} = transpose(tempS);
        for i = 1:length(cf_center)
            j = cf_center(i)/max_samples*size(model_contour,1);
            l = cf_len(i)/max_samples*size(model_contour,1);            
            MF = mod((round(j-l/2):round(j+l/2))-1,   size(model_contour, 1)) + 1;            
            F_sampled = sample_curve(F, length(MF), 0); %resampled query contour
            CF = model_contour(MF,:);  %corresponding contour fragment             
            if cf_rev(i)
                T = cp2tform(CF(end:-1:1,:), F_sampled, 'nonreflective similarity');
            else
                T = cp2tform(CF, F_sampled, 'nonreflective similarity');
            end           
            hypotheses{it}(i) = T;
        end
        hypotheses{it} = hypotheses{it}';
    end 
    hypotheses = cell2mat(hypotheses);  
    match_scores = cell2mat(match_scores);     
end

%might be useful later:
%Tmatrix = T.tdata.T;
%sc = Tmatrix(1,1);
%ss = Tmatrix(2,1);
%scale = sqrt(sc^2+ss^2);
%angle = atan2(ss,sc);
%tx = Tmatrix(3,1);
%ty = Tmatrix(3,2);
