% This is cpdetect.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 7, 2013

function [mins, maxs, saddles, J, WS, WC] = cpdetect2(I, sigma, do_plot)
    [saddles1, mins, WS, saddle_ws, J, K, H, W] = msdetect(I, sigma, false);
    [saddles2, maxs, WC, saddle_wc] = msdetect(-I, sigma, false);   
    
    function flag = wswc_intersect(s1, s2)
    	sad1 = saddles1(s1,:);
    	sad2 = saddles2(s2,:);
    	min1 = mins(saddle_ws(s1,1),:);
    	min2 = mins(saddle_ws(s1,2),:);
    	max1 = maxs(saddle_wc(s2,1),:);
    	max2 = maxs(saddle_wc(s2,2),:);
    	
    	temp1 = [min1;min2;sad1];
    	bb1_min = min(temp1);
    	bb1_max = max(temp1);
    	temp2 = [max1;max2;sad2];
    	bb2_min = min(temp2);
    	bb2_max = max(temp2);
    	
    	[~,sindex1] = sort([bb1_min(1) bb1_max(1) bb2_min(1) bb2_max(1)]);
    	[~,sindex2] = sort([bb1_min(2) bb1_max(2) bb2_min(2) bb2_max(2)]);
    	if (all(sindex1 == 1:4) || all(sindex1 == [3 4 1 2])) && (all(sindex2 == 1:4) || all(sindex2 == [3 4 1 2]))
    		flag = false;
    		return;
    	end    	
    	function flag = ls_intersect(p1, p2, r1, r2)
    		A = [p2(1)-p1(1), r1(1)-r2(1); p2(2)-p1(2), r1(2)-r2(2)];
    		if cond(A) > 1e10;
    			flag = false;
    			return
    		end
    		b = [r1(1)-p1(1); r1(2)-p1(2)];
    		x = A\b;
    		
    		flag = all(x >= 0 & x <= 1);
    	end    	
    	flag = ls_intersect(min1, sad1, max1, sad2) || ls_intersect(min1, sad1, max2, sad2) || ls_intersect(min2, sad1, max1, sad2) || ls_intersect(min2, sad1, max2, sad2);  	
    end    
    
	Matches1 = zeros(size(saddles1,1),1);
	to_delete = zeros(size(saddles2,1),1);    	
   	for i = 1:size(saddles1,1)
		for j = 1:size(saddles2,1)
			if wswc_intersect(i, j)
				Matches1(i) = j;
				to_delete(j) = 1;    				 
				break; 
			end
		end
	end
	saddles = [saddles1; saddles2(~to_delete,:)];
    
    
    function output_text = data_cursor(~, event_obj)
		DCM = get(event_obj);
		DCMpos = DCM.Position;
		v1 = find(ismember(saddles1, DCMpos, 'rows'));
		v2 = find(ismember(saddles2, DCMpos, 'rows'));
		v3 = find(ismember(mins, DCMpos, 'rows'));
		v4 = find(ismember(maxs, DCMpos, 'rows'));
		if ~isempty(v1)
			output_text = sprintf('Saddle #%d WS = %d, %d', v1, saddle_ws(v1,1), saddle_ws(v1,2));
			if ~isempty(v2)
				output_text = {output_text, sprintf('Saddle #%d WC = %d, %d', v2, saddle_wc(v2,1), saddle_wc(v2,2))};
			end
		elseif ~isempty(v2)
			output_text = sprintf('Saddle #%d WC = %d, %d', v2, saddle_wc(v2,1), saddle_wc(v2,2));
		elseif ~isempty(v3)
			output_text = sprintf('Min #%d', v3);
		elseif ~isempty(v4)
			output_text = sprintf('Max #%d', v4);			
		else
			output_text = 'Regular';
		end
	end
    
    if do_plot        
        
        figure;imagesc(J); colormap gray; hold on; 
        plot(maxs(:,1), maxs(:,2), '^', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
        plot(mins(:,1), mins(:,2), 'o', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);        
	    plot(saddles1(:,1), saddles1(:,2), 'r+', 'MarkerSize', 8, 'LineWidth', 2);
	    plot(saddles2(:,1), saddles2(:,2), 'g+', 'MarkerSize', 5, 'LineWidth', 2);		       
        hold off;
        axis image
        axis off
        set(datacursormode(gcf),'UpdateFcn',@data_cursor);   
        
        
        figure;imagesc(J); colormap gray; hold on; 
        plot(maxs(:,1), maxs(:,2), '^', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
        plot(mins(:,1), mins(:,2), 'o', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
        plot(saddles(:,1), saddles(:,2), '+', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);	    		       
        hold off;
        axis image
        axis off  
        
        %plot the surface network
        figure;imagesc(J); colormap gray; hold on;
        for i = 1:length(Matches1)
        	min1 = mins(saddle_ws(i,1),:);
        	min2 = mins(saddle_ws(i,2),:);
        	sad = saddles1(i,:);
        	line([sad(1) min1(1)], [sad(2) min1(2)],'Color', [255 160 0]/255, 'LineWidth', 2);
        	line([sad(1) min2(1)], [sad(2) min2(2)],'Color', [255 160 0]/255, 'LineWidth', 2);
        	if Matches1(i) > 0
        		max1 = maxs(saddle_wc(Matches1(i),1),:);
        		max2 = maxs(saddle_wc(Matches1(i),2),:);
        		line([sad(1) max1(1)], [sad(2) max1(2)],'Color', [139, 0, 255]/255, 'LineWidth', 2);
        		line([sad(1) max2(1)], [sad(2) max2(2)],'Color', [139, 0, 255]/255, 'LineWidth', 2);
        	end
        end
        remaining_sad = setdiff(1:size(saddles2,1), Matches1');
        for i = 1:length(remaining_sad)
    		sad = saddles2(remaining_sad(i),:);
    		max1 = maxs(saddle_wc(remaining_sad(i),1),:);
    		max2 = maxs(saddle_wc(remaining_sad(i),2),:);
    		line([sad(1) max1(1)], [sad(2) max1(2)],'Color', [139, 0, 255]/255, 'LineWidth', 2);
    		line([sad(1) max2(1)], [sad(2) max2(2)],'Color', [139, 0, 255]/255, 'LineWidth', 2);
        end
        plot(maxs(:,1), maxs(:,2), '^', 'MarkerSize', 8, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
        plot(mins(:,1), mins(:,2), 'o', 'MarkerSize', 8, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
        plot(saddles(:,1), saddles(:,2), '+', 'MarkerSize', 8, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
        hold off;
        axis image
        axis off 
           
    end
    %fprintf('#min: %d, #max: %d, #saddle: %d\n', size(mins,1), size(maxs, 1), size(saddles, 1));
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
