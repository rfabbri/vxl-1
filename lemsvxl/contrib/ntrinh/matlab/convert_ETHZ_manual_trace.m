function convert_ETHZ_manual_trace()

% This script converts the ground truth of ETHZ dataset, as provided by
% Ferrari in his INRIA tech report 08/2008
% (c) Nhon Trinh
% Date: Oct 22, 2008

close all;
clear all;

%% input data
source_folder = 'V:\projects\kimia\shockshape\symseg\data\ethz-manual-trace';
category = {'applelogos', 'bottles', 'giraffes', 'mugs', 'swans'};
target_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\groundtruth-manual-trace';

%% process 

% what to do
convert_type_1 = 0;
convert_type_2 = 1;

% 'bottles' and 'applelogos'
if (convert_type_1)
  
for cid = 3:3
  cat_name = char(category{cid});
    
  % target folder
  cat_folder = fullfile(target_folder, [cat_name, '-bw']);
  
  % create the folder if its not there yet
  if (~exist(cat_folder, 'dir'))
    mkdir(cat_folder);
  end
  

  % get a list of filenames in the source folder
  filelist = dir(fullfile(source_folder, cat_name));
  filenames = {};
  for i = 1 : length(filelist)
    if (filelist(i).isdir==1)
      continue;
    end;
    filenames{end+1} = filelist(i).name;
  end;
  
    
  for i = 1 : length(filenames)
    % source image
    source_filename = char(filenames{i});
    source_file = fullfile(source_folder, cat_name, source_filename);
    source_im = imread(source_file);
    
    % the source image mark boundary pixels with intensity = 1
    % we want to make it 255
    im = uint8(zeros(size(source_im)));
    im(find(source_im == 1)) = 255;
    
    % save this image back
    target_filename = [cat_name, '_', source_filename];
    target_file = fullfile(cat_folder, target_filename);
    imwrite(im, target_file);
  end;
end;

end; % convert_type_1


if (convert_type_2)
  cid = 4;
  cat_name = char(category{cid});
  
  % target folder
  cat_folder = fullfile(target_folder, [cat_name, '-bw']);
  
  % create the folder if its not there yet
  if (~exist(cat_folder, 'dir'))
    mkdir(cat_folder);
  end
 
  % get a list of filenames in the source folder
  filelist = dir(fullfile(source_folder, cat_name));
  filenames = {};
  for i = 1 : length(filelist)
    if (filelist(i).isdir==1)
      continue;
    end;
    filenames{end+1} = filelist(i).name;
  end;
  
  % work with each file individually
  for i = 1 : length(filenames)
    % source image
    source_filename = char(filenames{i});
    source_file = fullfile(source_folder, cat_name, source_filename)
    source_im = imread(source_file);
    
    % base for the new filename
    source_suffix = ['_', cat_name, '_outlines.pgm'];
    new_basename = [cat_name, '_', strrep(source_filename, source_suffix, '')];
    
    % first, find intensity of the background
    num_pixels = zeros(256, 1);
    for val = 0 : 255
      num_pixels(val+1) = length(find(source_im == val));
    end;
    [max_num_pixels, idx] = max(num_pixels);
    bg_intensity = idx-1;
    
    % each object corresponds to a pixel intensity level corresponding to
    count = 0;
    for intensity = 0 : 255
      if (intensity == bg_intensity)
        continue;
      end;
      im = zeros(size(source_im));
      y = find(source_im == intensity);
      if (isempty(y))
        continue;
      end;
      im(y) = 255;
      new_filename = [new_basename, '.mask.', num2str(count), '.png']
      count = count + 1;

      % save this image back
      new_file = fullfile(cat_folder, new_filename)
      imwrite(im, new_file);
    end;
  end;
end;


return;



imshow(im);