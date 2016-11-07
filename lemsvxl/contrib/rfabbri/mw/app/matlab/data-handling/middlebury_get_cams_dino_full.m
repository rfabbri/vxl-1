% First you process the file in the directory to strip redundant info:
%
%  1) Delete the first line (the count)
%  2) Cut only the columns[1] to the end and rename it to the variable below:
%  3) Load using the command below and certify that the intrinsic parameters are
%  all the same.

mydir='~/lib/data/capsule/dino/'
a = load('dino_par_simplified_fabbri.txt');
k = unique(a(:,1:9),'rows');

k = reshape(k,3,3)';

if size(k,1) ~= 1
  error('Data has different intrinsic parameters per img.');
end

save([mydir 'calib.intrinsic'], 'k', '-ascii', '-double');

nimgs = size(a,1);
for i=1:nimgs
  fname_base = ['dino_0'  num2str(i,'%.3i')];
  r = a(i,10:18);
  r = reshape(r,3,3)';
  t = a(i,19:21)';
  c = -r'*t;
  rc = [r; c'];
  save([mydir fname_base '.extrinsic'], 'rc', '-ascii', '-double');
end

