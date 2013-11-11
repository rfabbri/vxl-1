% This is /lemsvxl/contrib/firat/appearance/lab_descriptor.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Jul 15, 2010

function desc = lab_descriptor(im, nrows, ncols)
        addpath ~/lemsvxl/src/contrib/firat/appearance/color
        if ~isnumeric(im)
            im = imread(im);
        end
        im = rgb2lab(im);
        desc = rgb_descriptor(im, nrows, ncols);
        
end

