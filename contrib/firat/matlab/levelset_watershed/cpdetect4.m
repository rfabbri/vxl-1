% This is cpdetect4.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date June 26, 2013

function [mins, maxs, saddles, min_links, max_links, saddle_links] = cpdetect4(I, sigma, do_plot)
    [saddles1, mins, WS, saddle_ws, J, K, H, W] = msdetect3(I, sigma, false);
    [saddles2, maxs, WC, saddle_wc] = msdetect3(-I, sigma, false);   
       
    figure; imagesc(WS); axis image
    %1-to-1 matching
    addpath /home/firat/lemsvxl/src/contrib/firat/matlab/surface_networks/keypoint_evaluation
    D = pdist2(saddles1,saddles2);
    D(D > 0) = Inf;
    Matches = assignmentoptimal(D); 
    
    % remove all saddle candidates without a match
    saddles = saddles1(Matches > 0, :);  
    num_saddles = size(saddles,1);
    % a saddle can be connected to a min, max or another saddle
    % a generic saddle will have only 4 connections with at most 2 mins and at most 2 max points.
    % a degenerate saddle can have 2*n connections where n > 2. E.g. monkey saddle where n = 3.
    %saddle_links = cell(num_saddles, 3);    
    
    %find neighboring saddle candidates
	%D = pdist2(saddles, saddles);
	%D(D>sqrt(2)) = Inf;
	%D = D + tril(Inf(num_saddles),0);
	%Matches2 = assignmentoptimal(D);
	
	
	
	
	
	
