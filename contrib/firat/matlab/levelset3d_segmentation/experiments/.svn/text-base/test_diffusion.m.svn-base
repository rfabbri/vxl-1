% This is /lemsvxl/contrib/firat/matlab/levelset3d/test_diffusion.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 22, 2011

if 0 %sphere
	nx = 30;
	ny = 30;
	nz = 30;
	A = zeros(ny,nx,nz);
	A(round(ny/2), round(nx/2), round(nz/2)) = 1;
	phi = bwdist(A);
	phi = phi - 13;
else %cube
	nx = 30;
	ny = 30;
	nz = 30;
	A = zeros(ny,nx,nz);
	for i = 10:20
		A(10:20, 10:20, i) = 1;
	end
	B = zeros(ny,nx,nz);
	for i = 11:19
		B(11:19, 11:19, i) = 1;
	end
	C = A - B;	
	phi = sbwdist(A, C);
end

beta0 = 0;
beta1 = -.1;
alpha = 0;
delta_t = 1;
num_iter = 550;

S = 1;
Sx = 0;
Sy = 0;
Sz = 0;
vertices = visualize_foreground(phi, 0);

for i = 1:num_iter	
	if ~mod(i,10)  
		%phi = reinitlevelset(unique(vertices,'rows'), phi, ny, nx, nz);	
	end		
	clf
	phi = levelset3d_drg(phi, S, Sx, Sy, Sz, beta0, beta1, alpha, delta_t, 1);
	vertices = visualize_foreground(phi, 0);
	title(['iter: ' num2str(i)]);
	pause(.1)	
end
