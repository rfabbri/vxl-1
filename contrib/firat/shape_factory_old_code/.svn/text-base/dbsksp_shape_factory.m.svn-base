function tree = dbsksp_shape_factory(xgraph_folder, xgraph_file_list) 
    addpath ~/lemsvxl/src/contrib/ntrinh/matlab/xml_io_tools
    addpath ~/lemsvxl/src/contrib/firat/xgraph_to_binary
    
    filenames = textread(xgraph_file_list, '%s');
    
    if length(filenames) < 2
        error('At least 2 xgraph files are required!')
    end
    
    for i = 1:length(filenames)
        filename = [xgraph_folder '/' filenames{i}];
        if ~exist(filename, 'file')
            error(sprintf('File does not exist:%s\n', filename));
        end
    end
    
    [min_tree, treename] = xml_read([xgraph_folder '/' filenames{1}]);
    max_tree = min_tree;
    orig_tree = min_tree;
    
        
    for i = 2:length(filenames)
        filename = [xgraph_folder '/' filenames{i}];
        [tree, treename] = xml_read(filename);         
        for j = 1:length(tree.shock_node_list.shock_node)
            
        end
        return
    end

end
