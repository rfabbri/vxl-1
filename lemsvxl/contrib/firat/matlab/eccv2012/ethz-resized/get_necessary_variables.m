% This is /lemsvxl/contrib/firat/matlab/eccv2012/ethz-resized/get_necessary_variables.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jan 20, 2012

function P = get_necessary_variables(data_prefix, warehouse, image_name)
    P.img_contours_file = [data_prefix '/db/ethz_contours_gpb_0.10_kovesi_20/' image_name '_gpb_contours.txt'];
    P.img_file = [data_prefix '/all_originals_resized/' image_name '.png'];
    P.model_folder = [data_prefix '/db/' warehouse];
    P.edgemap_bw_file = [data_prefix '/db/ethz_contours_gpb_0.10_kovesi_20/' image_name '_gpb_edgemap.png'];    
    P.gt_file = [data_prefix '/db/all_groundtruths_patched2_resized/' image_name '.groundtruth'];
end
