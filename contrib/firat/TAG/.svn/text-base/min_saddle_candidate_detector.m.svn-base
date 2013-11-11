% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)

% detects minima and saddle candidates

function [saddles, mins, WS, saddle_ws, J, K, H, W] = min_saddle_candidate_detector(I, sigma, do_plot)
    	
	%image border extension. 
	do_extend = true;	
	
	% L is used to decide on how many pixels to pad on the image boundary
	L = round(7*sigma);
	if mod(L,2) == 0 % odd numbers are easier to work with.
		L = L + 1;
	end	
	L = max(L, 7); % at least 7 pixels
	
	% boundary extension by reflection
	% K is the amount of padding on each side.
	if do_extend
	    K = (L-1)/2;
	    I = [I(:, K+1:-1:2), I, I(:, end-1:-1:end-K)];
	    I = [I(K+1:-1:2, :); I; I(end-1:-1:end-K, :)];
	else
	    K = 0;
	end
	
	%% KK defines the thickness of border pixels that are ignored. This is useful to avoid out of bounds error. Should be at least 1.
	KK = 1;
	%% Border_Mask is the actual mask computed based on K. Pixels labeled as true are ignored.
	Border_Mask = padarray(false(size(I)-2*KK), [KK,KK], true);
		
	% smooth the image
	if sigma > 0					
		H = fspecial('gaussian', [L L], sigma);
		phi = imfilter(I, H, 'replicate');	
	else
		phi = I;	
	end	
	
	saddles = java.util.LinkedList;	%saddle candidates
	mins = java.util.LinkedList; % minima
	saddle_ws = java.util.LinkedList; % ids of the two basins giving rise to each saddle candidate
	
	J = phi; % smoothed image
	[H,W] = size(phi); % image size	
	sorted_I = unique(phi); % sorted intensity values. the water-levels to be visited.
	num_water_levels = length(sorted_I);
	
	watershed_pixels = zeros(H, W); % a binary mask to indicate the watershed pixels.
	
	% masks and label maps to detect creation and collision events
	Mask_prev = false(size(phi));
	Label_prev = zeros(size(phi));	
	
	iter = 0; % loop index
	next_min_lab = 1; % next available min label
	
	while true
	   iter = iter + 1;
	   WaterLevel = sorted_I(iter);
	   %the current mask (0/-1)
	   Mask_curr = -(phi <= WaterLevel); 	  
	    
	   %transfer the labels from the previous iteration
	   Label_curr = Mask_curr;
	   Label_curr(Mask_prev) = Label_prev(Mask_prev);
	   
	   Label_prev = Label_curr;   	   
	   
	   %determine the event locations
	   Event_mask = Label_curr == -1;	   
	   %ignore the events on the border
	   Event_mask(Border_Mask) = 0;     
	         
	   %compute event pixel connected components
	   [Event_labels, Event_count] = bwlabel(Event_mask);
	     
	   %iterate over the events 	 	   
	   for i = 1:Event_count	        
	        cp_type = -1; %critical point type = unknown
	        ith_event_mask = Event_labels == i;	          
	        [r,c] = find(ith_event_mask); % pixel coordinates of each event pixel of the ith connected component
			neig = unique([c-1 r-1; c r-1; c+1 r-1; c-1 r; c+1 r; c-1 r+1; c r+1; c+1 r+1], 'rows');	        
	        neig_x = neig(:,1); % x coordinates of the immediate neighbors
	        neig_y = neig(:,2); % y coordinates of the immediate neighbors
	        neig_ind = sub2ind([H,W], neig_y, neig_x); % linear index corresponding to the neighbor coordinates.	        
	        
	        All_labels = Label_curr(neig_ind); % basin labels of all neighbors
	        All_labels(All_labels <= 0) = []; % remove the irrelevant labels. only the positive basin labels are important.
	        Unique_labels = unique(All_labels); % the list of unique labels.        
	        num_unique = length(Unique_labels); % number of unique labels.
	        
	        % event pixel classification:	        
	        if num_unique == 1 % basin growth
	        	Label_prev(ith_event_mask) = Unique_labels(1);	        
	        elseif num_unique > 1 % watershed creation	        	 
	        	Label_prev(ith_event_mask) = 0; % watershed pixels do not belong to any basin. so its label should be 0.
	        	watershed_pixels(ith_event_mask) = 1; % mark the watershed pixels.	        	
	        else % basin creation
	        	Label_prev(ith_event_mask) = next_min_lab; %use the next available min label.
	        	next_min_lab = next_min_lab + 1; %increment the next available min label.
	        end        
	               
	        if num_unique == 1 % regular point        	
	        	continue
	        elseif num_unique > 1           	
        		cp_type = 3; % saddle
        		saddle_ws.add(Unique_labels(1:2)); % record the ids/labels of the mins that create this saddle candidate        	        		        	
	        else % min
	        	cp_type = 1;
	        end           
	        
	        cp_loc = round(mean([neig_x, neig_y])); % DO NOT USE SURFACE FITTING
	                	        
	        if cp_type == 1 % min creation   
                mins.add(cp_loc); % new minimum           
            elseif cp_type == 3 % collision: saddle creation
            	saddles.add(cp_loc); % saddle candidate           
            end		       	        
       end	  
	   Mask_prev = logical(Mask_curr);     
	  
	   if WaterLevel == sorted_I(end)
	   		break
	   end
	end
    
   	% reformat the Java linked lists and obtain the regular MATLAB arrays.
   	mins = reshape(cell2mat(cell(mins.toArray(mins))), 2, mins.size())'-K;
   	saddles = reshape(cell2mat(cell(saddles.toArray(saddles))), 2, saddles.size())'-K;
   	saddle_ws = reshape(cell2mat(cell(saddle_ws.toArray(saddle_ws))), 2, saddle_ws.size())';
    
    % get rid of the mirror reflected extensions.
    J = J(K+1:end-K,K+1:end-K);
    WS = Label_prev; % watershed transform
    WS = WS(K+1:end-K,K+1:end-K); % crop the unnecessary parts (mirror reflections)
    
    H = H - 2*K; % update the image height after cropping
    W = W - 2*K; % update the image width after cropping 
    
    if do_plot  % plots for debugging purposes              
        figure;imagesc(J); colormap gray; hold on; 
        plot(mins(:,1), mins(:,2), 'o', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);	     	     
	    plot(saddles(:,1), saddles(:,2), '+', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);   
        hold off;  
        axis image         
                
        figure  ; imagesc(WS); hold on;
        plot(saddles(:,1), saddles(:,2), '+', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2); 
        hold off;  
        axis image        
    end      
end



