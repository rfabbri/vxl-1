% This is /lemsvxl/contrib/firat/matlab/convert_dataset.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Jul 27, 2010

function convert_dataset(datasetpath, target_prefix, target_img_ext, ndig, outfolder, max_h, max_w)
    files = dir(datasetpath);
    files = files(3:end);
    k = 1;
    if ~exist(outfolder, 'dir')
        mkdir(outfolder);
    end
    for i = 1:length(files)
        try
            I = imread([datasetpath '/' files(i).name]);
        catch
            fprintf('%s is not a proper image file!\n', [datasetpath '/' files(i).name]);
            continue
        end
        rh = max_h / size(I,1); rw = max_w / size(I,2);
        rmin = min([rh rw]);
        if rmin < 1
            I = imresize(I, rmin, 'bicubic');
        end
        outfile = [outfolder '/' target_prefix getIndex(ndig, k) '.' target_img_ext];
        imwrite(I, outfile);
        
        k = k + 1;
    end
end

function s = getIndex(ndig, i)
        numd =  floor(log10(i)) + 1;
        numz = ndig - numd;
        s = '';
        for k = 1:numz
                s = [s '0'];
        end
        s = [s num2str(i)];
end
