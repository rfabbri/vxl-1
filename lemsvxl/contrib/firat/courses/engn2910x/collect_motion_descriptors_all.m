function [actions, dataset_cell] = collect_motion_descriptors_all(dataset, movie_file_extension, frame_file_extension)
	action_folders = dir(dataset);
	action_folders = action_folders(3:end);
	num_actions = length(action_folders);
	actions = cell(num_actions, 1);
	dataset_cell = cell(num_actions, 1);
	for i = 1:num_actions
		actions{i} = action_folders(i).name;
		action_folder = [dataset '/' actions{i}];
		movie_files = dir([action_folder '/*.' movie_file_extension]);
		num_movies = length(movie_files);
		dataset_cell{i} = cell(num_movies, 1);
		for j = 1:num_movies
			fprintf('Collecting %s...\n',movie_files(j).name)
			movie_matrix = load_frames([action_folder '/frames/' movie_files(j).name], frame_file_extension);
			dataset_cell{i}{j} = get_motion_descriptors(movie_matrix);
		end
	end
end
