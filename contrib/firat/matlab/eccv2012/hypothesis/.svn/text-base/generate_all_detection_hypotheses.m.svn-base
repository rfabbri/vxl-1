% This is generate_all_detection_hypotheses.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jan 19, 2012

function [hypotheses, match_scores, model_ids, model_contours] = generate_all_detection_hypotheses(img_contours_file, model_folder, max_samples, min_samples, inc_samples, num_detections_per_img_contour, max_hypo)
    files = dir([model_folder '/exemplar*.txt']);
    hypotheses = cell(length(files),1);
    match_scores = cell(length(files),1);
    model_contours = cell(length(files),1);
    model_ids = cell(length(files),1);
    img_contours = load_contours(img_contours_file);
    for i = 1:length(files)
        model_contour = load_contours([model_folder '/' files(i).name]); 
        model_contour = model_contour{1};
        model_contours{i} = model_contour;
        fprintf('Model %d...\n',i);
        [hypotheses{i}, match_scores{i}] = generate_detection_hypotheses_given_model_contour(img_contours, model_contour, max_samples, min_samples, inc_samples, num_detections_per_img_contour);
        model_ids{i} = ones(size(hypotheses{i}, 1), 1)*i;        
    end    
    hypotheses = cell2mat(hypotheses);
    match_scores = cell2mat(match_scores);
    model_ids = cell2mat(model_ids);
    
    [match_scores, sort_index] = sort(match_scores, 'descend');   
    hypotheses = hypotheses(sort_index);    
    model_ids = model_ids(sort_index, :);
    
    if length(model_ids) > max_hypo
        hypotheses = hypotheses(1:max_hypo);
        model_ids = model_ids(1:max_hypo);
        match_scores = match_scores(1:max_hypo);         
    end
end
