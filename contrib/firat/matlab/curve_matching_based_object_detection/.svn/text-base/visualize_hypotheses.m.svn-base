% This is /lemsvxl/contrib/firat/matlab/curve_matching_based_object_detection/visualize_hypotheses.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 22, 2011

function visualize_hypotheses(hypotheses, scores, img_file, model_contour, hypo_per_page_row, hypo_per_page_col, top_N, curve_frag)
    
    close all
    [sorted_scores, sort_index] = sort(scores, 'descend');
    
    delete_index = sorted_scores < -3000;
    
    sorted_scores(delete_index) = [];
    scores = sorted_scores;
    
    hypotheses = hypotheses(sort_index, :);
    hypotheses(delete_index, :) = [];
    
    
    
    if length(scores) > top_N
        scores = scores(1:top_N);
        hypotheses = hypotheses(1:top_N,:);
    end
    
    I = imread(img_file);
    hypo_per_page = hypo_per_page_col * hypo_per_page_row;
    num_hypo = size(hypotheses, 1);
    num_pages = floor(num_hypo / hypo_per_page);
    num_hypo_last_page = mod(num_hypo, hypo_per_page);
    for p = 1:num_pages
        figure;
        for h = 1:hypo_per_page
            hypo = hypotheses((p-1)*hypo_per_page + h, :);
            
            sc = hypo(4)*cos(hypo(3));
            ss = hypo(4)*sin(hypo(3));

            T = [ sc -ss;
                  ss  sc;
                  hypo(1)  hypo(2)];  
            t = maketform('affine',T);
            [tCurvex, tCurvey] = tformfwd(t, model_contour(:,1), model_contour(:,2));        
            subplot(hypo_per_page_row, hypo_per_page_col, h)
            imshow(I);
            hold on; plot(tCurvex,tCurvey, 'g', 'LineWidth', 2); 
            if exist('curve_frag','var') 
                plot(curve_frag(:,1),curve_frag(:,2), 'r', 'LineWidth', 2); 
            end                    
            hold off; 
            title(num2str(scores((p-1)*hypo_per_page + h))); 
        end
    end
    if num_hypo_last_page > 0
        figure;
        for h = 1:num_hypo_last_page
            hypo = hypotheses(num_pages*hypo_per_page + h, :);
            
            sc = hypo(4)*cos(hypo(3));
            ss = hypo(4)*sin(hypo(3));

            T = [ sc -ss;
                  ss  sc;
                  hypo(1)  hypo(2)];  
            t = maketform('affine',T);
            [tCurvex, tCurvey] = tformfwd(t, model_contour(:,1), model_contour(:,2));        
            subplot(hypo_per_page_row, hypo_per_page_col, h)
            imshow(I);
            hold on; plot(tCurvex,tCurvey, 'g', 'LineWidth', 2);            
            hold off; 
            title(num2str(scores(num_pages*hypo_per_page + h)));
        end
    end
end
