function words = generate_motion_words(training_set, movie_file_extension, frame_file_extension, actions, train_indices, num_clusters)
	tset = cell(length(actions),1);
	for i = 1:length(actions)
		tset{i} = collect_motion_descriptors([training_set '/' actions{i}], movie_file_extension, frame_file_extension, train_indices, 1);
				
	end
	tset = cell2mat(tset);
	[idx, words] = kmeans(tset, num_clusters);
end
