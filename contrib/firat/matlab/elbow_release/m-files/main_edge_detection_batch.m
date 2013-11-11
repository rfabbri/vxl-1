function edg = main_edge_detection_batch(V, objectname)
	global start_index end_index;	
	%%load edge parameters
	edge_params = load_parameters(fullfile('..','parameters','edge_parameters.txt'));
	[x,y,z,nx,ny,nz,str] = third_order_3d_NMS_edge_detector_h(V(:,:,start_index:end_index), str2num(edge_params.sigma), str2num(edge_params.edge_thresh), str2num(edge_params.hx), str2num(edge_params.hy), str2num(edge_params.hz));
	edg = [x,y,z-1+start_index,nx,ny,nz,str];
	dlmwrite(fullfile('..','edges',[objectname '_' datestr(now,30) '.edg']), edg, 'delimiter', ' ');	
end
