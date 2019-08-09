function [Rots, Transls] = rf_p3p_root_find_function_any(gamas,Gamas)
  % write points ---------------------------------------------------------------
  assert(size(gamas,1) == 3 && size(gamas,2) == 2);
  assert(size(Gamas,1) == 3 && size(Gamas,2) == 3);
  save('image_pts.txt','gamas','-ascii','-double');
  save('world_pts.txt','Gamas','-ascii','-double');
  
  % run p3p --------------------------------------------------------------------
  retval = unix('dbccl_pnp');
  if retval
    error('something wrong with p3p pose app.');
  end
  
  % read results ---------------------------------------------------------------
  Rots = {};
  Transls = {};
  fsols = dir('*-out.txt')
  nsols = max(size(fsols))
  disp 'p3p: num of sols' 
  disp nsols
  for i=1:nsols
    rt = load(fsols(i).name);
    Rots{end+1} = rt(1:3,1:3);
    Tranls{end+1} = rc(4,:)';
  end
