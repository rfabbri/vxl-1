function bb = dbsksp_get_bounding_box(xgraph_file, image_file)
    addpath /home/firat/lemsvxl/src/contrib/firat/xgraph_to_binary
    binary_mask = dbsksp_convert_xgraph_to_binary_mask(xgraph_file, image_file, 0);
    props1 = regionprops(double(binary_mask), 'BoundingBox');
    bb1 = props1.BoundingBox;
    bb = round([bb1(1) bb1(2) (bb1(1)+bb1(3)) (bb1(2)+bb1(4))]);
end
