% This is draw_slope_line.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 17, 2012

function p = draw_slope_line(I, sigma, pp, pm, s)
	[nrows,ncols] = size(I);
	[Gx, Gy] = gaussian_derivative_filter(sigma, 1, 1);
	Ix = imfilter(I, Gx, 'replicate', 'conv');
	Iy = imfilter(I, Gy, 'replicate', 'conv');
	if sigma > 0
		H = fspecial('gaussian', ceil(7*sigma)*[1 1], sigma);
		I = imfilter(I, H, 'replicate');
	end
	figure;imagesc(I); colormap gray; axis off; axis image
	%for j = 1:1000
	
	hold on;
	while true
		
		p = pp + 2*rand(1,2)-1;
		plot(p(1),p(2),'r.');
		drawnow
		for i = 1:50
			Fx = interp2(Ix, p(1), p(2));	
			Fy = interp2(Iy, p(1), p(2));
			magF = sqrt(Fx.^2+Fy.^2);
			%if magF < 0.000000001
			%	break
			%end
			if magF == 0
				break
			end
			Fx = Fx ./ magF;
			Fy = Fy ./ magF;
						
			p = p - s*[Fx Fy];
			if p(1) < 1 || p(2) < 1 || p(1) > ncols || p(2) > nrows 
				break
			end
			plot(p(1),p(2),'r.')
			drawnow
		end
	end
	hold off;	
end
