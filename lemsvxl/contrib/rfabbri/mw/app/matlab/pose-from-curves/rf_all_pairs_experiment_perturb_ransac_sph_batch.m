clear all;

v_ini=@INI@;
v_f=@END@;
%v_ini=1; v_f=10;
b_adj = true;
N = 1000; % 1000 RANSAC iters TODO if change here, need to change in the
 % RANSAC fn called below - not a param

workdir = ['~/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/work-views-' num2str(v_ini) '-' num2str(v_f)]
unix(['mkdir ' workdir ' 2>/dev/null']);
cd (workdir);

% when creating a new work folder, copy the "clean" script over

[gama_all_img, tgt_all_img, gama_all, tgts_all, Gama_all, Tgt_all, K_gt, R_gt, C_gt] = synthetic_data_sph();
K_gt_inv = inv(K_gt);

nsamples_pool = max(size(Gama_all));
maxcount = nsamples_pool;
%maxcount = 1000;  % random sample maxcount points out of the 5117 in the dataset
%maxcount = 3;     % instead of using the entire dataset
if (maxcount > nsamples_pool)
  error('maxcount too high');
end

% The random subset of maxcount of all points (5117 to be used in random sampling)
ids1 = zeros(maxcount,1);  
scount=0;
while scount < maxcount
  id1 = randi([1 nsamples_pool],1,1);
  while ~isempty(find(ids1 == id1))
    id1 = randi([1 nsamples_pool],1,1);
  end
  scount = scount + 1;
  ids1(scount) = id1;
end

perturb_levels = [0 0.5 1 2];
theta_perturbs_deg = [0 0.5 1 5 10];

%perturb_levels = [0 0.5 1 2];  % PAPER XXX TODO
%theta_perturbs_deg = [0 0.5 1 5 10];

perturb_levels = [0 0.5];
theta_perturbs_deg = [0 1];

%perturb_levels = [0 0.1 0.5 1 2];
%theta_perturbs_deg = [0 0.1 0.5 1 2 5 7 10];
n_perturbs = length(perturb_levels);
n_theta_perts = length(theta_perturbs_deg);
nviews = size(R_gt, 3);
all_errs_views = cell(1,nviews);
all_errs_no_badj_views = cell(1,nviews);
all_errs_rt_views = cell(1,nviews);
all_times_views = cell(1,nviews);

rf_all_pairs_experiment_perturb_ransac_sph
