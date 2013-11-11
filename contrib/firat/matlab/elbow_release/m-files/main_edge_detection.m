function edg = main_edge_detection(V, objectname)
	global start_index end_index;
	choice = questdlg('Select', ...
	'', ...
	'Detect new edges', 'Load existing edges', 'Detect new edges');
	if strcmp(choice, 'Detect new edges')
		%%load edge parameters
		edge_params = load_parameters(fullfile('..','parameters','edge_parameters.txt'));
		prompt = {'Sigma:','Edge strength threshold:', 'hx:', 'hy:', 'hz:'};
		dlg_title = 'Edge detection parameters';
		num_lines = 1;
		def = {edge_params.sigma, edge_params.edge_thresh, edge_params.hx, edge_params.hy, edge_params.hz};
		answer = inputdlg(prompt,dlg_title,num_lines,def);
		h = waitbar(0,'Please wait...');
		[x,y,z,nx,ny,nz,str] = third_order_3d_NMS_edge_detector_h(V(:,:,start_index:end_index), str2num(answer{1}), str2num(answer{2}), str2num(answer{3}), str2num(answer{4}), str2num(answer{5}));
		close(h);
		edg = [x,y,z-1+start_index,nx,ny,nz,str];
		dlmwrite(fullfile('..','edges',[objectname '_' datestr(now,30) '.edg']), edg, 'delimiter', ' ');		
	else
		[edge_file, pathname] = uigetfile('*.edg', 'Choose an edge file', fullfile('..','edges'));
		h = waitbar(0,'Please wait...');
		edg = load(fullfile(pathname, edge_file));
		close(h);
	end
end
