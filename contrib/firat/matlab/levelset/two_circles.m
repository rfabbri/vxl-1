% This is /lemsvxl/contrib/firat/matlab/elbow/sythetic/two_circles.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 11, 2011

function I = two_circles(M,N,r1,r2,x1,y1, x2, y2)
	I = zeros(M,N);
	I = circle(I, M, N, x1, y1, r1);
	I = circle(I, M, N, x2, y2, r2);
	%se = strel('disk',1);
    %I2 = imerode(I,se);
    %I = I - I2;
end

function I = circle(I, M, N, x, y, r)
	if r > 0
		for i = 1:M
			for j = 1:N
				if (i-y)^2+(j-x)^2 < r^2
					I(i,j) = 1;
				end
			end
		end
	end
end

