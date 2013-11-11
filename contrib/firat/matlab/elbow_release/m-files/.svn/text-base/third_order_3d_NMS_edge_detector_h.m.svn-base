% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector_3d/third_order_3d_NMS_edge_detector_h.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 2, 2011

function [x,y,z,nx,ny,nz,str,Fx,Fy,Fz,Ix,Iy,Iz] = third_order_3d_NMS_edge_detector_h(I, sigma, thresh, hx, hy, hz)	
	V = padarray(I, [2 2 2], 'replicate');	
	tic;
	fprintf('Compute grad(F) and grad(I)\n');		
	[Fx, Fy, Fz, Ix, Iy, Iz] = compute_gradF_gradI_fast(V, sigma, hx, hy, hz);
	toc
	tic;
	fprintf('Do non-max suppression\n');
	[x, y, z, str] = NMS_token_3d(Fx, Fy, Fz, sqrt(Ix.^2+Iy.^2+Iz.^2), thresh, 0);
	toc
	tic;
	fprintf('Compute edge normals\n');
	nx = interp3(Fx,x,y,z,'*cubic');
	ny = interp3(Fy,x,y,z,'*cubic');
	nz = interp3(Fz,x,y,z,'*cubic');
	toc
	tic;
	fprintf('Housekeeping\n');
	x = (x-1)*hx + 1;
	y = (y-1)*hy + 1;
	z = (z-1)*hz + 1;
	x = x - 2;
	y = y - 2;
	z = z - 2;		
	toc
end
