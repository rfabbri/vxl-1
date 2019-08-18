% after this script, call coalesce and boxplot sph
clear
% load data -------------------------------------------------------------------

workdir='~/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth-pami/b'
cd(workdir);
%load('all-joined-v7-v100_2.mat');

% preload 

%coalesce_sph

% UNCOMENT to recompute
%return %%----------------------------------------------------------------------
%------------------------------------------------------------------------------


nviews = 100;
all_errs_no_badj_views_join = cell(1,nviews);
all_errs_rt_views_join = cell(1,nviews);
all_errs_rt_no_badj_views_join = cell(1,nviews);
all_errs_views_join = cell(1,nviews);
all_times_views_join = cell(1,nviews);
all_errs_no_badj_views_join_p3p = cell(1,nviews);
all_errs_rt_views_join_p3p = cell(1,nviews);
all_errs_rt_no_badj_views_join_p3p = cell(1,nviews);
all_errs_views_join_p3p = cell(1,nviews);
all_times_views_join_p3p = cell(1,nviews);
has_data = zeros(1,nviews);
has_data_p3p = zeros(1,nviews);
prefix = ''
prefix_out = 'p2pt-'
dat = dir([prefix 'views*']);
nfiles = length(dat);

% join -------------------------------------------------------------------------
for f=1:nfiles
  load([dat(f).name '/all_pairs_experiment_perturb-maxcount_5117-ransac-sph.mat']);
  for v=v_ini:v_f
    if v~=60  % strange behaviour
    all_errs_views_join{v} = all_errs_views{v}
    all_errs_no_badj_views_join{v} = all_errs_no_badj_views{v};
    all_errs_rt_views_join{v} = all_errs_rt_views{v};
    all_errs_rt_no_badj_views_join{v} = all_errs_rt_no_badj_views{v};
    all_times_views_join{v} = all_times_views{v};
    has_data(v) = 1;
    end
  end
end

prefix = 'p3p-'
prefix_out = prefix;
dat = dir([prefix 'views*']);
nfiles = length(dat);
for f=1:nfiles
  load([dat(f).name '/all_pairs_experiment_perturb-maxcount_5117-ransac-sph-p3p.mat']);
  for v=v_ini:v_f
    assert(isempty(all_errs_views_join_p3p{v}));
    all_errs_views_join_p3p{v} = all_errs_p3p_views{v}
    all_errs_no_badj_views_join_p3p{v} = all_errs_p3p_no_badj_views{v};
    all_errs_rt_views_join_p3p{v} = all_errs_p3p_rt_views{v};
    all_errs_rt_views_no_badj_join_p3p{v} = all_errs_p3p_rt_no_badj_views{v};
    all_times_views_join_p3p{v} = all_times_p3p_views{v};
    has_data_p3p(v) = 1;
  end
end

% save -------------------------------------------------------------------------
save(['all-joined-v7-v100_2.mat'])
