%mydir='~/lib/data/dec/CapitolSite500/cameras_opt_500/'
%mydir='~/tmp/cameras_KRT_opt/'
mydir='/vision/video/dec/CapitolSiteHigh/cameras_KRT/'


n_ima = 500;
for i=0:n_ima
  fname_base = ['camera_00'  num2str(i,'%.3i')];
  disp(['Reading ' fname_base '.txt'])
  a=load([mydir fname_base '.txt']);
  r=a(4:6,:);
  t=a(7,:)';
  c = -r'*t;
  rc=[r; c'];
  save(['/home/rfabbri/work/' fname_base '.extrinsic'],'rc','-ascii','-double');


  k = a(1:3,1:3);
  if i ~= 0
    if sum(sum(abs(k-kold))) ~= 0
      error('there are different intrinsics for each view -- rewrite this script.');
    end
  else
    save(['/home/rfabbri/work/calib.intrinsic'],'k','-ascii','-double');
  end
  kold = k;
end
