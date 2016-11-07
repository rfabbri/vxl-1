% This is /lemsvxl/contrib/firat/matlab/levelset/synthetic_image_1.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 15, 2011

clear;
close all;

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
G = fspecial('gaussian',5,sigma);
Is = I;%conv2(I,G,'same'); 
Ix = (circshift(Is, [0 -1]) - circshift(Is, [0 1]))/2;
Iy = (circshift(Is, [-1 0]) - circshift(Is, [1 0]))/2;		
J = sqrt(Ix.^2 + Iy.^2);
S = 1 ./ (1 + (J/.5).^2);  
	
beta0 = .7;
beta1 = -.1;%-1;
delta_t = 1;
num_iter = 1000;
close all
figure;
imagesc(I); colormap gray; axis image; axis off;  
axis image  
hold on;
[C, H] = contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
hold off;
for i = 1:num_iter
	if ~mod(i,30) 
		phi = reinitlevelset(C, phi, nrows, ncols);	
	end	
		
	imagesc(I); colormap gray; axis image; axis off;  
	axis image  
	hold on;
	[C, H] = contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
	hold off;	
	
	phi = levelset2d(phi, S, beta0, beta1, delta_t, 1);
	
	pause(.1)
end






