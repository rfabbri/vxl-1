function TP = min_sift_performance(sift_folder, top_percent, score_type)
	files = dir([sift_folder '/*.mat']);
	num_files = length(files);
	classes = zeros(num_files, 1);
	desc = cell(1, num_files);
	counts = zeros(num_files, 1);
	for i = 1:num_files
		X = load([sift_folder '/' files(i).name]);
		all_desc = double(X.min_d);
		num_desc = size(all_desc, 2);
		num_to_keep = round(num_desc * top_percent);
		switch score_type
			case 'min'
				[ign, sort_index] = sort(X.min_s_min, 'descend');
			case 'gauss'
				[ign, sort_index] = sort(X.min_s_gauss, 'descend');
			case 'mean'
				[ign, sort_index] = sort(X.min_s_mean, 'descend');
			otherwise
				error('Unknown score type!');
		end	
		desc{i} = all_desc(:, sort_index(1:num_to_keep));
		counts(i) = num_to_keep;
		classes(i) = str2num(files(i).name(4:6));
	end
	unique_classes = unique(classes);
	cumcounts = cumsum(counts);
	function c = idx2class(i)
		j = find(i <= cumcounts, 1);
        c = classes(j);
	end
	%leave-one-out cross validation
	TP = 0;
	for i = 1:num_files
		fprintf('%d ', i);
		actual_class = classes(i);
		q = desc{i}'; %query
		ds = desc;
		ds(i) = [];
		ds = cell2mat(ds)'; %remaining descriptors
		idx = knnsearch(ds, q);
		cl = arrayfun(@idx2class, idx);
		h = hist(cl, unique_classes);
		[ign, max_id] = max(h);
		assigned_class = unique_classes(max_id);
		%fprintf('Actual: %d Assigned: %d\n', actual_class, assigned_class);
		if actual_class == assigned_class
			TP = TP + 1;
		end
	end
	TP
	TP/num_files
end


