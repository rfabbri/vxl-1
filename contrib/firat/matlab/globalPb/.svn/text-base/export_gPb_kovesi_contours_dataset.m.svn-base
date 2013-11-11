function export_gPb_kovesi_contours_dataset(gPb_folder, pb_thresh, len_thresh, out_folder)
    addpath ~/lemsvxl/src/contrib/firat/matlab
    if ~exist(out_folder, 'dir')
        mkdir(out_folder);
    end
    files = dir([gPb_folder '/*.mat']);
    for i = 1:length(files)
        image_name = get_objectname_from_filename(files(i).name);
        [edgmap, edgorient, contour_matrix] = export_gPb_kovesi_contours([gPb_folder '/' files(i).name], pb_thresh, len_thresh);        
        dlmwrite([out_folder '/' image_name '_contours.txt'], contour_matrix, 'delimiter', ' ');
        imwrite(uint8(edgmap * 255), [out_folder '/' image_name '_edgemap.png']);
        dlmwrite([out_folder '/' image_name '_edgeorient.txt'], edgorient, 'delimiter', ' ');
    end
end
