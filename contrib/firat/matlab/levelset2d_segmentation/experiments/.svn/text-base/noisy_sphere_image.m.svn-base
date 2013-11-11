% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/experiments/noisy_sphere_image.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 30, 2011

function [V, radius, cx, cy, cz] = noisy_sphere_image(nrows, ncols, nbands, scale, FG, BG, radius)
	M = nrows*scale;
	N = ncols*scale;
	R = nbands*scale;
	cx = N/2;
	cy = M/2;
	cz = R/2;
	phi = subpixelDT_fast3d([cx cy cz], M, N, R, 1, 1, 1) - radius*scale;
	W = phi <= 0;
	A = zeros(size(W));
	A(W) = FG;
	A(~W) = BG;
	V =  zeros(nrows, ncols, nbands);
	for y = 1:scale:N
		for x = 1:scale:M
			for z = 1:scale:R
				B = A(y:y+scale-1, x:x+scale-1, z:z+scale-1);
				V((y+scale-1)/scale, (x+scale-1)/scale, (z+scale-1)/scale ) = mean(B(:));
			end
		end
	end
	V = V + abs(2*randn(size(V)));
	
	b = ceil((cx - 0.5)/scale);
	a1 = b*scale+0.5-cx;
	a2 = cx - (b-1)*scale - 0.5;
	cx = (a2*(b+0.5)+a1*(b-0.5))/(a1+a2);
	
	b = ceil((cy - 0.5)/scale);
	a1 = b*scale+0.5-cy;
	a2 = cy - (b-1)*scale - 0.5;
	cy = (a2*(b+0.5)+a1*(b-0.5))/(a1+a2);
	
	b = ceil((cz - 0.5)/scale);
	a1 = b*scale+0.5-cz;
	a2 = cz - (b-1)*scale - 0.5;
	cz = (a2*(b+0.5)+a1*(b-0.5))/(a1+a2);
	
	
	%b = ceil(cx/scale);
	%a = b*scale - (scale-1);	
	%cx = (cx - a - (scale-1)/2)/scale + b;
	
	%b = ceil(cy/scale);
	%a = b*scale - (scale-1);	
	%cy = (cy - a - (scale-1)/2)/scale + b;
	
	%b = ceil(cz/scale);
	%a = b*scale - (scale-1);	
	%cz = (cz - a - (scale-1)/2)/scale + b;
end

function [DT,idx] = subpixelDT_fast3d(points, maxy, maxx, maxz, hx, hy, hz)
	[xgrid,ygrid,zgrid] = meshgrid(1:hx:maxx, 1:hx:maxy, 1:hz:maxz);
	query_pts = [xgrid(:), ygrid(:), zgrid(:)];
	[idx, D] = knnsearch(points, query_pts);	
	Nx = (maxx-1)/hx + 1;
    Ny = (maxy-1)/hy + 1;
    Nz = (maxz-1)/hz + 1;
    DT = reshape(D, [Ny Nx Nz]);    
	idx = reshape(idx, [Ny Nx Nz]);	
end
