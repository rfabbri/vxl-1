% This is msdetect.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)

% detects critical points of an image by analyzing interactions between level curves

function [saddles, mins, WS, saddle_ws, J, K, H, W] = msdetect(I, sigma, do_plot)
    %addpath ~/lemsvxl/src/contrib/firat/matlab/levelset2d_segmentation/tools
	%addpath ~/lemsvxl/src/contrib/firat/matlab/topographical
	%addpath ~/lemsvxl/src/contrib/firat/matlab/subpixel_wcws	
	
	do_extend = true;	
	strel_square_3 = strel('square',3);
	
	L = round(7*sigma);
	if mod(L,2) == 0
		L = L + 1;
	end
	
	L = max(L, 7);
	
	% boundary extension by reflection
	if do_extend
	    K = (L-1)/2;
	    I = [I(:, K+1:-1:2), I, I(:, end-1:-1:end-K)];
	    I = [I(K+1:-1:2, :); I; I(end-1:-1:end-K, :)];
	else
	    K = 0;
	end
	
	KK = 1;
	Border_Mask = padarray(false(size(I)-2*KK), [KK,KK], true);
		
	% smooth the image
	if sigma > 0					
		H = fspecial('gaussian', [L L], sigma);
		phi = imfilter(I, H, 'replicate');	
	else
		phi = I;	
	end	
	
	saddles = java.util.LinkedList;	
	mins = java.util.LinkedList;	
	
	tabsize = 3000;
	collisions = false(tabsize, tabsize);
	
	saddle_ws = java.util.LinkedList;
	
	J = phi;
	[H,W] = size(phi);	
	sorted_I = unique(phi);
	num_water_levels = length(sorted_I);
	
	watershed_pixels = zeros(H, W);
	
	% masks and label maps to detect creation and collision events
	Mask_prev_min = false(size(phi));
	Label_prev_min = zeros(size(phi));	
	
	%figure
	iter = 0;
	next_ws_lab = 1;
	
	while true
	   iter = iter + 1;
	   WaterLevel = sorted_I(iter);
	   %the new masks
	   Mask_curr_min = -(phi <= WaterLevel); 	  
	    
	   %transfer the labels from the previous iteration
	   Label_curr_min = Mask_curr_min;
	   Label_curr_min(Mask_prev_min) = Label_prev_min(Mask_prev_min);
	   
	   Label_prev_min = Label_curr_min;   	   
	   
	   %determine the event locations
	   Event_mask = Label_curr_min == -1;	   
	   Event_mask(Border_Mask) = 0;     
	         
	   [Event_labels, Event_count] = bwlabel(Event_mask);
	     
	   %iterate over the events 	 	   
	   for i = 1:Event_count
	        ith_event_mask = Event_labels == i;	          
	        [r,c] = find(ith_event_mask);
			neig = unique([c-1 r-1; c r-1; c+1 r-1; c-1 r; c+1 r; c-1 r+1; c r+1; c+1 r+1], 'rows');	        
	        neig_x = neig(:,1);
	        neig_y = neig(:,2);
	        neig_ind = sub2ind([H,W], neig_y, neig_x);	        
	        
	        All_labels = Label_curr_min(neig_ind);
	        All_labels(All_labels <= 0) = [];
	        Unique_labels_min = unique(All_labels);        
	        num_unique_min = length(Unique_labels_min);
	        
	        
	        if num_unique_min == 1
	        	Label_prev_min(ith_event_mask) = Unique_labels_min(1);	        
	        elseif num_unique_min > 1	        	 
	        	Label_prev_min(ith_event_mask) = 0 ;
	        	watershed_pixels(ith_event_mask) = 1;	        	
	        else
	        	Label_prev_min(ith_event_mask) = next_ws_lab;
	        	next_ws_lab = next_ws_lab + 1;
	        end
	        
	        
	        if num_unique_min > 1
	        	pairs = nchoosek(Unique_labels_min, 2);	        	
	        	pairind = sub2ind([tabsize, tabsize], pairs(:,1), pairs(:,2));
	        	if any(collisions(pairind))
	        		num_unique_min = 1;
	        	else
	        		collisions(pairind) = true;	 
	        	end
	        end
	        
	               
	        if num_unique_min == 1 % regular	        	
	        	continue
	        elseif num_unique_min > 1 
	        	Visited_pixels = (Label_prev_min + watershed_pixels) > 0;
	        	NeigMask = zeros(H,W);
	        	NeigMask(neig_ind) = 1;
	        	Boundary = bwboundaries(NeigMask, 'noholes');
	        	BoundaryIdx = sub2ind([H,W],Boundary{1}(:,1), Boundary{1}(:,2));
	        	vp = Visited_pixels(BoundaryIdx);
	        	
	        	%neig_watershed = unique(watershed_pixels(neig_ind));        
	        	%if length(neig_watershed) == 1 % no neighboring watershed pixel
	        	if length(find(conv(double(vp), [1 -1], 'valid'))) >= 4
	        		cp_type = 3; % saddle
	        		saddle_ws.add(Unique_labels_min(1:2));
	        	else
	        		continue
	        	end	        		        	
	        else % min
	        	cp_type = 1;
	        end        
            	            
            
            %%Uncomment for surface fitting!!!
            %num_neig = length(neig_x);
            %while num_neig < 9
            %    neig = imdilate(neig, strel_square_3);
            %    num_neig = sum(neig(:));
            %end            
	        %[cp_loc, rmse] = find_subpix_cp_quadratic(J, neig, H, W, cp_type);
	        
	        cp_loc = mean([neig_x, neig_y]); % DO NOT USE SURFACE FITTING	
	        	        
	        %if is_out_of_bounds(neig, cp_loc, H, W)
	        if false % no surface fitting for now
                fprintf('Out of bounds / type inconsistency\n');
                rmse
                problematic_critical_point = cp_loc - K
                average_neighbor = mean([neig_x, neig_y]) - K
                                
                %[cp_loc2, rmse2] = find_subpix_neig_cp_quadratic(J, neig, H, W, cp_type);
                %eno_style_localization = cp_loc2 - K
                cp_loc2 = average_neighbor + K;
                rmse2 = 0;
                if isinf(rmse2)
                    if cp_type == 1
                        fprintf('No min/max point can be localized\n');
                        %mins.add(average_neighbor + K);
                    else
                        fprintf('No saddle point can be localized\n');
                        %saddles.add(average_neighbor + K);
                    end
                    %fprintf('Use the average point instead\n');
                elseif cp_type == 1 % min creation
                    mins.add(cp_loc2);
                    fprintf('Min point localization completed successfully...\n');                 
                else % min-min saddle creation
                	saddles.add(cp_loc2);
                    fprintf('Saddle (min-min) point localization completed successfully...\n');                                   
                end                          
            elseif cp_type == 1 % min creation   
                mins.add(cp_loc);            
            else % collision: saddle creation
                saddles.add(cp_loc);            
            end		       	        
       end	   
	   
	   %imagesc(Label_prev_min - watershed_pixels);axis([K+1 W-K K+1 H-K]);drawnow	     
	   %pause
	   Mask_prev_min = logical(Mask_curr_min);     
	  
	   if WaterLevel == sorted_I(end)
	   		break
	   end
	end
    
   	mins = reshape(cell2mat(cell(mins.toArray(mins))), 2, mins.size())'-K;
   	saddles = reshape(cell2mat(cell(saddles.toArray(saddles))), 2, saddles.size())'-K;
   	saddle_ws = reshape(cell2mat(cell(saddle_ws.toArray(saddle_ws))), 2, saddle_ws.size())';
    
    J = J(K+1:end-K,K+1:end-K);
    WS = Label_prev_min;
    WS = WS(K+1:end-K,K+1:end-K);   
    
    if do_plot                
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

