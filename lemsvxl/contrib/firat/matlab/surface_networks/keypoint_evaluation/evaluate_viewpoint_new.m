% This is evaluate_viewpoint_new.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 17, 2012

function evaluate_viewpoint_new(L1, L2, C, size2, dthresh, is_normalized, color)
	fprintf('Number of points in view 1: %d\n', size(L1,1));
	fprintf('Number of points in view 2: %d\n', size(L2,1));
	%transfer the points to the other view, delete the points in the first view with no correspondence
	converter = @(x0,y0)([C(y0,x0,1), C(y0,x0,2)]);
	L1 = [cell2mat(arrayfun(converter, L1(:,1), L1(:,2), 'UniformOutput', false)), L1(:,3)];
	[rd, cd] = find(L1(:,1) < 0);
	L1(rd,:) = [];
	fprintf('Number of points in view 1 after preprocessing: %d\n', size(L1,1));
	%delete the points in the second view with no correspondence
	origL2 = L2;
	Cx = C(:,:,1);	
	Cx(Cx < 0) = [];
	Cy = C(:,:,2);
	Cy(Cy < 0) = [];
	avail_ind = sub2ind(size2, Cy, Cx);
	Z = zeros(size2);
	Z(avail_ind) = 1;
	Z = imfilter(Z, ones(5)); 
	avail_ind = find(Z > 0);
	%figure; imagesc(Z>0); colormap gray; axis image; pause
	all_ind = 1:sub2ind(size2, size2(1), size2(2));
	miss_ind = setdiff(all_ind, avail_ind);
	L2ind = sub2ind(size2, L2(:,2), L2(:,1));
	[L2good_ind, IA] = setdiff(L2ind, miss_ind);
	[L2y, L2x] = ind2sub(size2, L2good_ind);
	L2 = [L2x', L2y', origL2(IA, 3)];
	fprintf('Number of points in view 2 after preprocessing: %d\n\n', size(L2,1));	
	%%
	evaluate_illumination_new(L1, L2, dthresh, is_normalized, color);
	
	if 0
		tic
		clear		
		addpath ~/Desktop/sift
		addpath ~/Desktop/harris
		addpath ../../topographical
		configure
		I = double(rgb2gray(imread('/home/firat/Downloads/face/face_v1.bmp')));
		J = double(rgb2gray(imread('/home/firat/Downloads/face/face_v2.bmp')));
		load /home/firat/Downloads/face/face_v1-v2_undist.mat
		C = make_prob_corresp_one_to_one(size(I), size(J), final_correspMap, final_pMap);
		corners1 = harris_corners_with_scores(I, 2);
		corners2 = harris_corners_with_scores(J, 2);
		[S, mins1, maxs1, saddles1] = get_critical_points_with_scores(I, 2);
		[S, mins2, maxs2, saddles2] = get_critical_points_with_scores(J, 2);
		sf1 = sift_with_scores(I);
		sf1 = [round(sf1(:,1:2)) sf1(:,3)];
		sf2 = sift_with_scores(J);
		sf2 = [round(sf2(:,1:2)) sf2(:,3)];		
		figure
		dist_thresh = 2;
		evaluate_viewpoint_new(corners1, corners2, C, size(J), dist_thresh, 1, 'g--')
		evaluate_viewpoint_new(sf1, sf2, C, size(J), dist_thresh, 1, 'm--')
		evaluate_viewpoint_new(mins1, mins2, C, size(J), dist_thresh, 1,'r')
		evaluate_viewpoint_new(maxs1, maxs2, C, size(J), dist_thresh, 1,'b')
		evaluate_viewpoint_new(saddles1, saddles2, C, size(J), dist_thresh, 1,'k')
		legend({'harris', 'sift', 'min', 'max', 'saddle'})
		title('v2 vs v7 - sigma = 2')
		toc		
	end
	
	if 0		
		clear		
		addpath ~/Desktop/sift
		addpath ~/Desktop/harris
		addpath ../../topographical
		configure
		I = double(rgb2gray(imread('/home/firat/Downloads/face/face_v2.bmp')));
		J = double(rgb2gray(imread('/home/firat/Downloads/face/face_v7.bmp')));
		load /home/firat/Downloads/face/face_v2-v7_undist.mat
		C = make_prob_corresp_one_to_one(size(I), size(J), final_correspMap, final_pMap);
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
		evaluate_viewpoint_new(mins1, mins2, C, size(J), sqrt(2), 1,'r')
		evaluate_viewpoint_new(mins3, mins4, C, size(J), sqrt(2), 1,'g')
		evaluate_viewpoint_new(mins5, mins6, C, size(J), sqrt(2), 1,'b')
		evaluate_viewpoint_new(mins7, mins8, C, size(J), sqrt(2), 1,'k')
		evaluate_viewpoint_new(mins9, mins10, C, size(J), sqrt(2), 1,'r--')
		evaluate_viewpoint_new(mins11, mins12, C, size(J), sqrt(2), 1,'k--')
		legend({'min 2', 'min 3', 'min 4', 'min 5', 'min 6', 'min ms'})
		
		figure
		evaluate_viewpoint_new(maxs1, maxs2, C, size(J), sqrt(2), 1,'r')
		evaluate_viewpoint_new(maxs3, maxs4, C, size(J), sqrt(2), 1,'g')
		evaluate_viewpoint_new(maxs5, maxs6, C, size(J), sqrt(2), 1,'b')
		evaluate_viewpoint_new(maxs7, maxs8, C, size(J), sqrt(2), 1,'k')
		evaluate_viewpoint_new(maxs9, maxs10, C, size(J), sqrt(2), 1,'r--')
		evaluate_viewpoint_new(maxs11, maxs12, C, size(J), sqrt(2), 1,'k--')
		legend({'max 2', 'max 3', 'max 4', 'max 5', 'max 6', 'max ms'})
		
		
		figure
		evaluate_viewpoint_new(saddles1, saddles2, C, size(J), sqrt(2), 1,'r')
		evaluate_viewpoint_new(saddles3, saddles4, C, size(J), sqrt(2), 1,'g')
		evaluate_viewpoint_new(saddles5, saddles6, C, size(J), sqrt(2), 1,'b')
		evaluate_viewpoint_new(saddles7, saddles8, C, size(J), sqrt(2), 1,'k')
		evaluate_viewpoint_new(saddles9, saddles10, C, size(J), sqrt(2), 1,'r--')
		evaluate_viewpoint_new(saddles11, saddles12, C, size(J), sqrt(2), 1,'k--')
		legend({'saddle 2', 'saddle 3', 'saddle 4', 'saddle 5', 'saddle 6', 'saddle ms'})
		
				
	end
	
	
			
end





