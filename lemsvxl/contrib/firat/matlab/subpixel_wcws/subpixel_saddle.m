% This is subpixel_saddle.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Nov 12, 2012

function sad_loc = subpixel_saddle(f)
	
	
	WS = watershed(f);
	WC = watershed(-f);
	saddle_mask = double((WS == 0) & (WC == 0));
	figure; imagesc(saddle_mask); axis image; axis off; colormap gray
	saddle_mask = imdilate(saddle_mask, strel('square',3));
	[sad_y,sad_x] = find(saddle_mask);
	window = 1;
	[x,y] = meshgrid(-window:window,-window:window);
	distance_threhsold = 1.5*sqrt(2);
	sad_loc = zeros(length(sad_y), 2);
	for i = 1:length(sad_y)
		try			
			z = f(sad_y(i)-window:sad_y(i)+window, sad_x(i)-window:sad_x(i)+window);
			p = polyfitn([x(:),y(:)],z(:),2);
			
			if p.Coefficients(1) * p.Coefficients(4) < 0
				subpix_loc = ([2*p.Coefficients(1) p.Coefficients(2); p.Coefficients(2) 2*p.Coefficients(4)]\[-p.Coefficients(3); -p.Coefficients(5)])';
				if norm(subpix_loc) <= distance_threhsold
					sad_loc(i,:) = [sad_x(i) sad_y(i)] + subpix_loc;
				else
					sad_loc(i,:) = [-1 -1];
				end
			else
				sad_loc(i,:) = [-1 -1];
			end
		catch
			sad_loc(i,:) = [-1 -1];
		end
	end
	%sad_loc
	sad_loc(sad_loc(:,1) < 0, :) = [];
	%sad_loc
	
end
