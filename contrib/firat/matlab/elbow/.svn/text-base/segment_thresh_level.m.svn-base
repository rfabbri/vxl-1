% This is /lemsvxl/contrib/firat/matlab/elbow/segment_thresh_level.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 5, 2011

function segment_thresh_level(V, t, outfolder)
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
	[L, n_old] = bwlabel(ST);
	S = ST;
	for i = 2:size(V,3)	
		ST = thresholding_all_bones(V(:,:,i), t, 0, sigma, mu);	
		[L, n] = bwlabel(ST);
		if n ~= n_old
			n_old = n;
			S = ST;
		end
		se = strel('disk', 4); %4
		S0 = imdilate(S, se);
		S = levelset(V(:,:,i), S0);
		figure; imagesc(V(:,:,i)); colormap gray; axis image; axis off;
		hold on;
		contour(S, [0,0], 'r', 'LineWidth', 3);
		hold off;
		saveas(gcf, [outfolder '/' num2str(i) '.png']);
		close all
		[L, n_old] = bwlabel(S);		
	end
end
