%example:
%extract_tag_features('/home/firat/Desktop/data', '.bmp', '/home/firat/Desktop/data/tag_sift', 'saddle-extremum', '/home/firat/Desktop/data/tag_sift/saddle_extremum2');
%extract_tag_features('/home/firat/Desktop/data', '.bmp', '/home/firat/Desktop/data/tag_sift', 'saddle-centric', '/home/firat/Desktop/data/tag_sift/saddle_centric');

function extract_tag_features(data_folder, img_ext, tag_folder, feature_type, outfolder)
	addpath vl_feat_tools
	addpath ~/lemsvxl/src/contrib/firat/matlab/levelset2d_segmentation/tools
	addpath ~/lemsvxl/src/contrib/firat/matlab/levelset_watershed
	if ~exist(outfolder,'dir')
		mkdir(outfolder);
	end
	files = dir([data_folder '/*' img_ext]);
	sigma = 2;
	switch feature_type
		case 'saddle-extremum' % saddle-min and saddle-max
			for i = 1:length(files)
				fprintf('%d ', i);
				I = double(rgb2gray(imread([data_folder '/' files(i).name])));				
				J = smooth_image(I, sigma);
				X = load([tag_folder '/' files(i).name '.mat']);
				mins = remove_duplicate_keypoints(X.min_f(1:2,:)');   	
    			maxs = remove_duplicate_keypoints(X.max_f(1:2,:)');
    			saddles = remove_duplicate_keypoints(X.saddle_f(1:2,:)');
    			
				[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(sigma, 1, 1);
				Ixx = imfilter(I, Gxx, 'replicate', 'conv');
				Iyy = imfilter(I, Gyy, 'replicate', 'conv');
				Ixy = imfilter(I, Gxy, 'replicate', 'conv');			
				
				min_s_minc = get_min_curvature(Ixx, Ixy, Iyy, X.min_f);		
				max_s_minc = get_min_curvature(Ixx, Ixy, Iyy, X.max_f);
				saddle_s_minc = get_min_curvature(Ixx, Ixy, Iyy, X.saddle_f);
				
				num_saddles = size(X.saddle_links, 1);
				num_mins = size(X.min_links, 1);
				num_maxs = size(X.max_links, 1);
				if size(mins, 1) ~= num_mins || size(maxs, 1) ~= num_maxs || size(saddles, 1) ~= num_saddles
					error('Duplicate keypoint elimination error');
				end
				sad_min_d = zeros(128*2, num_saddles*2);				
				sad_max_d = zeros(128*2, num_saddles*2);
				sad_min_pers_s = zeros(1, num_saddles*2); 
				sad_max_pers_s = zeros(1, num_saddles*2);
				sad_max_minc_s = zeros(1, num_saddles*2);
				sad_min_minc_s = zeros(1, num_saddles*2);
				
				for s = 1:num_saddles
					sad = saddles(s, :);
					min1 = mins(X.saddle_links(s, 1), :);
					min2 = mins(X.saddle_links(s, 2), :);
					max1 = maxs(X.saddle_links(s, 3), :);
					max2 = maxs(X.saddle_links(s, 4), :);
					sad_ind = find(ismember(X.saddle_f(1:2,:)', sad, 'rows'), 1);
					min1_ind = find(ismember(X.min_f(1:2,:)', min1, 'rows'), 1);
					min2_ind = find(ismember(X.min_f(1:2,:)', min2, 'rows'), 1);
					max1_ind = find(ismember(X.max_f(1:2,:)', max1, 'rows'), 1);
					max2_ind = find(ismember(X.max_f(1:2,:)', max2, 'rows'), 1);
					sad_min_d(:, [2*s-1, 2*s]) = [repmat(X.saddle_d(:, sad_ind), 1, 2); X.min_d(:, min1_ind), X.min_d(:, min2_ind)];
					sad_max_d(:, [2*s-1, 2*s]) = [repmat(X.saddle_d(:, sad_ind), 1, 2); X.max_d(:, max1_ind), X.max_d(:, max2_ind)];					
					V = interp2(J, [sad(1) min1(1) min2(1) max1(1) max2(1)], [sad(2) min1(2) min2(2) max1(2) max2(2)], 'cubic');
					sad_min_pers_s([2*s-1, 2*s]) = abs([V(1)-V(2), V(1)-V(3)]);
					sad_max_pers_s([2*s-1, 2*s]) = abs([V(1)-V(4), V(1)-V(5)]);
					sad_min_minc_s([2*s-1, 2*s]) = [min([min_s_minc(min1_ind) saddle_s_minc(sad_ind)])  min([min_s_minc(min2_ind) saddle_s_minc(sad_ind)])];
					sad_max_minc_s([2*s-1, 2*s]) = [min([max_s_minc(max1_ind) saddle_s_minc(sad_ind)])  min([max_s_minc(max2_ind) saddle_s_minc(sad_ind)])];					
				end
				save([outfolder '/' files(i).name '.mat'], 'sad_min_d', 'sad_max_d', 'sad_min_pers_s', 'sad_max_pers_s', 'sad_min_minc_s', 'sad_max_minc_s');
			end
		case 'extremum-saddle-extremum' % max-saddle-max, min-saddle-min, max-saddle-min
			fprintf('Not implemented\n');
			return
		case 'saddle-centric'
			for i = 1:length(files)
				fprintf('%d ', i);
				I = double(rgb2gray(imread([data_folder '/' files(i).name])));				
				
				X = load([tag_folder '/' files(i).name '.mat']);
				mins = remove_duplicate_keypoints(X.min_f(1:2,:)');   	
    			maxs = remove_duplicate_keypoints(X.max_f(1:2,:)');
    			saddles = remove_duplicate_keypoints(X.saddle_f(1:2,:)');
    			
				[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(sigma, 1, 1);
				Ixx = imfilter(I, Gxx, 'replicate', 'conv');
				Iyy = imfilter(I, Gyy, 'replicate', 'conv');
				Ixy = imfilter(I, Gxy, 'replicate', 'conv');			
				
				saddle_s_minc = get_min_curvature(Ixx, Ixy, Iyy, X.saddle_f);
				
				num_saddles = size(X.saddle_links, 1);
				num_mins = size(X.min_links, 1);
				num_maxs = size(X.max_links, 1);
				if size(mins, 1) ~= num_mins || size(maxs, 1) ~= num_maxs || size(saddles, 1) ~= num_saddles
					error('Duplicate keypoint elimination error');
				end
				sad_centric_d = zeros(128*5, num_saddles);						
				sad_centric_minc_s = zeros(1, num_saddles);				
				
				for s = 1:num_saddles
					sad = saddles(s, :);
					min1 = mins(X.saddle_links(s, 1), :);
					min2 = mins(X.saddle_links(s, 2), :);
					max1 = maxs(X.saddle_links(s, 3), :);
					max2 = maxs(X.saddle_links(s, 4), :);
					sad_ind = find(ismember(X.saddle_f(1:2,:)', sad, 'rows'), 1);
					min1_ind = find(ismember(X.min_f(1:2,:)', min1, 'rows'), 1);
					min2_ind = find(ismember(X.min_f(1:2,:)', min2, 'rows'), 1);
					max1_ind = find(ismember(X.max_f(1:2,:)', max1, 'rows'), 1);
					max2_ind = find(ismember(X.max_f(1:2,:)', max2, 'rows'), 1);
					sad_centric_d(:, s) = [X.saddle_d(:, sad_ind); X.min_d(:, min1_ind); X.min_d(:, min2_ind); X.max_d(:, max1_ind); X.max_d(:, max2_ind)];				
					sad_centric_minc_s(s) = saddle_s_minc(sad_ind);										
				end
				save([outfolder '/' files(i).name '.mat'], 'sad_centric_d', 'sad_centric_minc_s');	
			end		
	end 
end

function minc = get_min_curvature(Ixx, Ixy, Iyy, pts)
	fxx = interp2(Ixx, pts(1,:), pts(2,:), 'cubic');
	fxy = interp2(Ixy, pts(1,:), pts(2,:), 'cubic');
	fyy = interp2(Iyy, pts(1,:), pts(2,:), 'cubic');
	A = fxx + fyy;
	B = (fxx.*fyy) - (fxy.^2);
	L1 = (A + sqrt(A.^2 - 4*B))/2;
	L2 = (A - sqrt(A.^2 - 4*B))/2;
	minc = min(abs([L1;L2]));
end
