% This script extract the object regions in the groundtruth data of ETHZ
% dataset
% (c) Nhon Trinh
% Date: Oct 15, 2008

%% input data
image_folder = 'V:\projects\kimia\shockshape\symseg\data\ethz-manual-trace\bottles';
image_filename = 'heineken.mask.0.png';


%% Process

image_file = fullfile(image_folder, image_filename);
im = imread(image_file) * 255;
imshow(im);