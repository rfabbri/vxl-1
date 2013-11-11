% This is evaluate_viewpoint.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 02, 2012

function [TP,GT] = evaluate_viewpoint(size1, size2, locs1, locs2, final_correspMap, final_pMap, th, do_plot, color)
	num_sets = length(locs1);
	%th = 1:-.2:0;
	num_th = length(th);
	TP = cell(num_sets, 1);
	GT = zeros(1, num_sets);
	for j = 1:num_sets
		TP{j} = zeros(1, num_th);
		L1 = round(locs1{j});
		L2 = round(locs2{j});
		GT(j) = size(L1, 1);
		for k = 1:num_th
			D = Inf(size(L1,1), size(L2,1));	
			for i = 1:size(L1,1)
				%ind = sub2ind(size1, L1(i,2), L1(i,1));	
				ind = (L1(i,2)-1)*size1(2)+L1(i,1);
				[row,col] = ind2sub(size2, final_correspMap{ind}(final_pMap{ind} >= th(k)));
				
				if ~isempty(row)
					D(i,find(ismember(L2, [col; row]', 'rows'))) = 0;
				elseif k == num_th
					GT(j) = GT(j) - 1; % this keypoint cannot have a correspondence!
				end
			end	
			[Matching,Cost] = assignmentoptimal(D);	
			TP{j}(k) = length(find(Matching(:) > 0));				
		end
	end
	S = sum(GT);
	TP1 = zeros(1, num_th);
	for j = 1:num_sets		
		TP1 = TP1 + TP{j};
	end
	if do_plot
		hold on
		plot(th, TP1/S, color);
		hold off
		xlabel('probability threshold')
		ylabel('recall')
		axis([0 max(th) 0 1])
	end	
end
