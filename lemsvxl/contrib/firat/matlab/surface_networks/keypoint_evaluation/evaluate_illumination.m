% This is evaluate_illumination.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 27, 2012

function [TP1,GT] = evaluate_illumination(locs1, locs2, th, do_plot, color)
	num_sets = length(locs1);
	%th = 0:.5:3;
	num_th = length(th);
	TP = cell(num_sets, 1);
	GT = zeros(1, num_sets);
	for j = 1:num_sets
		L1 = locs1{j};
		L2 = locs2{j};
		GT(j) = size(L1, 1);
		D = pdist2(L1, L2);
		TP{j} = zeros(1, num_th);
		for i = 1:num_th
			i
			DM = D;
			DM(DM > th(i)) = Inf;
			[Matching,Cost] = assignmentoptimal(DM);
			%[Matching,Cost] = Hungarian(DM);
			%Matching = csaWrapper(DM);
			TP{j}(i) = length(find(Matching(:) > 0));		
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
		xlabel('distance threshold')
		ylabel('recall')
		axis([0 max(th) 0 1])
	end
	
	%hold on
	%plot(th, rec, color);
	%hold off
	%xlabel('distance threshold')
	%ylabel('recall')
	%min_th = min(DM(:));
	%max_th = max(DM(:));
	%num_th = 200;
	%th = linspace(min_th, max_th, num_th);	
	%prec = zeros(1, num_th);
	%rec = zeros(1, num_th);
	%for i = 1:num_th
	%	mask = DM <= th(i);
	%	FN = length(find(sum(mask) == 0));
	%	TP = GT - FN;
	%	FP = length(find(sum(mask,2) >= 2));
	%	prec(i) = TP/(TP+FP);
	%	rec(i) = TP/GT;
		%pause
	%end	
	%hold on
	%plot(rec,prec,color);	
	%hold off
	%xlabel('recall')
	%ylabel('precision')
	%axis([0 1 0 1])	
	
end
