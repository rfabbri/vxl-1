function c3 = readrec(imgnum, contour_num,suf,ext)
  if (nargin == 2)
    suf='-cam01'; %:< suffix
    ext='.dat';
  end
%  prefix = 'ct-spheres/dat/reconstr-600x-';
  prefix='ct-spheres/dat/analytic-reconstr-';
  fname = [prefix int2str(imgnum) '-' int2str(contour_num-1) suf ext];
  disp(['reading ' fname]);
  c3 = myreadv(fname);
