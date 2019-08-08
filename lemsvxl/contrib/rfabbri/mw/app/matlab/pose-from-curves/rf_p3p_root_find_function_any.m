
  % P3P ------------------------------------------------------------------------
  % input for bundle adjustment.
  unix('./clean');
  save('image_pts.txt','gama_pert_img','-ascii','-double');
  save('world_pts.txt','Gama_all','-ascii','-double');
  
  % run bundle adjustment.
  retval = unix('dbccl_pnp');
  if retval
    error('something wrong with refine pose app.');
  end

  % read results
  % for xxx
  fsols = dir('*-out.txt')
  nsols = max(size(fsols))
  for i=1:nsols
    fsols(i).name
    rt = load(fsols(i).name);
    Rot = rt(1:3,1:3);
    Transl = rc(4,:)';
  end
