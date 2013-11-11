% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector_3d/NMS_edges_given_slice.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 26, 2011

function [x,y,nx,ny] = NMS_edges_given_slice(Fx, Fy, G, thresh, z)
	Gx = Fx(:,:,z);
	Gy = Fy(:,:,z);
	mask = G(:,:,z) > thresh;
	[x, y, subpix_dir_x, subpix_dir_y, subpix_grad_x, subpix_grad_y] = NMS_token(Gx, Gy, G(:,:,z), mask, 0);
	nx = interp3(Fx,x,y,repmat(z,1,length(x)),'*linear');
	ny = interp3(Fy,x,y,repmat(z,1,length(x)),'*linear');
end
