function extract_amir_contours(len_thresh, dataset_path, prefix, ext, out_folder)
    files = dir([dataset_path '/' prefix '*' ext]);
    addpath ~/lemsvxl/src/contrib/firat/matlab
    for i = 1:length(files)
        filename = files(i).name;
        objname = get_objectname_from_filename(filename);
        cmd = sprintf('!/home/firat/lemsvxl/bin/contrib/firat/contour/detect_and_export_contours %s/%s %s/%s.txt %d', dataset_path, filename, out_folder, objname, len_thresh);
        fprintf([cmd '\n']);
        eval(cmd);
    end

end

