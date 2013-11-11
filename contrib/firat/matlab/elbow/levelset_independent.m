% This is /lemsvxl/contrib/firat/matlab/elbow/levelset_independent.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 5, 2011

function levelset_independent(V, t, outfolder)
	if ~exist(outfolder, 'dir')
		mkdir(outfolder);	
	end
	
	for i = 1:size(V,3)	
		S0 = initialize_levelset(V(:,:,i), t);
		S = levelset(V(:,:,i), S0);
		figure; imagesc(V(:,:,i)); colormap gray; axis image; axis off;
		hold on;
		contour(S, [0,0], 'r', 'LineWidth', 3);
		hold off;
		saveas(gcf, [outfolder '/' num2str(i) '.png']);
		close all				
	end

end

function S0 = initialize_levelset(I, t);
	ST = thresholding_all_bones(I, t, 0, 1.0367e+03, 104.6015);
	props = regionprops(double(ST), 'ConvexHull');
	ch = props.ConvexHull;
	S0 = poly2mask(ch(:,1), ch(:,2), size(I,1), size(I,2));
	se = strel('disk', 3);
    S0 = logical(imdilate(S0, se));
end

