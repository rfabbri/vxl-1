%get_min_max_saddle.m

% Created on: Mar 15, 2012
% Updated on: Jul 26, 2012
%     Author: firat

function [S, mins, maxs, saddles] = get_min_max_saddle(I, sigma)
	%%options
	use_hexsaddle = 0;
	use_suppression = 1;
	show_points = 1;
	detect_boundary = 1;
	%%	
		
	if sigma > 0
		[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(sigma, 1, 1);
		Ixx = imfilter(I, Gxx, 'replicate', 'conv');
		Iyy = imfilter(I, Gyy, 'replicate', 'conv');
		Ixy = imfilter(I, Gxy, 'replicate', 'conv');	
		H = fspecial('gaussian', ceil(7*sigma)*[1 1], sigma);
		I = imfilter(I, H, 'replicate');		
	end		
	[H,W] = size(I);
	maxness = zeros(H,W);
	minness = zeros(H,W);
	if ~use_hexsaddle
		saddleness = zeros(H,W);
	end
	%%inner critical points
	for i = 2:H-1
		for j = 2:W-1
			P = I(i-1:i+1, j-1:j+1);
			v = P(5);
			P(5) = -Inf; 
			maxness(i,j) = v - max(P(:));
			P(5) = Inf;
			minness(i,j) = min(P(:)) - v;
			if ~use_hexsaddle
				S = P([1 2 3 6 9 8 7 4 1]) - v > 0;
				count = 0;
				for k = 1:8
					if S(k) ~= S(k+1)
						count = count + 1;
					end
				end
				Hess = [Ixx(i,j) Ixy(i,j); Ixy(i,j) Iyy(i,j)];
				Q = det(Hess);
				%saddleness(i,j) = double(count == 4);				
				saddleness(i,j) = count == 4 && Q < 0; 
			end 
		end
	end
	if detect_boundary
		%%
		%%boundary critical points
		%% (1,1)
		neig = [I(1,2) I(2,2) I(2,1)];
		maxness(1,1) = I(1,1) - max(neig);
		minness(1,1) = min(neig) - I(1,1);
		S = neig - I(1,1) > 0;
		Hess = [Ixx(1,1) Ixy(1,1); Ixy(1,1) Iyy(1,1)];
		Q = det(Hess);
		saddleness(1,1) = S(1) ~= S(2) && S(2) ~= S(3) && Q < 0;	
		%% (1,W)
		neig = [I(1,W-1) I(2,W-1) I(2,W)];
		maxness(1,W) = I(1,W) - max(neig);
		minness(1,W) = min(neig) - I(1,W);
		S = neig - I(1,W) > 0;
		Hess = [Ixx(1,W) Ixy(1,W); Ixy(1,W) Iyy(1,W)];
		Q = det(Hess);
		saddleness(1,W) = S(1) ~= S(2) && S(2) ~= S(3) && Q < 0;	
		%% (H,1)
		neig = [I(H-1,1) I(H-1,2) I(H,2)];
		maxness(H,1) = I(H,1) - max(neig);
		minness(H,1) = min(neig) - I(H,1);
		S = neig - I(H,1) > 0;
		Hess = [Ixx(H,1) Ixy(H,1); Ixy(H,1) Iyy(H,1)];
		Q = det(Hess);
		saddleness(H,1) = S(1) ~= S(2) && S(2) ~= S(3) && Q < 0;		
		%% (H,W)
		neig = [I(H-1,W) I(H-1,W-1) I(H,W-1)];
		maxness(H,W) = I(H,W) - max(neig);
		minness(H,W) = min(neig) - I(H,W);
		S = neig - I(H,W) > 0;
		Hess = [Ixx(H,W) Ixy(H,W); Ixy(H,W) Iyy(H,W)];
		Q = det(Hess);
		saddleness(H,W) = S(1) ~= S(2) && S(2) ~= S(3) && Q < 0;
		%% (1,j) where 1 < j < width
		for j = 2:W-1
			P = I(1:2, j-1:j+1);
			v = P(3);
			P(3) = -Inf;
			maxness(1,j) = v - max(P(:));
			P(3) = Inf;
			minness(1,j) = min(P(:)) - v;
			S = P([1 2 4 5 6]) - v > 0;
			count = 0;
			for k = 1:4
				if S(k) ~= S(k+1)
					count = count + 1;
				end
			end
			Hess = [Ixx(1,j) Ixy(1,j); Ixy(1,j) Iyy(1,j)];
			Q = det(Hess);
			saddleness(1,j) = count >= 2 && Q < 0;			
		end
		%% (H,j) where 1 < j < width
		for j = 2:W-1
			P = I(H-1:H, j-1:j+1);
			v = P(4);
			P(4) = -Inf;
			maxness(H,j) = v - max(P(:));
			P(4) = Inf;
			minness(H,j) = min(P(:)) - v;
			S = P([2 1 3 5 6]) - v > 0;
			count = 0;
			for k = 1:4
				if S(k) ~= S(k+1)
					count = count + 1;
				end
			end
			Hess = [Ixx(H,j) Ixy(H,j); Ixy(H,j) Iyy(H,j)];
			Q = det(Hess);
			saddleness(H,j) = count >= 2 && Q < 0;			
		end
		%% (i,1) where 1 < i < height
		for i = 2:H-1
			P = I(i-1:i+1, 1:2);
			v = P(2);
			P(2) = -Inf;
			maxness(i,1) = v - max(P(:));
			P(2) = Inf;
			minness(i,1) = min(P(:)) - v;
			S = P([1 4 5 6 3]) - v > 0;
			count = 0;
			for k = 1:4
				if S(k) ~= S(k+1)
					count = count + 1;
				end
			end
			Hess = [Ixx(i,1) Ixy(i,1); Ixy(i,1) Iyy(i,1)];
			Q = det(Hess);
			saddleness(i,1) = count >= 2 && Q < 0;
		end
		%% (i,W) where 1 < i < height
		for i = 2:H-1
			P = I(i-1:i+1, W-1:W);
			v = P(5);
			P(5) = -Inf;
			maxness(i,W) = v - max(P(:));
			P(5) = Inf;
			minness(i,W) = min(P(:)) - v;
			S = P([4 1 2 3 6]) - v > 0;
			count = 0;
			for k = 1:4
				if S(k) ~= S(k+1)
					count = count + 1;
				end
			end
			Hess = [Ixx(i,W) Ixy(i,W); Ixy(i,W) Iyy(i,W)];
			Q = det(Hess);
			saddleness(i,W) = count >= 2 && Q < 0;
		end
	end
		
	[min_r, min_c] = find(minness > 0);
	[max_r, max_c] = find(maxness > 0);
	if ~use_hexsaddle
		sadmask = saddleness > 0;			
		if use_suppression			
			%% saddle point suppression
			sadlab = bwlabel(sadmask, 8);
			rp = struct2array(regionprops(sadlab, 'centroid'));
			sad_c = rp(1:2:end)';
			sad_r = rp(2:2:end)';
			
			%D = squareform(pdist([sad_c sad_r]));
			%[sup_r, sup_c] = find(D <= 1.5);
			%sad_r(sup_r(sup_r < sup_c)) = [];
			%sad_c(sup_r(sup_r < sup_c)) = [];
			%%
		else
			[sad_r, sad_c] = find(sadmask);		
		end
		saddles = [sad_c, sad_r];
	else	
		%%use HEXSADDLE
		saddles = hexsaddle(I, sigma);
		sad_c = saddles(:,1);
		sad_r = saddles(:,2);
		%%
	end
	mins = [min_c, min_r];
	maxs = [max_c, max_r];	
	S = I;
	if show_points
		figure;imagesc(I); colormap gray;
		hold on;	
		plot(min_c, min_r, 'rx','MarkerSize', 6,'LineWidth', 2);
		plot(max_c, max_r, 'gx','MarkerSize', 6,'LineWidth', 2);
		plot(sad_c, sad_r, 'bo','MarkerSize', 6,'LineWidth', 2);
		legend({'min','max','saddle'},'Location','NorthEastOutside')
		title(sprintf('Gaussian, sigma = %.2f', sigma))
		axis image; axis off
		hold off;
	end	
end
