% Author: Jason Mallios
% Purpose: Converts a 16-bit indexed tiff image into a non-colormapped 
%          8-bit tiff image.
% IMPORTANT: The image being converted must not have been converted
%            already.

function nomapIM = mapToNoMap(mapIM)

% READ 16-BIT INDEXED (MAPPED) TIFs
[im map] = imread(mapIM);
% CONVERT TO RGB (NOT MAPPED)
if (sum(size(map))>0);
    nomapIM = ind2rgb(im, map);
else
    nomapIM = im;
end

end