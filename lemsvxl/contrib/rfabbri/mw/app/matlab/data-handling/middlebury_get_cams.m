%
% Code to get cameras from the middlebury multiview database and translate into
% my own format suitable to be read by me (calib.intrinsic-calib.extrinsic, etc.)
%
%
% First, delete the first line, then use this command to strip out the filenames
% and first row, making the file look like a table
%
%  cut -f 1 --complement -d ' ' dinoR_par.txt >dinoR_par_matlab.txt
%  cut -f 1 -d ' ' dinoR_par.txt >dinoR_par_names.txt

mydir='~/lib/data/middlebury-mview-work/dinoRing/cameras/'
cams=load([mydir 'dinoR_par_matlab.txt']);

k = reshape(cams(1,1:9), 3, 3);
k = k';

save([mydir 'calib.intrinsic'],'k','-ascii','-double');

ncams=size(cams,1);
for i=1:ncams
  fname_base = ['dinoR0' num2str(i,'%.3i')]
  ki = reshape(cams(i,1:9), 3, 3);
  ki = ki';
  if (ki ~= k)
    error('there are different intrinsics for each view -- rewrite this script.');
  end
  r = reshape(cams(i,10:18), 3, 3);
  r = r';
  t = cams(i,19:21);
  t = t';
  c = -r'*t;
  rc = [r; c'];
  save([mydir fname_base '.extrinsic'],'rc','-ascii','-double');
end
