function [best_cm, best_rate, worst_cm, worst_rate, avg_cm, avg_rate] = multi_test_dataset(dataset_cell, action_ids, num_clusters, num_neighbors, n)
	best_rate = -Inf;
	worst_rate = Inf;
	total_cm = zeros(length(action_ids),length(action_ids));
	for i = 1:n
		cm = test_dataset(dataset_cell, action_ids, num_clusters, num_neighbors);
		r = get_rate(cm);
		if r > best_rate
			best_rate = r;
			best_cm = cm;
		end
		if r < worst_rate
			worst_rate = r;
			worst_cm = cm;
		end
		total_cm = total_cm + cm; 
	end
	avg_rate = get_rate(total_cm);
	avg_cm = total_cm / n;	
end

function r = get_rate(cm)
	r = trace(cm) / sum(cm(:));
end
