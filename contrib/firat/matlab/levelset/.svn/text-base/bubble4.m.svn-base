% This is /lemsvxl/contrib/firat/matlab/levelset/bubble4.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 21, 2011


function bubble4
addpath ../third_order_edge_detector
if 0
I = 100*ones(50,50);

[nrows,ncols] = size(I);
I = circle(I, nrows, ncols, 25, 25, 10, 50);

I = I + 2*randn(nrows,ncols);

else
I = 100*ones(85,85);

[nrows,ncols] = size(I);
k = 1;
N = 4;
M = 4;
a = 1.5;
for i = 1:N
	for j = 1:M		
		I = circle(I, nrows, ncols, 18*j, 18*i, 5, 100 - k);
		%I = circle(I, nrows, ncols, 18*j, 18*i, 5, 50);
		k = a*k;
	end
end
%imagesc(I);colormap gray; axis image; pause
%I = I + 2*randn(nrows,ncols);
end

if 1
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
	phi = sbwdist(bw, bw3);
elseif 0

	A = zeros(nrows,ncols);
	for i = 1:20
		A = circle(A, nrows, ncols, unidrnd(nrows-6)+3, unidrnd(ncols-6)+3, 1.5, 1); 
	end
	se = strel('disk',1);
	bw2 = imerode(A, se);
	bw3 = A - bw2;
	phi = -sbwdist(A, bw3);
	%imagesc(A); pause
else
	fid = figure;
	imagesc(I);colormap gray; axis image; axis off;

	hold on;  
	  
	[x{1},y{1},b] = ginput(1);
	plot(x{1},y{1},'rd');    
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
end

[edg, edgemap] = third_order_edge_detector_wrapper(I, 1, 1.5, 1);
f = double(edgemap > 0);
[u,v] = GVF(f, .2, 80);
mag = sqrt(u.*u+v.*v);
Sx = -u./(mag); Sy = -v./(mag);
figure;imagesc(Sx);colormap gray
figure;imagesc(Sy);colormap gray;pause
S = 1;



	
beta0 = 0;
beta1 = 0;%-.1;%-1;
alpha = 0.1;
delta_t = 1;
num_iter = 2000;
close all
figure;
imagesc(I); colormap gray; axis image; axis off;  
axis image  
hold on;
[C, H] = contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);

hold off;
for i = 1:num_iter
	if ~mod(i,15)  
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






