% load data --------------------------------------------------------------------
clear
workdir='~/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth-pami/a'
cd workdir

nviews = 100;
all_errs_no_badj_views_join = cell(1,nviews);
all_errs_rt_views_join = cell(1,nviews);
all_errs_views_join = cell(1,nviews);
all_times_views_join = cell(1,nviews);

p3p=0;

if p3p
  prefix = 'p3p-'
  prefix_out = prefix;
else
  prefix = ''
  prefix_out = 'p2pt-'
end

dat = dir([prefix 'views*']);
nfiles = length(dat);


% join -------------------------------------------------------------------------
for f=1:nfiles
  load(dat(f));
  all_errs_views_join(v_ini:v_f) = all_errs_views(v_ini:v_f);
  all_errs_no_badj_views_join(v_ini:v_f) = all_errs_no_badj_views(v_ini:v_f);
  all_errs_rt_views_join(v_ini:v_f) = all_errs_rt_views(v_ini:v_f);
  all_times_views_join_join(v_ini:v_f) = all_times_views_join(v_ini:v_f);
end


% save -------------------------------------------------------------------------

save([prefix_out '-joined.mat'])
