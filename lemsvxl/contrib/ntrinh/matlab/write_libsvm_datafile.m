function write_libsvm_datafile(label, feat, filename)
% function write_libsvm_datafile(label, feat, filename)
% Write SVM training and testing data in LIBSVM's format
% (c) Nhon Trinh
% Date: Oct 2, 2008

if (nargin < 3)
  fprintf(2, 'ERROR: Not enough arguments.\n');
  return;
end;

if (size(label, 1) ~= size(feat, 1))
  fprintf(2, 'ERROR: label and feat do not have the same length.\n');
  return;
end;

if (size(label, 2) ~= 1)
  fprintf(2, 'ERROR: label vector should have width = 1.\n');
  return;
end;

fid = fopen(filename, 'w');
if (fid == -1)
  fprintf(2, 'Could not open file "%s" for writing.\n', filename);
  return;
end;

for k = 1 : size(feat, 1)
  fprintf(fid, '%d', label(k));
  for i = 1 : size(feat, 2)
    fprintf(fid, ' %d:%g', (i+1), feat(k, i));
  end;
  fprintf(fid, '\n');
end;

