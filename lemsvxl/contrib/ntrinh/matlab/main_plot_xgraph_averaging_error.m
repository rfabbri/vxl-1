% This script plots the relativer error of shape averaging for
% intra-category shapes
% (c) Nhon Trinh
% Date: Mar 19, 2010

%% Input data

% %% Version -9
% average_folder = 'V:\projects\kimia\shockshape\learnskeleton\results\kimia-99-dataset\average-two-xgraphs';
% category_dirnames = {'object225091-fish-vs-fish', ...
%               'object225213-buny-vs-buny', ...
%               'object225335-dude-vs-dude', ...
%               'object225457-fgen-vs-fgen', ...
%               'object225579-fighterjet-vs-fighterjet', ...
%               'object225701-flatfish-vs-flatfish', ...
%               'object225823-hand-vs-hand', ...
%               'object225945-quadruped-vs-quadruped', ...
%               'object226067-tool-vs-tool', ...
%               };
%             
%             
% 
% % folder containing all pairwise distance measures
% pairwise_folder = 'V:\projects\kimia\shockshape\learnskeleton\results\kimia-99-dataset\xgraph-edit-distance-v4';
% pairwise_distance_dirname = 'object218240-xgraph-distance-kimia-99-vs-kimia-99';




average_folder = 'V:\projects\kimia\shockshape\learnskeleton\results\kimia-99-dataset\average-two-xgraphs-v10'
category_dirnames = {...
  'object227713-bunny-vs-bunny',...
  'object227835-dude-vs-dude',...
  'object228057-fgen-vs-fgen',...
  'object228179-fighterjet-vs-fighterjet',...
  'object228301-fish-vs-fish',...
  'object228423-flatfish-vs-flatfish',...
  'object228545-hand-vs-hand',...
  'object228667-quadruped-vs-quadruped',...
  'object228789-tool-vs-tool',...
              };
            
            

% folder containing all pairwise distance measures
pairwise_folder = 'V:\projects\kimia\shockshape\learnskeleton\results\kimia-99-dataset\xgraph-edit-distance-v6';
pairwise_distance_dirname = 'object227957-kimia_99-vs-kimia_99';


%% Compute relative errors of computed average shape
load_relative_error_of_computed_average = 1;


if (load_relative_error_of_computed_average)
error_list = [];

num_dirs = length(category_dirnames);
for k = 1:num_dirs
  dir_name = category_dirnames{k};
  fprintf(1, 'dir_name = %s\n', dir_name);
  
  % Load files in folder
  dir_path = fullfile(average_folder, dir_name);
  data_files = dir(fullfile(dir_path, '*-data.txt'));
  
  num_files = length(data_files);
  for i = 1 : num_files
    fname = data_files(i).name;
    fprintf('  fname= %s\n', fname);
    info = read_xgraph_average_data_file(fullfile(dir_path, fname));
    if (strcmp(info.object1, info.object2))
      continue;
    end;
    
    if (info.relative_error > 1)
      continue;
    end;
    error_list(end+1) = info.relative_error;
  end;
end;

end;

%% Compute error of baseline average

load_distance_records = 1;
if (load_distance_records)
  distance_records = [];

  fprintf(1, 'pairwise distance dirname=%s\n', pairwise_distance_dirname);
  dir_path = fullfile(pairwise_folder, pairwise_distance_dirname);
  
  distance_files = dir(fullfile(dir_path, '*-xgraph_edit_distance.xml'));
  num_files = length(distance_files);
  
  for i = 1 : num_files
    fname = distance_files(i).name;
    fprintf('  fname= %s\n', fname);
    
    % Load the distance xml file
    [tree, treename] = xml_read(fullfile(dir_path, fname));
    
    num_distances = length(tree.xgraph_distance_list.xgraph_distance);
    for j = 1 : num_distances
      d = tree.xgraph_distance_list.xgraph_distance(j);
      record.object1 = d.model_name.CONTENT;
      record.object2 = d.query_name.CONTENT;
      record.distance = d.distance.CONTENT;
      
      % save to overall record list
      idx = length(distance_records);
      distance_records(idx+1).object1 = record.object1;
      distance_records(idx+1).object2 = record.object2;
      distance_records(idx+1).distance = record.distance;    
    end;
  end;
end;

%% Do analysis with distance records

compute_distance_matrix = 1;

if (compute_distance_matrix)
  fprintf(1, 'Computing distance matrix ...\n');
  
% Collect list of names for object1 and object
clear list_object1;
clear list_object2;
num_records = length(distance_records);
for i = 1 : num_records
  list_object1{i,1} = distance_records(i).object1;
  list_object2{i,1} = distance_records(i).object2;
end;

list_object1 = unique(list_object1);
list_object2 = unique(list_object2);

% combine the two list
list_object = [list_object1; list_object2];
list_object = unique(list_object);

% construct a distance matrix
dmatrix = zeros(length(list_object), length(list_object));
for i = 1  :num_records
  record = distance_records(i);
  index1 = strmatch(record.object1, list_object, 'exact');
  index2 = strmatch(record.object2, list_object, 'exact');
  dmatrix (index1, index2) = record.distance;
end;
end;

% analyze dmatrix to compute baseline error

num_objects = length(list_object);
baseline_err = [];

for i = 1 : num_objects
  for j = 1 : num_objects
    object1 = list_object(i);
    object2 = list_object(j);
    
    % ignore self-averaging!
    if (strcmp(object1, object2))
      continue;
    end;
    
    d0 = dmatrix(i, j);
    
    % search for the best d1 and d2
    d1  = dmatrix(i, :);
    d2 = dmatrix(j, :);
    
    err = (abs(d1 - d0/2) + abs(d2 - d0/2)) / d0;
    
    baseline_err(end+1) = min(err);
  end;
end;

fprintf(1, 'Baseline - mean   = %g\n', mean(baseline_err));
fprintf(1, 'Baseline - std dev= %g\n', std(baseline_err));
fprintf(1, 'Using shock graph - mean   = %g\n', mean(error_list));
fprintf(1, 'Using shock graph - std dev= %g\n', std(error_list));

%% Plot
x = [0.05 : 0.05 : 1];
avg_hist = hist(error_list, x) / length(error_list);
baseline_hist = hist(baseline_err, x) / length(baseline_err);


figure;
hold on;
plot(x, baseline_hist, 'b-', 'linewidth', 3);
plot(x, avg_hist, 'r-', 'linewidth', 3);
hold off;
xlabel('relative error');
ylabel('frequency');
title('Compare relative error of shape averaging');
legend({'baseline - intra-category best exemplar', 'averaging using shock graph'});



save main_plot_xgraph_averaging_error;

[dmatrix, list_object1, list_object2] = distance_matrix(distance_records);
save kimia_99_dmatrix dmatrix list_object1 list_object2;