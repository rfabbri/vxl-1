% This is /lemsvxl/contrib/firat/matlab/elbow/sythetic/two_semicircles.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 11, 2011

function I = two_semicircles(M, N, x, y, r1, r2, r3)
	I = zeros(M,N);
	J = zeros(M,N);
	I = semicircle(I, M, N, x, y, r1);	
	J = semicircle(J, M, N, x, y, r2);
	I = I - J;	
	I = semicircle(I, M, N, x, y, r3);	
	%se = strel('disk',1);
    %I2 = imerode(I,se);
    %I = I - I2;
end

function I = semicircle(I, M, N, x, y, r)
	if r > 0
		for i = 1:M
			for j = 1:N
				if (i-y)^2+(j-x)^2 < r^2 && i <= y
					I(i,j) = 1;
				end
			end
		end
	end
end
