%examples:
%tag_feature_performance('/home/firat/Desktop/data/tag_sift/saddle_extremum', 'saddle-min', 'persistence', 0.2, 1:45, true)
%tag_feature_performance('/home/firat/Desktop/data/tag_sift/saddle_extremum', 'saddle-max', 'curvature', 0.2, 1:45, true)
%tag_feature_performance('/home/firat/Desktop/data/tag_sift/saddle_extremum', 'saddle-extremum', 'curvature', 0.2, 1:45, true)

function TP = tag_feature_performance(sift_folder, feature_type, score_type, percentage, interval, save_decision_histogram)
    warning off
    sad_min_prefix = 'sad_min';
    sad_max_prefix = 'sad_max';
    if strcmp(feature_type, 'saddle-min')
    	prefix = sad_min_prefix;   	   	
    elseif strcmp(feature_type, 'saddle-max')
    	prefix = sad_max_prefix;     	   	   	
    elseif strcmp(feature_type, 'saddle-extremum') 
    	prefix = 'ignore';  
    else
    	error('Not implemented');
    end 
    
    if strcmp(score_type, 'persistence')
    	score_string = [prefix  '_pers_s'];    	
    	hist_name = sprintf('%s_pers_h_%d', prefix, uint8(100*percentage(1)));    	
    	hist_name1 = sprintf('%s_pers_h_%d', sad_min_prefix, uint8(100*percentage(1)));
    	if length(percentage) >= 2
    		hist_name2 = sprintf('%s_pers_h_%d', sad_max_prefix, uint8(100*percentage(2)));
    	else
    		hist_name2 = sprintf('%s_pers_h_%d', sad_max_prefix, uint8(100*percentage(1)));
    	end
    elseif strcmp(score_type, 'curvature')
    	score_string = [prefix  '_minc_s'];    	
    	hist_name = sprintf('%s_minc_h_%d', prefix, uint8(100*percentage(1)));
    	hist_name1 = sprintf('%s_minc_h_%d', sad_min_prefix, uint8(100*percentage(1)));
    	if length(percentage) == 2
    		hist_name2 = sprintf('%s_minc_h_%d', sad_max_prefix, uint8(100*percentage(2)));
    	else
    		hist_name2 = sprintf('%s_minc_h_%d', sad_max_prefix, uint8(100*percentage(1)));
    	end
    else
    	error('Not implemented');
    end  
    
    descriptor_string = [prefix '_d']; 
      
    files = dir([sift_folder '/*.mat']);
	num_files = length(files);
	classes = zeros(num_files, 1);
	desc = cell(1, num_files);
	counts = zeros(num_files, 1);
	for i = 1:num_files
		if ~strcmp(feature_type, 'saddle-extremum')			
			X = load([sift_folder '/' files(i).name]);
		    all_desc = double(getfield(X, descriptor_string));
			num_desc = size(all_desc, 2);
		    num_to_keep = round(num_desc * percentage(1));	
		    [ign, sort_index] = sort(getfield(X, score_string), 'descend');			
			desc{i} = all_desc(:, sort_index(1:num_to_keep));
			counts(i) = num_to_keep;
		end
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
    
    for i = interval
        fprintf('%d ', i);
        actual_class = classes(i);
        compute_h = true;        
        if strcmp(feature_type, 'saddle-extremum')
        	X = load([sift_folder '/' files(i).name], hist_name1, hist_name2);
        else
        	X = load([sift_folder '/' files(i).name], hist_name);
        end
        if ~strcmp(feature_type, 'saddle-extremum') && isfield(X, hist_name) && length(getfield(X, hist_name)) == num_unique_classes
            h = getfield(X, hist_name);
        elseif strcmp(feature_type, 'saddle-extremum') && isfield(X, hist_name1) && length(getfield(X, hist_name1)) == num_unique_classes && isfield(X, hist_name2) && length(getfield(X, hist_name2)) == num_unique_classes  
        	h1 = getfield(X, hist_name1);
        	h2 = getfield(X, hist_name2); 
        elseif strcmp(feature_type, 'saddle-extremum')
        	error('First run the code with saddle-min and saddle-max.');        
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
        
        if strcmp(feature_type, 'saddle-extremum')       
			[ign, max_id] = max(h1/sum(h1) + h2/sum(h2));
		else
			[ign, max_id] = max(h);
		end
		assigned_class = unique_classes(max_id);
		%fprintf('Actual: %d Assigned: %d\n', actual_class, assigned_class);
		if actual_class == assigned_class
			TP = TP + 1;
		end
	end
	TP
	TP/num_files
end


