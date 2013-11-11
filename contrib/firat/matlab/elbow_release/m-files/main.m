function main	
	%%load dataset
	[vtk_file, pathname] = uigetfile('*.vtk', 'Choose a VTK file to be segmented', fullfile('..','data'));
	[pathstr, objectname, ext] = fileparts(vtk_file);
	global V start_index end_index;
	[W, origin, spacing] = readVTK(fullfile(pathname, vtk_file));
	V = double(W);
	Max = max(V(:));
	Min = min(V(:));
	V = 255 * (V - Min)/(Max-Min);
	start_index = 1;
	end_index = size(V,3);
	slice_selector;
	start_index = double(start_index)
	end_index = double(end_index)
	edg = main_edge_detection(V, objectname);
	%%Choose the segmentation algorithm
	choice = questdlg('Choose one of the segmentation techniques', ...
	'', ...
	'Edge Clustering', '2D Level-set', 'Edge Clustering');
	if strcmp(choice, 'Edge Clustering')
		main_adhoc(edg, objectname, origin, spacing);
	else
		main_levelset(V, edg, objectname, origin, spacing);
	end
	
end

