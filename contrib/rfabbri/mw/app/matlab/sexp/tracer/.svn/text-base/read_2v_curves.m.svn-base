% Reads the files for two-view recs, *dat, such as in app/dat/reconstr_curves_workdir/
%
% The input files will be named like $prefix-3dcurve-$crv_id-{points|tangents}$ext
%
% See mw_util.h for mywritev(prefix, ext, curves3d)

% mypath='/home/rfabbri/cprg/vxlprg/lemsvxlsrc/contrib/rfabbri/mw/app/dat/reconstr_curves_workdir'
mypath='/home/rfabbri/tmp/dat/'
%mypath='/home/rfabbri/mnt/capsule/ricardo/data/video_grey/out'
recs = cell(0,0);


[ret, myfiles] = unix(['ls ' mypath '/*-3dcurve-*-points*dat | xargs echo']);

myfiles;

while length(myfiles) ~= 0
  [f,rem]=strtok(myfiles);
  myfiles = rem;
  if length(f) == 0
    break;
  end
  f = strip_trailing_blanks(f);
  
  r = myreadv(f);
  if isempty(r)
    warning(['file is empty: ' f]);
  else
    recs{1,end+1} = r;
  end
end


disp(['ncurves = ' num2str(max(size(recs)))])
