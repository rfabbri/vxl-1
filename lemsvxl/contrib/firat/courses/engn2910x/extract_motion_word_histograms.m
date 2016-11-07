function [histograms, labels] = extract_motion_word_histograms(data_set, movie_file_extension, frame_file_extension, actions, target_action, words)
	num_clusters = size(words, 1);
	histograms =  cell(length(actions),1);
	labels =  cell(length(actions),1);
	
	for i = 1:length(actions)
		samples = collect_motion_descriptors([data_set '/' actions{i}], movie_file_extension, frame_file_extension, indices, 0);
		histograms1 = zeros(length(samples), num_clusters);
		for j = 1:length(samples)
			idx = assign_cluster(samples{j}, words);
			h = hist(idx, 1:num_clusters);
			histograms1(j,:) = h / sum(h);
		end
		histograms{i} = histograms1;
		labels{i} = ones(length(samples), 1)*i;		
	end
	histograms = cell2mat(histograms);
	labels = cell2mat(labels);
end
