% This is /lemsvxl/contrib/firat/matlab/levelset2d/level2d_double_edge_bubble.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 7, 2011

function phi = level2d_double_edge_bubble(I, edge_grad_thresh, dist_edges, bubble_distance)	
	I = double(I);
	[nrows,ncols] = size(I);	
			
	%%third order edge detection
	[edg, edgemap] = third_order_edge_detector_wrapper(I, 1, edge_grad_thresh, 1);
	num_edges = size(edg,1);
	double_edg = zeros(2*num_edges, size(edg,2));
	bubble_centers = zeros(2*num_edges, 2);
	for i = 1:num_edges
		edge1 = edg(i,:);
		x = edge1(1); y = edge1(2); theta = pi/2 - edge1(3);
		double_edg(2*i-1,:) = [x + dist_edges*cos(theta), y - dist_edges*sin(theta), edg(i,3:end)];
		double_edg(2*i,:) = [x - dist_edges*cos(theta), y + dist_edges*sin(theta), edg(i,3:end)];
		bubble_centers(2*i-1,:) = [x + bubble_distance*cos(theta), y - bubble_distance*sin(theta)];
		bubble_centers(2*i,:) = [x - bubble_distance*cos(theta), y + bubble_distance*sin(theta)];	
	end	
	DT = subpixelDT_fast(double_edg(:,1:2), nrows, ncols);	
	DTx = (circshift(DT, [0 -1]) - circshift(DT, [0 1]))/2;
	DTy = (circshift(DT, [-1 0]) - circshift(DT, [1 0]))/2;	
	S = DT/4;
	S(S>1) = 1;
	REACTION_UPDATE_REGION = DT > 3;
	DIFFUSION_UPDATE_REGION = 1;
	GEODESIC_UPDATE_REGION = 1;
	
	if 1 %visualize the bubble locations
		figure;
		imagesc(I); colormap gray; axis image; axis off;  
		axis image
		hold on
		plot(bubble_centers(:,1),bubble_centers(:,2),'.');axis image;
		disp_edg(double_edg,'r')
		hold off	
		pause
	end
	
	phi = subpixelDT_fast(bubble_centers, nrows, ncols);
	
	%%%Hard coded levelset evolution parameters
	beta0 = -0.01;-0.2;
	beta1 = 0;-0.1;
	alpha = .2;
	delta_t = 1;
	num_iter = 500;
	geodesic_inputs = {DTx, DTy};	
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
		phi = levelset2d_drg(phi, S, 'graddt', geodesic_inputs, beta0, beta1, alpha, delta_t, 1, REACTION_UPDATE_REGION, DIFFUSION_UPDATE_REGION, GEODESIC_UPDATE_REGION);	
		%phi = reinitlevelset_sussman(phi, 1, 1);		
		pause(.1)
	end
end
