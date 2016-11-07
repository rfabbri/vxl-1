function TP = dense_sift_performance(sift_folder, save_decision_histogram)
    warning off;
    files = dir([sift_folder '/*.mat']);
	num_files = length(files);
	classes = zeros(num_files, 1);
	desc = cell(1, num_files);
	counts = zeros(num_files, 1);
	for i = 1:num_files
		X = load([sift_folder '/' files(i).name]);
        desc{i} = double(X.dense_d);
        counts(i) = size(X.dense_d, 2);
		classes(i) = str2num(files(i).name(4:6));
	end
	unique_classes = unique(classes);
	num_unique_classes = length(unique_classes);
	
    function c = idx2class(t_i)
		t_j = find(t_i <= t_cumcounts, 1);
        c = t_classes(t_j);
    end
    
	%leave-one-out cross validation
	TP = 0;
	
	for i = 1:num_files
        fprintf('%d ', i);
		actual_class = classes(i);
		compute_h = true;
        X = load([sift_folder '/' files(i).name], 'dense_h');
        if isfield(X, 'dense_h') && length(X.dense_h) == num_unique_classes
            h = X.dense_h;
		else 
		    %%
		    q = desc{i}'; %query
		    %%
		    ds = desc;
			ds(i) = [];
		    ds = cell2mat(ds)'; %remaining descriptors
		    %%
		    t_classes = classes;
		    t_classes(i) = [];
		    %%
		    t_counts = counts;
		    t_counts(i) = [];
		    t_cumcounts = cumsum(t_counts);
		    %%
			idx = knnsearch(ds, q);
			cl = arrayfun(@idx2class, idx);
			h = hist(cl, unique_classes);
			if save_decision_histogram 
                dense_h = h;           
                save([sift_folder '/' files(i).name], 'dense_h', '-append');
    		end
		end
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


