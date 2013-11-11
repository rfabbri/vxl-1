% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector_3d/compute_gradF.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 26, 2011


function [Fx, Fy, Fz, Ix, Iy, Iz] = compute_gradF_gradI(V, sigma, hx, hy, hz)
	[nrows,ncols,nbands] = size(V);
	[XI,YI,ZI] = meshgrid(1:hx:ncols, 1:hy:nrows, 1:hz:nbands);
	H = (hx*hy*hz);
	V = interp3(V*H, XI, YI, ZI, '*linear');	
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
	[Gxxx, Gyyy, Gzzz, Gxxy, Gxxz, Gxyy, Gyyz, Gxzz, Gyzz, Gxyz] = gaussian_third_derivative_filter(sigma, hx, hy, hz);
    Ixxx = imfilter(V, Gxxx, 'replicate');
    Iyyy = imfilter(V, Gyyy, 'replicate');
    Izzz = imfilter(V, Gzzz, 'replicate');
    Ixxy = imfilter(V, Gxxy, 'replicate');
    Ixxz = imfilter(V, Gxxz, 'replicate');
    Ixyy = imfilter(V, Gxyy, 'replicate');
    Iyyz = imfilter(V, Gyyz, 'replicate');
    Ixzz = imfilter(V, Gxzz, 'replicate');
    Iyzz = imfilter(V, Gyzz, 'replicate');
    Ixyz = imfilter(V, Gxyz, 'replicate');
    
    Fx = (2*Ix.*Ixx.^2 + Ix.^2.*Ixxx) + (2*Iy.*Ixy.*Iyy + Iy.^2.*Ixyy) +...
        (2*Iz.*Ixz.*Izz + Iz.^2.*Ixzz) + (2*Ixx.*Iy.*Ixy + 2*Ix.*Ixy.^2 + 2*Ix.*Iy.*Ixxy) + ...
        (2*Ixx.*Iz.*Ixz + 2*Ix.*Ixz.^2 + 2*Ix.*Iz.*Ixxz) + (2*Ixy.*Iz.*Iyz + 2*Iy.*Ixz.*Iyz + 2*Iy.*Iz.*Ixyz);
    
    Fy = (2*Iy.*Iyy.^2 + Iy.^2.*Iyyy) + (2*Ix.*Ixy.*Ixx + Ix.^2.*Ixxy) +...
        (2*Iz.*Iyz.*Izz + Iz.^2.*Iyzz) + (2*Iyy.*Ix.*Ixy + 2*Iy.*Ixy.^2 + 2*Ix.*Iy.*Ixyy) + ...
        (2*Iyy.*Iz.*Iyz + 2*Iy.*Iyz.^2 + 2*Iy.*Iz.*Iyyz) + (2*Ixy.*Iz.*Ixz + 2*Ix.*Ixz.*Iyz + 2*Ix.*Iz.*Ixyz);
    
    Fz = (2*Iz.*Izz.^2 + Iz.^2.*Izzz) + (2*Iy.*Iyz.*Iyy + Iy.^2.*Iyyz) +...
        (2*Ix.*Ixz.*Ixx + Ix.^2.*Ixxz) + (2*Izz.*Iy.*Iyz + 2*Iz.*Iyz.^2 + 2*Iz.*Iy.*Iyzz) + ...
        (2*Izz.*Ix.*Ixz + 2*Iz.*Ixz.^2 + 2*Ix.*Iz.*Ixzz) + (2*Iyz.*Ix.*Ixy + 2*Iy.*Ixz.*Ixy + 2*Iy.*Ix.*Ixyz);
end
