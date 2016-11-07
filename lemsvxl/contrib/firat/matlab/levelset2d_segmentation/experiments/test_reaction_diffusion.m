% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/experiments/test_reaction_diffusion.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 10, 2011

clear
close all

addpath ../initialization
addpath ../tools
addpath ..

nrows = 100;
ncols = 100;
hx = 1;
hy = 1;
I = zeros(nrows, ncols);
[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);
if 1
	phi = manual_multicontour_initialization(I, 1,hx, hy);
else
	phi = manual_bubble_initialization(I, .1, hx, hy);
end

imagesc(phi);colormap gray; axis image; pause

phi = reinitlevelset_narrow(phi,2);

beta0 = -0.5;
beta1 = -.1;
alpha = 0;
mu = .4;
delta_t = 1;
num_iter = 100;
reinit_freq = 10;


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
%C = view_zero_level_curve(phi, hx, hy);
hold off;
for i = 1:num_iter			
	imagesc(I); colormap gray; axis image; axis off;  
	axis image  
	hold on;
	C = contour(gridx, gridy,phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
	%C = view_zero_level_curve(phi, hx, hy);
	%if length(C) == 2
	%	return
	%end
	hold off;	
	title(['iter:' num2str(i)])		
	phi = levelset2dupdate(phi, S, 'NHN', geodesic_inputs1, beta0, beta1, alpha, delta_t, hx, hy);
	phi = reinitlevelset_narrow(phi,2);
	%if ~mod(i, reinit_freq)
	%	phi = reinitlevelset(C, phi, nrows, ncols, hx, hy);
	%end		
	%phi = levelset2dupdate_dist_reg(phi, S, 'NHN', geodesic_inputs1, beta0, beta1, alpha, mu, delta_t, hx, hy);
	pause(.1)	
end




