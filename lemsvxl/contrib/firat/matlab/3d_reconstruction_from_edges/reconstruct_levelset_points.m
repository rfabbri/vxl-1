% This is /lemsvxl/contrib/firat/matlab/3d_reconstruction_from_edges/reconstruct_levelset_points.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 27, 2011

function opts = reconstruct_levelset_points(x,y,z,hx,hy,hz,sh, Fx, Fy, Fz)
    x1 = x + sh;
    y1 = y + sh;
    z1 = z + sh;
    x1 = (x1-1)/hx + 1;
    y1 = (y1-1)/hy + 1;
    z1 = (z1-1)/hz + 1;
    nx = interp3(Fx,x1,y1,z1,'*cubic');
	ny = interp3(Fy,x1,y1,z1,'*cubic');
	nz = interp3(Fz,x1,y1,z1,'*cubic');
	N = [nx ny nz];
	N_mag = sqrt(sum(N.^2,2));
	N = -N ./ repmat(N_mag, 1, 3); 
	opts = [x y z N];
end