function r = is_out_of_bounds(neig, p, H, W)
    if any(isinf(p))
        r = true;
        return;
    end
    p = round(p);
    if p(1) < 1 || p(2) < 1 || p(1) > W || p(2) > H
        r = 1;
        return
    end
    r = ~neig(p(2),p(1));
end

function [loc, rmse] = find_subpix_cp_quadratic(J, neig, H, W, cp_type)
    [neig_y,neig_x] = find(neig);
    neig_ind = sub2ind([H,W], neig_y, neig_x);
    neig_z = J(neig_ind);
    p = polyfitn([neig_x,neig_y],neig_z,2);	
    if 0 % additional check to see if the surface shape is consistent with the critical point type
        p_det = 4*p.Coefficients(1)*p.Coefficients(4) - p.Coefficients(2)^2; 
        p_trace = p.Coefficients(1) + p.Coefficients(4);    
        if cp_type == 1 % min
            if p_det <= 0 || p_trace <= 0
                loc = [Inf Inf];
                rmse = -Inf;
                %fprintf('Min coefficients\n')
                %p.Coefficients
                %eigenvalues = eig([p.Coefficients(1) p.Coefficients(2);p.Coefficients(2) p.Coefficients(4)])
                %pause            
                return
            end
        else
            if p_det >= 0
                loc = [Inf Inf];
                rmse = -Inf;
                %fprintf('Saddle coefficients\n')
                %p.Coefficients
                %eigenvalues = eig([p.Coefficients(1) p.Coefficients(2);p.Coefficients(2) p.Coefficients(4)])
                %pause            
                return
            end
        end	
    end
    loc = ([2*p.Coefficients(1) p.Coefficients(2); p.Coefficients(2) 2*p.Coefficients(4)]...
            \[-p.Coefficients(3); -p.Coefficients(5)])';
    rmse = p.RMSE;
