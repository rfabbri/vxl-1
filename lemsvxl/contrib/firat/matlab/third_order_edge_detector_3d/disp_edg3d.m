% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector_3d/disp_edg3d.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 29, 2011


function disp_edg3d(V, slice_index, x, y, z, nx, ny, nz, str, dthresh, sthresh, show_lines)	
	slices = find(abs(z  - slice_index) < dthresh & str > sthresh);
	x = x(slices);
	y = y(slices);
	nx = nx(slices);
	ny = ny(slices);
	magnxy = sqrt(nx.^2 + ny.^2);
	nx = nx ./ magnxy;
	ny = ny ./ magnxy;
	dx = ny;
	dy = -nx;
	m = length(x);
	figure;
	imagesc(V(:,:,slice_index), [0 255]);colormap gray; axis image; axis off; 
	hold on
	if show_lines
		for i = 1:m 
			plot([x(i)+0.5*dx(i), x(i)-0.5*dx(i)], [y(i)+0.5*dy(i), y(i)-0.5*dy(i)], 'r-')
		end
	else
		plot(x,y,'r.');
	end
	hold off
end
