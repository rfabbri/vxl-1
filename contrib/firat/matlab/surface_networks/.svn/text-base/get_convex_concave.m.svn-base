%get_convex_concave.m

% Created on: Mar 20, 2012
%     Author: firat
% idea from Critical nets paper

function get_convex_concave(I, sigma)
	I = double(I);%/255;
	
	if sigma > 0
		H1 = fspecial('gaussian', ceil(7*sigma)*[1 1], sigma);
		J = imfilter(I, H1, 'replicate');
		H = fspecial('log', ceil(7*sigma)*[1 1], sigma);
		I = imfilter(-I, H, 'replicate');		
	else
		error('needs sigma > 0');
	end	
	[H,W] = size(I);
	%saddle_index = [];
	maxness = zeros(H,W);
	minness = zeros(H,W);
	saddleness = zeros(H,W);
	for i = 2:H-1
		for j = 2:W-1
			P = I(i-1:i+1, j-1:j+1);
			v = P(5);
			P(5) = -Inf; 
			maxness(i,j) = I(i,j) - max(P(:));
			P(5) = Inf;
			minness(i,j) = min(P(:)) - I(i,j);
			S = P([1 2 3 6 9 8 7 4 1]) - v > 0;
			count = 0;
			for k = 1:8
				if S(k) ~= S(k+1)
					count = count + 1;
				end
			end
			saddleness(i,j) = double(count == 4);  
		end
	end
	figure;imagesc(J, [0,255]); colormap gray;
	[min_r, min_c] = find(minness > 0);
	[max_r, max_c] = find(maxness > 0);
	[sad_r, sad_c] = find(saddleness > 0);
	%% saddle point suppression
	D = squareform(pdist([sad_c sad_r]));
	[sup_r, sup_c] = find(D <= 1.5);
	sad_r(sup_r(sup_r < sup_c)) = [];
	sad_c(sup_r(sup_r < sup_c)) = [];
	%%
	length(min_r)
	length(max_r)
	length(sad_r)
	hold on;	
	plot(min_c, min_r, 'rx','MarkerSize', 6,'LineWidth', 2);
	plot(max_c, max_r, 'gx','MarkerSize', 6,'LineWidth', 2);
	plot(sad_c, sad_r, 'bo','MarkerSize', 6,'LineWidth', 2);
	legend({'min','max','saddle'},'Location','NorthEastOutside')
	title(sprintf('LoG, sigma = %.2f', sigma))
	axis image; axis off
	hold off;
end
