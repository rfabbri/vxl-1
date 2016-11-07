% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/tools/split_merged_regions.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 9, 2011

function phi = split_merged_regions(phi, gridx, gridy, dtresh)
	%fig = figure;
	hold on
	C = contour(gridx, gridy, phi, [0,0]);
	%close(fid);	
	temp_seg_points = [];		
	L = size(C,2);		
	i = 1;
	while i < L		
		polygon = C(:,i+1:i+C(2,i)-1)';
		D = squareform(pdist(polygon));
		[r,c] = find(D < dtresh);
		Mask = min(mod(r-c, C(2,i)-1),mod(c-r, C(2,i)-1)) > C(2,i)/10;
		r = r(Mask);
		c = c(Mask);
		%ind = (c-1)*(C(2,i)-1) + r;
		%[min_val, min_index] = min(D(ind));
		%r = r(min_index);
		%c = c(min_index);
		plot([polygon(r,1) polygon(c,1)], [polygon(r,2) polygon(c,2)], 'g.');
		i = i + C(2,i) + 1;			
	end	
	hold off;
end
