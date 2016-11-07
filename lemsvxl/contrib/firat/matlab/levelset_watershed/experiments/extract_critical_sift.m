% extract_critical_sift('..', '.bmp', '../critical_sift', 1:56)
% 5 core intervals: 1:45, 46:90, 91:135, 136:180, 181:225

% remember:
% addpath ~/lemsvxl/src/contrib/firat/matlab/
% configure_vlfeat
% extract_critical_sift('~/Desktop/data', '.bmp', '~/Desktop/data/tag_sift', 1:45);

function extract_critical_sift(data_path, img_ext, outfolder, interval)
	addpath ~/lemsvxl/src/contrib/firat/matlab/levelset_watershed
	addpath ~/lemsvxl/src/contrib/firat/matlab/levelset_watershed/experiments/vl_feat_tools
    files = dir([data_path '/*' img_ext]);
	if ~exist(outfolder,'dir')
		mkdir(outfolder);
	end
	sigma = 2;
	test_mode = true;
	for i = interval
		outfile = [outfolder '/' files(i).name '.mat'];
		if exist(outfile, 'file')
			fprintf('%d...Already done\n', i);
			continue
		end
		I = imread([data_path '/' files(i).name]);
		%[saddles, mins, maxs] = evolving_basin_critical_point_detector(double(rgb2gray(I)), sigma, 0);
		[mins, maxs, saddles, min_links, max_links, saddle_links] = cpdetect3(double(rgb2gray(I)), sigma, 0);
		[min_f, min_d, max_f, max_d, saddle_f, saddle_d] = compute_critical_sift(rgb2gray(I), sigma, mins, maxs, saddles);
		if test_mode
			mins1 = remove_duplicate_keypoints(min_f(1:2,:)');   	
			maxs1 = remove_duplicate_keypoints(max_f(1:2,:)');
			saddles1 = remove_duplicate_keypoints(saddle_f(1:2,:)');
			if all(all(mins1 == mins)) && all(all(maxs1 == maxs)) && all(all(saddles1 == saddles))
				fprintf('%d...Passed\n', i);
			else
				error('Failed');
			end
		end
		%save([outfolder '/' files(i).name '.mat'], 'min_f', 'min_d', 'max_f', 'max_d', 'saddle_f', 'saddle_d');
		save(outfile, 'min_f', 'min_d', 'mins', 'max_f', 'max_d', 'maxs', 'saddle_f', 'saddle_d', 'saddles', 'min_links', 'max_links', 'saddle_links');
    end
    if 0 % example
    	cd levelset_watershed
    	cd experiments
    	addpath ~/lemsvxl/src/contrib/firat/matlab/
    	configure_vlfeat
    	extract_critical_sift('~/Desktop/data', '.bmp', '~/Desktop/data/tag_sift_test_mode', 1:45);
    	
    	cd levelset_watershed
    	cd experiments
    	addpath ~/lemsvxl/src/contrib/firat/matlab/
    	configure_vlfeat
    	extract_critical_sift('/home/firat/Datasets/RobotDatasetSubset/SET002', '.bmp', '/home/firat/Datasets/RobotDatasetSubset/SET002/tag_sift', 1:133);
    	extract_critical_sift('/home/firat/Datasets/RobotDatasetSubset/SET002', '.bmp', '/home/firat/Datasets/RobotDatasetSubset/SET002/tag_sift', 134:266);
    	extract_critical_sift('/home/firat/Datasets/RobotDatasetSubset/SET007', '.bmp', '/home/firat/Datasets/RobotDatasetSubset/SET007/tag_sift', 1:133);
    	extract_critical_sift('/home/firat/Datasets/RobotDatasetSubset/SET007', '.bmp', '/home/firat/Datasets/RobotDatasetSubset/SET007/tag_sift', 134:266);
    	extract_critical_sift('/home/firat/Datasets/RobotDatasetSubset/SET030', '.bmp', '/home/firat/Datasets/RobotDatasetSubset/SET030/tag_sift', 1:133);
    	extract_critical_sift('/home/firat/Datasets/RobotDatasetSubset/SET030', '.bmp', '/home/firat/Datasets/RobotDatasetSubset/SET030/tag_sift', 134:266);
    	
    end
end
