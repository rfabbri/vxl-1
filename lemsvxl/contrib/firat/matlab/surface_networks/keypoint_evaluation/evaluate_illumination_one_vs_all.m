% This is evaluate_illumination_one_vs_all.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 30, 2012

function evaluate_illumination_one_vs_all(gt_loc, locs, color)
	th = 0:.5:3;
	num_th = length(th);
	TP = zeros(1, num_th);
	for i = 1:length(locs)
		[TP1,GT] = evaluate_illumination(gt_loc, locs{i}, th, 0, color);
		TP = TP + TP1;
	end
	S = sum(GT);
	hold on
	plot(th, TP/S/length(locs), color);
	hold off
	xlabel('distance threshold')
	ylabel('average recall')
	axis([0 max(th) 0 1])
	
	%experiment script
	%sift
	if 0
		N = 10;
		locs = cell(1, N);
		for i = 1:N
			
			if i < 10
				[image, descriptors, locs{i}] = sift(['/home/firat/Downloads/jpg/mogrify/0' num2str(i) '.png']);	
			else
				[image, descriptors, locs{i}] = sift(['/home/firat/Downloads/jpg/mogrify/' num2str(i) '.png']);
			end
		end
		gt_loc = locs{1};
		q_loc = cell(1,N-1);
		for i = 2:N
			q_loc{i-1} = {locs{i}};
		end
		evaluate_illumination_one_vs_all({gt_loc}, q_loc, 'g');
	end
	%critical point single scale, sigma = 2
	if 0
		N = 10;
		mins = cell(1, N);
		maxs = cell(1, N);
		saddles = cell(1, N);
		for i = 1:N			
			if i < 10
				I = double(rgb2gray(imread(['/home/firat/Downloads/jpg/mogrify/0' num2str(i) '.png'])));				
			else
				I = double(rgb2gray(imread(['/home/firat/Downloads/jpg/mogrify/' num2str(i) '.png'])));
			end
			[S, mins{i}, maxs{i}, saddles{i}] = get_critical_points(I, 2);
		end
		gt_loc = {mins{1};maxs{1};saddles{1}};
		q_loc = cell(1,N-1);
		for i = 2:N
			q_loc{i-1} = {mins{i};maxs{i};saddles{i}};
		end
		evaluate_illumination_one_vs_all(gt_loc, q_loc, 'r');
	end
	%critical point multi scale, sigma = 2,3,4
	if 0
		N = 10;
		mins = cell(1, N);
		maxs = cell(1, N);
		saddles = cell(1, N);
		for i = 1:N			
			if i < 10
				I = double(rgb2gray(imread(['/home/firat/Downloads/jpg/mogrify/0' num2str(i) '.png'])));				
			else
				I = double(rgb2gray(imread(['/home/firat/Downloads/jpg/mogrify/' num2str(i) '.png'])));
			end
			[S, mins1, maxs1, saddles1] = get_min_max_saddle_reflection(I, 2);
			[S, mins2, maxs2, saddles2] = get_min_max_saddle_reflection(I, 3);
			[S, mins3, maxs3, saddles3] = get_min_max_saddle_reflection(I, 4);
			mins{i} = unique([mins1;mins2;mins3], 'rows');
			maxs{i} = unique([maxs1;maxs2;maxs3], 'rows');
			saddles{i} = unique([saddles1;saddles2;saddles3], 'rows');
		end
		gt_loc = {mins{1};maxs{1};saddles{1}};
		q_loc = cell(1,N-1);
		for i = 2:N
			q_loc{i-1} = {mins{i};maxs{i};saddles{i}};
		end
		evaluate_illumination_one_vs_all(gt_loc, q_loc, 'b');
	end
	%comparison plots
	if 0
		I1 = double(rgb2gray(imread(['/home/firat/Downloads/jpg/mogrify/01.png'])));
		I2 = double(rgb2gray(imread(['/home/firat/Downloads/jpg/mogrify/02.png'])));	
		[S, mins1, maxs1, saddles1] = get_critical_points(I1, 2);
		[S, mins2, maxs2, saddles2] = get_critical_points(I2, 2);
		figure; imagesc(S(2:end-1,2:end-1)); colormap gray
		axis image; axis off; hold on;
		plot(mins2(:,1), mins2(:,2), 'ro','MarkerSize', 8,'LineWidth', 2);
		plot(maxs2(:,1), maxs2(:,2), 'r^','MarkerSize', 8,'LineWidth', 2);
		plot(saddles2(:,1), saddles2(:,2), 'r+','MarkerSize', 8,'LineWidth', 2);
		
		plot(mins1(:,1), mins1(:,2), 'go','MarkerSize', 6,'LineWidth', 2);
		plot(maxs1(:,1), maxs1(:,2), 'g^','MarkerSize', 6,'LineWidth', 2);
		plot(saddles1(:,1), saddles1(:,2), 'g+','MarkerSize', 6,'LineWidth', 2);
		
		hold off
	
	
		[im, descriptors, locs1] = sift('/home/firat/Downloads/jpg/mogrify/01.png');
		[im, descriptors, locs2] = sift('/home/firat/Downloads/jpg/mogrify/02.png');
		figure; imagesc(im); colormap gray
		axis image; axis off; hold on;
		plot(locs2(:,2), locs2(:,1), 'rs','MarkerSize', 6,'LineWidth', 2);
		plot(locs1(:,2), locs1(:,1), 'gs','MarkerSize', 6,'LineWidth', 2);		
		
		
		hold off
	
	end
	
end
