% This is trace_discrete_watershed.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Nov 26, 2012

function trace_discrete_watershed(M, color)	
	M = double(padarray(M,[1 1]));
	hold on
	[r,c] = find(M);
	iter_thresh = 20;
	while length(r) > 0
		selected = false;
		for i = 1:length(r)
			 N = get_num_neigh(M, r(i), c(i));
			 if N >= 1
				r1 = r(i);
				c1 = c(i);
				selected = true;
				break
			 end
		end
		if ~selected
			break
			error('Could not find a pixel with neighbors');
		end
		[curv,M] = trace_curve(M, N, r1, c1, iter_thresh);
		if ~isempty(curv)
			h = plot(curv(:,2)-1, curv(:,1)-1,'LineWidth',2);
			set(h, 'Color', color);
		end		
		M(M==2) = 1;
		[r,c] = find(M);
	end
	hold off
end

function N = get_num_neigh(M, r, c)
	N = (M(r-1,c)==1) + (M(r,c-1)==1) + (M(r+1,c)==1) + (M(r,c+1)==1);
end

function [rn, cn] = get_next(M, r, c)
	if M(r-1,c) == 1
		rn = r-1;
		cn = c;
	elseif M(r, c-1) == 1
		rn = r;
		cn = c-1;
	elseif M(r+1,c) == 1
		rn = r+1;
		cn = c;
	elseif M(r, c+1) == 1
		rn = r;
		cn = c+1;
	else
		error('abc')
	end
	
end

function [curv,M] = trace_curve(M, N, r, c, iter_thresh)
	sl = java.util.LinkedList;
	rn = r;
	cn = c;
	sl.add([r c]);
	if N <= 1
		M(r,c) = 0;
	else
		M(r,c) = 2;
	end
	iter = 0;
	while true
		iter = iter + 1;
		[r, c] = get_next(M, r, c);
		sl.add([r c]);	
		N = get_num_neigh(M, r, c);	
		if N == 1			
			M(r,c) = 0;
		elseif N == 0
			M(r,c) = 0;
			break;
		else	
			break
		end	
	end
	if iter == 1 && N > 1
		M(rn,cn) = 0;
	end
	curv = reshape(cell2mat(cell(sl.toArray(sl))), 2, sl.size())';	
end
