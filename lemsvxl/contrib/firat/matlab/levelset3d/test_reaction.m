% This is /lemsvxl/contrib/firat/matlab/levelset3d/test_reaction.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 22, 2011

if 1 %sphere
	nx = 50;
	ny = 50;
	nz = 50;
	A = zeros(ny,nx,nz);
	A(round(ny/2), round(nx/2), round(nz/2)) = 1;
	phi = bwdist(A);
	phi = phi - 2;
elseif 0 %cube
	nx = 100;
	ny = 100;
	nz = 100;
	A = zeros(ny,nx,nz);
	for i = 40:60
		A(40:60, 40:60, i) = 1;
	end
	B = zeros(ny,nx,nz);
	for i = 41:59
		B(41:59, 41:59, i) = 1;
	end
	C = A - B;	
	phi = sbwdist(A, C);
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

beta0 = -.8;
beta1 = 0;
alpha = 0;
delta_t = 1;
num_iter = 50;

S = 1;
Sx = 0;
Sy = 0;
Sz = 0;
visualize_foreground(phi, 0);
for i = 1:num_iter			
	pause(.1)
	clf
	phi = levelset3d_drg(phi, S, Sx, Sy, Sz, beta0, beta1, alpha, delta_t, 1);
	phi = reinitlevelset_sussman(phi, 1, 1);
	visualize_foreground(phi, 0);
	%vrml(gcf,'~/Desktop/shape.wrl');	
end




