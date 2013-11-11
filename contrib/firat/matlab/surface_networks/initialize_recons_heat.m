% This is initialize_recons_heat.m
% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date May 15, 2012


function [nrows, ncols, index, value, A] = initialize_recons_heat(I, mins, maxs, saddles, slope_lines, assignments, mode);
	[nrows,ncols] = size(I);
	if mode == 1 % use mins, maxs only
		if isempty(mins)
			P = maxs;
		elseif isempty(maxs)
			P = mins;
		else
			P = [mins; maxs];
		end		
		index = sub2ind([nrows ncols], round(P(:,2)), round(P(:,1)));						
	elseif mode == 2		
		index = sub2ind([nrows ncols], round([mins(:,2);maxs(:,2);saddles(:,2)]), round([mins(:,1);maxs(:,1);saddles(:,1)]));
	elseif mode == 3
		index = [];
		for i = 1:length(slope_lines)
			II = round(slope_lines{i}(1:end,2));
			JJ = round(slope_lines{i}(1:end,1));
			in = find(II >= 1 & II <= nrows & JJ >= 1 & JJ <= ncols);
			II = II(in);
			JJ = JJ(in);
			index = [index; sub2ind([nrows ncols], II, JJ)];
		end
	elseif mode == 4
		index = find(slope_lines);
	end
	K = zeros(nrows, ncols);
	K(:,1) = 1;
	K(:,end) = 1;
	K(1,:) = 1;
	K(end,:) = 1;
	index = [index; find(K)];
	index = unique(index);
	value = I(index);
	MA = max(I(:));
	MI = min(I(:));
	
	A = (MA-MI)*rand(nrows, ncols) + MI;	
	A(index) = value;
	%A = I;		
end
