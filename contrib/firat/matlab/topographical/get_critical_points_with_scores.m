%get_critical_points_with_scores.m

% Created on: Aug 28, 2012
%     Author: firat

% 8-neighbor critical point detection with Hessian based saddle point verification
% critical point format: [x y score]
% score types: min, max, mean, gauss, neighbor

function [S, mins, maxs, saddles] = get_critical_points_with_scores(I, sigma, score_type)
	if ~exist('score_type', 'var')
		score_type = 'min';
	end
	
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
	
	maxI = max(I(:));
	%%inner critical points
	for i = 2:H-1
		for j = 2:W-1
			Hess = [Ixx(i,j) Ixy(i,j); Ixy(i,j) Iyy(i,j)];
			Q = eig(Hess);
			switch(score_type)
				case 'min'
					sm_eig = min(abs(Q))/I(i,j);
				case 'mean'
					sm_eig = mean(abs(Q))/I(i,j);
				case 'gauss'
					sm_eig = prod(abs(Q))/I(i,j).^2;
				case 'max'
					sm_eig = max(abs(Q))/I(i,j);
			end 
			P = I(i-1:i+1, j-1:j+1);
			v = P(5);
			P(5) = -Inf; 
			maxdiff = v - max(P(:));
			if strcmp(score_type, 'neighbor')
				maxness(i,j) = (maxdiff > 0)*maxdiff/maxI;
			else
				maxness(i,j) = (maxdiff > 0)*sm_eig;
			end
			P(5) = Inf;
			mindiff = min(P(:)) - v;
			if strcmp(score_type, 'neighbor')
				minness(i,j) = (mindiff > 0)*mindiff/maxI;
			else
				minness(i,j) = (mindiff > 0)*sm_eig;			
			end
			S = P([1 2 3 6 9 8 7 4 1]) - v;
			
			count = 0;
			S(S == 0) = [];
			saddiff = min(abs(S))/maxI;
			S = S > 0;
			for k = 1:length(S)-1
				if S(k) ~= S(k+1)
					count = count + 1;
				end
			end
			if count == 4
				%Hess = [Ixx(i,j) Ixy(i,j); Ixy(i,j) Iyy(i,j)];
				%Q = det(Hess);
				
				if strcmp(score_type, 'neighbor')
					saddleness(i,j) = (det(Hess) < 0)*saddiff;
				else
					saddleness(i,j) = (det(Hess) < 0)*sm_eig;
				end
			%	saddleness(i,j) = 1;
			end 			 
		end
	end
			
	[min_r, min_c] = find(minness > 0);
	[max_r, max_c] = find(maxness > 0);
	
	if use_suppression			
		%% saddle point suppression
		%sadlab = bwlabel(sadmask, 8);
		%rp = struct2array(regionprops(sadlab, 'centroid'));
		%sad_c = round(rp(1:2:end)');
		%sad_r = round(rp(2:2:end)');
		
		%D = squareform(pdist([sad_c sad_r]));
		%[sup_r, sup_c] = find(D <= 1.5);
		%sad_r(sup_r(sup_r < sup_c)) = [];
		%sad_c(sup_r(sup_r < sup_c)) = [];
		%%
		
		[sad_r,sad_c] = nonmaxsuppts(saddleness, 1, 0);
		
		
	else
		[sad_r, sad_c] = find(saddleness > 0);		
	end
	saddles = [sad_c, sad_r]-1;	
	mins = [min_c, min_r]-1;
	maxs = [max_c, max_r]-1;	
	S = I(2:end-1,2:end-1);
	if show_points
		figure;imagesc(S); colormap gray;
		hold on;	
		plot(min_c-1, min_r-1, 'rx','MarkerSize', 6,'LineWidth', 2);
		plot(max_c-1, max_r-1, 'gx','MarkerSize', 6,'LineWidth', 2);
		plot(sad_c-1, sad_r-1, 'bo','MarkerSize', 6,'LineWidth', 2);
		legend({'min','max','saddle'},'Location','NorthEastOutside')
		title(sprintf('Gaussian, sigma = %.2f', sigma))
		axis image; axis off
		hold off;
	end	
	
	min_ind = sub2ind([H W], min_r, min_c);
	max_ind = sub2ind([H W], max_r, max_c);
	sad_ind = sub2ind([H W], sad_r, sad_c);
	
	mins = [mins, minness(min_ind)];
	maxs = [maxs, maxness(max_ind)];
	saddles = [saddles, saddleness(sad_ind)];
end
