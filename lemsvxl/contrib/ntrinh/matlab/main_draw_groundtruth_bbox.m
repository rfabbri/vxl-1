% This script allows user to interactively draw rectangle boxes on the
% images and save the rectangles to a file. The goal is to draw groundtruth
% bounding boxes on image data sets
% (c) Nhon Trinh
% Date: Feb 2, 2009

close all;
clear all;

%% input
orig_image_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\all_originals';
image_extension = '.jpg';
output_groundtruth_folder = 'D:\vision\data\ETHZ-shape\giraffe-groundtruth-short-legs';
groundtruth_extension = '.groundtruth';

%% draw rectangle boxes

filename_list = dir(fullfile(orig_image_folder, ['*', image_extension]));

% iterate thru the files and draw the bbox, if desired

h = figure(1);
for m = 1 : length(filename_list)
  filename = filename_list(m).name;
  
  orig_image_file = fullfile(orig_image_folder, filename);
  im = imread(orig_image_file);
  imshow(im);
  
  % parse filename to remove extension
  [pathstr, objectname, ext, versn] = fileparts(filename);
  
  % ask if user wants to draw bounding boxes on this image
  fprintf(1, 'Image name = %s\n', filename);
  reply = input('  Do you want to draw bbox on this image? Y/N [N] ', 's');
  if (isempty(reply))
    reply = 'N';
  end;
  
  if (~strcmp(upper(reply), 'Y'))
    continue;
  end;
 
  % draw a recantangle interactively
  draw_another_rectangle = true;
  bbox = [];
  while (draw_another_rectangle)
    rectangle_not_good_enough = true;
    while (rectangle_not_good_enough)
      fprintf(1, '  Draw a rectangle bbox on current image\n');
      figure(h);
      imshow(im);
      % redraw all the rectangles that have been drawn earlier
      for i = 1 : size(bbox, 1)
        bb = bbox(i, :);
        rectangle('Position', [bb(1), bb(2), bb(3)-bb(1), bb(4)-bb(2)], ...
              'EdgeColor', 'Blue', ...
              'LineWidth', 2);
      end;
      k = waitforbuttonpress;
      point1 = get(gca,'CurrentPoint');    % button down detected
      finalRect = rbbox;                   % return figure units
      point2 = get(gca,'CurrentPoint');    % button up detected
      point1 = point1(1,1:2);              % extract x and y
      point2 = point2(1,1:2);
      p1 = min(point1,point2);             % calculate locations
      offset = abs(point1-point2);         % and dimension
      
      rectangle('Position', [p1(1), p1(2), offset(1), offset(2)], ...
              'EdgeColor', 'Red', ...
              'LineWidth', 2);

      % ask user if the rectangle just drawn was good enough
      reply = input('    Is the current bbox good enough? Y/N [Y] ', 's');
      if (isempty(reply))
        reply = 'Y';
      end;

      if (strcmp(upper(reply), 'Y'))
        rectangle_not_good_enough = false;
      end;
    end;
    
    % record the rectangle coordinate
    xmin = p1(1);
    ymin = p1(2);
    xmax = p1(1) + offset(1);
    ymax = p1(2) + offset(2);
    bbox = [bbox; xmin, ymin, xmax, ymax];
    
    % ask user if the rectangle just drawn was good enough
    reply = input('  Do you want to draw another rectangle? Y/N [Y] ', 's');
    if (isempty(reply))
      reply = 'Y';
    end;

    if (~strcmp(upper(reply), 'Y'))
      draw_another_rectangle = false;
    end;
  end;
  
  % save list of rectangles to a file
  groundtruth_filename = [objectname, groundtruth_extension];
  fprintf(1, '  Writing bbox to file "%s". There are %d bbox(es).\n', ...
    groundtruth_filename, size(bbox, 1));
  dlmwrite(fullfile(output_groundtruth_folder, groundtruth_filename), bbox, ' ');
end;


