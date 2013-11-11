% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector_3d/sequential_convolution.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 2, 2011

function R = sequential_convolution(I, filters)
	R = I;
	for i = 1:length(filters)
		R = imfilter(R, filters{i}, 'replicate', 'conv');
	end
end
