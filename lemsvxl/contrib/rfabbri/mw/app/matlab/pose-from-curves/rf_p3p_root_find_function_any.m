function [Rots, Transls, runtime] = rf_p3p_root_find_function_any(gamas,Gamas)
  % see rf_demo_p3p for sample usage
  % write points ---------------------------------------------------------------
  tname = tempname;
  hash=num2str(round(sum(gamas(:))*1000))
  workdir = [tname hash]
  unix(['mkdir ' workdir ' 2>/dev/null']);
  orig_dir = pwd;
  cd (workdir);
  
  assert(size(gamas,1) == 3 && size(gamas,2) == 2);
  assert(size(Gamas,1) == 3 && size(Gamas,2) == 3);
  save('image_pts.txt','gamas','-ascii','-double');
  save('world_pts.txt','Gamas','-ascii','-double');
  
  % run p3p --------------------------------------------------------------------
  % VM: retval = unix('ssh 192.168.1.116 /Users/rfabbri/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/scripts/pnp-run');
  retval = unix('$HOME/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/scripts/pnp-run');
  if retval
    error('something wrong with p3p pose app.');
  end
  
  % read results ---------------------------------------------------------------
  Rots = {};
  Transls = {};
  fsols = dir('*-out-*.txt');
  nsols = size(fsols,1);
  disp (['p3p: num of sols: ' num2str(nsols)])
  for i=1:nsols
    rt = load(fsols(i).name);
    Rots{end+1} = rt(1:3,1:3);
    Transls{end+1} = rt(4,:)';
  end
  runtime = load('time');
  unix(['rm -rf ' workdir]);
  cd(orig_dir);
