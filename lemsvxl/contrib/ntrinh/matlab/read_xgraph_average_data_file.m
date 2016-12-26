function info = read_xgraph_average_data_file(data_file)
% function info = read_xgraph_average_data_file(data_file)
% (c) Nhon Trinh
% Date: Mar 19, 2010

fid = fopen(data_file, 'r');
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

d0 = str2num(data.distance_object1_to_object2);
d1 = str2num(data.distance_average_to_object1);
d2 = str2num(data.distance_average_to_object2);

if (strcmp(object1, object2)) % same object
  error = 0;
else
  error = (abs(d1 - d0/2) + abs(d2 - d0/2)) / d0;
end;



% Save output data
info = struct;
info.object1 = data.object1;
info.object2 = data.object2;
info.distance_object1_to_object2 = d0;
info.distance_object1_to_average = d1;
info.distance_object2_to_average = d2;
info.relative_error = error;
return;
