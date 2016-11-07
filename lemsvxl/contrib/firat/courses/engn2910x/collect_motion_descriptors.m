function samples = collect_motion_descriptors(training_set, movie_file_extension, frame_file_extension, train_indices, return_matrix)
	num_train = length(train_indices);
	files = dir([training_set '/*.' movie_file_extension]);
	samples = cell(num_train, 1);
	for i = train_indices
		try
			fprintf('Collecting %s...\n',files(i).name)
		catch
			break
		end
		movie_matrix = load_frames([training_set '/frames/' files(i).name], frame_file_extension);
		samples{i} = get_motion_descriptors(movie_matrix);
	end
	if return_matrix
		samples = cell2mat(samples);
	end
end
