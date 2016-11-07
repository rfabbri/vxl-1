% This is evaluate_viewpoint_new_prob.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Oct 04, 2012

function evaluate_viewpoint_new_prob(size1, size2, L1, L2, final_correspMap, final_pMap, do_plot, color)
	maxscore = max(max(L1(:,3)), max(L2(:,3)));	
	score_th = linspace(0, maxscore, 200);
	num_th = length(score_th);
	TPR = zeros(1,num_th);
	NUM = zeros(1,num_th);
	
	%preprocess
	del_mask = false(1, size(L1,1));
	for i = 1:size(L1,1)
		ind = (L1(i,2)-1)*size1(2)+L1(i,1);
		if isempty(final_correspMap{ind})
			del_mask(i) = true;
		end			
	end	
	L1(del_mask,:) = [];
	origL2 = L2;
	Z = zeros(size2);
	avail_ind = unique(cell2mat(final_correspMap'));
	Z(avail_ind) = 1;
	Z = imfilter(Z, ones(5)); 
	avail_ind = find(Z > 0);
	all_ind = 1:sub2ind(size2, size2(1), size2(2));
	miss_ind = setdiff(all_ind, avail_ind);
	L2ind = sub2ind(size2, L2(:,2), L2(:,1));
	[L2good_ind, IA] = setdiff(L2ind, miss_ind);
	[L2y, L2x] = ind2sub(size2, L2good_ind);
	L2 = [L2x', L2y', origL2(IA, 3)];		
	
	for si = 1:num_th		
		L1(L1(:,3) < score_th(si),:) = [];			
		L2(L2(:,3) < score_th(si),:) = [];
		D = Inf(size(L1,1), size(L2,1));
		for i = 1:size(L1,1)
			ind = (L1(i,2)-1)*size1(2)+L1(i,1);
			cor = final_correspMap{ind};
			[corry,corrx] = ind2sub(size2, cor);
			MM = [corrx; corry]';
			match_ind_L2 = find(ismember(L2(:,1:2), MM, 'rows'));
			prob_vals = final_pMap{ind}(find(ismember(MM, L2(:,1:2), 'rows')));
			D(i, match_ind_L2) = 1-prob_vals;			
		end	
		Matching = assignmentoptimal(D);
		N1 = size(L1,1);
		N2 = size(L2,1);
		TPR(si) = length(find(Matching > 0))/min(N1,N2);
		NUM(si) = (N1+N2)/2;
		if si == 1
			NC = NUM(1);
		end	
	end
	hold on;
	plot(NUM/NC,TPR, color, 'LineWidth', 2);
	xlabel('#points')
	ylabel('symm tpr')
	axis([0 Inf 0 1])
	hold off;
			
		
	
	if 0
		tic
		clear		
		addpath ~/Desktop/sift
		addpath ~/Downloads/sift
		addpath ~/Desktop/harris
		addpath ../../topographical
		configure
		I = double(rgb2gray(imread('/home/firat/Downloads/face/face_v1.bmp')));
		J = double(rgb2gray(imread('/home/firat/Downloads/face/face_v2.bmp')));
		load /home/firat/Downloads/face/face_v1-v2_undist.mat
		corners1 = harris_corners_with_scores(I, 2);
		corners2 = harris_corners_with_scores(J, 2);
		[S, mins1, maxs1, saddles1] = get_critical_points_with_scores(I, 2);
		[S, mins2, maxs2, saddles2] = get_critical_points_with_scores(J, 2);
		sf1 = sift_with_scores(I);
		sf1 = [round(sf1(:,1:2)) sf1(:,3)];
		sf2 = sift_with_scores(J);
		sf2 = [round(sf2(:,1:2)) sf2(:,3)];		
		figure		
		%evaluate_viewpoint_new_prob(size(I), size(J), corners1, corners2, final_correspMap, final_pMap, 1, 'g--')
		%evaluate_viewpoint_new_prob(size(I), size(J), sf1, sf2, final_correspMap, final_pMap,1, 'm--')
		evaluate_viewpoint_new_prob(size(I), size(J), mins1, mins2, final_correspMap, final_pMap,1,'r')
		evaluate_viewpoint_new_prob(size(I), size(J), maxs1, maxs2,final_correspMap, final_pMap, 1,'b')
		%evaluate_viewpoint_new_prob(size(I), size(J), saddles1, saddles2, final_correspMap, final_pMap,1,'k')
		%legend({'harris', 'sift', 'min', 'max', 'saddle'})
		title('v1 vs v2 - sigma = 2')
		toc	
	end
	
	if 0
		tic
		clear		
		addpath ~/Desktop/sift
		addpath ~/Downloads/sift
		addpath ~/Desktop/harris
		addpath ../../topographical
		configure
		I = double(rgb2gray(imread('/home/firat/Downloads/face/face_v2.bmp')));
		J = double(rgb2gray(imread('/home/firat/Downloads/face/face_v7.bmp')));
		load /home/firat/Downloads/face/face_v2-v7_undist.mat
		
		[S, mins1, maxs1, saddles1] = get_critical_points_with_scores(I, 2, 'min');
		[S, mins2, maxs2, saddles2] = get_critical_points_with_scores(J, 2, 'min');
		[S, mins3, maxs3, saddles3] = get_critical_points_with_scores(I, 2, 'max');
		[S, mins4, maxs4, saddles4] = get_critical_points_with_scores(J, 2, 'max');
		[S, mins5, maxs5, saddles5] = get_critical_points_with_scores(I, 2, 'mean');
		[S, mins6, maxs6, saddles6] = get_critical_points_with_scores(J, 2, 'mean');
		[S, mins7, maxs7, saddles7] = get_critical_points_with_scores(I, 2, 'gauss');
		[S, mins8, maxs8, saddles8] = get_critical_points_with_scores(J, 2, 'gauss');
		[S, mins9, maxs9, saddles9] = get_critical_points_with_scores(I, 2, 'neighbor');
		[S, mins10, maxs10, saddles10] = get_critical_points_with_scores(J, 2, 'neighbor');
			
		figure			
		evaluate_viewpoint_new_prob(size(I), size(J), mins1, mins2, final_correspMap, final_pMap,1,'r')
		evaluate_viewpoint_new_prob(size(I), size(J), mins3, mins4, final_correspMap, final_pMap,1,'g')
		evaluate_viewpoint_new_prob(size(I), size(J), mins5, mins6, final_correspMap, final_pMap,1,'b')
		evaluate_viewpoint_new_prob(size(I), size(J), mins7, mins8, final_correspMap, final_pMap,1,'k')
		evaluate_viewpoint_new_prob(size(I), size(J), mins9, mins10, final_correspMap, final_pMap,1,'k--')
		
		legend({'min-curv', 'max-curv', 'mean-curv', 'gauss-curv', 'neighbor'})
		title('v2 vs v7 - sigma = 2')
		
		figure			
		evaluate_viewpoint_new_prob(size(I), size(J), maxs1, maxs2, final_correspMap, final_pMap,1,'r')
		evaluate_viewpoint_new_prob(size(I), size(J), maxs3, maxs4, final_correspMap, final_pMap,1,'g')
		evaluate_viewpoint_new_prob(size(I), size(J), maxs5, maxs6, final_correspMap, final_pMap,1,'b')
		evaluate_viewpoint_new_prob(size(I), size(J), maxs7, maxs8, final_correspMap, final_pMap,1,'k')
		evaluate_viewpoint_new_prob(size(I), size(J), maxs9, maxs10, final_correspMap, final_pMap,1,'k--')
		
		legend({'min-curv', 'max-curv', 'mean-curv', 'gauss-curv', 'neighbor'})
		title('v2 vs v7 - sigma = 2')
		
		figure			
		evaluate_viewpoint_new_prob(size(I), size(J), saddles1, saddles2, final_correspMap, final_pMap,1,'r')
		evaluate_viewpoint_new_prob(size(I), size(J), saddles3, saddles4, final_correspMap, final_pMap,1,'g')
		evaluate_viewpoint_new_prob(size(I), size(J), saddles5, saddles6, final_correspMap, final_pMap,1,'b')
		evaluate_viewpoint_new_prob(size(I), size(J), saddles7, saddles8, final_correspMap, final_pMap,1,'k')
		evaluate_viewpoint_new_prob(size(I), size(J), saddles9, saddles10, final_correspMap, final_pMap,1,'k--')
		
		legend({'min-curv', 'max-curv', 'mean-curv', 'gauss-curv', 'neighbor'})
		title('v2 vs v7 - sigma = 2')
		toc	
	end
	
end
