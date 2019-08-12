% load data --------------------------------------------------------------------

% view pairs matching rf_all_pairs_experiment_perturb_ransac_sph_batch.sh -- the scatter/toploevel app
vpairs = [01 06 11 16 21 26 31 41 46 51 56 61 71 81 91;
        05 10 15 20 25 30 40 45 50 55 60 70 80 90 100];
maxcount=5117;

npairs = size(vpairs,2);

perturb_levels = [0 0.5 1 2]; % todo: infer from file
theta_perturbs_deg = [0 0.5 1 5 10];

all_errs_views_join = cell(1,nviews);
all_errs_no_badj_views_join = cell(1,nviews);
all_errs_rt_views_join = cell(1,nviews);
all_times_views_join = cell(1,nviews);
for pair=1:npairs
  v_ini = vpairs(1,pair);
  v_end = vpairs(2,pair);
  workdir = ['~/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/work-views-' num2str(v_ini) '-' num2str(v_f)]
  load([workdir '/all_pairs_experiment_perturb-maxcount_' num2str(maxcount) '-ransac-sph.mat']);

  all_errs_views_join(v_ini:v_f) = all_errs_views(v_ini:v_f);
  all_errs_no_badj_views_join(v_ini:v_f) = all_errs_no_badj_views(v_ini:v_f);
  all_errs_rt_views_join(v_ini:v_f) = all_errs_rt_views(v_ini:v_f);
  all_times_views_join_join(v_ini:v_f) = all_times_views_join(v_ini:v_f);
end

% join -------------------------------------------------------------------------
