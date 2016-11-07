function draw_bb(orig_folder, img_ext, gt_folder, screenshot_folder)
% This scripts draw groundtruth bounding boxes on top of the original
% images of the ETHZ dataset
% (c) Nhon Trinh
% Modified by Firat Kalaycilar
% Date: Nov 10, 2008

fprintf(1, 'Generating screenshots of ETHZ groundtruth\n');

%orig_folder = '/vision/projects/kimia/shockshape/symseg/results/UIUC-cars/cars_ms';
%gt_folder = '/vision/projects/kimia/shockshape/symseg/results/UIUC-cars/cars_ms_gt';
%screenshot_folder = '/vision/projects/kimia/shockshape/symseg/results/UIUC-cars/cars_ms_screenshot';

fprintf(1, 'Original image folder = %s\n', orig_folder);
fprintf(1, 'Groundtruth bounding box folder = %s\n', gt_folder);
fprintf(1, 'Screenshot folder = %s\n', gt_folder);

filelist = dir(fullfile(orig_folder, ['*.' img_ext]));
num_files = size(filelist, 1);


cur_pref = iptgetpref('ImshowBorder');
iptsetpref('ImshowBorder','tight');

for i = 1 : num_files
  % get name of the image and groundtruth files
  orig_filename = filelist(i).name;
  [pathstr, name, ext, versn] = fileparts(orig_filename);
  gt_filename = [name, '.groundtruth'];
  
  % load image
  figure(1);
  fullfile(orig_folder, orig_filename)
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
