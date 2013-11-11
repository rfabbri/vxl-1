% This is subpixel_watershed_transform.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Nov 13, 2012

function subpixel_watershed_transform(f, sigma)
	if sigma > 0
		L = ceil(6*sigma);
		if mod(L,2) == 0
			L = L + 1;
		end
		H = fspecial('gaussian', [L L], sigma);
		f = imfilter(f, H, 'replicate');
	end
	[H,W] = size(f);
	if 0
	sad_loc = subpixel_saddle(f);
	figure;imagesc(f); colormap gray; axis image; axis off
	hold on
	plot(sad_loc(:,1), sad_loc(:,2),'r.')
	hold off
	end
	
	if 0
	saddle_mask2 = zeros(H,W);
	
	for yi = window+1:H-window
		for xi = window+1:W-window
			z = f(yi-window:yi+window, xi-window:xi+window);
			p = polyfitn([x(:),y(:)],z(:),2);
			if p.Coefficients(1) * p.Coefficients(4) < 0
				saddle_mask2(yi,xi) = 1;
				if norm([2*p.Coefficients(1) p.Coefficients(2); p.Coefficients(2) 2*p.Coefficients(4)]\[-p.Coefficients(3); -p.Coefficients(5)]) <= distance_threhsold
					saddle_mask2(yi,xi) = 2;
				end
			end	
		end
	end
	
	figure;imagesc(saddle_mask2); colormap gray; axis image; axis off
	end
	%might need this part later
	if 1
		local_max = imregionalmax(f);
		[max_y,max_x] = find(local_max);
		max_ind = sub2ind(size(f), max_y, max_x);
		%local_max = subpixel_max(f);
		%max_x = local_max(:,1);
		%max_y = local_max(:,2);
		WS = watershed(f);
		WC = double(watershed(-f));
		wc_index = WC(max_ind);
		N = max(WS(:));
		M = max(WC(:));
		
		for i = 1:N
			ws_region = double(WS == i);
			neig_wc_index = unique(imdilate(ws_region,strel('square',5)) .* WC);
			neig_max = ismember(wc_index, neig_wc_index);
			n_max_x = max_x(neig_max);
			n_max_y = max_y(neig_max);
			subplot(1,2,1);; imagesc(ws_region + double(WS > 0)); colormap gray; axis image; hold on
			title('Watershed')
			plot(max_x, max_y, 'r.');
			plot(n_max_x, n_max_y, 'go');
			hold off
			subplot(1,2,2); imagesc(WC > 0); colormap gray; axis image; hold on
			plot(max_x, max_y, 'r.');
			title('Watercourse')
			hold off
			pause			
		end	
		
	end
end
