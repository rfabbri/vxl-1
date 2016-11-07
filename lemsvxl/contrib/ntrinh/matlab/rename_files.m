% this script renames files
% Nhon Trinh
% Sep 16, 2008

clear all;



%% Task 14 Similar to task 12 but for applelogos

% 1. Rename applelogos groundtruth xgraph from applelogos-01.prototype1.xml to
% applelogos_another.xgraph.0.prototype1.xml
% 2. Generate a screenshot of the xgraph on top of its original image

if (1)  
  category = 'applelogos';
  old_xgraph_folder = 'D:\vision\projects\symseg\xshock\xshock-graph\applelogos-xgraph-indexed';
  new_xgraph_folder = 'D:\vision\projects\symseg\xshock\xshock-graph\applelogos-xgraph';
  list_manual_trace_file = 'D:\vision\projects\symseg\xshock\xshock-graph\list_manual_trace_applelogos.txt'
  
  jpg_folder = 'D:\vision\projects\symseg\xshock\xshock-graph\applelogos-jpg';
  screenshot_folder = 'D:\vision\projects\symseg\xshock\xshock-graph\applelogos-xgraph-screenshot';
  xgraph_screenshot_cmd = 'D:\vision\projects\lemsvxl\build32-vs05\brcv\shp\dbsksp\examples\debug\screenshot_xgraph.exe';

  fid = fopen(list_manual_trace_file);
  c = textscan(fid, '%s');
  fclose(fid);
  filenames = c{1};
  for i = 1 : length(filenames);
    obj_name = char(filenames{i}); %% get: obj_name = applelogos_another.mask.0.png
    [pathstr, obj_name, ext, versn] = fileparts(obj_name); %% get: obj_name = applelogos_another.mask.0
    [pathstr, obj_name, xgraph_idx, versn] = fileparts(obj_name); %% get: obj_name = applelogos_another.mask xgraph_idx = .0
    [pathstr, obj_name, ext, versn] = fileparts(obj_name); %% get: applelogos_another
    
    % 2 possibilities for old xgraph filenames, depending on the prototypes
    % of the xshock graph
    old_xgraph_filename1 = [category, '-', num2str(i, '%02d'), '.prototype1', '.xml'];
    old_xgraph_file1 = fullfile(old_xgraph_folder, old_xgraph_filename1);
    
    old_xgraph_filename2 = [category, '-', num2str(i, '%02d'), '.prototype2', '.xml'];
    old_xgraph_file2 = fullfile(old_xgraph_folder, old_xgraph_filename2);
     
