function idx = assign_cluster(X, C)
	num_classes = size(C,1);
	num_samples = size(X,1);
	idx = zeros(num_samples, 1);
	data_matrix = repmat(X, num_classes, 1);
	centroid_matrix = zeros(num_classes * num_samples, size(C, 2));
	for i = 1:num_classes
		centroid_matrix(((i-1)*num_samples+1):(i*num_samples), :) = repmat(C(i,:), num_samples, 1);
	end
	diff_matrix = data_matrix - centroid_matrix;
	sq_diff_matrix = diff_matrix .^ 2;
	dist_matrix = sum(sq_diff_matrix, 2);
	for i = 1:num_samples
		distances = dist_matrix(i:num_samples:((num_classes-1) * num_samples + i));
		[min_val, idx(i)] = min(distances);
	end
end
