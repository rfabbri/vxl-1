function confusion_matrix = test_dataset(dataset_cell, action_ids, num_clusters, num_neighbors)
	dataset_cell = dataset_cell(action_ids);
	num_actions = length(dataset_cell);
	confusion_matrix = zeros(num_actions,num_actions);
	for i = 1:num_actions
		num_movies = length(dataset_cell{i});
		for j = 1:num_movies
			[target_action_id, training_features, test_feature, labels, words] = get_leave_one_out_features(dataset_cell, i, j, num_clusters);
			k = knnclassify(test_feature, training_features, labels, num_neighbors(i));
			confusion_matrix(i,k) = confusion_matrix(i,k) + 1;
			if i == k
				fprintf('Correct match!\n');
			else
				fprintf('Wrong match!\n');
			end
		end
	end
end
