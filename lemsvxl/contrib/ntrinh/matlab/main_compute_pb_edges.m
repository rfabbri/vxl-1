% This script compute Pb edges for a pyramid images of the ETHZ dataset
% Authour: Nhon Trinh
% Date: June 27, 2009

%% input

% input directory of the pyramids
input_dir = 'D:\vision\projects\symseg\xshock\all_originals_pyramid';

% output directory
output_dir = 'D:\vision\projects\symseg\xshock\all_pb_edges_pyramid';

% minimum size of image to compute edges
min_height = 32;
min_width = 32;

log_file = 'D:\vision\projects\symseg\xshock\compute_pb.log';

%% Process

log_fid = fopen(log_file, 'w');
fprintf(log_fid, '# width(pixels) height(pixels) elapsed_time(seconds)\n');

file_list = dir(input_dir);
for k = 1 : length(file_list)
  % ignore '.' and '..'
  if (length(file_list(k).name) < 5)
    continue;
  end;
  
  image_folder = file_list(k).name;
  
  fprintf(1, 'Processing %s ...\n', image_folder);
  output_image_folder = fullfile(output_dir, image_folder);
  if (~exist(output_image_folder, 'dir'))
    mkdir(output_image_folder);
  end;
  
  % iterate thru all images in each folder and compute their pb edges
  regexp = '*.png';
  pyramid_file_list = dir(fullfile(input_dir, image_folder, regexp));
  for kk = 1 : length(pyramid_file_list)
    image_filename = pyramid_file_list(kk).name;
    
    fprintf(1, '  - Processing %s ...', image_filename);
    
    % load the image
    im = imread(fullfile(input_dir, image_folder, image_filename));
    im = im2double(im);
    
    if (size(im, 1) < min_height || size(im, 2) < min_width)
      fprintf(1, ' Too small. Skipped.\n');
      continue;
    end;
    
    
    % names for edgemap and orientation
    [pathstr, image_name, ext, versn] = fileparts(image_filename);
    edgemap_filename = [image_name, '_pb_edges.png'];
    edgeorient_filename = [image_name, '_pb_orient.txt'];
    
    % skip images that have been computed
    if (exist(fullfile(output_image_folder, edgemap_filename), 'file'))
      fprintf(1, ' Already computed. Skipped.\n');
      continue;
    end;
    
    % compute Pb edges
    tic; % start the timer to record computation time
    
    fprintf(1, '\n');
    if (size(im, 3) > 1)
      [pb,theta] = pbCGTG(im);
    else
      [pb,theta] = pbBGTG(im);
    end;
    
    theta = theta .* (pb > 0);
    
    pb_time = toc; % stop the timer.
    
    % save computation time to file
    fprintf(log_fid, '%d %d %g\n', size(im, 2), size(im, 1), pb_time);
    
    % save edge map and orientation
    imwrite(pb, fullfile(output_image_folder, edgemap_filename));
    dlmwrite(fullfile(output_image_folder, edgeorient_filename), theta, ' ');
    
    fprintf(1, '----> Done. Elapsed time = %g seconds\n', pb_time);
  end;
end;