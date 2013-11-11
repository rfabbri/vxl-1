function plot_tag(I, sigma, mins, maxs, saddles, saddle_links, inp_mode)
	addpath ~/lemsvxl/src/contrib/firat/matlab/levelset_watershed/experiments/vl_feat_tools
	%plot tag
	J = smooth_image(I, sigma);
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
    figure;imagesc(J); colormap gray; hold on;
    for i = 1:size(saddles, 1)     	
    	sad = saddles(i,:);          	
    	min1 = mins(saddle_links(i,1),:);
    	min2 = mins(saddle_links(i,2),:);	    	
    	line([sad(1) min1(1)], [sad(2) min1(2)],'Color', [255 160 0]/255, 'LineWidth', 2);
    	line([sad(1) min2(1)], [sad(2) min2(2)],'Color', [255 160 0]/255, 'LineWidth', 2);	
		max1 = maxs(saddle_links(i,3),:);
		max2 = maxs(saddle_links(i,4),:);
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


