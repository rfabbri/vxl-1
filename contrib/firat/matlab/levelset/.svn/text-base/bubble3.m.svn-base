% This is /lemsvxl/contrib/firat/matlab/levelset/bubble3.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 21, 2011

function bubble2
addpath ../third_order_edge_detector

I = 100*ones(160,320);

[nrows,ncols] = size(I);

I(30:110, 50:280) = 50;
I(60:110, 80) = 100;
I(60:110, 110:111) = 100;
I(60:110, 140:142) = 100;
I(60:110, 170:173) = 100;
I(60:110, 200:204) = 100;
I(60:110, 230:235) = 100;
I(100:150, 260:262) = 50;
%imagesc(I);colormap gray; axis image; pause
I = I + 2*randn(nrows,ncols);


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
A = zeros(nrows,ncols);
for i = 1:length(x)
	A = circle(A, nrows, ncols, x{i}, y{i}, 1.5, 1); 
end
se = strel('disk',1);
bw2 = imerode(A, se);
bw3 = A - bw2;
phi = -sbwdist(A, bw3);


[edg, edgemap] = third_order_edge_detector_wrapper(I, 1, 8, 1);
S = subpixelDT_naive(edg(:,1:2), nrows, ncols);
S(S > 10) = 10;
S = S/4;


Sx = (circshift(S, [0 -1]) - circshift(S, [0 1]))/2;
Sy = (circshift(S, [-1 0]) - circshift(S, [1 0]))/2;
Sx = make_border_and_nans_zero(Sx);
Sy = make_border_and_nans_zero(Sy);
	
beta0 = .2;
beta1 = -.1;%-1;
alpha = 1;
delta_t = 1;
num_iter = 450;
close all
figure;
imagesc(I); colormap gray; axis image; axis off;  
axis image  
hold on;
[C, H] = contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
hold off;
for i = 1:num_iter
	if ~mod(i,10)  
		phi = reinitlevelset(C, phi, nrows, ncols);	
	end	
		
	imagesc(I); colormap gray; axis image; axis off;  
	axis image  
	hold on;
	[C, H] = contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
	hold off;	
	title(['iter:' num2str(i)])
	phi = levelset2d_drg(phi, S, Sx, Sy, beta0, beta1, alpha, delta_t, 1);
	
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






