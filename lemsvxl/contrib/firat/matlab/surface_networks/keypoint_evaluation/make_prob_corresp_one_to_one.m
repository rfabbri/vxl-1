% This is make_prob_corresp_one_to_one.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 17, 2012

function C = make_prob_corresp_one_to_one(size1, size2, final_correspMap, final_pMap)
	C = -ones([size1 2]);
	M = size1(1);
	N = size1(2);
	
	for i = 1:length(final_correspMap)
		if ~isempty(final_correspMap{i})
			x = mod(i, N);
			y = (i - x)/N + 1;
			if x == 0
				x = N;
				y = y - 1;
			end
			maxp = max(final_pMap{i});
			[dest_y, dest_x] = ind2sub(size2, final_correspMap{i}(final_pMap{i} == maxp));
			C(y,x,1) = round(mean(dest_x));
			C(y,x,2) = round(mean(dest_y));			 
		end
	end
	
	
end
