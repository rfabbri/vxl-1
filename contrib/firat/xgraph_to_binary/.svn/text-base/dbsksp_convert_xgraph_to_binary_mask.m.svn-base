function binary_mask = dbsksp_convert_xgraph_to_binary_mask(xgraph_file, image_file, do_crop)
    try
        I = imread(image_file);
        [im_height, im_width, im_bands] = size(I); 
    catch
        im_height = 1000;
        im_width = 1000;
    end
    
    alphabet = 'abcdefghijk12345678';
    temp_name = [alphabet(unidrnd(length(alphabet), 1, 20)) '.png'];
    eval(sprintf('!/home/firat/home.lems/Codes/nn/dbsksp_convert_xgraph_to_binary_mask %s %s %d %d\n',xgraph_file, temp_name, im_width, im_height));
    binary_mask = imread(temp_name) > 0;
    if do_crop
        props1 = regionprops(double(binary_mask), 'BoundingBox');
        binary_mask = logical(padarray(imcrop(binary_mask, props1.BoundingBox), [5 5]));
    end
    eval(sprintf('!rm %s\n', temp_name));
end
