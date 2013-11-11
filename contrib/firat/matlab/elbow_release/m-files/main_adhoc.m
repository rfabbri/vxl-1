function main_adhoc(edg, objectname, origin, spacing)
	adhoc_params = load_parameters(fullfile('..','parameters','clustering_parameters.txt'));
	prompt = {'Edge strength threshold:'};
	dlg_title = 'Edge clustering parameters';
	num_lines = 1;
	def = {adhoc_params.edge_thresh};
	answer = inputdlg(prompt,dlg_title,num_lines,def);
	h = waitbar(0,'Please wait...');
	clusters = cluster_edges(edg, str2num(answer{1}));
	waitbar(1/3);
	largest = get_largest_cluster(clusters);
	waitbar(2/3);
	largest_file = fullfile('..','oriented_points', [objectname '_cluster_' datestr(now,30) '.xyz']);
	largest_coordinates = ((largest(:,1:3)-1).*repmat(spacing, size(largest, 1), 1))+repmat(origin, size(largest, 1), 1);	
	dlmwrite(largest_file, [largest_coordinates largest(:,4:6)], 'delimiter', ' ');
	close(h);
	msgbox(sprintf('Please run Poisson reconstruction using MeshLab\nInput file: %s', largest_file),'Action Required');
end
