% This is /lemsvxl/contrib/firat/matlab/levelset/reinitlevelset.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 14, 2011

function I = reinitlevelset(C, phi, M, N)
	I = zeros(M,N);
	points = [];
	total_num = 0;
	while ~isempty(C)
		num = C(2,1);
		total_num = total_num + num;
		points = [points, C(:, 2:num+1)];
		C(:,1:num+1) = [];
	end
	if ~isempty(points)
		for i = 1:M
			for j = 1:N		
				p = repmat([j;i], 1, total_num);			
				D = sum((points - p).^2);			
				min_val = min(D);
				if phi(i,j) < 0
					I(i,j) = -sqrt(min_val);
				else
				 	I(i,j) = sqrt(min_val);
				end
			end
		end
	end
end