%    old_xgraph_file = [];
    prototype = [];
    if (exist(old_xgraph_file1, 'file'))
      old_xgraph_filename = old_xgraph_filename1;
      prototype = '.prototype1';
    elseif (exist(old_xgraph_file2, 'file'))
      old_xgraph_filename = old_xgraph_filename2;
      prototype = '.prototype2';
    else
      continue;
    end;
    
    % file path to old xgraph file
    old_xgraph_file = fullfile(old_xgraph_folder, old_xgraph_filename);
    
    % move on this model does not exist
    if (~exist(old_xgraph_file, 'file'))
      continue;
    end;
    
    % new filename for the shock graph
    new_xgraph_filename = [obj_name, '.xgraph', xgraph_idx, prototype, '.xml'];
    new_xgraph_file = fullfile(new_xgraph_folder, new_xgraph_filename);
    
    % jpg file
    jpg_filename = [obj_name, '.jpg'];
    jpg_file = fullfile(jpg_folder, jpg_filename);
    
    % screenshot
    [pathstr, name, ext, versn] = fileparts(new_xgraph_filename);
    screenshot_filename = [name, '.png'];
    screenshot_file = fullfile(screenshot_folder, screenshot_filename);
    
    fprintf(1, 'old_xgraph_filename=%s\n', old_xgraph_filename);
    fprintf(1, 'new_xgraph_filename=%s\n', new_xgraph_filename);
    fprintf(1, 'jpg_filename=%s\n', jpg_filename);
    fprintf(1, 'screenshot_filename=%s\n', screenshot_filename);
    
    command = [xgraph_screenshot_cmd, ' ', old_xgraph_file, ' ', jpg_file, ...
      ' ', screenshot_file];
    fprintf(1, 'screenshot command=%s\n', command);
      
    % copy & rename xgraph file
    copyfile(old_xgraph_file, new_xgraph_file);
    
    % generate screenshot
    command_unix = strrep(command, '\', '/')
    [status,result] = system(command_unix)
  end;  
end; % if






%% Task 13 Create jpg equivalence of the manual_contour_trace files

if (0)  
  category = 'applelogos';
  old_jpg_folder = 'D:\vision\projects\symseg\xshock\xshock-graph\applelogos-jpg';
  bw_trace_folder = 'D:\vision\projects\symseg\xshock\xshock-graph\applelogos-bw';
  list_manual_trace_file = 'D:\vision\projects\symseg\xshock\xshock-graph\list_manual_trace_applelogos.txt'
  new_jpg_folder = 'D:\vision\projects\symseg\xshock\xshock-graph\applelogos-jpg-indexed';
  
 
  fid = fopen(list_manual_trace_file);
  c = textscan(fid, '%s');
  fclose(fid);
  filenames = c{1};
  for i = 1 : length(filenames);
    obj_name = char(filenames{i}); %% get: bw_name = mugs_apple.mask.0.png
    [pathstr, obj_name, ext, versn] = fileparts(obj_name); %% get: obj_name = mugs_apple.mask.0
    [pathstr, obj_name, xgraph_idx, versn] = fileparts(obj_name); %% get: obj_name = mugs_apple.mask and xgraph_idx = .0
    [pathstr, obj_name, ext, versn] = fileparts(obj_name); %% get: mugs_apple
    
    % old jpg file
    old_jpg_filename = [obj_name, '.jpg'];
    old_jpg_file = fullfile(old_jpg_folder, old_jpg_filename);
    
    % move on this model does not exist
    if (~exist(old_jpg_file, 'file'))
      fprintf(1, 'Missing jpg file %s\n', old_jpg_file);
      continue;
    end;
    
    
    % new filename for the shock graph
    new_jpg_filename = [category, '-', num2str(i, '%02d'), '.jpg'];
    new_jpg_file = fullfile(new_jpg_folder, new_jpg_filename);
    
    
    fprintf(1, 'old_jpg_filename=%s\n', old_jpg_filename);
    fprintf(1, 'new_jpg_filename=%s\n', new_jpg_filename);
    
              
    % copy & rename jpg file
    copyfile(old_jpg_file, new_jpg_file);
  end;  
end; % if










%% Task 12

% 1. Rename mugs groundtruth xgraph from index-type mugs-open-01.xml to
% mugs_apple.xgraph.0.xml
% 2. Generate a screenshot of the xgraph on top of its original image

if (0)  
  category = 'mugs';
  old_xgraph_folder = 'D:\vision\projects\symseg\xshock\xshock-graph\mugs-xgraph-indexed';
  new_xgraph_folder = 'D:\vision\projects\symseg\xshock\xshock-graph\mugs-xgraph';
  list_manual_trace_file = 'D:\vision\projects\symseg\xshock\xshock-graph\list_manual_trace_mugs.txt'
  
  jpg_folder = 'D:\vision\projects\symseg\xshock\xshock-graph\mugs-jpg';
  screenshot_folder = 'D:\vision\projects\symseg\xshock\xshock-graph\mugs-xgraph-screenshot';
  xgraph_screenshot_cmd = 'D:\vision\projects\lemsvxl\build32-vs05\brcv\shp\dbsksp\examples\debug\screenshot_xgraph.exe';

  fid = fopen(list_manual_trace_file);
  c = textscan(fid, '%s');
  fclose(fid);
  filenames = c{1};
  for i = 11 : 11%length(filenames);
    obj_name = char(filenames{i}); %% get: obj_name = mugs_apple.mask.0.png
    [pathstr, obj_name, ext, versn] = fileparts(obj_name); %% get: obj_name = mugs_apple.mask.0
    [pathstr, obj_name, xgraph_idx, versn] = fileparts(obj_name); %% get: obj_name = mugs_apple.mask and xgraph_idx = .0
    [pathstr, obj_name, ext, versn] = fileparts(obj_name); %% get: mugs_apple
    
    % old xgraph filename
    old_xgraph_filename = [category, '-open-', num2str(i, '%02d'), '.xml'];
    old_xgraph_file = fullfile(old_xgraph_folder, old_xgraph_filename);
    
    % move on this model does not exist
    if (~exist(old_xgraph_file, 'file'))
      continue;
    end;
    
    % new filename for the shock graph
    new_xgraph_filename = [obj_name, '.xgraph', xgraph_idx, '.xml'];
    new_xgraph_file = fullfile(new_xgraph_folder, new_xgraph_filename);
    
    % jpg file
    jpg_filename = [obj_name, '.jpg'];
    jpg_file = fullfile(jpg_folder, jpg_filename);
    
    % screenshot
    screenshot_filename = [obj_name, '.xgraph', xgraph_idx, '.png'];
    screenshot_file = fullfile(screenshot_folder, screenshot_filename);
    
    fprintf(1, 'old_xgraph_filename=%s\n', old_xgraph_filename);
    fprintf(1, 'new_xgraph_filename=%s\n', new_xgraph_filename);
    fprintf(1, 'jpg_filename=%s\n', jpg_filename);
    fprintf(1, 'screenshot_filename=%s\n', screenshot_filename);
    
    command = [xgraph_screenshot_cmd, ' ', old_xgraph_file, ' ', jpg_file, ...
      ' ', screenshot_file];
    fprintf(1, 'screenshot command=%s\n', command);
      
    % copy & rename xgraph file
    copyfile(old_xgraph_file, new_xgraph_file);
    
    % generate screenshot
    command_unix = strrep(command, '\', '/')
    [status,result] = system(command_unix)
  end;  
end; % if





%% Task 11: 
% 1. Rename bottle groundtruth xgraph from index-type 01.xml to
% bottles_acaw.xgraph.0.xml
% 2. Generate a screenshot of the xgraph on top of its original image

if (0)  
  category = 'swans';
  old_xgraph_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-graph\swans-xgraph-indexed';
  new_xgraph_folder = 'D:\vision\projects\symseg\xshock\xshock-graph\swans-xgraph';
  list_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-graph\list_swans_jpg.txt'
  
  jpg_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-graph\swans-jpg';
  screenshot_folder = 'D:\vision\projects\symseg\xshock\xshock-graph\swans-xgraph-screenshot';
  xgraph_screenshot_cmd = 'D:\vision\projects\lemsvxl\build32-vs05\brcv\shp\dbsksp\examples\debug\screenshot_xgraph.exe';

  fid = fopen(list_file);
  c = textscan(fid, '%s');
  filenames = c{1};
  for i = 1 : length(filenames);
    filename = char(filenames{i});
    [pathstr, name, ext, versn] = fileparts(filename);
    
    % old xgraph filename
    old_xgraph_filename = [category, '-', num2str(i, '%02d'), '.xml'];
    old_xgraph_file = fullfile(old_xgraph_folder, old_xgraph_filename);
    
    % move on this model does not exist
    if (~exist(old_xgraph_file, 'file'))
      continue;
    end;
    
    % new filename for the shock graph
    new_xgraph_filename = [name, '.xgraph.0.xml'];
    new_xgraph_file = fullfile(new_xgraph_folder, new_xgraph_filename);
    
    % jpg file
    jpg_filename = [name, '.jpg'];
    jpg_file = fullfile(jpg_folder, jpg_filename);
    
%     % png file
%     png_filename = [name, '.png'];
%     png_file = fullfile(png_folder, png_filename);

    
    % screenshot
    screenshot_filename = [name, '.xgraph.0.png'];
    screenshot_file = fullfile(screenshot_folder, screenshot_filename);
    
    fprintf(1, 'old_xgraph_filename=%s\n', old_xgraph_filename);
    fprintf(1, 'new_xgraph_filename=%s\n', new_xgraph_filename);
    fprintf(1, 'jpg_filename=%s\n', jpg_filename);
    fprintf(1, 'screenshot_filename=%s\n', screenshot_filename);
    
    command = [xgraph_screenshot_cmd, ' ', old_xgraph_file, ' ', jpg_file, ...
      ' ', screenshot_file];
    fprintf(1, 'screenshot command=%s\n', command);
      
    % copy & rename xgraph file
    copyfile(old_xgraph_file, new_xgraph_file);
    
    % generate screenshot
    command_unix = strrep(command, '\', '/')
    system(command_unix);   
  end;  
end; % if








%% task 10
% Rename edge orientation files (eg, double_small_edges.tif) files in ETHZ to bottles_double_orient.mat files
% Add the category in front and remove the words 'small', 'big', 'half' at
% the end.

if (0)
  clear all;
  old_folder = 'D:\vision\projects\pas\release-learn-shapes\ETHZ-Shape-Classes\Test\TestSwans';
  category = 'swans';
  all_image_list_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\list_all_images.txt';
  new_folder = 'D:\vision\data\ETHZ-shape\all_edgeorients';

  % parse all image list file to get the object list
  fid = fopen(all_image_list_file, 'r');
  C = textscan(fid, '%s\n');
  
  
  filelist = C{1};
  objectlist = {};
  for i = 1 : length(filelist)
    filename = char(filelist{i});
    [pathstr, objectname, ext, versn] = fileparts(filename);
    objectlist{end+1} = objectname;
  end;
  
  % check existence of each file in the folder and copy over
  for i = 1 : length(objectlist)
    objectname = char(objectlist(i));
    
    % parse objectname into name and category
    idx = strfind(objectname, '_');
    idx = idx(1);
    
    if (~strcmp(category, objectname(1:idx-1)))
      continue;
    end;
    instance = objectname(idx+1:end);
    
    % potential names for the edgemap
    potential_edgemap_filenames = {[instance, '.orient'], ...
      [instance, '_small.orient'], ...
      [instance, '_big.orient'], ...
      [instance, '_half.orient']};
    
    old_filename = [];
    for m = 1 : length(potential_edgemap_filenames)
      if (exist(fullfile(old_folder, char(potential_edgemap_filenames{m})), 'file'))
        old_filename = char(potential_edgemap_filenames{m});
        break;
      end;
    end;
    
    % move on if the file doesn't exist
    if (isempty(old_filename))
      continue;
    end;


    % display info
    new_filename = [objectname, '_orient.mat'];
    fprintf(1, 'old filename= %s\n    new filename= %s\n', ...
      old_filename, new_filename);
     
    % copyfile
    old_file = fullfile(old_folder, old_filename);
    new_file = fullfile(new_folder, new_filename);
    copyfile(old_file, new_file);
  end;
  
end;



%% task 9
% Rename edge maps files (eg, double_edges.tif) files in ETHZ to bottles_double_edges.tif files
% Add the category in front and remove the words 'small', 'big', 'half' at
% the end.

if (0)
  clear all;
  old_folder = 'D:\vision\projects\pas\release-learn-shapes\ETHZ-Shape-Classes\Test\TestSwans';
  category = 'swans';
  all_image_list_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\list_all_images.txt';
  new_folder = 'D:\vision\data\ETHZ-shape\all_edgemaps';

  % parse all image list file to get the object list
  fid = fopen(all_image_list_file, 'r');
  C = textscan(fid, '%s\n');
  
  
  filelist = C{1};
  objectlist = {};
  for i = 1 : length(filelist)
    filename = char(filelist{i});
    [pathstr, objectname, ext, versn] = fileparts(filename);
    objectlist{end+1} = objectname;
  end;
  
  % check existence of each file in the folder and copy over
  for i = 1 : length(objectlist)
    objectname = char(objectlist(i));
    
    % parse objectname into name and category
    idx = strfind(objectname, '_');
    idx = idx(1);
    
    if (~strcmp(category, objectname(1:idx-1)))
      continue;
    end;
    instance = objectname(idx+1:end);
    
    % potential names for the edgemap
    potential_edgemap_filenames = {[instance, '_edges.tif'], ...
      [instance, '_small_edges.tif'], ...
      [instance, '_big_edges.tif'], ...
      [instance, '_half_edges.tif']};
    
    old_filename = [];
    for m = 1 : length(potential_edgemap_filenames)
      if (exist(fullfile(old_folder, char(potential_edgemap_filenames{m})), 'file'))
        old_filename = char(potential_edgemap_filenames{m});
        break;
      end;
    end;
    
    % move on if the file doesn't exist
    if (isempty(old_filename))
      continue;
    end;


    % display info
    new_filename = [objectname, '_edges.tif'];
    fprintf(1, 'old filename= %s\n    new filename= %s\n', ...
      old_filename, new_filename);
     
    % copyfile
    old_file = fullfile(old_folder, old_filename);
    new_file = fullfile(new_folder, new_filename);
    copyfile(old_file, new_file);
  end;
  
end;



%% task 8
% line-by-line comparison between the two sets of groundtruth bboxes for
% ETHZ giraffes (old and patched)
if (0)
  old_gt_folder = 'V:\projects\kimia\shockshape\symseg\data\ethz_shape_classes_v101\ETHZShapeClasses\Giraffes';
  patched_gt_folder = 'V:\projects\kimia\shockshape\symseg\data\GiraffesGT\GifaffesGroundTruth';
  
  filelist = ls(fullfile(patched_gt_folder, '*.groundtruth'));
  for i = 1 : size(filelist, 1)
    filename = filelist(i, :);
    fprintf(1, 'filename = %s\n', filename);
    old_gt_file = fullfile(old_gt_folder, filename);
    patched_gt_file = fullfile(patched_gt_folder, filename);
    
    if (~exist(old_gt_file, 'file'))
      fprintf(1, 'ERROR: missing %s\n', old_gt_file);
      continue;
    end;
    
    if (~exist(patched_gt_file, 'file'))
      fprintf(1, 'ERROR: missing %s\n', patched_gt_file);
      continue;
    end;
    
    old_gt = load(old_gt_file);
    patched_gt = load(patched_gt_file);
    
    if (size(old_gt) ~= size(patched_gt))
      fprintf(1, '    different\n');
      continue;
    end;
    
    if (old_gt ~= patched_gt)
      fprintf(1, '    different\n');
      continue;
    end;
    
    fprintf(1, '   same\n');
    
  end;
end;



%% task 7
% Rename patched groundtruth files of ETHZ giraffes from
% african_giraffes.groundtruth to giraffes_african.groundtruth

if (0)
  % input
  old_folder = 'V:\projects\kimia\shockshape\symseg\data\NewGiraffes-patched-ntrinh';
  new_folder = 'D:\vision\data\ETHZ-shape\giraffes_groundtruths_patched_ntrinh';

  filelist = ls(fullfile(old_folder, '*.groundtruth'));

  for i = 1 : size(filelist, 1)
    % old filename
    old_filename = filelist(i, :);

    % parse the filename
    [pathstr, name, ext, versn] = fileparts(old_filename);

    % find the object name
    loc = strfind(name, '_giraffes');
    if (isempty(loc))
      continue;
    end;

    loc = loc(1);
    object_name = name(1:loc-1);

    % new filename
    new_filename = ['giraffes_', object_name, '.groundtruth'];

    % print out info
    fprintf(1, 'index= %d\n  old_filename= %s\n  new_filename= %s\n', ...
      i, old_filename, new_filename);

    % copy file
    old_file = fullfile(old_folder, old_filename);
    new_file = fullfile(new_folder, new_filename);

    copyfile(old_file, new_file);
  end;

end;

%% task 6
% Convert ETHZ edge orientation matrix files from -mat format to ascii format

if (0)
  old_folder = 'D:\vision\data\ETHZ-shape\all_edgeorients-mat';
  new_folder = 'D:\vision\data\ETHZ-shape\all_edgeorients-txt';
 
  % extract list of files
  
  filelist = ls(fullfile(old_folder, '*.mat'));
  for i = 1 : size(filelist, 1)
    old_filename = filelist(i, :);
    [pathstr, name, ext, versn] = fileparts(old_filename);
    
    new_filename = [name, '.txt'];
    
    % load the old file and save the variable 'theta' as a new text file
    load(fullfile(old_folder, old_filename), '-mat');
    
    % new filename
    new_file = fullfile(new_folder, new_filename);
    
    % print info
    fprintf(1, 'index= %d\n    old_filename= %s\n    new_filename= %s\n', ...
      i, old_filename, new_filename);
    fprintf(1, '    theta-height= %d\n    theta-width= %d\n', ...
      size(theta, 1), size(theta,2));
    
    % save file
    dlmwrite(new_file, theta, ' ');
    
  end;
end;



%% task 5
% Rename edge maps files (_edges.tif) files in ETHZ to .edges.tif files

if (0)
  old_folder = 'D:\vision\projects\pas\release-learn-shapes\ETHZ-Shape-Classes\Test\TestApplelogos';
  category = 'applelogos';
  new_folder = 'D:\vision\data\ETHZ-shape\all_edgemaps';

  filelist = ls(fullfile(old_folder, '*.tif'));
  for i = 1 : size(filelist, 1)
    old_filename = filelist(i, :);
    
    % form new filename
    [pathstr, name, ext, versn] = fileparts(old_filename);
    
    cend = strfind(name, '_edges');
    name = name(1:cend-1);
        
    % new filename
    new_filename = [category, '_', name, '_edges.tif'];
    
    % display info
    fprintf(1, 'index= %d\n    old filename= %s\n    new filename= %s\n', ...
      i, old_filename, new_filename);
    
    % copyfile
    old_file = fullfile(old_folder, old_filename);
    new_file = fullfile(new_folder, new_filename);
    
    copyfile(old_file, new_file);
    
  end;
end;



%% Task 4
% Rename edge orientation files (.orient) files in ETHZ to .orient.mat files

if (0)
  old_folder = 'D:\vision\projects\pas\release-learn-shapes\ETHZ-Shape-Classes\Test\TestSwans';
  category = 'swans';
  new_folder = 'D:\vision\data\ETHZ-shape\all_edgeorients';

  filelist = ls(fullfile(old_folder, '*.orient'));
  for i = 1 : size(filelist, 1)
    old_filename = filelist(i, :);
    
    % form new filename
    [pathstr, name, ext, versn] = fileparts(old_filename);
    new_filename = [category, '_', name, '.orient.mat'];
    
    % display info
    fprintf(1, 'index= %d\n    old filename= %s\n    new filename= %s\n', ...
      i, old_filename, new_filename);
    
    % copyfile
    old_file = fullfile(old_folder, old_filename);
    new_file = fullfile(new_folder, new_filename);
    
    copyfile(old_file, new_file);
    
  end;
end;


%% Task 3
% Remove the jobid number from downloaded files of VOX
if (0)
  old_folder = 'D:\vision\data\ethz-cfg\cfg2\object36106';
  new_folder = 'D:\vision\data\ethz-cfg\cfg3';
  a = dir(old_folder);
  for i = 1 : length(a);
    if (~a(i).isdir)
      old_filename = a(i).name;
      
      % remove the object id in front
      underscore = strfind(old_filename, '_');
      if (~isempty(underscore))
        new_filename = old_filename(underscore(1)+1 : end);
        fprintf(1, 'old name= %s\n', old_filename);
        fprintf(1, '  new name= %s\n', new_filename);
        
        % move file
        movefile(fullfile(old_folder, old_filename), ...
          fullfile(new_folder, new_filename));
      end;
      
    end;
  end;
end;


%% Task 2
if (0)
  old_xgraph_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-graph\indexed-giraffes\giraffes-xgraph-numerals';
  new_xgraph_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-graph\giraffes-xgraph1';
  list_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-graph\list_giraffes.txt'
  
  jpg_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\all_originals';
  screenshot_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-graph\giraffes-xgraph-screenshot';
  xgraph_screenshot_cmd = 'D:\vision\projects\lemsvxl\build32-vs05\brcv\shp\dbsksp\examples\debug\screenshot_xgraph.exe';

  fid = fopen(list_file);
  c = textscan(fid, '%s');
  filenames = c{1};
  for i = 1 : length(filenames);
    filename = char(filenames{i});
    [pathstr, name, ext, versn] = fileparts(filename);
    
    % old xgraph filename
    old_xgraph_filename = [num2str(i, '%02d'), '.xml'];
    old_xgraph_file = fullfile(old_xgraph_folder, old_xgraph_filename);
    
    % move on this model does not exist
    if (~exist(old_xgraph_file, 'file'))
      continue;
    end;
    
    % new filename for the shock graph
    new_xgraph_filename = [name, '.xgraph.0.xml'];
    new_xgraph_file = fullfile(new_xgraph_folder, new_xgraph_filename);
    
    % jpg file
    jpg_filename = [name, '.jpg'];
    jpg_file = fullfile(jpg_folder, jpg_filename);
    
    % screenshot
    screenshot_filename = ['giraffes_', name, '.xgraph.0.png'];
    screenshot_file = fullfile(screenshot_folder, screenshot_filename);
    
    fprintf(1, 'old_xgraph_filename=%s\n', old_xgraph_filename);
    fprintf(1, 'new_xgraph_filename=%s\n', new_xgraph_filename);
    fprintf(1, 'jpg_filename=%s\n', jpg_filename);
    fprintf(1, 'screenshot_filename=%s\n', screenshot_filename);
    
    command = [xgraph_screenshot_cmd, ' ', old_xgraph_file, ' ', jpg_file, ...
      ' ', screenshot_file];
    fprintf(1, 'screenshot command=%s\n', command);
      
    % copy & rename file
%    copyfile(old_xgraph_file, new_xgraph_file);
    
    % generate screenshot
    command_unix = strrep(command, '\', '/')
    system(command_unix);
    
    
  end;  
  
end; % if



%% Task 1
if (0)
  folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-graph\bottles-png';
  list_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-graph\list_bottles.txt';

  fid = fopen(list_file);
  c = textscan(fid, '%s');
  filenames = c{1};
  for i = 1 : length(filenames);
    filename = char(filenames{i});
    [pathstr, name, ext, versn] = fileparts(filename);
    png_filename = ['bottles_', name, '.png'];
    png_file = fullfile(folder, png_filename)

    if (exist(png_file, 'file'))
      % remove bottles

       new_filename = [name, '.png'];
       copyfile(png_file, fullfile(folder, new_filename));
    end;  
  end;


end; % if


