filename = 'kermit000.jpg';
% filename = '0001.jpg';

img = single(rgb2gray(imread(filename)));


step = 5;
binSize = 4;
flat_window = false;

[f, d] = vl_dsift(img,'size',binSize,'step',step,'floatDescriptors');

f = f';
d = d';

dlmwrite('d.txt', d,'delimiter','\t','precision',16);
dlmwrite('f.txt', f,'delimiter','\t','precision',16);
