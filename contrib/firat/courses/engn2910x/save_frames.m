function save_frames(movie_matrix, output_directory, file_extension)
	frame_count = size(movie_matrix, 3);
	for i = 1:frame_count
		imwrite(movie_matrix(:,:,i), sprintf('%s/frame%010d.%s',output_directory, i, file_extension));		
	end
end
