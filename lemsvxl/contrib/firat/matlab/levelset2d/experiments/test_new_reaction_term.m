% This is /lemsvxl/contrib/firat/matlab/levelset2d/experiments/test_new_reaction_term.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 4, 2011


clear
nrows = 100;
ncols = 100;
hx = .5;
hy = .5;
I = zeros(nrows, ncols);
[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);
if 1
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
	%phi = sbwdist(bw, bw3);
	bwi = interp2(bw, gridx, gridy);
	[ar,ac] = find(bwi >= 1 );
	phi1 = subpixelDT_fast([(ac-1)*hx+1 (ar-1)*hy+1], nrows, ncols, hx, hy);
	[ar,ac] = find(bwi < 1);
	phi2 = subpixelDT_fast([(ac-1)*hx+1 (ar-1)*hy+1], nrows, ncols, hx, hy);
	phi = phi1 - phi2;
else
	phi = subpixelDT_fast([50.734 50.123], nrows, ncols, hx, hy) - 3.2;
end
%plot(phi(100,:)); pause

%figure;imagesc(phi);colormap gray;pause

beta0 = 0;-0.2;
beta1 = -0.1;
alpha = 0;
delta_t = 1;
num_iter = 300;
reinit_freq = 30;


Ix = 0;
Iy = 0;
Ixx = 0;
Ixy = 0;
Iyy = 0;
J = 0;
S = 1;
Sx = 0;
Sy = 0;

geodesic_inputs1 = {Ix, Iy, Ixx, Ixy, Iyy};
geodesic_inputs2 = {Sx, Sy};

close all
figure;
imagesc(I); colormap gray; axis image; axis off;  
axis image  
hold on;
C = contour(gridx, gridy,phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
hold off;
for i = 1:num_iter			
	imagesc(I); colormap gray; axis image; axis off;  
	axis image  
	hold on;
	C = contour(gridx, gridy,phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
	hold off;	
	title(['iter:' num2str(i)])
	if ~mod(i, reinit_freq)
		phi = reinitlevelset2(C, phi, nrows, ncols, hx, hy);
	end
	%phi = levelset2d_drg(phi, S, 'proposed', geodesic_inputs1, beta0, beta1, alpha, delta_t, 1);
	phi = levelset2dupdate(phi, S, 'NHN', geodesic_inputs1, beta0, beta1, alpha, delta_t, hx, hy);
	%phi = reinitlevelset_sussman(phi, 1, 1);
		
	pause(.1)	
end




