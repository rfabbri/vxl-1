% This is /lemsvxl/contrib/firat/matlab/levelset2d/experiments/test_proposed_geodesic_term.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 27, 2011

function [phi,I, Ix, Iy, Ixx, Ixy, Iyy] = test_proposed_geodesic_term

addpath ..
addpath ../stop_functions
addpath ../../third_order_edge_detector


dist_edges = 0;
hx = .5;
hy = .5;

I = synthetic_image('rectanglecavities2', 0);
%I = double(imread('~/Desktop/elbow1/35.png'));
[nrows,ncols] = size(I);

[edg, edgemap] = third_order_edge_detector_wrapper(I, 1, 10, 1);
num_edges = size(edg,1);
double_edg = zeros(2*num_edges, size(edg,2));
for i = 1:num_edges
	edge1 = edg(i,:);
	x1 = edge1(1); y1 = edge1(2); theta = pi/2 - edge1(3);
	double_edg(2*i-1,:) = [x1 + dist_edges*cos(theta), y1 - dist_edges*sin(theta), edg(i,3:end)];
	double_edg(2*i,:) = [x1 - dist_edges*cos(theta), y1 + dist_edges*sin(theta), edg(i,3:end)];		
end	



[DT,idx] = subpixelDT_fast(double_edg(:,1:2), nrows, ncols, hx, hy);
%[DTx, DTy] = grad_DT(DT, double_edg(:,1:2), idx, hx, hy);	
%imagesc(sqrt(DTx.^2 + DTy.^2));colormap gray; axis image;pause
DTx = (circshift(DT, [0 -1]) - circshift(DT, [0 1]))/(2*hx);
DTy = (circshift(DT, [-1 0]) - circshift(DT, [1 0]))/(2*hy);

[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);

I_interp = interp2(I, gridx, gridy);
sigma = 2/hx;

[Gx, Gy] = gaussian_derivative_filter(105, sigma);
Ix = conv2(I_interp, Gx, 'same')/hx;
Iy = conv2(I_interp, Gy, 'same')/hx;

