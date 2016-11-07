%get_min_max_saddle_reflection.m

% Created on: Jul 30, 2012
%     Author: firat

function [S, mins, maxs, saddles] = get_min_max_saddle_reflection(I, sigma)
	%%options
	use_suppression = 1;
	show_points = 0;
	reflect_first = 0;	
	%%	
	I = double(I);	
	
	if sigma > 0			
		H = fspecial('gaussian', ceil(7*sigma)*[1 1], sigma);
		I = imfilter(I, H, 'replicate');		
	end	
	if reflect_first
		I = [I(1,:); I; I(end,:)];
		I = [I(:,1), I, I(:,end)];
	else %reflect second
		I = [I(2,:); I; I(end-1,:)];
		I = [I(:,2), I, I(:,end-1)];
		I(1,1) = I(3,3);
		I(1, end) = I(3, end-2);
		I(end, 1) = I(end-2, 3);
		I(end, end) = I(end-2, end-2);
	end
	
	[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(sigma, 1, 1);
	Ixx = imfilter(I, Gxx, 'replicate', 'conv');
	Iyy = imfilter(I, Gyy, 'replicate', 'conv');
	Ixy = imfilter(I, Gxy, 'replicate', 'conv');
		
	[H,W] = size(I);
	maxness = zeros(H,W);
	minness = zeros(H,W);
	saddleness = zeros(H,W);
	
	%%inner critical points
	for i = 2:H-1
		for j = 2:W-1
			P = I(i-1:i+1, j-1:j+1);
			v = P(5);
			P(5) = -Inf; 
			maxness(i,j) = v - max(P(:));
			P(5) = Inf;
			minness(i,j) = min(P(:)) - v;			
			S = P([1 2 3 6 9 8 7 4 1]) - v > 0;
			count = 0;
			for k = 1:8
				if S(k) ~= S(k+1)
					count = count + 1;
				end
			end
			if count == 4
				Hess = [Ixx(i,j) Ixy(i,j); Ixy(i,j) Iyy(i,j)];
				Q = det(Hess);
				saddleness(i,j) = Q < 0;
			end 			 
		end
	end
	
		
	[min_r, min_c] = find(minness > 0);
	[max_r, max_c] = find(maxness > 0);
	
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
	saddles = [sad_c, sad_r]-1;	
	mins = [min_c, min_r]-1;
	maxs = [max_c, max_r]-1;	
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
