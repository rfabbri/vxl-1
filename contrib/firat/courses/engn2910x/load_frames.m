function movie_matrix = load_frames(img_movie_folder, file_extension)
	file_list = dir([img_movie_folder '/*.' file_extension]);
	for i = 1:length(file_list)
		frame_path = [img_movie_folder '/' file_list(i).name];
		frame_I = imread(frame_path);
		if i == 1
			movie_matrix = zeros(size(frame_I,1), size(frame_I,2), length(file_list), 'uint8');		
		end
		if ndims(frame_I) == 3 				
			frame_I = rgb2gray(frame_I);
		end
		movie_matrix(:,:,i) = frame_I;
	end	
end
