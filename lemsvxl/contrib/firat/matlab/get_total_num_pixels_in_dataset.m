function n = get_total_num_pixels_in_dataset(ds_path, img_ext);
    files = dir([ds_path '/*.' img_ext]);
    n = 0;
    for i = 1:length(files)
        I = imread([ds_path '/' files(i).name]);
        A = size(I);
        n = n + A(1)*A(2);
    end
end
