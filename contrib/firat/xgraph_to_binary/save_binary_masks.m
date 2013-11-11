function save_binary_masks(xfolder, outfolder)
    files = dir([xfolder '/*.xml']);
    
    for i = 1:length(files)
        binary_mask = 255 * dbsksp_convert_xgraph_to_binary_mask([xfolder '/' files(i).name], '', 1);
        imwrite(binary_mask, [outfolder '/' strrep(files(i).name, '.xml', '.png')]);
    end
end
