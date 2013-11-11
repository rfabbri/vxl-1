% This is /lemsvxl/contrib/firat/matlab/eccv2012/verification/visualize_detections.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jan 20, 2012

function visualize_detections(img_file, object_contours, object_bbs, scores, det_per_page_row, det_per_page_col, top_N, show_contour)
    close all
    if length(scores) > top_N
        scores = scores(1:top_N);
        object_contours = object_contours(1:top_N); 
        object_bbs = object_contours(1:top_N,:);       
    end 
    I = imread(img_file);
    det_per_page = det_per_page_col * det_per_page_row;
    num_det = length(object_contours);
    num_pages = floor(num_det / det_per_page);
    num_det_last_page = mod(num_det, det_per_page); 
    for p = 1:num_pages
        figure;
        for h = 1:det_per_page
            hindex = (p-1)*det_per_page + h;
            C = object_contours{hindex};
            bb = object_bbs(hindex,:);
            subplot(det_per_page_row, det_per_page_col, h)
            imshow(I);
            hold on; 
            rectangle('Position', [bb(1), bb(2), bb(3)-bb(1)+1, bb(4)-bb(2)+1], ...
      'EdgeColor', 'Red', ...
      'LineWidth', 2);
            if show_contour
                visualize_contours({C}, 'g');
            end                              
            hold off; 
            title(num2str(scores(hindex))); 
        end
    end
    if num_det_last_page > 0
        figure;
        for h = 1:num_det_last_page
            hindex = num_pages*det_per_page + h;
            C = object_contours{hindex};
            bb = object_bbs(hindex,:);
            subplot(det_per_page_row, det_per_page_col, h)
            imshow(I);
            hold on; 
            rectangle('Position', [bb(1), bb(2), bb(3)-bb(1)+1, bb(4)-bb(2)+1], ...
      'EdgeColor', 'Red', ...
      'LineWidth', 2);
            if show_contour
                visualize_contours({C}, 'g');
            end                              
            hold off; 
            title(num2str(scores(hindex)));
        end
    end
end
