% This is /lemsvxl/contrib/firat/matlab/levelset2d/experiments/test_reaction.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 26, 2011

if 0 %sphere
	nx = 50;
	ny = 50;
	nz = 50;
	A = zeros(ny,nx,nz);
	A(round(ny/2), round(nx/2), round(nz/2)) = 1;
	phi = bwdist(A);
	phi = phi - 20;
elseif 1 %cube
	nx = 50;
	ny = 50;
	A = zeros(ny,nx);
	A(20:30, 20:30) = 1;
	B = zeros(ny,nx);
	B(21:29, 21:29) = 1;	
	C = A - B;	
	phi = sbwdist(A, C);
	%imagesc(phi); pause
else
	nx = 50;
	ny = 50;
	nz = 50;
	A = zeros(ny,nx,nz);
	for i = 10:40
		A(10:40, 10:40, i) = 1;
		A(:, :, i) = imrotate(A(:, :, i),15,'bilinear','crop');
	end
	B = zeros(ny,nx,nz);
	for i = 11:39
		B(11:39, 11:39, i) = 1;
		B(:, :, i) = imrotate(B(:, :, i),15,'bilinear','crop');
	end
	C = A - B;	
	phi = sbwdist(A, C);
end

beta0 = .1;
beta1 = 0;
alpha = 0;
delta_t = 1;
num_iter = 100;
reinit_freq = 15;

S = 1;
Sx = 0;
Sy = 0;

close all
figure;
I = zeros(ny,nx);
imagesc(I); colormap gray; axis image; axis off;  
axis image  
hold on;
[C, H] = contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
hold off;
for i = 1:num_iter
	if ~mod(i,reinit_freq)  
		%phi = reinitlevelset(C, phi, ny, nx);
		%figure; plot(phi(25,:)); axis([0 50 -10 21]);
		phi = reinitlevelset_sussman(phi, 1e-10, 10, .1);
			%figure; plot(phi(25,:)); axis([0 50 -10 21]); pause
	end		
	imagesc(I); colormap gray; axis image; axis off;  
	axis image  
	hold on;
	[C, H] = contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
	hold off;	
	title(['iter:' num2str(i)])
	phi = levelset2d_drg(phi, S, Sx, Sy, beta0, beta1, alpha, delta_t, 1);	
	pause(.1)
end




