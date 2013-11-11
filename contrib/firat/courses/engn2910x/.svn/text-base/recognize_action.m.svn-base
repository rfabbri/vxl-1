function ret = recognize_action(dataset_cell, action_id, movie_id, num_clusters, num_neighbors)
	[target_action_id, training_features, test_feature, labels, words] = get_leave_one_out_features(dataset_cell, action_id, movie_id, num_clusters);
	ret = knnclassify(test_feature, training_features, labels, num_neighbors);	
end
