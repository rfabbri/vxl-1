% This is /lemsvxl/contrib/firat/matlab/levelset2d/level2d_two_bubbles_per_edge.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 4, 2011


function phi = level2d_two_bubbles_per_edge(I, edge_grad_thresh, edge_margin, bubble_distance)	
	I = double(I);
	[nrows,ncols] = size(I);	
	%%compute image derivatives
	sigma = 1;	
	[Gx, Gy] = gaussian_derivative_filter(21, sigma);
	Ix = conv2(I, Gx, 'same');
	Iy = conv2(I, Gy, 'same');
	[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(21, sigma);	
	Ixx = conv2(I, Gxx, 'same');
	Iyy = conv2(I, Gyy, 'same');
	Ixy = conv2(I, Gxy, 'same');	
		
		
	%%third order edge detection
	[edg, edgemap] = third_order_edge_detector_wrapper(I, 1, edge_grad_thresh, 1);
	%DT = subpixelDT_naive(edg(:,1:2), nrows, ncols);
	DT = subpixelDT_fast(edg(:,1:2), nrows, ncols);		
	%%compute stop function
	%J = sqrt(Ix.^2 + Iy.^2);
	%S = 1./(1+(J).^2);
	S = DT/4;
	S(S>1) = 1;
	REACTION_UPDATE_REGION = DT >= edge_margin;
	DIFFUSION_UPDATE_REGION = DT >= edge_margin;
	GEODESIC_UPDATE_REGION = 1;%DT >= edge_margin;
	%%%Levelset initialization
	num_edges = size(edg,1);
	bubble_centers = zeros(2*num_edges, 2);
	for i = 1:num_edges
		edge1 = edg(i,:);
		x = edge1(1); y = edge1(2); theta = pi/2 - edge1(3);
		bubble_centers(2*i-1,:) = [x + bubble_distance*cos(theta), y - bubble_distance*sin(theta)];
		bubble_centers(2*i,:) = [x - bubble_distance*cos(theta), y + bubble_distance*sin(theta)];	
	end	
	if 0 %visualize the bubble locations
		figure;
		imagesc(I); colormap gray; axis image; axis off;  
		axis image
		hold on
		plot(bubble_centers(:,1),bubble_centers(:,2),'.');axis image;
		disp_edg(edg,'r')
		hold off	
		pause
	end
	
	%phi = zeros(nrows,ncols);
	%for i = 1:nrows
	%	for j = 1:ncols
	%		dists = sum((repmat([j i], 2*num_edges, 1) - bubble_centers).^2, 2);
	%		min_dist = min(dists);
	%		phi(i,j) = sqrt(min_dist);
	%	end
	%end
	phi = subpixelDT_fast(bubble_centers, nrows, ncols);
	
	%%%Hard coded levelset evolution parameters
	beta0 = -0.5;
	beta1 = 0;-0.1;
	alpha = .005;
	delta_t = 1;
	num_iter = 150;
	geodesic_inputs = {Ix, Iy, Ixx, Ixy, Iyy};	
	%%%Evolution loop
	close all
	figure;
	imagesc(I); colormap gray; axis image; axis off;  
	axis image  
	hold on;
	[C, H] = contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
	hold off;
	for i = 1:num_iter			
		imagesc(I); colormap gray; axis image; axis off;  
		axis image  
		hold on;
		[C, H] = contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
		hold off;	
		title(['iter:' num2str(i)])
		phi = levelset2d_drg(phi, S, 'proposed', geodesic_inputs, beta0, beta1, alpha, delta_t, 1, REACTION_UPDATE_REGION, DIFFUSION_UPDATE_REGION, GEODESIC_UPDATE_REGION);	
		%phi = reinitlevelset_sussman(phi, 1, 1);		
		pause(.1)
	end
end
