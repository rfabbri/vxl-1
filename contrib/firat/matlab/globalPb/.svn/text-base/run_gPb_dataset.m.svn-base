function run_gPb_dataset(datasetpath, output_folder)
    addpath ~/Downloads/BSR/grouping/lib
    files = dir(datasetpath);
    files = files(3:end);
    if ~exist(output_folder, 'dir')
        mkdir(output_folder);
    end
    for i = 1:length(files)
        image_name = get_objectname_from_filename(files(i).name);
        try
            globalPb([datasetpath '/' files(i).name], [output_folder '/' image_name '_gpb.mat']);
        catch
            disp('Skipping due to an error!')
        end
    end
end
