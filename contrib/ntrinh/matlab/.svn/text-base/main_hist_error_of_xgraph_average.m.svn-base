% This script plots error of xgraph average computation
% (c) Nhon Trinh
% Date: Mar 7, 2010


clear all;

%% Input

%input_folder = 'V:\projects\kimia\shockshape\learnskeleton\results\object205468-exp_1-kimia_99_fish_vs_fish-hellboy1';
%input_folder = 'V:\projects\kimia\shockshape\learnskeleton\results\object205592-exp_2-kimia_99_quadruped_vs_quadruped';
%input_folder = 'V:\projects\kimia\shockshape\learnskeleton\results\object205714-exp_3-kimia_99_fish_vs_all-hellboy1-complete';
%input_folder = 'V:\projects\kimia\shockshape\learnskeleton\results\object214107-exp_6-v6-kimia_99_quadruped_vs_quadruped-hellboy1';
input_folder = 'V:\projects\kimia\shockshape\learnskeleton\results\object212895-exp_6-v6-kimia_99_bunny_vs_all-smarteyes';
input_folder = 'V:\projects\kimia\shockshape\learnskeleton\results\object214107-exp_7-v6-kimia_99_quadruped_vs_quadruped-hellboy1';


%% Process

txt_list = dir(fullfile(input_folder, '*.txt'));
num_files = length(txt_list);

error_list = [];
for k = 1 : num_files
  fname = txt_list(k).name;
  
  % only look for data file (ending with '-data.txt')
  if (isempty(strfind(fname, '-data.txt')))
    continue;
  end;
  
  fid = fopen(fullfile(input_folder, fname), 'r');
  C = textscan(fid, '%s %s\n');
  fclose(fid);
  
  field_names = C{1};
  field_values = C{2};
  
  data = struct;%
  for i = 1: length(field_names)
    % take care illegal name due to '-' in the name
    old_name = field_names{i};
    new_name = strrep(old_name, '-', '_');
    data = setfield(data, new_name, field_values{i});
  end;
  
  object1 = data.object1;
  object2 = data.object2;
  
  % ignore self-averaging pairs
  if (strcmp(object1, object2))
    continue;
  end;
  
  d0 = str2num(data.distance_object1_to_object2);
  d1 = str2num(data.distance_average_to_object1);
  d2 = str2num(data.distance_average_to_object2);
  
  error = (abs(d1 - d0/2) + abs(d2 - d0/2)) / d0;
 
  
  fprintf(1, 'pair k=%d, relative error = %g\n', k, error);
  
  % ignore outliers
  if (error > 1)
    continue;
  end;
  
  error_list(end+1) = error;  
end;



%% Plot
fprintf(1, 'Number of shape pairs= %d\n', length(error_list));
fprintf(1, 'Mean error (percent)= %g\n', mean(error_list)*100);
fprintf(1, 'Std error (percent)= %g\n', std(error_list)*100);

hist(error_list, 0.05 : 0.05 : 0.95);
title('Histogram of relative error');
xlabel('Relative error (%)');
h = findobj(gca,'Type','patch');
set(h,'FaceColor','b','EdgeColor','w')




