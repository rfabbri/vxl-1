function show_sift_based_rank_ordering(img_folder, tag_folder, img_name, sigma, crit_id, K, interval)
	
	files = dir([tag_folder '/*.mat']);	
	
	if ~exist('interval', 'var')
		interval = 1:length(files);
	end
	
	num_files = length(interval);
	classes = zeros(num_files, 1);
	desc = cell(1, num_files);
	counts = zeros(num_files, 1);
	keypoint_unique_ids = cell(num_files, 1);
	
	cp_type = find(crit_id > 0, 1);
	feat_id = crit_id(cp_type);
	
	switch cp_type
		case 1
			descriptor_string = 'min_d';
			frame_string = 'min_f';
		case 2
			descriptor_string = 'max_d';
			frame_string = 'max_f';
		case 3
			descriptor_string = 'saddle_d';
			frame_string = 'saddle_f';
	end
	
	for i = interval
		X = load([tag_folder '/' files(i).name]);
		all_desc = double(getfield(X, descriptor_string));
		all_frames = getfield(X, frame_string);
		keypoint_unique_ids{i} = duplicate_keypoint_unique_labels(all_frames(1:2, :)');			
		desc{i} = all_desc;		
		classes(i) = str2num(files(i).name(4:6));
		counts(i) = size(all_desc, 2);
	end
	
	
	
	unique_classes = unique(classes);
    num_unique_classes = length(unique_classes);
	
	function [img_id2, feat_id2, c] = idx2class(t_i)
		img_id2 = find(t_i <= t_cumcounts, 1);
		if img_id2 == 1
			feat_id2 = t_i;
		else
			feat_id2 = t_i - t_cumcounts(img_id2-1);
		end 
		feat_id2 = keypoint_unique_ids{img_id2}(feat_id2);		
        c = t_classes(img_id2);        
    end
    
	img_id = find(ismember({files.name}, [img_name '.mat']), 1);	
	actual_class = classes(img_id);                             
	%%
	effective_feat_id = find(keypoint_unique_ids{img_id} == feat_id, 1);
	q = desc{img_id}(:, effective_feat_id)'; %query
	%%
	ds = cell2mat(desc)'; %all descriptors
	%%
	t_classes = classes;	
	%
	t_cumcounts = cumsum(counts);
	%%
	%K = 3;
	DistMat = pdist2(q, ds);
	[sorted_dist, sort_index] = sort(DistMat);	
	idx = sort_index(1:K+1);	
	[img_id2, feat_id2, assigned_class] = arrayfun(@idx2class, idx(1:end));
	removal_index = find(img_id2 == img_id & feat_id2 == feat_id);
	img_id2(removal_index) = [];
	feat_id2(removal_index) = [];
	assigned_class(removal_index) = [];
	I1 = double(rgb2gray(imread([img_folder '/' img_name])));
	X1 = load([tag_folder '/' files(img_id).name]);
	
	set(0,'defaulttextinterpreter','none')
	
	show_critical_points(I1, sigma, X1.mins, X1.maxs, X1.saddles);
	
	figure;show_local_tag_struct(I1, sigma, X1.mins, X1.maxs, X1.saddles, X1.min_links, X1.max_links, X1.saddle_links, crit_id);
	title(sprintf('Query %s, Feat_id: %d, Class: %d', img_name, feat_id, actual_class));
	for i = 1:length(img_id2)
		img_name2 = files(img_id2(i)).name(1:end-4);
		I1 = double(rgb2gray(imread([img_folder '/' img_name2])));
		X1 = load([tag_folder '/' files(img_id2(i)).name]);
		switch cp_type
			case 1
				figure;show_local_tag_struct(I1, sigma, X1.mins, X1.maxs, X1.saddles, X1.min_links, X1.max_links, X1.saddle_links, [feat_id2(i) 0 0]);
			case 2
				figure;show_local_tag_struct(I1, sigma, X1.mins, X1.maxs, X1.saddles, X1.min_links, X1.max_links, X1.saddle_links, [0 feat_id2(i) 0]);
			case 3
				figure;show_local_tag_struct(I1, sigma, X1.mins, X1.maxs, X1.saddles, X1.min_links, X1.max_links, X1.saddle_links, [0 0 feat_id2(i)]);
		end
		
		title(sprintf('%s, Feat_id: %d, Class: %d', img_name2, feat_id2(i), assigned_class(i)));
	end
end
