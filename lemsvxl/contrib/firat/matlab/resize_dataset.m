% This is /lemsvxl/contrib/firat/matlab/resize_dataset.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 18, 2011

function resize_dataset(datasetpath, outfolder, out_img_ext, max_h, max_w)
    files = dir(datasetpath);
    files = files(3:end);
    if ~exist(outfolder, 'dir')
        mkdir(outfolder);
    end
    for i = 1:length(files)
        try
            I = imread([datasetpath '/' files(i).name]);
            image_name = get_objectname_from_filename(files(i).name);
        catch
            fprintf('%s is not a proper image file!\n', [datasetpath '/' files(i).name]);
            continue
        end
        rh = max_h / size(I,1); rw = max_w / size(I,2);
        rmin = min([rh rw]);
        if rmin < 1
            I = imresize(I, rmin, 'bicubic');
        end
        outfile = [outfolder '/' image_name '.' out_img_ext];
        imwrite(I, outfile);       
    end
end


