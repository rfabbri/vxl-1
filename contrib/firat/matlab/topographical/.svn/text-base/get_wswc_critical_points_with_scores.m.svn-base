% This is get_wswc_critical_points_with_scores.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 12, 2012

function [S, mins, maxs, saddles] = get_wswc_critical_points_with_scores(I, sigma)
	
	show_points = false;
	
	[nrows,ncols] = size(I);
	G = fspecial('gaussian', ceil(7*sigma)*[1 1], sigma);
	[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(sigma, 1, 1);
	Ixx = imfilter(I, Gxx, 'replicate', 'conv');
	Iyy = imfilter(I, Gyy, 'replicate', 'conv');
	Ixy = imfilter(I, Gxy, 'replicate', 'conv');	
	S = imfilter(I,G,'replicate');
	SMEIG = arrayfun(@get_smallest_abs_eigenvalue, Ixx, Ixy, Iyy)./S;
	%min detection
	WS = watershed(S, 8); %watershed = max + saddle + ridges
	num_min = max(WS(:));
	min_ind = zeros(1, num_min);	
	for i = 1:num_min
		reg_ind = find(WS == i);
		ws_val = S(reg_ind);
		[ign, ind1] = min(ws_val);	
		min_ind(i) = reg_ind(ind1);	
	end	
	[y0,x0] = ind2sub([nrows, ncols], min_ind);
	mins = [x0; y0]';
	%max detection
	WC = watershed(-S, 8); %watercourse = min + saddle + valleys
	num_max = max(WC(:));
	max_ind = zeros(1, num_max);
	for i = 1:num_max
		reg_ind = find(WC == i);
		wc_val = S(reg_ind);
		[ign, ind1] = max(wc_val);	
		max_ind(i) = reg_ind(ind1);	
	end	
	[y0,x0] = ind2sub([nrows, ncols], max_ind);
	maxs = [x0; y0]';
	%saddle detection
	sadmask = (WC == 0) & (WS == 0);
	sadlab = bwlabel(sadmask, 8);
	rp = struct2array(regionprops(sadlab, 'centroid'));
	sad_c = round(rp(1:2:end)');
	sad_r = round(rp(2:2:end)');
	saddles = [sad_c, sad_r];
	
	min_ind = sub2ind([nrows, ncols], mins(:,2), mins(:,1));
	max_ind = sub2ind([nrows, ncols], maxs(:,2), maxs(:,1));
	sad_ind = sub2ind([nrows, ncols], saddles(:,2), saddles(:,1));
	
	mins = [mins, SMEIG(min_ind)];
	maxs = [maxs, SMEIG(max_ind)];
	saddles = [saddles, SMEIG(sad_ind)];
	
	if show_points
		figure;imagesc(S); colormap gray;
		hold on;	
		plot(mins(:,1), mins(:,2), 'rx','MarkerSize', 6,'LineWidth', 2);
		plot(maxs(:,1), maxs(:,2), 'gx','MarkerSize', 6,'LineWidth', 2);
		plot(saddles(:,1), saddles(:,2), 'bo','MarkerSize', 6,'LineWidth', 2);
		legend({'min','max','saddle'},'Location','NorthEastOutside')
		title(sprintf('Gaussian, sigma = %.2f', sigma))
		axis image; axis off
		hold off;
	end
end

function sm_eig = get_smallest_abs_eigenvalue(Ixx, Ixy, Iyy)
	Hess = [Ixx Ixy; Ixy Iyy];
	Q = eig(Hess);
	sm_eig = min(abs(Q));
end
