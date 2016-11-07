function [target_action_id, training_features, test_feature, labels, words] = get_leave_one_out_features(dataset_cell, action_id, movie_id, num_clusters)
	test_movie = dataset_cell{action_id}{movie_id};
	dataset_cell{action_id}(movie_id) = [];
	target_action_id = action_id;
	num_actions = length(dataset_cell);
	dataset_flat = cell(num_actions, 1);
	for i = 1:num_actions
		movies = dataset_cell{i};
		dataset_flat{i} = cell2mat(movies);
	end
	dataset_flat = cell2mat(dataset_flat);
	[idx, words] = mykmeans(dataset_flat, num_clusters);
	fprintf('Motion words extracted!\n')
	training_features = cell(num_actions, 1);
	labels = cell(num_actions, 1);
	for i = 1:num_actions
		movies = dataset_cell{i};
		num_movies = length(movies);
		histograms = zeros(num_movies, num_clusters);
		for j = 1:num_movies
			num_frames = size(movies{j}, 1);
			cluster_assignments = idx(1:num_frames);
			h = hist(cluster_assignments,1:num_clusters);
			histograms(j,:) = h / sum(h);
			idx(1:num_frames) = [];
		end
		training_features{i} = histograms;
		labels{i} = ones(num_movies, 1) * i;
	end
	training_features = cell2mat(training_features);
	labels = cell2mat(labels);
	if length(idx) > 0
		error('There is something wrong!')
	end
	idx2 = assign_cluster(test_movie, words);
	h = hist(idx2,1:num_clusters);
	test_feature = h / sum(h);
end

function [idx, words] = mykmeans(dataset_flat, num_clusters)
	try
		[idx, words] = kmeans(dataset_flat, num_clusters);
	catch
		[idx, words] = mykmeans(dataset_flat, num_clusters);
	end
end
