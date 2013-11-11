% This is /lemsvxl/contrib/firat/appearance/dense_sift_descriptor.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Jul 9, 2010

function desc = dense_sift_descriptor(imageFile, grad_thresh, spatial_scales, grid_spacing)
    addpath ~/lemsvxl/src/contrib/firat/appearance/dense_sift
    %default parameters
    %grad_thresh = 0.1;
    %spatial_scales = [1 .8 .8^2 .8^3 .8^4];
    %grid_spacing = 3;
    %%
    desc = cell(length(spatial_scales), 1);
    for s = 1:length(spatial_scales)
        desc{s} = compute_dense_sift_wrapper(imageFile, grad_thresh, spatial_scales(s), grid_spacing);          
    end
    desc = cell2mat(desc);
end
