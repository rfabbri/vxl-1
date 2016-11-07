% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)

% This version is capable of producing a TAG, but the resulting graph can have non-planar parts which mean that the implementation is incorrect
% TAG_detector_v2 is an incomplete attempt to solve v1's problems.

function [mins, maxs, saddles, min_links, max_links, saddle_links] = TAG_detector_v1(I, sigma, do_plot)
    [saddles1, mins, WS, saddle_ws, J, K, H, W] = min_saddle_candidate_detector(I, sigma, false); % detect mins and saddle candidates
    [saddles2, maxs, WC, saddle_wc] = min_saddle_candidate_detector(-I, sigma, false);  % detect maxima and saddle candidates 
    
    %1-to-1 matching
    % find the common saddle candidates
    D = pdist2(saddles1,saddles2); 
    D(D > 0) = Inf; %match candidates only if their distance is 0 -> if they are the same!!!
    Matches1 = assignmentoptimal(D); % Hungarian matching
    for i = 1:length(Matches1)
    	if Matches1(i) > 0
			for j = i+1:length(Matches1)
				if Matches1(j) > 0 
					if all(ismember(saddle_ws(i,:), saddle_ws(j,:))) && all(ismember(saddle_wc(Matches1(i),:), saddle_wc(Matches1(j),:))) % if the basin and hill ids are the same, it is the same saddle point, one of them can be discarded.
						Matches1(j) = 0;					
					end			
				end
			end
		end
    end  
    
    saddles = saddles1(Matches1 > 0, :); % eliminate non-saddles 
    num_saddles = size(saddles,1);
    %%saddle neighbors
    saddle_links = zeros(num_saddles, 4); % TAG: format [min1 min2 max1 max2]
    j = 1;
	for i = 1:length(Matches1)
		if Matches1(i) > 0
			saddle_links(j, :) = [sort(saddle_ws(i,:)) sort(saddle_wc(Matches1(i),:))]; % now we know which basins and hills have created this saddle.
			j = j + 1;
		end
	end   
	
	
	% min centric representation of the TAG. A list showing the each saddle connected to each min
	min_links = cell(size(mins,1), 1);
	for i = 1:size(mins, 1)
		[min_links{i}, ignore] = find(saddle_links(:,1:2) == i);    	  
	end 
	
	%max centric representation of the TAG. A list showing the each saddle connected to each max
	max_links = cell(size(maxs,1), 1);
	for i = 1:size(maxs, 1)
		[max_links{i}, ignore] = find(saddle_links(:,3:4) == i);    	  
	end 
    
    if do_plot       
		% just show the critical points
		show_critical_points(I, sigma, mins, maxs, saddles);    
        
               
        %plot the fixed surface network
        plot_tag(I, sigma, mins, maxs, saddles, saddle_links);     
    end
    %fprintf('#min: %d, #max: %d, #saddle: %d\n', size(mins,1), size(maxs, 1), size(saddles, 1));
end

