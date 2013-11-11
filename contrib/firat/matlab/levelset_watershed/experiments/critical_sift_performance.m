function TP = critical_sift_performance(sift_folder, cp_type, percentage, score_type, save_decision_histogram)
    score_min_string = sprintf('%s_s_min', cp_type);
    score_gauss_string = sprintf('%s_s_gauss', cp_type);
    score_mean_string = sprintf('%s_s_mean', cp_type);    
    descriptor_string = sprintf('%s_d', cp_type);
      
    files = dir([sift_folder '/*.mat']);
	num_files = length(files);
	classes = zeros(num_files, 1);
	desc = cell(1, num_files);
	counts = zeros(num_files, 1);
	for i = 1:num_files
		X = load([sift_folder '/' files(i).name]);
        all_desc = double(getfield(X, descriptor_string));
		num_desc = size(all_desc, 2);
        num_to_keep = round(num_desc * percentage);
		switch score_type
			case 'min'
                [ign, sort_index] = sort(getfield(X, score_min_string), 'descend');
			case 'gauss'
                [ign, sort_index] = sort(getfield(X, score_gauss_string), 'descend');
			case 'mean'
                [ign, sort_index] = sort(getfield(X, score_mean_string), 'descend');
			otherwise
				error('Unknown score type!');
		end		
		desc{i} = all_desc(:, sort_index(1:num_to_keep));
		counts(i) = num_to_keep;
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
    hist_name = sprintf('%s_h_%s_%d', cp_type, score_type, uint8(100*percentage));
    for i = 1:num_files
        fprintf('%d ', i);
        actual_class = classes(i);
        compute_h = true;        
        X = load([sift_folder '/' files(i).name], hist_name);
        if isfield(X, hist_name) && length(getfield(X, hist_name)) == num_unique_classes
            h = getfield(X, hist_name);            
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
			    TempStruct = struct;
			    eval(sprintf('TempStruct.%s = h;', hist_name));
			    save([sift_folder '/' files(i).name], '-struct', 'TempStruct', hist_name, '-append');
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


