% This is levelset_watershed.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jan 3, 2013

function [phi, J] = levelset_watershed(I, sigma, WS, WC)	
	addpath ~/lemsvxl/bin/mex
	addpath ~/lemsvxl/src/contrib/firat/matlab/levelset2d_segmentation/tools
	addpath ~/lemsvxl/src/contrib/firat/matlab/topographical
	do_extend = true;
	if sigma > 0
		L = round(7*sigma);
		if mod(L,2) == 0
			L = L + 1;
		end	
		if do_extend
		    K = (L-1)/2;
		    I = [I(:, K+1:-1:2), I, I(:, end-1:-1:end-K)];
		    I = [I(K+1:-1:2, :); I; I(end-1:-1:end-K, :)];
		else
		    K = 0;
		end
		
		H = fspecial('gaussian', [L L], sigma);
		phi = imfilter(I, H, 'replicate');	
	else
		phi = I;	
	end	
	J = phi;
	if exist('WC', 'var')
		imagesc(J);axis image; colormap gray; hold on
		contour(WS, [0, 0],'r');contour(WC, [0, 0],'g');
		hold off;
		return
	end
	
	[S, mins, maxs, saddles] = get_critical_points(J, 0);
	[H,W] = size(phi);
	
	maxv = max(phi(:));
	minv = min(phi(:));
	
	c1 = (maxv - minv)/300;
	c2 = (maxv - minv)/800;
	phi = phi - minv;
	iter = 0;
	comp2 = zeros(H,W);
	zero_count = 0;
	while true
		iter = iter + 1;
		imagesc(J);axis image; colormap gray; hold on
		ctrs = plot_contours(phi, 0);
		%ctrs = contourc(phi, [0, 0]);
		plot(mins(:,1), mins(:,2), 'o', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
		plot(maxs(:,1), maxs(:,2), '^', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
		hold off
		title(sprintf('Iter = %d', iter));
		axis([K+1 W-K K+1 H-K]);
		drawnow
		
		%[regmask,N] = bwlabel(phi <= 0);
		S = zeros(size(phi));
		j = 1;
		%all_contours = {};
		if ~isempty(ctrs)
			while j <= size(ctrs,2)
				con = ctrs(:, j+1:j+ctrs(2,j))';
		%		all_contours = [all_contours; con];
				D = mex_poly_dist_trans({con}, H, W, 1, 1);
				S = S + (D <= 1.5);
				j = j + ctrs(2,j) + 1;
			end
		end
		comp1 = comp2;
		%DT = mex_poly_dist_trans(all_contours, H, W, 1, 1);
		phi = phi - c1*(S < 2);% - c2*(S >= 2 & DT > .5);
		%phi = phi - c1;
		phi = make_border_and_nans_zero(phi);
		comp2 = phi < 0;
		diff = norm(comp1(:) - comp2(:));
		if diff == 0
			zero_count = zero_count + 1;
		else
			zero_count = 0;
		end
		if zero_count == 60
			break
		end
	end
	
end

function ctrs = plot_contours(phi, mode)
	if mode == 0
		ctrs = contour(phi, [0, 0], 'r');
	else
		ctrs = compute_zero_level_curve(phi, 1, 1);
		j = 1;
		if ~isempty(ctrs)
			while j <= size(ctrs,2)
				con = ctrs(:, j+1:j+ctrs(2,j))';
				plot(con([1:end, 1],1), con([1:end,1],2), 'r');
				j = j + ctrs(2,j) + 1;
			end
		end
	end
end


