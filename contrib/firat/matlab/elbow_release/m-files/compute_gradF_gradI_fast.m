% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector_3d/compute_gradF_gradI_fast.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 2, 2011

function [Fx, Fy, Fz, Ix, Iy, Iz, XI, YI, ZI] = compute_gradF_gradI_fast(V, sigma, hx, hy, hz)
	[nrows,ncols,nbands] = size(V);
	[XI,YI,ZI] = meshgrid(1:hx:ncols, 1:hy:nrows, 1:hz:nbands);
	H = (hx*hy*hz);
	V = interp3(V*H, XI, YI, ZI, '*linear');	
	
	[GX0, GY0, GZ0, GX1, GY1, GZ1, GX2, GY2, GZ2, GX3, GY3, GZ3] = gaussian_1d_derivative_filters(sigma, hx, hy, hz);
	Ix = sequential_convolution(V, {GX1, GY0, GZ0});
	Iy = sequential_convolution(V, {GX0, GY1, GZ0});
	Iz = sequential_convolution(V, {GX0, GY0, GZ1});
	
	Ixx = sequential_convolution(V, {GX2, GY0, GZ0});
	Iyy = sequential_convolution(V, {GX0, GY2, GZ0});
	Izz = sequential_convolution(V, {GX0, GY0, GZ2});
	Ixy = sequential_convolution(V, {GX1, GY1, GZ0});
	Ixz = sequential_convolution(V, {GX1, GY0, GZ1});
	Iyz = sequential_convolution(V, {GX0, GY1, GZ1});
	
	Ixxx = sequential_convolution(V, {GX3, GY0, GZ0});
	Iyyy = sequential_convolution(V, {GX0, GY3, GZ0});
	Izzz = sequential_convolution(V, {GX0, GY0, GZ3});
	Ixxy = sequential_convolution(V, {GX2, GY1, GZ0});
	Ixxz = sequential_convolution(V, {GX2, GY0, GZ1});
	Ixyy = sequential_convolution(V, {GX1, GY2, GZ0});
	Iyyz = sequential_convolution(V, {GX0, GY2, GZ1});
	Ixzz = sequential_convolution(V, {GX1, GY0, GZ2});
	Iyzz = sequential_convolution(V, {GX0, GY1, GZ2});
	Ixyz = sequential_convolution(V, {GX1, GY1, GZ1});	
    
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