[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(105, sigma);	
Ixx = conv2(I_interp, Gxx, 'same')/(hx*hx);
Iyy = conv2(I_interp, Gyy, 'same')/(hy*hy);
Ixy = conv2(I_interp, Gxy, 'same')/(hx*hy);

%figure;imagesc(Ixx); colormap gray; axis image
%figure;imagesc(Ixy); colormap gray; axis image
%figure;imagesc(Iyy); colormap gray; axis image
%pause

%imagesc(I);colormap gray; axis image;disp_edg(double_edg, 'g');hold on
%quiver(gridx, gridy, -DTx, -DTy);pause
%hold off

RUR = DT > 1;
imagesc(RUR);axis image;pause
GUR = ~RUR;
imagesc(GUR);axis image;pause

initialization_method = 'manualbubbles';

if strcmp(initialization_method, 'manualcontour')
	fid = figure;
	imagesc(I);colormap gray; axis image; axis off;
	hold on; 	  
	[x{1},y{1},b] = ginput(1);
	plot(x{1},y{1},'rd');    
	[x{2},y{2},b] = ginput(1);        
	plot([x{1} x{2}], [y{1} y{2}], 'rd-')
	t = 3;
	while b == 1
		[x{t},y{t},b] = ginput(1);
		plot([x{t-1} x{t}], [y{t-1} y{t}], 'rd-');
		t = t + 1;    
	end  
	plot([x{t-1} x{1}], [y{t-1} y{1}], 'rd-');  
	hold off;  
	pause(.1)  
	bw = poly2mask(cell2mat(x), cell2mat(y), size(I,1), size(I,2));
	se = strel('disk',1);
	bw2 = imerode(bw, se);
	bw3 = bw - bw2;
	%phi = sbwdist(bw, bw3);
	bwi = interp2(bw, gridx, gridy);
	[ar,ac] = find(bwi >= 1 );
	phi1 = subpixelDT_fast([(ac-1)*hx+1 (ar-1)*hy+1], nrows, ncols, hx, hy);
	[ar,ac] = find(bwi < 1);
	phi2 = subpixelDT_fast([(ac-1)*hx+1 (ar-1)*hy+1], nrows, ncols, hx, hy);
	phi = phi1 - phi2;
elseif 	strcmp(initialization_method, 'inandout')
	fid = figure;
	imagesc(I);colormap gray; axis image; axis off;
	hold on; 	  
	[x{1},y{1},b] = ginput(1);
	plot(x{1},y{1},'rd');    
	[x{2},y{2},b] = ginput(1);        
	plot([x{1} x{2}], [y{1} y{2}], 'rd-')
	t = 3;
	while b == 1
		[x{t},y{t},b] = ginput(1);
		plot([x{t-1} x{t}], [y{t-1} y{t}], 'rd-');
		t = t + 1;    
	end  
	plot([x{t-1} x{1}], [y{t-1} y{1}], 'rd-');  
	hold off;  
	pause(.1)  
	bw = poly2mask(cell2mat(x), cell2mat(y), size(I,1), size(I,2));
	se = strel('disk',5);
	bw2 = imerode(bw, se);
	region = bw - bw2;
	figure;imagesc(region);pause
	se = strel('disk',1);
	bw2 = imerode(region, se);
	bw3 = region - bw2;
	
	phi = sbwdist(region, bw3);
	figure; imagesc(phi); pause
	
elseif strcmp(initialization_method, 'randombubbles')
	A = zeros(nrows,ncols);
	for i = 1:20
		A = circle(A, nrows, ncols, unidrnd(nrows-6)+3, unidrnd(ncols-6)+3, 1.5, 1); 
	end
	se = strel('disk',1);
	bw2 = imerode(A, se);
	bw3 = A - bw2;
	phi = -sbwdist(A, bw3);
	%imagesc(A); pause
elseif strcmp(initialization_method, 'manualbubbles')
	fid = figure;
	imagesc(I);colormap gray; axis image; axis off;
	hold on;  		  
	[x{1},y{1},b] = ginput(1);
	plot(x{1},y{1},'r.');    
	[x{2},y{2},b] = ginput(1);        
	plot([x{1} x{2}], [y{1} y{2}], 'r.')
	t = 3;
	while b == 1
		[x{t},y{t},b] = ginput(1);
		plot([x{t-1} x{t}], [y{t-1} y{t}], 'r.');
		t = t + 1;    
	end  
	plot([x{t-1} x{1}], [y{t-1} y{1}], 'r.');  
	hold off;  
	pause(.1)  
	%A = zeros(nrows,ncols);
	%for i = 1:length(x)
	%	A = circle(A, nrows, ncols, x{i}, y{i}, 3, 1); 
	%end
	%se = strel('disk',1);
	%bw2 = imerode(A, se);
	%bw3 = A - bw2;
	%phi = sbwdist(A, bw3);
	phi = subpixelDT_fast([cell2mat(x)' cell2mat(y)'], nrows, ncols, hx, hy)-.5;
end


%%graddt parameters
%beta0 = -0.2;.4;
%beta1 = 0;-0.1;
%alpha = .15;
%delta_t = 1;
%num_iter = 300;
%reinit_freq = 3;

%%NHN parameters
beta0 = -0.8;.4;
beta1 = 0;-0.1;
alpha = .2/hx;
delta_t = 1;
num_iter = 200;
reinit_freq = 1;


%J = sqrt(Ix.^2 + Iy.^2);
S = 1;% 1./(1+(J).^2);
%Sx = (circshift(S, [0 -1]) - circshift(S, [0 1]))/2;
%Sy = (circshift(S, [-1 0]) - circshift(S, [1 0]))/2;
%Sx = make_border_and_nans_zero(Sx);
%Sy = make_border_and_nans_zero(Sy);

geodesic_inputs1 = {Ix, Iy, Ixx, Ixy, Iyy};
%geodesic_inputs2 = {Sx, Sy};
%geodesic_inputs3 = {DTx, DTy};

%[edg, edgemap] = third_order_edge_detector_wrapper(I, 1, 1, 1 );
%imagesc(edgemap > 0); axis image; colormap gray; pause
%DT = subpixelDT_naive(edg(:,1:2), nrows, ncols);
%S(DT <= 2) = 0;
%Ix(DT <= Inf) = 0;
%Iy(DT <= Inf) = 0;
%UPDATE_REGION = DT >= 1;

%S(S > 3) = 3;
%S = S/3;

[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);

close all
figure;
imagesc(I); colormap gray; axis image; axis off;  
axis image  
hold on;
contour(gridx, gridy, phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
pause
hold off;
for i = 1:num_iter
		
		
	imagesc(I); colormap gray; axis image; axis off; 
	%disp_edg(double_edg, 'r'); 
	axis image  
	hold on;
	C = contour(gridx, gridy, phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
	
	hold off;	
	title(['iter:' num2str(i)])
	%phi = levelset2d_drg(phi, S, 'graddt', geodesic_inputs3, beta0, beta1, alpha, delta_t, 1);	
	%phi = levelset2d_drg(phi, S, 'snake', geodesic_inputs2, beta0, beta1, alpha, delta_t, 1);
	%[phi, phi_x, phi_y] = levelset2d_geodesic_proposed(phi, Ix, Iy, Ixx, Ixy, Iyy, alpha, delta_t, 1);
	%phi = levelset2dupdate(phi, S, 'graddt', geodesic_inputs3, ...
    %beta0, beta1, alpha, delta_t, hx, hy,RUR,1,GUR);
    phi = levelset2dupdate(phi, S, 'NHN', geodesic_inputs1, ...
    beta0, beta1, alpha, delta_t, hx, hy,RUR,1,GUR);
	%phi = reinitlevelset_sussman(phi, 1, hx);
	if(~mod(i,reinit_freq) && length(C) > 0)
		phi = reinitlevelset2(C, phi, nrows, ncols, hx, hy);
	end
	pause(.1)
end
end

function I = circle(I, M, N, x, y, r, V)
	if r > 0
		for i = 1:M
			for j = 1:N
				if (i-y)^2+(j-x)^2 < r^2
					I(i,j) = V;
				end
			end
		end
	end
end

