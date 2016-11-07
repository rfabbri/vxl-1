% This is /lemsvxl/contrib/firat/matlab/levelset/subpixelDT_naive.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 20, 2011

function DT = subpixelDT_naive(points, M, N)
	DT = zeros(M, N);
	total_num = size(points, 1);
	for i = 1:M
		for j = 1:N		
			p = repmat([j i], total_num, 1);			
			D = sum((points - p).^2, 2);			
			min_val = min(D);
			DT(i,j) = sqrt(min_val);			
		end
	end
end

