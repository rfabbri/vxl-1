% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector_3d/compute_F.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 26, 2011


function [F, Ix, Iy, Iz, Ixx, Iyy, Izz, Ixy, Ixz, Iyz] = compute_F(V, sigma, hx, hy, hz)
	[Gx, Gy, Gz] = gaussian_derivative_filter(sigma, hx, hy, hz);
	Ix = imfilter(V, Gx, 'replicate');
	Iy = imfilter(V, Gy, 'replicate');
	Iz = imfilter(V, Gz, 'replicate');
	[Gxx, Gyy, Gzz, Gxy, Gxz, Gyz] = gaussian_second_derivative_filter(sigma, hx, hy, hz);
	Ixx = imfilter(V, Gxx, 'replicate');
	Iyy = imfilter(V, Gyy, 'replicate');
	Izz = imfilter(V, Gzz, 'replicate');
	Ixy = imfilter(V, Gxy, 'replicate');
	Ixz = imfilter(V, Gxz, 'replicate');
	Iyz = imfilter(V, Gyz, 'replicate');
	F = Ix.*(Ix.*Ixx + Ixy.*Iy + Ixz.*Iz) + Iy.*(Ix.*Ixy + Iy.*Iyy + Iyz.*Iz) + Iz.*(Ix.*Ixz + Iy.*Iyz + Iz.*Izz);	
end
