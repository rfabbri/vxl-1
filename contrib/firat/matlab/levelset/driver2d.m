% This is /lemsvxl/contrib/firat/matlab/levelset/driver2d.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 13, 2011

function driver2d(I)
	if size(I,3) > 1
		I = rgb2gray(I);
	end
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
	se = strel('disk',1);
    bw2 = imerode(bw, se);
    bw3 = bw - bw2;
    phi = sbwdist(bw, bw3);
    
    sigma=1.5; 
	G = fspecial('gaussian',15,sigma);
	Is = conv2(I,G,'same');  
	
	beta0 = .2;
	beta1 = 0.0;
	delta_t = 1;
	num_iter = 100;
    close all
    figure;
    for i = 1:num_iter
		imagesc(I); colormap gray; axis image; axis off;
		hold on;
		contour(phi, [0,0], 'r', 'LineWidth', 3);
		hold off;
		phi = levelset2d(phi, Is, beta0, beta1, delta_t, 1);
		pause(.1)
	end
    
    
    
    imagesc(I); colormap gray; axis image; axis off;
	hold on;
	contour(phi, [0,0], 'r', 'LineWidth', 3);
	hold off;
    	
end

