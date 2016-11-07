function show_critical_points(I, sigma, mins, maxs, saddles)
	
    function output_text = data_cursor(~, event_obj)
		DCM = get(event_obj);
		DCMpos = DCM.Position;
		v1 = find(ismember(saddles, DCMpos, 'rows'));			
		v2 = find(ismember(mins, DCMpos, 'rows'));
		v3 = find(ismember(maxs, DCMpos, 'rows'));
		if ~isempty(v1)
			output_text = sprintf('Saddle #%d', v1);			
		elseif ~isempty(v2)
				output_text = sprintf('Min #%d', v2);
		elseif ~isempty(v3)
			output_text = sprintf('Max #%d', v3);			
		else
			output_text = 'Regular';
		end
	end
	
	J = smooth_image(I,sigma);	
	figure;imagesc(J); colormap gray; hold on; 
    plot(maxs(:,1), maxs(:,2), '^', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
    plot(mins(:,1), mins(:,2), 'o', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
    plot(saddles(:,1), saddles(:,2), '+', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);	    		       
    hold off;
    axis image
    axis off
    set(datacursormode(gcf),'UpdateFcn',@data_cursor);
end
