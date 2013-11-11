% This is /lemsvxl/contrib/firat/matlab/intensity_profile.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 29, 2011

function intensity_profile(I)
	[M,N] = size(I);
	fid = figure;
	subplot(1,2,1)
	imagesc(I);colormap gray; axis image; axis off;
	hold on; 	  
	[x{1},y{1},b] = ginput(1);
	plot(x{1},y{1},'rd');    
	[x{2},y{2},b] = ginput(1);        
	plot([x{1} x{2}], [y{1} y{2}], 'rd-')
	t = 3;
	while b == 1
		[x{t},y{t},b] = ginput(1);
		plot([x{t-1} x{t}], [y{t-1} y{t}], 'rd-');
		t = t + 1;    
	end 
	hold off;
	all_ind = [];
	for i = 1:length(x)-1
		x0 = x{i};
		y0 = y{i};
		x1 = x{i+1};
		y1 = y{i+1};
		lambda = 0:.001:1;
		x_vals = round((1-lambda)*x0 + lambda*x1);
		y_vals = round((1-lambda)*y0 + lambda*y1);
		ind = M * (x_vals-1) + y_vals;
		remove_mask = false(1, length(ind));
		for j = 1:length(ind)-1
			if ind(j) == ind(j+1)
				remove_mask(j+1) = 1;
			end
		end		
		ind(remove_mask) = [];
		all_ind = [all_ind, ind];
	end
	remove_mask = false(1, length(ind));
	for j = 1:length(all_ind)-1
		if all_ind(j) == all_ind(j+1)
			remove_mask(j+1) = 1;
		end
	end		
	all_ind(remove_mask) = [];
	subplot(1,2,2);plot(1:length(all_ind), I(all_ind)); axis([-Inf Inf 0 255]); axis square
end

