% This is /lemsvxl/contrib/firat/matlab/elbow/segment_thresh.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 5, 2011


function segment_thresh(V, t, outfolder)
	if ~exist(outfolder, 'dir')
		mkdir(outfolder);	
	end
	[ST, sigma, mu] = thresholding_all_bones(V(:,:,1), t, 0);
	figure; imagesc(V(:,:,1)); colormap gray; axis image; axis off;
	hold on;
	contour(ST, [0,0], 'r', 'LineWidth', 3);
	hold off;
	saveas(gcf, [outfolder '/1.png']);
	close all	
	for i = 2:size(V,3)	
		ST = thresholding_all_bones(V(:,:,i), t, 0, sigma, mu);	
		figure; imagesc(V(:,:,i)); colormap gray; axis image; axis off;
		hold on;
		contour(ST, [0,0], 'r', 'LineWidth', 3);
		hold off;
		saveas(gcf, [outfolder '/' num2str(i) '.png']);
		close all			
	end
end
