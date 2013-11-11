% This is /lemsvxl/contrib/firat/matlab/levelset/reinitlevelset.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 14, 2011

function I = reinitlevelset(V, phi, M, N, O)
	I = zeros(M,N,O);
	total_num = size(V,1);
	points = V';
	if ~isempty(points)
		for i = 1:M
			for j = 1:N	
				for k = 1:O	
				p = repmat([j;i;k], 1, total_num);			
				D = sum((points - p).^2);			
				min_val = min(D);
				if phi(i,j,k) < 0
					I(i,j,k) = -sqrt(min_val);
				else
				 	I(i,j,k) = sqrt(min_val);
				end
			end
		end
	end
end
