% alpha: importance factor
% saddle_centric_performance('/home/firat/Desktop/data/tag_sift/saddle_centric', 0.3, 0.5, false, 1:5)
function [TP, correctly_classified] = saddle_centric_performance(sift_folder, percentage, alpha, save_decision_histogram, interval)
    warning off
    beta1 = (1-alpha)/4;
    function D2 = distfun(ZI, ZJ)
		N = size(ZJ,1);
		D2 = zeros(N, 1);
		sad1  = ZI(1:128);
		min11 = ZI(129:256);
		min12 = ZI(257:384);
		max11 = ZI(385:512);
		max12 = ZI(513:640);	
		for ii = 1:N
			sad2  = ZJ(ii, 1:128);
			min21 = ZJ(ii, 129:256);
			min22 = ZJ(ii, 257:384);
			max21 = ZJ(ii, 385:512);
			max22 = ZJ(ii, 513:640);
			c0 = norm(sad1-sad2);
			c1 = norm(min11-min21);
			c2 = norm(min12-min22);
			c3 = norm(min11-min22);
			c4 = norm(min12-min21);
			c5 = norm(max11-max21);
			c6 = norm(max12-max22);
			c7 = norm(max11-max22);
			c8 = norm(max12-max21);
			D2(ii) = alpha*c0 + beta1*min(c1+c2, c3+c4) + beta1*min(c5+c6, c7+c8);
		end
	end
    
    score_string = 'sad_centric_minc_s';
    descriptor_string = 'sad_centric_d';
      
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
        [ign, sort_index] = sort(getfield(X, score_string), 'descend');			
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
    correctly_classified = false(num_files, 1);
    hist_name = sprintf('sad_centric_h_%d_%d', uint8(100*alpha), uint8(100*percentage));
    for i = interval
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
			idx = knnsearch(ds, q, 'Distance', @distfun);				
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
			correctly_classified(i) = true;
			TP = TP + 1;
		end
	end
	TP
	TP/num_files
end




