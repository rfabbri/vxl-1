function boundary = dbsksp_boundary(xgraph_file, image_file)
    addpath /home/firat/lemsvxl/src/contrib/firat/xgraph_to_binary
    try
        binary_mask = double(dbsksp_convert_xgraph_to_binary_mask(xgraph_file, image_file, 0));
    catch
        I = imread(image_file);
        boundary = zeros(size(I,1),size(I,2));
        return
    end
    strel1 = strel('disk',1);
    mask2 = imerode(binary_mask, strel1);
    boundary = binary_mask - mask2;
end 
