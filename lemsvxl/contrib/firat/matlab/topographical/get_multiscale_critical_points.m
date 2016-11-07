% This is get_multiscale_critical_points.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 8, 2012

function [S, mins, maxs, saddles] = get_multiscale_critical_points(I, sigmas)
	if ~exist('sigmas', 'var')
		sigmas = [1, sqrt(2),2, 2*sqrt(2), 4, 4*sqrt(2), 8];
	end
	show_points = false;
	J = double(I);	
	
	[H,W] = size(I);
	H = H+2;
	W = W+2;
	num_sigma = length(sigmas);
	maxness = zeros(H,W,num_sigma);
	minness = zeros(H,W,num_sigma);
	saddleness = zeros(H,W,num_sigma);
	
	for sigma_index = 1:num_sigma
		sigma = sigmas(sigma_index);
		if sigma > 0	
			L = ceil(7*sigma);
			if mod(L,2) == 0
				L = L + 1;
			end		
			Hfilt = fspecial('gaussian', [L L], sigma);
			I = imfilter(J, Hfilt, 'replicate');
		else
			I = J;		
		end	
		
		I = [I(2,:); I; I(end-1,:)];
		I = [I(:,2), I, I(:,end-1)];
		I(1,1) = I(3,3);
		I(1, end) = I(3, end-2);
		I(end, 1) = I(end-2, 3);
		I(end, end) = I(end-2, end-2);
		
	
		[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(sigma, 1, 1);
		Ixx = imfilter(I, Gxx, 'replicate', 'conv');
		Iyy = imfilter(I, Gyy, 'replicate', 'conv');
		Ixy = imfilter(I, Gxy, 'replicate', 'conv');	
		
		%%inner critical points
		for i = 2:H-1
			for j = 2:W-1
				Hess = [Ixx(i,j) Ixy(i,j); Ixy(i,j) Iyy(i,j)];
				Q = eig(Hess);
				sm_eig = (sigma^2)*min(abs(Q)); %normalized wrt scale
				P = I(i-1:i+1, j-1:j+1);
				v = P(5);
				P(5) = -Inf; 
				maxness(i,j,sigma_index) = ((v - max(P(:))) > 0)*sm_eig;
				P(5) = Inf;
				minness(i,j,sigma_index) = ((min(P(:)) - v) > 0)*sm_eig;			
				S = P([1 2 3 6 9 8 7 4 1]) - v;
				count = 0;
				S(S == 0) = [];
				S = S > 0;
				for k = 1:length(S)-1
					if S(k) ~= S(k+1)
						count = count + 1;
					end
				end
				if count == 4					
					saddleness(i,j,sigma_index) = (det(Hess) < 0)*sm_eig;				
				end 			 
			end
		end
	end
	
	
	
	for sigma_index = 2:num_sigma-1
		for i = 2:H-1
			for j = 2:W-1
				v = maxness(i,j,sigma_index);
				if v > 0
					P = maxness(i-1:i+1, j-1:j+1, sigma_index-1:sigma_index+1);				
					P(14) = 0;
					if ~all(v > P(:))
						maxness(i,j,sigma_index) = 0;
					end
				end
				
				v = minness(i,j,sigma_index);
				if v > 0
					P = minness(i-1:i+1, j-1:j+1, sigma_index-1:sigma_index+1);				
					P(14) = 0;
					if ~all(v > P(:))
						minness(i,j,sigma_index) = 0;
					end
				end
				
				v = saddleness(i,j,sigma_index);
				if v > 0
					P = saddleness(i-1:i+1, j-1:j+1, sigma_index-1:sigma_index+1);				
					P(14) = 0;
					if ~all(v > P(:))
						saddleness(i,j,sigma_index) = 0;
					end
				end
			end
		end
	end
	
	maxness(:,:,num_sigma) = 0;
	minness(:,:,num_sigma) = 0;
	saddleness(:,:,num_sigma) = 0;
	
	maxness(:,:,1) = 0;
	minness(:,:,1) = 0;
	saddleness(:,:,1) = 0;
		
	
	maxness = max(maxness, [], 3);
	minness = max(minness, [], 3);
	saddleness = max(saddleness, [], 3);
	
	[min_r, min_c] = find(minness > 0);
	[max_r, max_c] = find(maxness > 0);
	[sad_r, sad_c] = find(saddleness > 0);
		
	saddles = [sad_c, sad_r]-1;	
	mins = [min_c, min_r]-1;
	maxs = [max_c, max_r]-1;	
	S = I;
	
	min_ind = sub2ind([H W], min_r, min_c);
	max_ind = sub2ind([H W], max_r, max_c);
	sad_ind = sub2ind([H W], sad_r, sad_c);
	
	mins = [mins, minness(min_ind)];
	maxs = [maxs, maxness(max_ind)];
	saddles = [saddles, saddleness(sad_ind)];
	
	if show_points
		figure;imagesc(J); colormap gray;
		hold on;	
		plot(min_c-1, min_r-1, 'rx','MarkerSize', 6,'LineWidth', 2);
		plot(max_c-1, max_r-1, 'gx','MarkerSize', 6,'LineWidth', 2);
		plot(sad_c-1, sad_r-1, 'bo','MarkerSize', 6,'LineWidth', 2);
		legend({'min','max','saddle'},'Location','NorthEastOutside')
		axis image; axis off
		hold off;
	end
	
end
