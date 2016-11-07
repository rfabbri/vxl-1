% This is /lemsvxl/contrib/firat/matlab/levelset2d/experiments/test_force_field.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 8, 2011

clear
dist_edges = .4;
hx = .25;
hy = .25;

I = synthetic_image('circle', 0);
[nrows,ncols] = size(I);

[edg, edgemap] = third_order_edge_detector_wrapper(I, 1, 2, 1);
num_edges = size(edg,1);
double_edg = zeros(2*num_edges, size(edg,2));
for i = 1:num_edges
	edge1 = edg(i,:);
	x1 = edge1(1); y1 = edge1(2); theta = pi/2 - edge1(3);
	double_edg(2*i-1,:) = [x1 + dist_edges*cos(theta), y1 - dist_edges*sin(theta), edg(i,3:end)];
	double_edg(2*i,:) = [x1 - dist_edges*cos(theta), y1 + dist_edges*sin(theta), edg(i,3:end)];		
end	
[DT,idx] = subpixelDT_fast(double_edg(:,1:2), nrows, ncols, hx, hy);
[DTx, DTy] = grad_DT(DT, double_edg(:,1:2), idx, hx, hy);
%DTx = (circshift(DT, [0 -1]) - circshift(DT, [0 1]))/(2*hx);
%DTy = (circshift(DT, [-1 0]) - circshift(DT, [1 0]))/(2*hy);
%DTx1 = DTx ./ sqrt(DTx.^2 + DTy.^2);
%DTy1 = DTy ./ sqrt(DTx.^2 + DTy.^2);
%DTx = DTx1;
%DTy = DTy1;
imagesc(sqrt(DTx.^2 + DTy.^2)); pause
imagesc(I); colormap gray; axis image
disp_edg(double_edg, 'r');
axis ij
hold on
[mgx, mgy] = meshgrid(1:hx:ncols, 1:hy:nrows);
whos
quiver(mgx, mgy, -DTx, -DTy);

hold off
