% This is visualize_hypotheses.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Jan 19, 2012

function visualize_hypotheses(hypotheses, scores, model_ids, model_contours, img_file, hypo_per_page_row, hypo_per_page_col, top_N)    
    close all
    if length(scores) > top_N
        scores = scores(1:top_N);
        hypotheses = hypotheses(1:top_N,:);
        model_ids = model_ids(1:top_N,:);
    end    
    I = imread(img_file);
    hypo_per_page = hypo_per_page_col * hypo_per_page_row;
    num_hypo = size(hypotheses, 1);
    num_pages = floor(num_hypo / hypo_per_page);
    num_hypo_last_page = mod(num_hypo, hypo_per_page);
    for p = 1:num_pages
        figure;
        for h = 1:hypo_per_page
            hindex = (p-1)*hypo_per_page + h;
            hypo = hypotheses(hindex, :);            
            [tCurvex, tCurvey] = get_hypothesis_contour(model_contours{model_ids(hindex)}, hypo);        
            subplot(hypo_per_page_row, hypo_per_page_col, h)
            imshow(I);
            hold on; plot(tCurvex,tCurvey, 'g', 'LineWidth', 2);                              
            hold off; 
            title(num2str(scores(hindex))); 
        end
    end
    if num_hypo_last_page > 0
        figure;
        for h = 1:num_hypo_last_page
            hindex = num_pages*hypo_per_page + h;
            hypo = hypotheses(hindex, :);            
            [tCurvex, tCurvey] = get_hypothesis_contour(model_contours{model_ids(hindex)}, hypo);          
            subplot(hypo_per_page_row, hypo_per_page_col, h)
            imshow(I);
            hold on; plot(tCurvex,tCurvey, 'g', 'LineWidth', 2);            
            hold off; 
            title(num2str(scores(hindex)));
        end
    end
end
