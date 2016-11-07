% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector/detect_and_show_edgemap.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 17, 2011

function detect_and_show_edgemap(img_file, sigma, grad_thresh, N)
	[edg, edgemap] = third_order_edge_detector_wrapper(img_file, sigma, grad_thresh, N);
	figure; imagesc(img_file); colormap gray; axis image; axis off
	disp_edg(edg, 'r',1);
end
