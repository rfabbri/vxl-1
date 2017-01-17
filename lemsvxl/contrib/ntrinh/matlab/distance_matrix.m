function [dmatrix, list_object1, list_object2] = distance_matrix(distance_records)
% [dmatrix, object_list1, object_list2] = distance_matrix(distance_records)
% (c) Nhon Trinh
% Date: Mar 27, 2010

  
% Collect list of names for object1 and object
%clear list_object1;
%clear list_object2;

num_records = length(distance_records);
for i = 1 : num_records
  list_object1{i,1} = distance_records(i).object1;
  list_object2{i,1} = distance_records(i).object2;
end;

list_object1 = unique(list_object1);
list_object2 = unique(list_object2);

% combine the two list
%list_object = [list_object1; list_object2];
%list_object = unique(list_object);

% construct a distance matrix
dmatrix = zeros(length(list_object1), length(list_object2));
for i = 1  :num_records
  record = distance_records(i);
  index1 = strmatch(record.object1, list_object1, 'exact');
  index2 = strmatch(record.object2, list_object2, 'exact');
  dmatrix (index1, index2) = record.distance;
end;
