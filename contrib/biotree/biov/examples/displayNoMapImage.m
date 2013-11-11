% Author: Jason Mallios
% Purpose: Displays a non-colormapped image.

function rgbIM = displayNoMapImage(greenIM)

% READ 16-BIT INDEXED TIFs
grnIM = imread(greenIM);
% DISPLAY
imagesc(grnIM), title(greenIM);

end