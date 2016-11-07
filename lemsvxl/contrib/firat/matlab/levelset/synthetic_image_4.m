% This is /lemsvxl/contrib/firat/matlab/levelset/synthetic_image_4.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 18, 2011

function synthetic_image_4


I = 100*ones(50,50);

[nrows,ncols] = size(I);
I = circle(I, nrows, ncols, 25, 25, 10, 50);

I = I + 2*randn(nrows,ncols);

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

sigma=2; 
[dx, dy] = gaussian_derivative_filter(5, sigma);
%Is = I;%conv2(I,G,'same'); 
%Ix = (circshift(Is, [0 -1]) - circshift(Is, [0 1]))/2;
%Iy = (circshift(Is, [-1 0]) - circshift(Is, [1 0]))/2;		
%
%[dx,dy] = gradient(G);
Ix = conv2(I, dx, 'same');
Iy = conv2(I, dy, 'same');
J = sqrt(Ix.^2 + Iy.^2);
S = 1 ./ (1 + (J).^2); 
S_x = (circshift(S, [0 -1]) - circshift(S, [0 1]))/2;
S_y = (circshift(S, [-1 0]) - circshift(S, [1 0]))/2;

tic;	
beta1 = .5;
delta_t = .2;
num_iter = 250;
close all
figure;
imagesc(I); colormap gray; axis image; axis off;  
axis image  
hold on;
[C, H] = contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
hold off;
pause
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
	phi = levelset2d_geodesic(phi, S, S_x, S_y, beta1, delta_t, 1);
	
	pause(.1)
end
toc
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




