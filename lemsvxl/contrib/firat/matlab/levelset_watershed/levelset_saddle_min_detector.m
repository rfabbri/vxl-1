% This is /lemsvxl/contrib/firat/matlab/levelset_watershed/levelset_saddle_min_detector.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)

function [saddles, mins, J, K, H, W] = levelset_saddle_min_detector(I, sigma, do_plot)
    addpath ~/lemsvxl/bin/mex
	addpath ~/lemsvxl/src/contrib/firat/matlab/levelset2d_segmentation/tools
	addpath ~/lemsvxl/src/contrib/firat/matlab/topographical
	addpath ~/lemsvxl/src/contrib/firat/matlab/subpixel_wcws
	
	do_extend = true;
	
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
	
	% smooth the image
	if sigma > 0					
		H = fspecial('gaussian', [L L], sigma);
		phi = imfilter(I, H, 'replicate');	
	else
		phi = I;	
	end
	
	saddles = java.util.LinkedList;
	mins = java.util.LinkedList;
	
	J = phi;
	[H,W] = size(phi);
	
	maxv = max(phi(:));
	minv = min(phi(:));
	
	% set min water level to 0.
	phi = phi - minv;
	
	% default water level increment
	c1 = (maxv - minv)/400;
	c2 = c1;
	
	% masks and label maps to detect creation and collision events
	Mask_prev = false(size(phi));
	Label_prev = zeros(size(phi));
	
	% backtracking parameters
	max_num_lower_water = 5; %used to be 8
	factor_lower_water = 2^max_num_lower_water;
	
	while any(any(phi >= 0))
	   %the new mask
	   Mask_curr = -(phi < 0); 	   
	   %transfer the labels from the previous iteration
	   Label_curr = Mask_curr;
	   Label_curr(Mask_prev) = Label_prev(Mask_prev);
	   %determine the event locations
	   Event_mask = Label_curr == -1;
	   [Event_labels, Event_count] = bwlabel(Event_mask);
	   %look at the number of events, if there is more than 1 event -> lower water
	   if Event_count > 1 
	        c2 = c2 / 2;
	        phi = phi + c2;
	        if c1/c2 <= factor_lower_water
	            continue
	        end
	   end	   
	   c2 = c1;  
	   %iterate over the events 	   
	   for i = 1:Event_count
	        ith_event_mask = Event_labels == i;
	        neig = imdilate(ith_event_mask, strel('square',3));
	        Unique_labels = unique(Label_curr(neig));        
	        Unique_labels(Unique_labels <= 0) = [];
	        num_unique = length(Unique_labels);	 
	        if num_unique == 1 % regular point
	            continue
	        elseif num_unique == 0
	            cp_type = 1; %min
	        else
	            cp_type = 2; %saddle
	        end       
	        [neig_y,neig_x] = find(neig);
            if all(neig_x < K)  || all(neig_y < K) || all(neig_x > W-K+1)  || all(neig_y > H-K+1) 
                break
            end 	            
            %num_neig = length(neig_x);
            %while num_neig < 9
            %    neig = imdilate(neig, strel('square',3));
            %    num_neig = sum(neig(:));
            %end            
	        %[cp_loc, rmse] = find_subpix_cp_quadratic(J, neig, H, W, cp_type);
	        cp_loc = mean([neig_x, neig_y]); % DO NOT USE SURFACE FITTING		        
            %if is_out_of_bounds(neig, cp_loc, H, W)
            if false
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
                elseif cp_type == 1 % creation
                    mins.add(cp_loc2);
                    fprintf('Min/max point localization completed successfully...\n'); 
                else % collision
                    saddles.add(cp_loc2);
                    fprintf('Saddle point localization completed successfully...\n');                   
                end                          
            elseif cp_type == 1 % creation   
                mins.add(cp_loc);
            else %collision
                saddles.add(cp_loc);
            end		       	        
       end	   
	   Label_prev = bwlabel(Mask_curr);
	   Mask_prev = logical(Mask_curr);
	   phi = phi - c1;
	end
    mins = reshape(cell2mat(cell(mins.toArray(mins))), 2, mins.size())'-K;
    [tbd1, ign] = find(mins < 0.5);
    tbd2 = find(mins(:,1) > W-2*K+.5);
    tbd3 = find(mins(:,2) > H-2*K+.5);
    mins([tbd1; tbd2; tbd3],:) = [];    
    saddles = reshape(cell2mat(cell(saddles.toArray(saddles))), 2, saddles.size())'-K;
    [tbd1, ign] = find(saddles < 0.5);
    tbd2 = find(saddles(:,1) > W-2*K+.5);
    tbd3 = find(saddles(:,2) > H-2*K+.5);
    saddles([tbd1; tbd2; tbd3],:) = [];
    if do_plot
        figure;imagesc(J); colormap gray; hold on; 
        plot(mins(:,1)+K, mins(:,2)+K, 'o', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
	    plot(saddles(:,1)+K, saddles(:,2)+K, '+', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);	    
        hold off;
        axis([K+1 W-K K+1 H-K]);
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

%subplot(1,3,1);imagesc(Regs_curr);
%subplot(1,3,2);imagesc(Mask_prev);
%subplot(1,3,3);imagesc(Label_curr);
%pause
