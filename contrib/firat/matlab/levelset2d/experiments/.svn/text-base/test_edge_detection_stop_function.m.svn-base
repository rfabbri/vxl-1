% This is /lemsvxl/contrib/firat/matlab/levelset2d/experiments/test_edge_detection_stop_function.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 27, 2011

addpath ..
addpath ../../third_order_edge_detector
addpath ../stop_functions

I = synthetic_image('square', 1);
[nrows,ncols] = size(I);
S = stop_edge_detection(I, 3, 1, 100);
%S = stop_traditional(I, 5, 1, .1, 2);

%imagesc(S_gradient);colormap gray; axis image; axis off; pause

if 1
	Sx = (circshift(S, [0 -1]) - circshift(S, [0 1]))/2;
	Sy = (circshift(S, [-1 0]) - circshift(S, [1 0]))/2;
elseif 0
	Sx = (circshift(S_gradient, [0 -1]) - circshift(S_gradient, [0 1]))/2;
	Sy = (circshift(S_gradient, [-1 0]) - circshift(S_gradient, [1 0]))/2;
	
elseif 0
	f = double(edgemap > 0);
	[u,v] = GVF(f, .2, 80);
	mag = sqrt(u.*u+v.*v);	
	Sx = -u./(mag); Sy = -v./(mag);
	Sx(mag == 0) = 0;
	Sy(mag == 0) = 0;	
	%figure;imagesc(Sx);colormap gray;axis image
	%figure;imagesc(Sy);colormap gray;axis image; pause
else
	S = zeros(nrows, ncols);
	for i = 1:5
		em = J >= i;
		[r,c] = find(em);
		if i == 1
			S = subpixelDT_naive([c r], nrows, ncols);
		else
			T = subpixelDT_naive([c r], nrows, ncols);
			T(:,:,2) = S;
			S = min(T, 3); 
		end
	end	
	
	imagesc(S); colormap gray; axis image; pause
	Sx = (circshift(S, [0 -1]) - circshift(S, [0 1]))/2;
	Sy = (circshift(S, [-1 0]) - circshift(S, [1 0]))/2;
end
Sx = make_border_and_nans_zero(Sx);
Sy = make_border_and_nans_zero(Sy);

beta0 = -.05;
beta1 = 0;-.1;
alpha = 0; .4;
delta_t = 1;
num_iter = 1000;
reinit_freq = 1;
initialization_method = 'manualcontour';

levelset2d_driver(I, initialization_method, S, Sx, Sy, beta0, beta1, alpha, delta_t, num_iter, reinit_freq);


