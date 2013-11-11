function tag_feature_performance_varying_percentage(sift_folder, feature_type, score_type, percentages, interval)
	if strcmp(feature_type, 'saddle-extremum')
		TP = zeros(length(percentages),length(percentages));
		for pmin = 1:length(percentages)
			for pmax = 1:length(percentages)
				TP(pmin, pmax) = tag_feature_performance(sift_folder, feature_type, score_type, [percentages(pmin) percentages(pmax)], interval, true)/225;	
			end
		end
		max_TP = max(TP(:))
		[max_r, max_c] = find(TP == max_TP);
		best_min_p = percentages(max_r)
		best_max_p = percentages(max_c)
		figure; surf(percentages, percentages, TP);
		xlabel('Percentage of saddle-max')
		ylabel('Percentage of saddle-min')
		zlabel('TP')
		axis([0 1 0 1 0 1])
	else
		TP = zeros(size(percentages));
		for p = 1:length(percentages)
			TP(p) = tag_feature_performance(sift_folder, feature_type, score_type, percentages(p), interval, true);		
		end
		if true
			figure;plot(percentages, TP/225);
			xlabel('Percentage');
			ylabel('TP');
			axis([0 1 0 1])
		end	
	end
	if 0 % example
		cd levelset_watershed
		cd experiments
		tag_feature_performance_varying_percentage('/home/firat/Desktop/data/tag_sift/saddle_extremum2', 'saddle-min', 'curvature', 1:-0.1:0.1, 1:75);
		tag_feature_performance_varying_percentage('/home/firat/Desktop/data/tag_sift/saddle_extremum2', 'saddle-max', 'curvature', 1:-0.1:0.1, 1:75);
		
		cd levelset_watershed
		cd experiments
		tag_feature_performance_varying_percentage('/home/firat/Desktop/data/tag_sift/saddle_extremum2', 'saddle-max', 'persistence', 1:-0.1:0.1, 1:75);
	end
end
