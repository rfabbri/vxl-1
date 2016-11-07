% This is critical_combined_sift_performance.m.

% Created on: Apr 15, 2013
%     Author: firat

function TP = critical_combined_sift_performance(sift_folder, percentage, score_type)
	files = dir([sift_folder '/*.mat']);
	num_files = length(files);
	classes = zeros(num_files, 1);
	%X = cell(num_files, 1);
	for i = 1:num_files
		%X{i} = load([sift_folder '/' files(i).name]);
		classes(i) = str2num(files(i).name(4:6));
	end
	unique_classes = unique(classes);	
	TP = 0;
	for i = 1:num_files
		fprintf('%d ', i);
		actual_class = classes(i);
		X = load([sift_folder '/' files(i).name]);
		if length(percentage) == 1
			min_hist_name = sprintf('min_h_%s_%d', score_type, uint8(100*percentage));
			max_hist_name = sprintf('max_h_%s_%d', score_type, uint8(100*percentage));
			saddle_hist_name = sprintf('saddle_h_%s_%d', score_type, uint8(100*percentage));
		elseif length(percentage) == 3
			min_hist_name = sprintf('min_h_%s_%d', score_type, uint8(100*percentage(1)));
			max_hist_name = sprintf('max_h_%s_%d', score_type, uint8(100*percentage(2)));
			saddle_hist_name = sprintf('saddle_h_%s_%d', score_type, uint8(100*percentage(3)));
		else
			error('Incompatible percentages');
		end
		h1 = getfield(X, min_hist_name);
		h2 = getfield(X, max_hist_name);
		h3 = getfield(X, saddle_hist_name);
		[ign, max_id] = max(h1/sum(h1) + h2/sum(h2) + h3/sum(h3));
		assigned_class = unique_classes(max_id);
		%fprintf('Actual: %d Assigned: %d\n', actual_class, assigned_class);
		if actual_class == assigned_class
			TP = TP + 1;
		end
	end
	TP
	TP/num_files
end