end

function [loc, rmse] = find_subpix_neig_cp_quadratic(J, neig, H, W, cp_type)
    neig_r = circshift(neig, [0 1]);
    neig_l = circshift(neig, [0 -1]);
    neig_d = circshift(neig, [1 0]);
    neig_u = circshift(neig, [-1 0]);
    loc = cell(4,1);
    rmse = zeros(4,1);
    test = zeros(4,1);
    [loc{1}, rmse(1)] = find_subpix_cp_quadratic(J, neig_r, H, W, cp_type);
    test(1) = is_out_of_bounds(neig_r, loc{1}, H, W);
    [loc{2}, rmse(2)] = find_subpix_cp_quadratic(J, neig_l, H, W, cp_type);
    test(2) = is_out_of_bounds(neig_l, loc{2}, H, W);
    [loc{3}, rmse(3)] = find_subpix_cp_quadratic(J, neig_d, H, W, cp_type);
    test(3) = is_out_of_bounds(neig_d, loc{3}, H, W);
    [loc{4}, rmse(4)] = find_subpix_cp_quadratic(J, neig_u, H, W, cp_type);
    test(4) = is_out_of_bounds(neig_u, loc{4}, H, W);
    
    
    pos_loc = find(test == 0);
    if isempty(pos_loc)
        loc = [Inf Inf];
        rmse = -Inf;
        return
    end
    [min_rmse, min_ind] = min(rmse(pos_loc));
    loc = loc{pos_loc(min_ind)};
    rmse = rmse(pos_loc(min_ind));    
end

function neig = get_neighbor_coord(mask, H, W);		
	[r,c] = find(mask);
	neig = unique([c-1 r-1; c r-1; c+1 r-1; c-1 r; c r; c+1 r; c-1 r+1; c r+1; c+1 r+1], 'rows');	
end




%the code pieces below might again become useful!!!

%subplot(1,3,1);imagesc(Regs_curr);
%subplot(1,3,2);imagesc(Mask_prev_min);
%subplot(1,3,3);imagesc(Label_curr_min);
%pause

%look at the number of events, if there is more than 1 event -> lower water
%if 0&&Event_count > 1 
%     c2 = c2 / 2;
%     phi = phi + c2;
%     if c1/c2 <= factor_lower_water
%         continue
%     end
%end	
 
%c2 = c1;

%phi = phi - c1;
%ind = find(phi > WaterLevel);
%if ~isempty(ind)
%		phi = phi - min(phi(ind));
%else
%		break
%end

%maxv = max(phi(:));
%minv = min(phi(:));
	
	
% set min water level to 0.
%phi = phi - minv;

% default water level increment
%c1 = (maxv - minv)/10000;
%c2 = c1;

% backtracking parameters
%max_num_lower_water = 5; %used to be 8
%factor_lower_water = 2^max_num_lower_water;
