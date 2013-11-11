% This is /lemsvxl/contrib/firat/matlab/select_gt_objects_from_dataset.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Jul 19, 2010

function select_gt_objects_from_dataset(datasetpath, img_ext, gt_folder, save_mask, save_bb)
    files = dir([datasetpath '/*.' img_ext]);
    for i = 1:length(files)
        close all;
        select_gt_object([datasetpath '/' files(i).name], gt_folder, save_mask, save_bb);
    end
end
