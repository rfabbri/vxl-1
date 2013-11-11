% This is /lemsvxl/contrib/firat/appearance/color/rgb2lab.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Jul 15, 2010
% Copyright (c) 2009 Peter Kovesi
% School of Computer Science & Software Engineering
% The University of Western Australia
% pk at csse uwa edu au
% http://www.csse.uwa.edu.au/

% PK May 2009

function Lab = rgb2lab(im)

    if ndims(im) ~= 3;
        error('Image must be a colour image');
    end
    
    cform = makecform('srgb2lab');
    if strcmp(class(im),'uint8')
        im = double(im)/255;
    end
    Lab = applycform(im, cform);
