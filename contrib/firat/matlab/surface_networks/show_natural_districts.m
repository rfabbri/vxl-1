% This is show_natural_districts.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 17, 2012

function show_natural_districts(I, sigma)
	[hills, dales, J] = compute_hills_dales(I, sigma);
	figure; imagesc(J); colormap gray; axis off; axis image
	hold on
	for i = 1:size(hills, 3)
		for j = 1:size(dales,3)
			M = hills(:,:,i) & dales(:,:,j);
			contour(M, [.5 .5], 'LineColor', rand(1,3))
		end
	end
	hold off
end
