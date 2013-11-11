% This is /lemsvxl/contrib/firat/matlab/resize_dataset_and_gt_bbs.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 20, 2011

function resize_dataset_and_gt_bbs(datasetpath, gt_path, outfolder, gt_outfolder, out_img_ext, max_h, max_w)
    files = dir(datasetpath);
    files = files(3:end);
    if ~exist(outfolder, 'dir')
        mkdir(outfolder);
    end
    if ~exist(gt_outfolder, 'dir')
        mkdir(gt_outfolder);
    end
    for i = 1:length(files)
        try
            I = imread([datasetpath '/' files(i).name]);            
            image_name = get_objectname_from_filename(files(i).name);
            gt_bbs = load([gt_path '/' image_name '.groundtruth']);
        catch
            fprintf('%s is not a proper image file!\n', [datasetpath '/' files(i).name]);
            continue
        end
        rh = max_h / size(I,1); rw = max_w / size(I,2);
        rmin = min([rh rw]);
        if rmin < 1
            I = imresize(I, rmin, 'bicubic');
            gt_bbs = gt_bbs * rmin;
        end
        outfile = [outfolder '/' image_name '.' out_img_ext];
        imwrite(I, outfile);
        gt_outfile = [gt_outfolder '/' image_name '.groundtruth'];
        dlmwrite(gt_outfile, gt_bbs, 'delimiter' ,' ');        
    end
end


