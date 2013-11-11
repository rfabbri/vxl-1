% This is /lemsvxl/contrib/firat/matlab/levelset2d/experiments/grow_subpixel_circle.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 3, 2011

clear

center_x = 43.4;
center_y = 44.2;
radius = 0;
nrows = 100;
ncols = 100;
phi = zeros(nrows,ncols);

for i = 1:nrows
	for j = 1:ncols
		phi(i,j) = sqrt((j-center_x)^2 + (i-center_y)^2) - radius;
	end
end

%figure;imagesc(phi);colormap gray;pause

beta0 = -0.2;
beta1 = -0.1;
alpha = 0;
delta_t = 1;
num_iter = 100;
reinit_freq = 1;

I = zeros(nrows, ncols);
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
contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
hold off;
for i = 1:num_iter			
	imagesc(I); colormap gray; axis image; axis off;  
	axis image  
	hold on;
	contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
	hold off;	
	title(['iter:' num2str(i)])
	phi = levelset2d_drg(phi, S, 'proposed', geodesic_inputs1, beta0, beta1, alpha, delta_t, 1);
	phi = reinitlevelset_sussman(phi, 1, 1);	
	pause(.1)	
end

figure; surf(phi);colormap gray

figure;
imagesc(I); colormap gray; axis image; axis off;  
axis image  
hold on;
contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
hold off;


