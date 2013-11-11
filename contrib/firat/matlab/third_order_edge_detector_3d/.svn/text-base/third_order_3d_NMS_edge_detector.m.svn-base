% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector_3d/third_order_3d_NMS_edge_detector.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 29, 2011

function [x,y,z,nx,ny,nz,str,Fx,Fy,Fz,Ix,Iy,Iz] = third_order_3d_NMS_edge_detector(I, sigma, thresh)	
	V = padarray(I, [2 2 2], 'replicate');	
	tic;
	fprintf('Compute grad(F) and grad(I)\n');		
	[Fx, Fy, Fz, Ix, Iy, Iz] = compute_gradF_gradI(V, sigma, 1, 1, 1);
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
	x = x - 2;
	y = y - 2;
	z = z - 2;
	Fx = array3dcrop(Fx,2);
	Fy = array3dcrop(Fy,2);
	Fz = array3dcrop(Fz,2);
	Ix = array3dcrop(Ix,2);
	Iy = array3dcrop(Iy,2);
	Iz = array3dcrop(Iz,2);	
	toc
end

function Fx = array3dcrop(Fx, n)
	Fx(1:n,:,:) = []; 
	Fx(:,1:n,:) = []; 
	Fx(:,:,1:n) = []; 
	Fx(end-n+1:end,:,:) = []; 
	Fx(:,end-n+1:end,:) = []; 
	Fx(:,:,end-n+1:end) = [];
end
