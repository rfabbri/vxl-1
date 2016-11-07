%function main_number_edge_points()
% This script plots the ratio of edge points over the total number of
% pixels in an image
% (c) Nhon Trinh
% Date: Oct 28, 2008

edge_folder = 'D:\vision\projects\symseg\xshock\ETHZ-shapes\detect\all_edgemaps';
filelist = ls(edge_folder);

edge_percentage = [];

threshold_vec = 0:5:255;

for i = 1 : size(filelist, 1)
  filename = filelist(i, :);
  if (strncmp(filename, '.', 1) || strncmp(filename, '..', 2))
    continue;
  end;
  
  im = imread(fullfile(edge_folder, filename));
  
  y = [];
  for threshold = threshold_vec 
    num_edge_pts = length(find(im > threshold));
    num_pts = length(im(:));
    y(end+1) = num_edge_pts / num_pts;
  end;
  
  edge_percentage(end+1, :) = y; 
end;

% fprintf(1, 'Mean percentage of edge points = %g\n', mean(edge_percentage));
plot(threshold_vec, mean(edge_percentage)*100);
title('Percentage of edge points in ETHZ as function of threshold');
xlabel('threshold');
ylabel('Percentage of edge points');
grid on;

