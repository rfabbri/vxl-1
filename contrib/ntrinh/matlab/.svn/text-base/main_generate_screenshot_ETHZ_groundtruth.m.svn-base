function main_generate_screenshot_ETHZ_groundtruth()
% This scripts draw groundtruth bounding boxes on top of the original
% images of the ETHZ dataset
% (c) Nhon Trinh
% Date: Nov 10, 2008

fprintf(1, 'Generating screenshots of ETHZ groundtruth\n');

orig_folder = 'D:\vision\data\ETHZ-shape\giraffes_jpg';
gt_folder = 'D:\vision\data\ETHZ-shape\giraffes-groundtruth-short-legs';
screenshot_folder = 'D:\vision\data\ETHZ-shape\screenshot_giraffes-groundtruth-short-legs';

fprintf(1, 'Original image folder = %s\n', orig_folder);
fprintf(1, 'Groundtruth bounding box folder = %s\n', gt_folder);
fprintf(1, 'Screenshot folder = %s\n', gt_folder);

filelist = ls(fullfile(orig_folder, '*.jpg'));
num_files = size(filelist, 1);


cur_pref = iptgetpref('ImshowBorder');
iptsetpref('ImshowBorder','tight');

for i = 1 : num_files
  % get name of the image and groundtruth files
  orig_filename = filelist(i, :);
  [pathstr, name, ext, versn] = fileparts(orig_filename);
  gt_filename = [name, '.groundtruth'];
  
  % load image
  figure(1);
  im = imread(fullfile(orig_folder, orig_filename));
  imshow(im);  
  gt_boxes = dlmread(fullfile(gt_folder, gt_filename), ' ');
  for i_box = 1:size(gt_boxes, 1)
    box = gt_boxes(i_box, :);
    x_min = box(1);
    y_min = box(2);
    w = box(3) - box(1);
    h = box(4) - box(2);
    rectangle('Position', [x_min, y_min, w, h], ...
      'EdgeColor', 'Red', ...
      'LineWidth', 2); 
  end;
  
  frame = getframe(1);
  screenshot_filename = [name, '.groundtruth.png'];
  
  imwrite(frame.cdata, fullfile(screenshot_folder, screenshot_filename));
  
end;

iptsetpref('ImshowBorder', cur_pref);

return;
