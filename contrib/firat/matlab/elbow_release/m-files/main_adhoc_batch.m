function main_adhoc_batch(edg, objectname, origin, spacing)
	adhoc_params = load_parameters(fullfile('..','parameters','clustering_parameters.txt'));
	clusters = cluster_edges(edg, str2num(adhoc_params.edge_thresh));
	largest = get_largest_cluster(clusters);	
	largest_file = fullfile('..','oriented_points', [objectname '_cluster_' datestr(now,30) '.xyz']);
	largest_coordinates = ((largest(:,1:3)-1).*repmat(spacing, size(largest, 1), 1))+repmat(origin, size(largest, 1), 1);	
	dlmwrite(largest_file, [largest_coordinates largest(:,4:6)], 'delimiter', ' ');	
end
