% This is /lemsvxl/contrib/firat/matlab/levelset/synthetic_image_5.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 26, 2011

function synthetic_image_5
	M = 85;
	N = 85;
	I = 100*ones(M,N);
	I = fading_circle(I, M, N, 40, 40, 20, 100, 20);
	imagesc(I); colormap gray;
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