%	j = 1;
%	for i = 1:length(Matches)
%		if Matches(i) > 0
%			saddle_links{j, 1} = sort(saddle_ws{i});
%			saddle_links{j, 2} = sort(saddle_wc{Matches(i)});
%			j = j + 1;
%		end
%	end   
		
	
    function output_text = data_cursor(~, event_obj)
		DCM = get(event_obj);
		DCMpos = DCM.Position;
		v1 = find(ismember(saddles1, DCMpos, 'rows'));
		v2 = find(ismember(saddles2, DCMpos, 'rows'));
		v3 = find(ismember(mins, DCMpos, 'rows'));
		v4 = find(ismember(maxs, DCMpos, 'rows'));
		output_text = {sprintf('Intensity: %f', J(DCMpos(2), DCMpos(1)))};
		if ~isempty(v3)
			output_text = [output_text,sprintf('Min #%d', v3)];
		elseif ~isempty(v4)
			output_text = [output_text,sprintf('Max #%d', v4)];			
		end
		if ~isempty(v1)			
			output_text = [output_text, ['Saddle #' num2str(v1) ' min = ' sprintf('%d ', saddle_ws{saddles1(v1,2)+K,saddles1(v1,1)+K})]];
			if ~isempty(v2)
				output_text = [output_text, ['Saddle #' num2str(v2) ' max = ' sprintf('%d ', saddle_wc{saddles2(v2,2)+K,saddles2(v2,1)+K})]];
			end
		elseif ~isempty(v2)			
			output_text = [output_text,['Saddle #' num2str(v2) ' max = ' sprintf('%d ', saddle_wc{saddles2(v2,2)+K,saddles2(v2,1)+K})]];
		end		
	end
    
    if do_plot         
        if true
		    figure;imagesc(J); colormap gray; hold on; 
		    plot(maxs(:,1), maxs(:,2), '^', 'MarkerSize', 9, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
		    plot(mins(:,1), mins(:,2), 'o', 'MarkerSize', 9, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);        
			plot(saddles1(:,1), saddles1(:,2), 'r+', 'MarkerSize', 8, 'LineWidth', 2);
			plot(saddles2(:,1), saddles2(:,2), 'g+', 'MarkerSize', 5, 'LineWidth', 2);       
			%plot(saddles(:,1), saddles(:,2), 'r+', 'MarkerSize', 9, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2); 
				       
		    hold off;
		    axis image
		    axis off
        	set(datacursormode(gcf),'UpdateFcn',@data_cursor);  
        	
        	
        	figure;imagesc(J); colormap gray; hold on; 
		    plot(maxs(:,1), maxs(:,2), '^', 'MarkerSize', 9, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
		    plot(mins(:,1), mins(:,2), 'o', 'MarkerSize', 9, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);		       
			plot(saddles(:,1), saddles(:,2), 'r+', 'MarkerSize', 9, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2); 
				       
		    hold off;
		    axis image
		    axis off
        	set(datacursormode(gcf),'UpdateFcn',@data_cursor); 
        end
        
        if false
		    show_critical_points(I, sigma, mins, maxs, saddles);
        end          
        
               
        %plot the old surface network
        %plot_tag(I, sigma, mins, maxs, old_saddles, old_saddle_links); 
        %hold on;
        %plot(old_saddles(saddle_error_mask,1), old_saddles(saddle_error_mask,2), 'r+', 'MarkerSize', 8, 'LineWidth', 2);
        %hold off;     
        
        %plot the fixed surface network
        %plot_tag(I, sigma, mins, maxs, saddles, saddle_links);     
    end
    %fprintf('#min: %d, #max: %d, #saddle: %d\n', size(mins,1), size(maxs, 1), size(saddles, 1));
end

function [curr_x, curr_y, do_stop] = find_min_neig(J, curr_x, curr_y, H, W)
	neig = remove_out_of_bounds([curr_x-1 curr_y-1; curr_x curr_y-1; curr_x+1 curr_y-1; curr_x-1 curr_y; curr_x+1 curr_y; curr_x-1 curr_y+1; curr_x curr_y+1; curr_x+1 curr_y+1], H, W);
	neig_ind = sub2ind([H,W], neig(:,2), neig(:,1));
	[min_val, min_ind] = min(J(neig_ind));
	if min_val <= J(curr_y, curr_x)
		do_stop = false;
		curr_x = neig(min_ind, 1);
		curr_y = neig(min_ind, 2);
	else
		do_stop = true;
	end
end

function neig = remove_out_of_bounds(neig, H, W)
	ind = find(neig(:,1) < 1 | neig(:,1) > W | neig(:,2) < 1 | neig(:,2) > H);
	neig(ind,:) = [];
end

%if false
%	to_delete = zeros(size(saddles2,1),1);
%   	for i = 1:size(saddles1,1)
%		for j = 1:size(saddles2,1)
%			[ign,num_slope_districts] = bwlabel((WS == saddle_ws(i,1) | WS == saddle_ws(i,2)) & (WC == saddle_wc(j,1) | WC == saddle_wc(j,2)));
%			if num_slope_districts == 4
%				to_delete(j) = 1; 
%				break; 
%			end
%		end
%	end
%	saddles = [saddles1; saddles2(~to_delete,:)];
%end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% test basins/hills for topological correctness
%saddle_error_mask = true(size(saddles,1),1);
%for i = 1:size(mins, 1)
%	if length(min_links{i}) == 1
%		good_saddles = 1;
%	else
%		neighbor_hills = saddle_links(min_links{i}, 3:4);
%		unique_hills = unique(neighbor_hills);
%		hill_hist = hist(neighbor_hills(:), unique_hills);
%		good_saddles = ismember(ismember(neighbor_hills, unique_hills(hill_hist == 2)), [1 1], 'rows');
%	end    	
%	saddle_error_mask(min_links{i}(good_saddles)) = false;    	
%end
%for i = 1:size(maxs, 1)
%	if length(max_links{i}) == 1
%		good_saddles = 1;
%	else
%		neighbor_basins = saddle_links(max_links{i}, 1:2);
%		unique_basins = unique(neighbor_basins);
%		basin_hist = hist(neighbor_basins(:), unique_basins);
%		good_saddles = ismember(ismember(neighbor_basins, unique_basins(basin_hist == 2)), [1 1], 'rows');
%	end    	
%	saddle_error_mask(max_links{i}(good_saddles)) = false;    	
%end

%    function output_text = data_cursor(~, event_obj)
%		DCM = get(event_obj);
%		DCMpos = DCM.Position;
%		v1 = find(ismember(saddles1, DCMpos, 'rows'));
%		v2 = find(ismember(saddles2, DCMpos, 'rows'));
%		v3 = find(ismember(mins, DCMpos, 'rows'));
%		v4 = find(ismember(maxs, DCMpos, 'rows'));
%		if ~isempty(v1)
%			output_text = sprintf('Saddle #%d WS = %d, %d', v1, saddle_ws(v1,1), saddle_ws(v1,2));
%			if ~isempty(v2)
%				output_text = {output_text, sprintf('Saddle #%d WC = %d, %d', v2, saddle_wc(v2,1), saddle_wc(v2,2))};
%			end
%		elseif ~isempty(v2)
%			output_text = sprintf('Saddle #%d WC = %d, %d', v2, saddle_wc(v2,1), saddle_wc(v2,2));
%		elseif ~isempty(v3)
%				output_text = sprintf('Min #%d', v3);
%		elseif ~isempty(v4)
%			output_text = sprintf('Max #%d', v4);			
%		else
%			output_text = 'Regular';
%		end
%	end
