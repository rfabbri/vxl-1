% This is /lemsvxl/contrib/firat/matlab/levelset2d/synthetic_image.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 26, 2011

function I = synthetic_image(image_type, add_noise)
	if strcmp(image_type, 'fading_circles')
		M = 80;
		N = 80;
		I = 100*ones(M,N);
		I = circle(I, M, N, 40, 40, 34, 200);
		I = fading_circle(I, M, N, 20, 20, 4, 200, 180);
		I = fading_circle(I, M, N, 30, 30, 4, 200, 180);
		I = fading_circle(I, M, N, 50, 30, 4, 200, 180);
		I = fading_circle(I, M, N, 40, 40, 4, 200, 180);
		I = fading_circle(I, M, N, 50, 50, 4, 200, 180);
		I = fading_circle(I, M, N, 60, 50, 4, 200, 180);
		I = fading_circle(I, M, N, 60, 40, 4, 200, 180);
		I = fading_circle(I, M, N, 60, 60, 4, 200, 180);
		I = fading_circle(I, M, N, 20, 30, 4, 200, 180);
		I = fading_circle(I, M, N, 30, 60, 4, 200, 180);
	elseif strcmp(image_type, 'circle')
		M = 101;
		N = 100;
		I = 100*ones(M,N);
		I = circle(I, M, N, 50, 50, 20, 200);
	elseif strcmp(image_type, 'square')
		M = 100;
		N = 100;
		I = 100*ones(M,N);
		I(30:70,30:70) = 200;
	elseif strcmp(image_type, 'rectangle_cavities')
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
	elseif strcmp(image_type, 'rectangle_cavities_mini')
		I = 100*ones(100,100);
		[nrows,ncols] = size(I);
		I(20:80, 20:80) = 50;
		I(40:80, 30) = 100;
		I(40:80, 40:41) = 100;
		I(40:80, 50:52) = 100;
		I(40:80, 60:63) = 100;
		I(80:90, 75:77) = 50;
	elseif strcmp(image_type, 'contrastcircles')
		M = 100;
		N = 100;
		I = 100*ones(M,N);
		k = 0;
		n = 5;
		m = 5;	
		startV = 50;
		endV = 150;
		inc = (endV - startV)/(m*n);
		for i = 1:n
			for j = 1:m		
				I = circle(I, M, N, 17*j, 17*i, 5, endV - k*inc);
				k = k + 1;				
			end
		end
	elseif strcmp(image_type, 'circle_cavity')
		M = 100;
		N = 100;
		I = 100*ones(M,N);
		I = circle(I, M, N, 50, 50, 20, 50);
		I(63:70, 48:51) = 100;
		I(56:63, 49:51) = 100;
		I(49:56, 50:51) = 100;
		I(42:49, 50) = 100;
		I(10:30, 50:51) = 50;
	elseif strcmp(image_type, 'blurred_circles')
		M = 80;
		N = 80;
		I = [];	
		for sigma = [1 2 3 4 5 6 7 8]%[.25 .5 1 2 3 6 8 12]
			J = blurred_circle(zeros(M,N), M, N, 40, 40, 12.123434233254, 50, 70, sigma);
			I = [I, J];
		end		
	end
	if add_noise
		I = I + 2*randn(M,N);
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

function I = blurred_circle(I, M, N, x, y, r, BACK, FORE, sigma)
	if r > 0
		for i = 1:M
			for j = 1:N
				D = sqrt((i-y)^2+(j-x)^2)-r;
				I(i,j) = (FORE-BACK)/2*erf(D/sqrt(2)/sigma)+(FORE+BACK)/2;				
			end
		end
	end
end

function I = fading_circle(I, M, N, x, y, r, V_left, V_right)
	inc = (V_left-V_right)/2/r;
	V = V_left:-inc:V_right;
	if r > 0
		for i = 1:M
			for j = 1:N
				if (i-y)^2+(j-x)^2 < r^2
					I(i,j) = V(j-x+r+1);
				end
			end
		end
	end
end


