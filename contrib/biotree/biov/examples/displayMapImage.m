% Author: Jason Mallios
% Purpose: Displays a colormapped image pair and their sum.
% IMPORTANT: The directory must consist of image pairs.

function rgbIM = displayMapImage(greenIM, greyIM)

% READ 16-BIT INDEXED TIFs
[grnIM grnMAP] = imread(greenIM);
[gryIM gryMAP] = imread(greyIM);
% CONVERT TO RGB AND COMBINE
rgb_grnIM = ind2rgb(grnIM, grnMAP);
rgb_gryIM = ind2rgb(gryIM, gryMAP);
rgbIM = rgb_grnIM + rgb_gryIM;
% NORMALIZE
rgb_grnIM(:,:,2) = rgb_grnIM(:,:,2)/max(max(rgb_grnIM(:,:,2)));
for i=1:3
    rgb_gryIM(:,:,i) = rgb_gryIM(:,:,i)/max(max(rgb_gryIM(:,:,i)));
    rgbIM(:,:,i) = rgbIM(:,:,i)/max(max(rgbIM(:,:,i)));
end
% DISPLAY
subplot(3,2,1), imagesc(rgb_grnIM), title(greenIM);
subplot(3,2,2), imagesc(rgb_gryIM), title(greyIM);
subplot(3,2,[3 4 5 6]), imagesc(rgbIM), title('COMBINED');

end