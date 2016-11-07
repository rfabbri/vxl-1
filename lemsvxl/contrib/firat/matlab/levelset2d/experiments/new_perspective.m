% This is /lemsvxl/contrib/firat/matlab/levelset2d/experiments/new_perspective.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 29, 2011

[nrows, ncols] = size(Ixx);
colors = [1 0 0; 0 1 0; 0 0 1; 1 1 0; 1 0 1; 0 1 1; 0 0 0; 1 .5 0];
figure; imagesc(I); colormap gray; axis image; axis off;  
labels = {};
axis image  
hold on;
for i = 1:8
	theta = (i-1)*pi/8;
	Nx = -sin(theta)*ones(nrows, ncols);
	Ny = cos(theta)*ones(nrows, ncols);
	V = ((Nx .* Ixx .* Ix) + (Nx .* Ixy .* Iy) + (Ny .* Ixy .* Ix) + (Ny .* Iyy .* Iy));
	%V = ((Ixx .* Ix) + (Ixy .* Iy)).^2 + ((Ixy .* Ix) + (Iyy .* Iy)).^2;
	contour(V, [0,0], 'LineWidth', 1, 'Color', colors(i,:));
	labels = [labels, num2str((i-1)*180/8)];
end
hold off;
legend(labels)


figure; imagesc(I); colormap gray; axis image; axis off;  
labels = {};
axis image  
hold on;
for i = 1:8
	theta = (i-1)*pi/8;
	Nx = -sin(theta)*ones(nrows, ncols);
	Ny = cos(theta)*ones(nrows, ncols);
	V = (((Nx.^2) .* Ixx) + (2 .* Nx .* Ny .* Ixy) + ((Ny.^2) .* Iyy));
	contour(V, [0,0], 'LineWidth', 1, 'Color', colors(i,:));
	labels = [labels, num2str((i-1)*180/8)];
end
hold off;
legend(labels)


figure; imagesc(I); colormap gray; axis image; axis off; 

axis image  
hold on;
V = ((Ixx .* Ix) + (Ixy .* Iy)).^2 + ((Ixy .* Ix) + (Iyy .* Iy)).^2;
contour(V, [.05,.05], 'LineWidth', 1, 'Color', colors(i,:));

hold off;
figure; imagesc(I); colormap gray; axis image; axis off;
hold on
Vx = (Ixx .* Ix) + (Ixy .* Iy);
Vy = (Ixy .* Ix) + (Iyy .* Iy);
V = sqrt(Vx.^2 + Vy.^2);
quiver(-Vx./V, -Vy./V)
axis ij
hold off

figure; imagesc(I); colormap gray; axis image; axis off;  
labels = {};
axis image  
hold on;
for i = 1:1
	theta = (i-1)*pi/8;
	Nx = -sin(theta)*ones(nrows, ncols);
	Ny = cos(theta)*ones(nrows, ncols);
	V = (((Nx.^2) .* Ixx) + (2 .* Nx .* Ny .* Ixy) + ((Ny.^2) .* Iyy)) .* sign(Ix .* Nx + Iy .* Ny);
	Vx = V .* Nx;
	Vy = V .* Ny;
	quiver(Vx, Vy);
end
hold off;

figure; imagesc(I); colormap gray; axis image; axis off;  
labels = {};
axis image  
hold on;
for i = 5:5
	theta = (i-1)*pi/8;
	Nx = -sin(theta)*ones(nrows, ncols);
	Ny = cos(theta)*ones(nrows, ncols);
	
	V = (((Nx.^2) .* Ixx) + (2 .* Nx .* Ny .* Ixy) + ((Ny.^2) .* Iyy)) .* sign(Ix .* Nx + Iy .* Ny);
	Vx = V .* Nx;
	Vy = V .* Ny;
	
	quiver(Vx, Vy);
end
hold off;

figure; imagesc(I); colormap gray; axis image; axis off;  
labels = {};
axis image  
hold on;
for i = 5:5
	theta = (i-1)*pi/8;
	Nx = -sin(theta)*ones(nrows, ncols);
	Ny = cos(theta)*ones(nrows, ncols);
	
	V = (((Nx.^2) .* Ixx) + (2 .* Nx .* Ny .* Ixy) + ((Ny.^2) .* Iyy)) .* sign(Ix .* Nx + Iy .* Ny);
	Vx = V .* Nx;
	Vy = V .* Ny;
	figure;plot(Vx(83,:));hold on
	M1 = Vx < 0;
	M2 = Vx > 0;
	Vx(M1) = Vx(M1) + 5;
	Vx(M2) = Vx(M2) - 5;
	figure;plot(Vx(83,:),'r'); hold off
end
hold off;


