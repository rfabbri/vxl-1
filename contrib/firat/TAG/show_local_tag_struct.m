function show_local_tag_struct(I, sigma, mins, maxs, saddles, min_links, max_links, saddle_links, crit_id, inp_mode)
	cp_type = find(crit_id > 0, 1);
	J = smooth_image(I, sigma);
	[H,W] = size(J);
	if ~exist('inp_mode', 'var')
		inp_mode = 'unique';
	end
    if strcmp(inp_mode, 'vl_feat')
    	mins = remove_duplicate_keypoints(mins(1:2,:)');   	
    	maxs = remove_duplicate_keypoints(maxs(1:2,:)');
    	saddles = remove_duplicate_keypoints(saddles(1:2,:)');
    end
    num_min = size(mins, 1);
    num_max = size(maxs, 1);
    num_saddles = size(saddles, 1);
    imagesc(J); colormap gray; hold on;
    axis image
    axis off
    i = crit_id(cp_type);
    pad = 3;
    
    
    function output_text = data_cursor(~, event_obj)
		DCM = get(event_obj);
		DCMpos = DCM.Position;
		v1 = find(ismember(temp1, DCMpos, 'rows'), 1) - 1;			
		
		if ~isempty(v1)
			if v1 == 0
				output_text = 'Central point';
			else
				output_text = {sprintf('Length(norm) = %.2f', norm_dist(v1)), ...
				               sprintf('Length(diag) = %.2f', diag_dist(v1)), ...
				               sprintf('Angle = %.2f', angles(v1)), ...
				               sprintf('CW Rel. Angle = %.2f', rel_angles(v1)), ...
				               sprintf('Intensity = %.2f', J(temp1(v1+1, 2), temp1(v1+1, 1))), ...
				               sprintf('Intensity(norm) = %.2f', J(temp1(v1+1, 2), temp1(v1+1, 1))/mean_intensity), ...
				               sprintf('Persistence = %.2f', abs(J(temp1(v1+1, 2), temp1(v1+1, 1)) - J(temp1(1, 2), temp1(1, 1)))), ...
				               sprintf('Persistence(norm) = %.2f', abs(J(temp1(v1+1, 2), temp1(v1+1, 1)) - J(temp1(1, 2), temp1(1, 1)))/mean_intensity) ... 
				              };
			end				
		else
			output_text = 'Ignored';
		end
	end    
    
    if cp_type == 1 % min
    	min0 = mins(i,:);
    	num_sads = length(min_links{i});
    	sads = saddles(min_links{i},:);
    	mxs = unique(maxs(saddle_links(min_links{i}, [3 4]), :), 'rows');
    	temp1 = [min0;sads;mxs];
    	
    	for j = 1:num_sads
    		line([sads(j,1) min0(1)], [sads(j,2) min0(2)],'Color', [255 160 0]/255, 'LineWidth', 2);
    		neig_max_id = saddle_links(min_links{i}(j), [3 4]);
    		line([sads(j,1) maxs(neig_max_id(1), 1)], [sads(j,2) maxs(neig_max_id(1), 2)],'Color', [139, 0, 255]/255, 'LineWidth', 2);
    		line([sads(j,1) maxs(neig_max_id(2), 1)], [sads(j,2) maxs(neig_max_id(2), 2)],'Color', [139, 0, 255]/255, 'LineWidth', 2);
    		plot(maxs(neig_max_id, 1), maxs(neig_max_id, 2), '^', 'MarkerSize', 8, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
    		plot(sads(j,1), sads(j,2), '+', 'MarkerSize', 8, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2); 
    		
    	end	
    	
    	plot(min0(:,1), min0(:,2), 'o', 'MarkerSize', 8, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
    	
    elseif cp_type == 2 % max
    	max0 = maxs(i,:);
    	num_sads = length(max_links{i});
    	sads = saddles(max_links{i},:);
    	mns = unique(mins(saddle_links(max_links{i}, [1 2]), :), 'rows');
    	temp1 = [max0;sads;mns];
    	
    	for j = 1:num_sads
    		line([sads(j,1) max0(1)], [sads(j,2) max0(2)],'Color', [139, 0, 255]/255, 'LineWidth', 2);    		
    		neig_min_id = saddle_links(max_links{i}(j), [1 2]);
    		line([sads(j,1) mins(neig_min_id(1), 1)], [sads(j,2) mins(neig_min_id(1), 2)],'Color', [255 160 0]/255, 'LineWidth', 2);
    		line([sads(j,1) mins(neig_min_id(2), 1)], [sads(j,2) mins(neig_min_id(2), 2)],'Color', [255 160 0]/255, 'LineWidth', 2);
    		plot(mins(neig_min_id, 1), mins(neig_min_id, 2), 'o', 'MarkerSize', 8, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);    		
    		plot(sads(j,1), sads(j,2), '+', 'MarkerSize', 8, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2); 
    	end
    	plot(max0(:,1), max0(:,2), '^', 'MarkerSize', 8, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
    elseif cp_type == 3 % saddle    	
    	sad = saddles(i,:);          	
		min1 = mins(saddle_links(i,1),:);
		min2 = mins(saddle_links(i,2),:);
		max1 = maxs(saddle_links(i,3),:);
		max2 = maxs(saddle_links(i,4),:);	    	
		
		temp1 = [sad;min1;min2;max1;max2];	
    	
		
		line([sad(1) min1(1)], [sad(2) min1(2)],'Color', [255 160 0]/255, 'LineWidth', 2);
		line([sad(1) min2(1)], [sad(2) min2(2)],'Color', [255 160 0]/255, 'LineWidth', 2);	
		
		line([sad(1) max1(1)], [sad(2) max1(2)],'Color', [139, 0, 255]/255, 'LineWidth', 2);
		line([sad(1) max2(1)], [sad(2) max2(2)],'Color', [139, 0, 255]/255, 'LineWidth', 2);
		
		plot(max1(:,1), max1(:,2), '^', 'MarkerSize', 8, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
		plot(min1(:,1), min1(:,2), 'o', 'MarkerSize', 8, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
		plot(max2(:,1), max2(:,2), '^', 'MarkerSize', 8, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
		plot(min2(:,1), min2(:,2), 'o', 'MarkerSize', 8, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
		plot(sad(:,1), sad(:,2), '+', 'MarkerSize', 8, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2); 		
		
    else
    	error('Unknown critical point type');
    end      
       
    hold off;
    angles = compute_angles(temp1);
    [angles,sort_index] = sort(angles);
    shift_angles = circshift(angles, 1);
    rel_angles = angles - shift_angles;
    rel_angles(rel_angles < 0) = rel_angles(rel_angles < 0) + 360;
    temp1(2:end,:) = temp1(sort_index+1, :);
    norm_dist = compute_norm_distances(temp1);		
	bb_min = min(temp1, [], 1);
	bb_max = max(temp1, [], 1);
	bb_W = bb_max(1) - bb_min(1);
	bb_H = bb_max(2) - bb_min(2);
	PP = J(bb_min(2):bb_max(2), bb_min(1):bb_max(1));
	mean_intensity = mean(PP(:));	
	bb_min = min(max(bb_min-pad, [1 1]), [W H]);    	
	bb_max = min(max(bb_max+pad, [1 1]), [W H]);	
	
	diag_dist = compute_diagnorm_dist(temp1, norm([bb_W bb_H]));	
	set(datacursormode(gcf),'UpdateFcn',@data_cursor); 
	axis([bb_min(1) bb_max(1) bb_min(2) bb_max(2)])
end

function norm_dist = compute_norm_distances(temp)
	D = pdist2(temp(1,:), temp(2:end,:));
	norm_dist = D/sum(D);
end

function diag_dist = compute_diagnorm_dist(temp, diag1)
	D = pdist2(temp(1,:), temp(2:end,:));
	diag_dist = D/diag1;
end

function angles = compute_angles(temp)
	vect = temp(2:end,:) - repmat(temp(1,:), size(temp,1)-1, 1);
	angles = atan2(-vect(:,2), vect(:,1))/pi*180;
	angles(angles < 0) = angles(angles < 0) + 360;
end
