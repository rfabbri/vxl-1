% This is /lemsvxl/contrib/firat/matlab/levelset/synthetic_image_2.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 15, 2011

function synthetic_image_2

I = 100*ones(300,400);

[nrows,ncols] = size(I);
k = 1;
N = 4;
M = 6;
a = 1.22;
for i = 1:N
	for j = 1:M		
		I = circle(I, nrows, ncols, 60*j, 60*i, 20, 100 - k);
		k = a*k;
	end
end


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
S = 1 ./ (1 + (J/0.1).^4); 
	
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
	title(['iter:' num2str(i)])
	phi = levelset2d(phi, S, beta0, beta1, delta_t, 1);
	
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




