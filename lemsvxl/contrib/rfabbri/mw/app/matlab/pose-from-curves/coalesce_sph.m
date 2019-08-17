% Input: cell arrays from join_sph.m
% Output: coalesce all data to be ready for box plot

% Join data for all views into a single one
n_theta_perts = length(theta_perturbs_deg)
all_errs_no_badj_all = cell(1,n_theta_perts+1); % +1 for p3p
all_errs_views_all = cell(1,n_theta_perts+1); % +1 for p3p
all_errs_rotation_views_all = cell(1,n_theta_perts+1); % +1 for p3p
all_errs_translation_views_all = cell(1,n_theta_perts+1); % +1 for p3p
for v=1:nviews
  if has_data(v)
    for tp=1:n_theta_perts
      all_errs_no_badj_all{tp} = [all_errs_no_badj_all{tp} all_errs_no_badj_views_join{v}{tp}];
      all_errs_views_all{tp} = [all_errs_views_all{tp} all_errs_views_join{v}{tp}];
      all_errs_rotation_views_all{tp} = [all_errs_rotation_views_all{tp} all_errs_rt_views_join{v}{tp}(:,1)];
      all_errs_translation_views_all{tp} = [all_errs_translation_views_all{tp} all_errs_rt_views_join{v}{tp}(:,2)];
    end
  end
end
%all_errs_no_badj_views_join

if has_data_p3p(1) % make boxplot work
  for tp=1:n_theta_perts
      all_errs_no_badj_all{tp} = replicate(all_errs_no_badj_all{tp}, size(all_errs_no_badj_views_join_p3p{1},2)*nviews);
      all_errs_views_all{tp} = replicate(all_errs_views_all{tp}, size(all_errs_views_join_p3p{1},2)*nviews);
      all_errs_rotation_views_all{tp} = replicate(all_errs_rotation_views_all{tp}, nviews);
      all_errs_translation_views_all{tp} = replicate(all_errs_translation_views_all{tp},nviews);
  end
end

tp = n_theta_perts + 1;
for v=1:nviews
  if has_data_p3p(v)
    all_errs_no_badj_all{tp} = [all_errs_no_badj_all{tp} all_errs_no_badj_views_join_p3p{v}];
    all_errs_views_all{tp} = [all_errs_views_all{tp} all_errs_views_join_p3p{v}];
    all_errs_rotation_views_all{tp} = [all_errs_rotation_views_all{tp} all_errs_rt_views_join_p3p{v}(:,1)];
    all_errs_translation_views_all{tp} = [all_errs_translation_views_all{tp} all_errs_rt_views_join_p3p{v}(:,2)];
  end
end

% boxplot_sph

% remove point perturb 0 
n_perts = length(perturb_levels);
n_perts = n_perts -1;
perturb_levels = perturb_levels(2:end);
for tp=1:(n_theta_perts+1)
  all_errs_no_badj_all{tp} = all_errs_no_badj_all{tp}(2:end,:);
  all_errs_views_all{tp} = all_errs_views_all{tp}(2:end,:);
  all_errs_rotation_views_all{tp} = all_errs_rotation_views_all{tp}(2:end,:);
  all_errs_translation_views_all{tp} = all_errs_translation_views_all{tp}(2:end,:);
end
save('data-coalesced-p3p_v0-p2pt_v5.mat', 'all_errs_no_badj_all', 'all_errs_views_all', 'all_errs_rotation_views_all','all_errs_translation_views_all','n_perts'); %n_perts was missing, just addding now
