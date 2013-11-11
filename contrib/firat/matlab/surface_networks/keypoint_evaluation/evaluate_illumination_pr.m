% This is evaluate_illumination_pr.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 28, 2012

function [TP1, Q1, GT1] = evaluate_illumination_pr(locs1, locs2, th, do_plot, color)
	num_sets = length(locs1);
	TP = cell(num_sets, 1);
	GT = cell(num_sets, 1);
	Q = cell(num_sets, 1);
	score_th = 0:.1:1.6;
	num_scores = length(score_th);
	for j = 1:num_sets
		L1 = locs1{j};
		L2 = locs2{j};
		TP{j} = zeros(1, num_scores);	
		GT{j} = zeros(1, num_scores);
		Q{j} = zeros(1, num_scores);
		for si = 1:num_scores			
			L1(L1(:,3) < score_th(si),:) = [];			
			L2(L2(:,3) < score_th(si),:) = [];			
			DM = pdist2(L1(:,1:2), L2(:,1:2));				
			DM(DM > th) = Inf;
			[Matching,Cost] = assignmentoptimal(DM);
			TP{j}(si) = length(find(Matching(:) > 0));
			Q{j}(si) = size(L2,1);
			GT{j}(si) = size(L1,1);
		end		
	end
	
	TP1 = zeros(1, num_scores);
	Q1 = zeros(1, num_scores);
	GT1 = zeros(1, num_scores);
	for j = 1:num_sets		
		TP1 = TP1 + TP{j};
		GT1 = GT1 + GT{j};
		Q1 = Q1 + Q{j};
	end
	rec = TP1./GT1;
	rec(isnan(rec)) = 1;
	prec = TP1./Q1;
	prec(isnan(rec)) = 1;
	if do_plot
		hold on
		plot(rec, prec, color);
		hold off
		xlabel('recall')
		ylabel('precision')
		axis([0 1 0 1])
	end
		
	
end
