% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)

% TAG_detector_v2 is an incomplete attempt to solve v1's problems. It cannot construct the TAG. Instead, it produces an intermediate result.

function [mins, maxs, saddles, min_links, max_links, saddle_links] = TAG_detector_v2(I, sigma, do_plot)
    [ws, mins, WS_transform, saddle_ws, J, K, H, W, extended_J] = min_watershed_detector(I, sigma, false); % detect mins and watershed pixels
    [wc, maxs, WC_transform, saddle_wc] = min_watershed_detector(-I, sigma, false); % detect maxima and watercourse pixels      
    
    %1-to-1 matching 
    % find the common saddle candidates   
    D = pdist2(ws,wc);
    D(D > 0) = Inf; %match candidates only if their distance is 0 -> if they are the same!!!
    Matches = assignmentoptimal(D); % Hungarian matching
    
    % remove all watershed pixels without a corresponding watercourse pixel
    saddles = ws(Matches > 0, :); % saddle candidates
    num_saddles = size(saddles,1);


	% TODO
	% 1) Find the saddle chains in saddle candidates
	% 2) Find the pixels corresponding to saddles in each saddle chain
	% 3) Create saddle_links (the TAG) as in TAG_detector_v1, but take saddle chains into account as well.
	% 4) Create the alternative representations for the TAG, min_links and max_links. See TAG_detector_v1.
	
    % a function to display useful information when a pixel is clicked.
    function output_text = data_cursor(~, event_obj)
		DCM = get(event_obj);
		DCMpos = DCM.Position-K;
		v1 = find(ismember(ws, DCMpos, 'rows'));
		v2 = find(ismember(wc, DCMpos, 'rows'));
		v3 = find(ismember(mins, DCMpos, 'rows'));
		v4 = find(ismember(maxs, DCMpos, 'rows'));
		try
			output_text = {sprintf('Intensity: %f', J(DCMpos(2), DCMpos(1)))};
		catch
			output_text = {};
		end
		if ~isempty(v3)
			output_text = [output_text,sprintf('Min #%d', v3)];
		elseif ~isempty(v4)
			output_text = [output_text,sprintf('Max #%d', v4)];			
		end
		if ~isempty(v1)			
			output_text = [output_text, ['WS_transform #' num2str(v1) ' min = ' sprintf('%d ', saddle_ws{ws(v1,2)+K,ws(v1,1)+K})]];
			if ~isempty(v2)
				output_text = [output_text, ['WC_transform #' num2str(v2) ' max = ' sprintf('%d ', saddle_wc{wc(v2,2)+K,wc(v2,1)+K})]];
			end
		elseif ~isempty(v2)			
			output_text = [output_text,['WC_transform #' num2str(v2) ' max = ' sprintf('%d ', saddle_wc{wc(v2,2)+K,wc(v2,1)+K})]];
		end		
	end
    
    if do_plot        
        		    
		figure;imagesc(extended_J); colormap gray; hold on; 
		plot(maxs(:,1)+K, maxs(:,2)+K, '^', 'MarkerSize', 9, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
		plot(mins(:,1)+K, mins(:,2)+K, 'o', 'MarkerSize', 9, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);        
		plot(ws(:,1)+K, ws(:,2)+K, 'rx', 'MarkerSize', 5, 'LineWidth', 2);
		plot(wc(:,1)+K, wc(:,2)+K, 'gx', 'MarkerSize', 5, 'LineWidth', 2);       
		plot(saddles(:,1)+K, saddles(:,2)+K, 'yx', 'MarkerSize', 8, 'LineWidth', 2);
				   
		hold off;
		axis image
		axis off
		axis([K+1 W+K K+1 H+K])
    	set(datacursormode(gcf),'UpdateFcn',@data_cursor);                     
            
    end    
end

