% This script colllect xgraph edit distance computed in VOX and form a
% distance table

%% Input data
data_folder = 'V:\projects\kimia\shockshape\learnskeleton\results\original-mpeg7\xgraph-edit-distance';
exp_dirnames = {...
 'object218340-apple-vs-all',...
 'object218362-bell-vs-all',...
 'object218363-bat-vs-all',...
 'object218375-brick-vs-all',...
 'object218383-bird-vs-all',...
 'object218466-bottle-vs-all',...
 'object218466-butterfly-vs-all',...
 'object218466-camel-vs-all',...
 'object220247-flatfish-vs-all',...
 'object220269-car-vs-all',...
 'object220270-Bone-vs-all',...
 'object220310-dog+children+cattle+carriage-vs-all',...
 'object220396-comma+classic+chopper+chicken+cellular_phone-vs-all',...
 'object220514-heart+hcircle+hat+hammer+guitar+glas+frog+fountain+fork+fly-vs-all',...
 'object221252-personal_car+pencil+octopus+Misk+lmfish+lizzard+key+jar+horseshoe+horse-vs-all',...
 'object224109-watch+turtle+truck+tree+teddy+stef+spring+spoon+shoe+sea_snake+ray+rat+pocket-vs-all',...
 'object224564-lizzard-8-9-vs-all',...
 'object224571-16_categories-vs-all',...
 'object226292-beetle-vs-all', ...
 };
object_list_file = 'V:\projects\kimia\shockshape\learnskeleton\results\original-mpeg7\objectlist_mpeg7.txt';




%% Load distance files


if (exist('original_mpeg7_dmatrix.mat', 'file'))
  load original_mpeg7_dmatrix.mat;
else
  % Load list of object names
  fid = fopen(object_list_file, 'r');
  C = textscan(fid, '%s');
  fclose(fid);
  object_list = C{1};

  % initialze distance matrix
  dmatrix = zeros(num_objects, num_objects) + 1e10;
  processed_fnames = [];
end;

num_objects = length(object_list);
num_dirs = length(exp_dirnames);
for k = 1 : num_dirs
  exp_dirname = exp_dirnames{k};

  fprintf(1, 'Experiment dirname=%s\n', exp_dirname);
  exp_path = fullfile(data_folder, exp_dirname);
  
  distance_files = dir(fullfile(exp_path, '*-xgraph_edit_distance.xml'));
  num_files = length(distance_files);
  
  for i = 1 : num_files
    fname = distance_files(i).name;

    % Has this file been processed yet?
    processed = strmatch(fname, processed_fnames, 'exact');
    
    if (~isempty(processed))
      fprintf('  fname= %s ... skipped\n', fname);
      continue;
    else
      fprintf('  fname= %s\n', fname);      
    end;
    
    % Load the distance xml file
    [tree, treename] = xml_read(fullfile(exp_path, fname));
    
    num_distances = length(tree.xgraph_distance_list.xgraph_distance);
    for j = 1 : num_distances
      d = tree.xgraph_distance_list.xgraph_distance(j);
      object1 = d.model_name.CONTENT;
      object2 = d.query_name.CONTENT;
      distance = d.distance.CONTENT;
      
      % remove the 'original-_' from the name
      object1 = object1(11:end);
      object2 = object2(11:end);
      
      if (isempty(object1) || isempty(object2))
        continue;
      end;
      
      % search for index of object1 and object2
      index1 = strmatch(object1, object_list, 'exact');
      index2 = strmatch(object2, object_list, 'exact');
      
      % save to matrix
      dmatrix (index1, index2) = distance;
      
    end;
    
    processed_fnames{end+1} = fname;
  end;
end;

save original_mpeg7_dmatrix object_list dmatrix processed_fnames;
