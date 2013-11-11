% This is plot_fuv.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 12, 2012

function plot_fuu_fuv(I, sigma)
	addpath ../levelset2d_segmentation/tools
	[Gx, Gy] = gaussian_derivative_filter(sigma,1,1);
	[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(sigma, 1, 1);
	L = (size(Gxx, 1)-1)/2;
	Ix = imfilter(I, Gx, 'replicate', 'conv');
	Iy = imfilter(I, Gy, 'replicate', 'conv');
	Ixx = imfilter(I, Gxx, 'replicate', 'conv');
	Ixy = imfilter(I, Gxy, 'replicate', 'conv');
	Iyy = imfilter(I, Gyy, 'replicate', 'conv');
	fuv = (Ix.*(Ix.*Ixy - Ixx.*Iy) + Iy.*(Ix.*Iyy - Ixy.*Iy))./(Ix.^2 + Iy.^2);
	any(any(isinf(fuv) | isnan(fuv)))
	fuu = (Ix.*(Ix.*Ixx + Ixy.*Iy) + Iy.*(Ix.*Ixy + Iy.*Iyy))./(Ix.^2 + Iy.^2);
	any(any(isinf(fuu) | isnan(fuu)))
	fuv = fuv(L+1:end-L, L+1:end-L);
	fuu = fuu(L+1:end-L, L+1:end-L);
	I = I(L+1:end-L, L+1:end-L);
	figure; %subplot(1,2,1)
	imagesc(I); colormap gray; axis image; axis off
	hold on;
	contour(fuu, [0,0], 'r');
	%hold off
	
	%subplot(1,2,2)
	%imagesc(I); colormap gray; axis image; axis off
	%hold on;
	contour(fuv, [0,0], 'g');
	hold off
	legend({'f_{uu} = 0', 'f_{uv} = 0'})
	
	
end
