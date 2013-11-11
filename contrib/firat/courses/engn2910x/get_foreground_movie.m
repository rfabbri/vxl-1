function get_foreground_movie(movie_matrix, output_movie_file)
	[height, width, frame_count] = size(movie_matrix);
	pixel_count = width*height;
	data_matrix = reshape(movie_matrix, pixel_count, frame_count);
	dens_matrix = zeros(size(data_matrix));
	for i = 1:size(data_matrix, 1)
		temp_hist = hist(data_matrix(i,:), 0:12.75:255);
		for j = 1:frame_count
			hist_index = floor(data_matrix(i,j)/12.75)+1;
			dens_matrix(i,j) = temp_hist(hist_index);
		end		
	end
	largest_conn_comp_movie_matrix = zeros(height, width, frame_count);
	motion_descriptors = zeros(frame_count - 1, 4);
	for i = 2:frame_count
		current_frame = vec2mat(dens_matrix(:,i), height)';
		binary_frame = (1 - (current_frame / frame_count))>=.85;
		binary_frame = medfilt2(binary_frame, [7 7]);
		props = regionprops(double(binary_frame), 'BoundindBox');
		largest_conn_comp_movie_matrix(:,:,i) = binary_frame;
		
		if i ~= frame_count
			
		end				
	end
	output_movie_matrix = zeros(height, width, frame_count);
	for i = 1:frame_count
		
		output_movie_matrix(:,:,i) = uint8(255*(largest_conn_comp_movie_matrix(:,:,i)));
	end
	save_frames(output_movie_matrix, '/home/firat/Code/action_recognition/tmp', 'png');
	frames2movie('/home/firat/Code/action_recognition/tmp', 'png', output_movie_file);
	eval('!rm /home/firat/Code/action_recognition/tmp/*');
	
end
