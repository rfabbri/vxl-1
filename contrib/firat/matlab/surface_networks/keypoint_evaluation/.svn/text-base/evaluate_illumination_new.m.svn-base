% This is evaluate_illumination_new.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 30, 2012

function evaluate_illumination_new(L1, L2, dthresh, is_normalized, color)
	maxscore = max(max(L1(:,3)), max(L2(:,3)));	
	mu = mean([L1(:,3);L2(:,3)]);
	std1 = std([L1(:,3);L2(:,3)], 1);	
	%score_th = unique([linspace(0, mu-std1, 100), linspace(mu-std1, mu+std1, 250), linspace(mu+std1, maxscore, 100)]);
	score_th = linspace(0, maxscore, 200);
	num_th = length(score_th);
	TPR = zeros(1,num_th);
	NUM = zeros(1,num_th);
	
	for si = 1:num_th
		L1(L1(:,3) < score_th(si),:) = [];			
		L2(L2(:,3) < score_th(si),:) = [];
		DM = pdist2(L1(:,1:2), L2(:,1:2));				
		DM(DM > dthresh) = Inf;
		Matching = assignmentoptimal(DM);
		%Matching = csaWrapper(DM);
		N1 = size(L1,1);
		N2 = size(L2,1);
		TPR(si) = length(find(Matching > 0))/min(N1,N2);
		NUM(si) = (N1+N2)/2;
		if si == 1
			NC = NUM(1);
		end
	end
	if ~is_normalized
		NC = 1;
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
		addpath ~/Desktop/harris
		addpath ../../topographical
		configure
		figure
		I = double(rgb2gray(imread('/home/firat/Desktop/mogrify/05.png')));
		J = double(rgb2gray(imread('/home/firat/Desktop/mogrify/06.png')));
		corners1 = harris_corners_with_scores(I, 2);
		corners2 = harris_corners_with_scores(J, 2);
		[S, mins1, maxs1, saddles1] = get_critical_points_with_scores(I, 2);
		[S, mins2, maxs2, saddles2] = get_critical_points_with_scores(J, 2);
		sf1 = sift_with_scores(I);
		sf2 = sift_with_scores(J);		
		evaluate_illumination_new(corners1, corners2, sqrt(2), 1, 'g--')
		evaluate_illumination_new(sf1, sf2, sqrt(2), 1, 'm--')
		evaluate_illumination_new(mins1, mins2, sqrt(2), 1,'r')
		evaluate_illumination_new(maxs1, maxs2, sqrt(2), 1,'b')
		evaluate_illumination_new(saddles1, saddles2, sqrt(2), 1,'k')
		legend({'harris', 'sift', 'min', 'max', 'saddle'})
		title('05 vs 06');
		toc		
	end
	
	if 0
		clear		
		addpath ~/Desktop/sift
		addpath ~/Desktop/harris
		addpath ../../topographical
		configure
		figure
		I = double(rgb2gray(imread('/home/firat/Desktop/mogrify/05.png')));
		J = double(rgb2gray(imread('/home/firat/Desktop/mogrify/06.png')));
		[S, mins1, maxs1, saddles1] = get_critical_points_with_scores(I, 2);
		[S, mins2, maxs2, saddles2] = get_critical_points_with_scores(J, 2);
		[S, mins3, maxs3, saddles3] = get_critical_points_with_scores(I, 3);
		[S, mins4, maxs4, saddles4] = get_critical_points_with_scores(J, 3);
		[S, mins5, maxs5, saddles5] = get_critical_points_with_scores(I, 4);
		[S, mins6, maxs6, saddles6] = get_critical_points_with_scores(J, 4);
		mins7 = [mins1; mins3;mins5];
		%[ign, ind] = unique(mins7(:,1:2), 'rows');
		%mins7 = mins7(ind,:);
		maxs7 = [maxs1; maxs3; maxs5];
		%[ign, ind] = unique(maxs7(:,1:2), 'rows');
		%maxs7 = maxs7(ind,:);
		saddles7 = [saddles1;saddles3;saddles5];
		%[ign, ind] = unique(saddles7(:,1:2), 'rows');
		%saddles7 = saddles7(ind,:);
		mins8 = [mins2; mins4;mins6];
		%[ign, ind] = unique(mins8(:,1:2), 'rows');
		%mins8 = mins8(ind,:);
		maxs8 = [maxs2; maxs4; maxs6];
		%[ign, ind] = unique(maxs8(:,1:2), 'rows');
		%maxs8 = maxs8(ind,:);
		saddles8 = [saddles2;saddles4;saddles6];
		%[ign, ind] = unique(saddles8(:,1:2), 'rows');
		%saddles8 = saddles8(ind,:);
		evaluate_illumination_new(mins1, mins2, sqrt(2), 1,'r')
		evaluate_illumination_new(maxs1, maxs2, sqrt(2), 1,'b')
		evaluate_illumination_new(saddles1, saddles2, sqrt(2), 1,'k')
		
		
		
		evaluate_illumination_new(mins7, mins8, sqrt(2), 1,'r--')
		evaluate_illumination_new(maxs7, maxs8, sqrt(2), 1,'b--')
		evaluate_illumination_new(saddles7, saddles8, sqrt(2), 1,'k--')
		
		
		
		legend({'min', 'max', 'saddle', 'ms min', 'ms max', 'ms saddle'})
	end
	
	if 0
		clear		
		addpath ~/Desktop/sift
		addpath ~/Desktop/harris
		addpath ../../topographical
		configure
		figure
		I = double(rgb2gray(imread('/home/firat/Desktop/mogrify/05.png')));
		J = double(rgb2gray(imread('/home/firat/Desktop/mogrify/06.png')));
		[S, mins1, maxs1, saddles1] = get_wswc_critical_points_with_scores(I, 2);
		[S, mins2, maxs2, saddles2] = get_wswc_critical_points_with_scores(J, 2);
		[S, mins3, maxs3, saddles3] = get_critical_points_with_scores(I, 2);
		[S, mins4, maxs4, saddles4] = get_critical_points_with_scores(J, 2);
		evaluate_illumination_new(mins1, mins2, sqrt(2), 1,'r--')
		evaluate_illumination_new(maxs1, maxs2, sqrt(2), 1,'b--')
		evaluate_illumination_new(saddles1, saddles2, sqrt(2), 1,'k--')
		
		evaluate_illumination_new(mins3, mins4, sqrt(2), 1,'r')
		evaluate_illumination_new(maxs3, maxs4, sqrt(2), 1,'b')
		evaluate_illumination_new(saddles3, saddles4, sqrt(2), 1,'k')
		
		legend({'wcws min', 'wcws max', 'wcws saddle', 'min', 'max', 'saddle'})
	
	end
	
	if 0
		clear		
		addpath ~/Desktop/sift
		addpath ~/Desktop/harris
		addpath ../../topographical
		configure
		figure
		I = double(rgb2gray(imread('/home/firat/Desktop/mogrify/04.png')));
		J = double(rgb2gray(imread('/home/firat/Desktop/mogrify/10.png')));
		[S, mins1, maxs1, saddles1] = get_multiscale_critical_points(I);
		[S, mins2, maxs2, saddles2] = get_multiscale_critical_points(J);
		[S, mins3, maxs3, saddles3] = get_critical_points_with_scores(I, 2);
		[S, mins4, maxs4, saddles4] = get_critical_points_with_scores(J, 2);
		
		evaluate_illumination_new(mins1, mins2, sqrt(2), 1,'r--')
		evaluate_illumination_new(maxs1, maxs2, sqrt(2), 1,'b--')
		evaluate_illumination_new(saddles1, saddles2, sqrt(2), 1,'k--')
		
		evaluate_illumination_new(mins3, mins4, sqrt(2), 1,'r')
		evaluate_illumination_new(maxs3, maxs4, sqrt(2), 1,'b')
		evaluate_illumination_new(saddles3, saddles4, sqrt(2), 1,'k')
		
		legend({'ms min', 'ms max', 'ms saddle', 'min', 'max', 'saddle'})
	end
	
	if 0
		clear		
		addpath ~/Desktop/sift
		addpath ~/Desktop/harris
		addpath ../../topographical
		configure
		
		I = double(rgb2gray(imread('/home/firat/Desktop/mogrify/04.png')));
		J = double(rgb2gray(imread('/home/firat/Desktop/mogrify/10.png')));
		[S, mins1, maxs1, saddles1] = get_critical_points_with_scores(I, 2);
		[S, mins2, maxs2, saddles2] = get_critical_points_with_scores(J, 2);
		[S, mins3, maxs3, saddles3] = get_critical_points_with_scores(I, 3);
		[S, mins4, maxs4, saddles4] = get_critical_points_with_scores(J, 3);
		[S, mins5, maxs5, saddles5] = get_critical_points_with_scores(I, 4);
		[S, mins6, maxs6, saddles6] = get_critical_points_with_scores(J, 4);
		[S, mins7, maxs7, saddles7] = get_critical_points_with_scores(I, 5);
		[S, mins8, maxs8, saddles8] = get_critical_points_with_scores(J, 5);
		[S, mins9, maxs9, saddles9] = get_critical_points_with_scores(I, 6);
		[S, mins10, maxs10, saddles10] = get_critical_points_with_scores(J, 6);
		[S, mins11, maxs11, saddles11] = get_multiscale_critical_points(I, [1 2 3 4 5 6 7]);
		[S, mins12, maxs12, saddles12] = get_multiscale_critical_points(J, [1 2 3 4 5 6 7]);
		figure
		evaluate_illumination_new(mins1, mins2, sqrt(2), 1,'r')
		evaluate_illumination_new(mins3, mins4, sqrt(2), 1,'g')
		evaluate_illumination_new(mins5, mins6, sqrt(2), 1,'b')
		evaluate_illumination_new(mins7, mins8, sqrt(2), 1,'k')
		evaluate_illumination_new(mins9, mins10, sqrt(2), 1,'k--')
		evaluate_illumination_new(mins11, mins12, sqrt(2), 1,'r--')
		legend({'min 2', 'min 3', 'min 4', 'min 5', 'min 6', 'min ms'})
		
		figure
		evaluate_illumination_new(maxs1, maxs2, sqrt(2), 1,'r')
		evaluate_illumination_new(maxs3, maxs4, sqrt(2), 1,'g')
		evaluate_illumination_new(maxs5, maxs6, sqrt(2), 1,'b')
		evaluate_illumination_new(maxs7, maxs8, sqrt(2), 1,'k')
		evaluate_illumination_new(maxs9, maxs10, sqrt(2), 1,'k--')
		evaluate_illumination_new(maxs11, maxs12, sqrt(2), 1,'r--')
		legend({'max 2', 'max 3', 'max 4', 'max 5', 'max 6', 'max ms'})
		
		figure
		evaluate_illumination_new(saddles1, saddles2, sqrt(2), 1,'r')
		evaluate_illumination_new(saddles3, saddles4, sqrt(2), 1,'g')
		evaluate_illumination_new(saddles5, saddles6, sqrt(2), 1,'b')
		evaluate_illumination_new(saddles7, saddles8, sqrt(2), 1,'k')
		evaluate_illumination_new(saddles9, saddles10, sqrt(2), 1,'k--')
		evaluate_illumination_new(saddles11, saddles12, sqrt(2), 1,'r--')
		legend({'saddle 2', 'saddle 3', 'saddle 4', 'saddle 5', 'saddle 6', 'saddle ms'})
	
	end
	
	if 0
		tic
		clear		
		addpath ~/Desktop/sift
		addpath ~/Desktop/harris
		addpath ../../topographical
		configure
		
		I = double(rgb2gray(imread('/home/firat/Desktop/mogrify/01.png')));
		J = double(rgb2gray(imread('/home/firat/Desktop/mogrify/04.png')));
		
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
		evaluate_illumination_new(mins1, mins2, sqrt(2), 1,'r')
		evaluate_illumination_new(mins3, mins4, sqrt(2), 1,'g')
		evaluate_illumination_new(mins5, mins6, sqrt(2), 1,'b')
		evaluate_illumination_new(mins7, mins8, sqrt(2), 1,'k')
		evaluate_illumination_new(mins9, mins10, sqrt(2), 1,'k--')
		legend({'min-curv', 'max-curv', 'mean-curv', 'gauss-curv', 'neighbor'})
		title('01 vs 04');
		
		figure
		evaluate_illumination_new(maxs1, maxs2, sqrt(2), 1,'r')
		evaluate_illumination_new(maxs3, maxs4, sqrt(2), 1,'g')
		evaluate_illumination_new(maxs5, maxs6, sqrt(2), 1,'b')
		evaluate_illumination_new(maxs7, maxs8, sqrt(2), 1,'k')
		evaluate_illumination_new(maxs9, maxs10, sqrt(2), 1,'k--')
		legend({'min-curv', 'max-curv', 'mean-curv', 'gauss-curv', 'neighbor'})
		title('01 vs 04');
		
		figure
		evaluate_illumination_new(saddles1, saddles2, sqrt(2), 1,'r')
		evaluate_illumination_new(saddles3, saddles4, sqrt(2), 1,'g')
		evaluate_illumination_new(saddles5, saddles6, sqrt(2), 1,'b')
		evaluate_illumination_new(saddles7, saddles8, sqrt(2), 1,'k')
		evaluate_illumination_new(saddles9, saddles10, sqrt(2), 1,'k--')
		evaluate_illumination_new(saddles11, saddles12, sqrt(2), 1,'r--')
		legend({'min-curv', 'max-curv', 'mean-curv', 'gauss-curv', 'neighbor'})
		title('01 vs 04');
		
		toc		
	end
	
end



