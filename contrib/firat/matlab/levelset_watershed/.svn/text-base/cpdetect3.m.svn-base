% This is cpdetect3.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 7, 2013

function [mins, maxs, saddles, min_links, max_links, saddle_links] = cpdetect3(I, sigma, do_plot)
    [saddles1, mins, WS, saddle_ws, J, K, H, W] = msdetect2(I, sigma, false);
    [saddles2, maxs, WC, saddle_wc] = msdetect2(-I, sigma, false);   
    
    %1-to-1 matching
    addpath /home/firat/lemsvxl/src/contrib/firat/matlab/surface_networks/keypoint_evaluation
    D = pdist2(saddles1,saddles2);
    D(D > 0) = Inf;
    Matches1 = assignmentoptimal(D); 
    for i = 1:length(Matches1)
    	if Matches1(i) > 0
			for j = i+1:length(Matches1)
				if Matches1(j) > 0 
					if all(ismember(saddle_ws{i}, saddle_ws{j})) && all(ismember(saddle_wc{Matches1(i)}, saddle_wc{Matches1(j)}))
						Matches1(j) = 0;					
					end			
				end
			end
		end
    end  
    
    saddles = saddles1(Matches1 > 0, :);  
    num_saddles = size(saddles,1);
    %%saddle neighbors
    saddle_links = zeros(num_saddles, 4);
    %% format [min1 min2 max1 max2]
    if 0
		j = 1;
		for i = 1:length(Matches1)
			if Matches1(i) > 0
				saddle_links(j, :) = [sort(saddle_ws(i,:)) sort(saddle_wc(Matches1(i),:))];
				j = j + 1;
			end
		end   
		
		%%min neighbors
		min_links = cell(size(mins,1), 1);
		for i = 1:size(mins, 1)
			[min_links{i}, ignore] = find(saddle_links(:,1:2) == i);    	  
		end 
		
		%%max neighbors
		max_links = cell(size(maxs,1), 1);
		for i = 1:size(maxs, 1)
			[max_links{i}, ignore] = find(saddle_links(:,3:4) == i);    	  
		end 
		
		
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		% test basins/hills for topological correctness
	%	saddle_error_mask = false(size(saddles,1),1);
	%	for i = 1:size(mins, 1)		
	%		neighbor_hills = saddle_links(min_links{i}, 3:4);
	%		unique_hills = unique(neighbor_hills);
	%		hill_hist = hist(neighbor_hills(:), unique_hills);
	%		[bad_saddles, ignore] = find(ismember(neighbor_hills, unique_hills(hill_hist > 2)));		    	
	%		saddle_error_mask(min_links{i}(bad_saddles)) = true;    	
	%	end
	%	for i = 1:size(maxs, 1)		
	%		neighbor_basins = saddle_links(max_links{i}, 1:2);
	%		unique_basins = unique(neighbor_basins);
	%		basin_hist = hist(neighbor_basins(:), unique_basins);
	%		[bad_saddles, ignore] = find(ismember(neighbor_basins, unique_basins(basin_hist > 2)));		    	
	%		saddle_error_mask(max_links{i}(bad_saddles)) = true;    	
	%	end
		
		%bad_saddle_ids = find(saddle_error_mask);    
		bad_saddle_ids = 1:size(saddles,1);
		saddle_error_mask = false(size(saddles,1),1);
		for i = 1:length(bad_saddle_ids)
			si = bad_saddle_ids(i);
			sad = saddles(si,:);
			sad_x = sad(1); sad_y = sad(2);
			debug1 = sad_x == 17 && sad_y == 32;    		 
			
			if sad_x < 1 || sad_y < 1 || sad_x > W || sad_y > H
				%saddle_error_mask(si) = true;
				continue	
			end
			min1 = mins(saddle_links(si, 1), :);
			min2 = mins(saddle_links(si, 2), :);
			max1 = maxs(saddle_links(si, 3), :);
			max2 = maxs(saddle_links(si, 4), :);
			neig = remove_out_of_bounds([sad_x-1 sad_y-1; sad_x sad_y-1; sad_x+1 sad_y-1; sad_x-1 sad_y; sad_x+1 sad_y; sad_x-1 sad_y+1; sad_x sad_y+1; sad_x+1 sad_y+1], H, W);
			visit_mask = zeros(1,4);
			for j = 1:size(neig, 1)
				sad_val = J(sad_y,sad_x);
				curr_x = neig(j,1);
				curr_y = neig(j,2);
				
				neig_val = J(curr_y, curr_x);
				
				if neig_val < sad_val
					do_stop = false;   			 
					while ~do_stop					
							[curr_x, curr_y, do_stop] = find_min_neig(J, curr_x, curr_y, H, W);									
					end	
					if debug1
						[curr_x, curr_y]
					end			
					if (curr_x == min1(1) && curr_y == min1(2))
						visit_mask(1) = true;
					elseif(curr_x == min2(1) && curr_y == min2(2))
						visit_mask(2) = true;
						%break
					end
				elseif neig_val > sad_val
					do_stop = false;   			 
					while ~do_stop
						[curr_x, curr_y, do_stop] = find_min_neig(-J, curr_x, curr_y, H, W);					
					end	
					if debug1
						[curr_x, curr_y]
					end			
					if (curr_x == max1(1) && curr_y == max1(2))
						visit_mask(3) = true;
					elseif(curr_x == max2(1) && curr_y == max2(2)) 
						visit_mask(4) = true;
						%break
					end
				end
			end
			if ~all(visit_mask)
				saddle_error_mask(si) = true;
			end
		end  

		old_saddles = saddles;
		old_min_links = min_links;
		old_max_links = max_links;
		old_saddle_links = saddle_links;
		%% remove the impostor saddles and graph edges connecting to them    
		impostor_ids = find(saddle_error_mask);
		good_ids = setdiff(1:size(saddles,1), impostor_ids);
		for i = 1:length(impostor_ids)
			si = impostor_ids(i);
			min_links{saddle_links(si, 1)}(min_links{saddle_links(si, 1)} == si) = [];
			min_links{saddle_links(si, 2)}(min_links{saddle_links(si, 2)} == si) = [];
			max_links{saddle_links(si, 3)}(max_links{saddle_links(si, 3)} == si) = [];
			max_links{saddle_links(si, 4)}(max_links{saddle_links(si, 4)} == si) = [];
		end
		saddles(impostor_ids,:) = [];
		saddle_links(impostor_ids,:) = [];
    end
    function output_text = data_cursor(~, event_obj)
		DCM = get(event_obj);
		DCMpos = DCM.Position;
		v1 = find(ismember(saddles1, DCMpos, 'rows'));
		v2 = find(ismember(saddles2, DCMpos, 'rows'));
		v3 = find(ismember(mins, DCMpos, 'rows'));
		v4 = find(ismember(maxs, DCMpos, 'rows'));
		output_text = {};
		if ~isempty(v3)
			output_text = [output_text,sprintf('Min #%d', v3)];
		elseif ~isempty(v4)
			output_text = [output_text,sprintf('Max #%d', v4)];			
		end
		if ~isempty(v1)			
			output_text = [output_text, ['Saddle #' num2str(v1) ' min = ' sprintf('%d ', saddle_ws{v1})]];
			if ~isempty(v2)
				output_text = [output_text, ['Saddle #' num2str(v2) ' max = ' sprintf('%d ', saddle_wc{v2})]];
			end
		elseif ~isempty(v2)			
			output_text = [output_text,['Saddle #' num2str(v2) ' max = ' sprintf('%d ', saddle_wc{v2})]];
		end
		if isempty(output_text)
			output_text = 'Regular';
		end
	end
    
    if do_plot         
        if true
		    figure;imagesc(J); colormap gray; hold on; 
		    plot(maxs(:,1), maxs(:,2), '^', 'MarkerSize', 9, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
		    plot(mins(:,1), mins(:,2), 'o', 'MarkerSize', 9, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);        
			plot(saddles1(:,1), saddles1(:,2), 'r+', 'MarkerSize', 8, 'LineWidth', 2);
			plot(saddles2(:,1), saddles2(:,2), 'g+', 'MarkerSize', 5, 'LineWidth', 2);		       
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
