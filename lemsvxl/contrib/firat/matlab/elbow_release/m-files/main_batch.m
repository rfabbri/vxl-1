function main_batch(vtk_file, algo)	
	%%load dataset
	[pathname, objectname, ext] = fileparts(vtk_file);
	global V start_index end_index;
	[W, origin, spacing] = readVTK(vtk_file);
	V = double(W);
	Max = max(V(:));
	Min = min(V(:));
	V = 255 * (V - Min)/(Max-Min);
	start_index = 1
	end_index = size(V,3)	
	if strcmp(algo, 'cluster')
		edg = main_edge_detection_batch(V, objectname);
		main_adhoc_batch(edg, objectname, origin, spacing);
	elseif strcmp(algo, 'levelset')
		edg = main_edge_detection_batch(V, objectname);
		main_levelset_batch(V, edg, objectname, origin, spacing);
	else
		error('Unknown algorithm!');
	end	
end

