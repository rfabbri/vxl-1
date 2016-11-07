% This is verify_hypothesis_by_levelset.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jan 19, 2012

%max_iter = 20 (originally)
function [object_bb, object_contour, detection_score] = verify_hypothesis_by_levelset(edg, hypo_x, hypo_y, model_contours, num_samples, rel_size_thresh, max_iter)    
    [nrows, ncols] = size(edg);
    pad = 3;
    [min_x, max_x, min_y, max_y] = get_minmax(hypo_x, hypo_y, pad, nrows, ncols);
    cropped_edg = imcrop(edg, [min_x min_y max_x-min_x+1 max_y-min_y+1]);
    [cropped_h, cropped_w] = size(cropped_edg);
    if cropped_h*cropped_w < nrows*ncols*rel_size_thresh
        fprintf('too small. skipping...\n');
        detection_score = 0;
        object_bb = [-1 -1 -1 -1];
        object_contour = [];
        return
    end 
    hx = hypo_x - min_x + 1;
    hy = hypo_y - min_y + 1;
    %phi0 = polygon_distance_transform({[hx hy]}, cropped_h, cropped_w, 1, 1);
    phi0 = mex_poly_dist_trans({[hx hy]}, cropped_h, cropped_w, 1, 1);
    alpha = 1;
    narrow_band = 1;
    phi = levelset_evolution(phi0, alpha, narrow_band, cropped_edg, max_iter);
    object_contour = extract_contour_from_phi(phi);
    if isempty(object_contour)
        detection_score = 0;
        object_bb = [-1 -1 -1 -1];
        return
    end
    object_contour(:,1) = object_contour(:,1) + min_x - 1;
    object_contour(:,2) = object_contour(:,2) + min_y - 1;
    detection_score = -inf;    
    sampled_object_contour = sample_curve(object_contour,num_samples,1);
    %workaround: look into this
    if isempty(sampled_object_contour)
        detection_score = 0;
        object_bb = [-1 -1 -1 -1];
        return
    end
    for i = 1:length(model_contours)
        sampled_model_contour = sample_curve(model_contours{i},num_samples,1);
        [m, score] = get_best_matches_single_scale(sampled_model_contour, sampled_object_contour, 1, 'chord', [1,9]);
        if score > detection_score
            detection_score = score;
        end
    end
    [min_x, max_x, min_y, max_y] = get_minmax(object_contour(:,1), object_contour(:,2), 0, nrows, ncols);
    object_bb = [min_x min_y max_x max_y]; 
    object_area = polyarea(object_contour(:,1), object_contour(:,2));
    %if (max_x-min_x+1)*(max_y-min_y+1) < nrows*ncols*rel_size_thresh
    if object_area < nrows*ncols*rel_size_thresh
        detection_score = 0;
    end    
end

function [min_x, max_x, min_y, max_y] = get_minmax(px, py, pad, nrows, ncols)
    max_x = max(min(max(px)+pad, ncols),1);
    max_y = max(min(max(py)+pad, nrows),1);
    min_x = min(max(min(px)-pad, 1), ncols);
    min_y = min(max(min(py)-pad, 1), nrows);
end

function object_contour = extract_contour_from_phi(phi)
        C = contour(phi, [0,0]);
		close(gcf);
		polygons = {};
		L = size(C,2);		
		i = 1;
		while i < L		
			vertices = C(:,i+1:i+C(2,i)-1)';
			polygons = [polygons; vertices];
			i = i + C(2,i) + 1;			
		end
		if length(polygons) > 0
		    object_contour = polygons{1};
		else
		    object_contour = [];
		end	
end
