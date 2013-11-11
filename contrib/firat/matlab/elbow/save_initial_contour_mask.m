% This is /lemsvxl/contrib/firat/matlab/elbow/save_initial_contour_mask.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 1, 2011

function save_initial_contour_mask(I, outfile)
		I = double(I);
		fid = figure;
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
		plot([x{t-1} x{1}], [y{t-1} y{1}], 'rd-');  
		hold off;  
		pause(.1)  
		bw = poly2mask(cell2mat(x), cell2mat(y), size(I,1), size(I,2));
		imwrite(uint8(255*bw), outfile);

end
