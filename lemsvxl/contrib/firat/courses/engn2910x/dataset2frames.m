function dataset2frames(dataset_path, movie_file_extension, frame_file_extension)
	folders = dir(dataset_path);
	folders = folders(3:end);
	for i = 1:length(folders)
		sub_dir = [dataset_path '/' folders(i).name];
		fprintf('Processing videos in %s\n', sub_dir);
		frame_dir = [sub_dir '/frames'];
		if isdir(sub_dir)
			movies = dir([sub_dir '/*.' movie_file_extension]);
			if exist(frame_dir) ~= 7
				mkdir(frame_dir);
			end
			for i = 1:length(movies)
				movie_path = [sub_dir '/' movies(i).name];
				fprintf('Processing video %s\n', movie_path);
				frame_path = [frame_dir '/' movies(i).name];
				if exist(frame_path) ~= 7
					mkdir(frame_path);
				end
				movie2frames(movie_path, frame_path, frame_file_extension);
			end 
						
		end
	end
end
