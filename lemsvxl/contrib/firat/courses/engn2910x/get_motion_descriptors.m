function motion_descriptors = get_motion_descriptors(movie_matrix)
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
	
	motion_descriptors = zeros(frame_count - 1, 4);
	
	bbref = frame_motion_descriptor(dens_matrix(:,1), height, frame_count, 0.85);	
	for i = 2:frame_count
		bb = frame_motion_descriptor(dens_matrix(:,i), height, frame_count, 0.85);				
		motion_descriptors(i-1,:) = motion_descriptor(bb, bbref);
		bbref = bb;							
	end	
end

function bb = frame_motion_descriptor(frame_data, height, frame_count, prob_threshold)
	current_frame = vec2mat(frame_data, height)';
	binary_frame = (1 - (current_frame / frame_count))>=prob_threshold;
	binary_frame = medfilt2(binary_frame, [7 7]);
	props = regionprops(double(binary_frame), 'BoundingBox');
	try
		tempbb = ceil(props.BoundingBox);
	catch
		tempbb = [0 0 0 0];
	end
	bb = [tempbb(1) (tempbb(2)+tempbb(4)-1) (tempbb(1)+tempbb(3)-1) tempbb(2)];
end
