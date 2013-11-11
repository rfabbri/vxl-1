% This is get_detections_ethz.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jan 25, 2012

function [object_bbs, scores, image_ids, gt_bbs] = get_detections_ethz(exp_folder, data_prefix, class_name, det_per_page_row, det_per_page_col, top_N, doVis)
    files = dir([exp_folder '/*.mat']);
    N = length(files);
    image_ids = cell(N,1);   
    all_detections = cell(N,1);
    all_scores = cell(N,1);
    images = cell(N,1);
    gt_bbs = cell(N,1);
    for i = 1:N
        load([exp_folder '/' files(i).name]);
        objname = files(i).name(1:end-8);
        P = get_necessary_variables(data_prefix, 'xxx', objname); 
        images{i} = imread(P.img_file);
        if length(strfind(objname, class_name)) > 0
            gt_bbs{i} = load(P.gt_file);
        end
        image_ids{i} = ones(length(detection_scores),1)*i;
        all_detections{i} = object_bbs;
        all_scores{i} = detection_scores;
    end
    image_ids = cell2mat(image_ids);
    all_detections = cell2mat(all_detections);
    all_scores = cell2mat(all_scores);
    
    [scores,sort_index] = sort(all_scores, 'descend');
    object_bbs = all_detections(sort_index,:);
    image_ids = image_ids(sort_index);
    
    if ~doVis
        return
    end
    
    if length(scores) <= top_N
        num_det = length(image_ids);
    else
        num_det = top_N;            
    end 
    
    
     
    
    det_per_page = det_per_page_col * det_per_page_row;
    num_pages = floor(num_det / det_per_page);
    num_det_last_page = mod(num_det, det_per_page); 
    for p = 1:num_pages
        figure;
        for h = 1:det_per_page
            hindex = (p-1)*det_per_page + h;
            %C = object_contours{hindex};
            bb = object_bbs(hindex,:);
            subplot(det_per_page_row, det_per_page_col, h)
            imshow(images{image_ids(hindex)});
            hold on; 
            rectangle('Position', [bb(1), bb(2), bb(3)-bb(1)+1, bb(4)-bb(2)+1], ...
      'EdgeColor', 'Red', ...
      'LineWidth', 2);
            %if show_contour
            %    visualize_contours({C}, 'g');
            %end                              
            hold off; 
            title(num2str(scores(hindex))); 
        end
    end
    if num_det_last_page > 0
        figure;
        for h = 1:num_det_last_page
            hindex = num_pages*det_per_page + h;
            %C = object_contours{hindex};
            bb = object_bbs(hindex,:);
            subplot(det_per_page_row, det_per_page_col, h)
            imshow(images{image_ids(hindex)});
            hold on; 
            rectangle('Position', [bb(1), bb(2), bb(3)-bb(1)+1, bb(4)-bb(2)+1], ...
      'EdgeColor', 'Red', ...
      'LineWidth', 2);
            %if show_contour
            %    visualize_contours({C}, 'g');
            %end                              
            hold off; 
            title(num2str(scores(hindex)));
        end
    end    
   

end
