outdir='/vision/video/rfabbri/beijing'
my_nimages = 669
for i=0:my_nimages
  n_str = int2str(i);
  while (length(n_str) < 5)
    n_str = ['0' n_str];
  end

  nm1_str = int2str(i+1);
  while (length(nm1_str) < 5)
    nm1_str = ['0' nm1_str];
  end
  fname = ['camera' n_str '.txt'];

  a=load(fname);
  P = a(1:3,:)*[a(4:6,:) a(7,:)'];
  save([outdir '/videoframe' nm1_str '.projmatrix'],'-ascii','-double');
end
