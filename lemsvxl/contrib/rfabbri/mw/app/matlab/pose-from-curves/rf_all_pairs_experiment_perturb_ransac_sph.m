clear all;

b_adj = true;
N_RANSAC = 3; % 1000 RANSAC iters TODO if change here, need to change in the
                 % RANSAC fn called below - not a param

%cd ('~/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/work')
% when creating a new work floder, copy the "clean" script over

[gama_all_img, tgt_all_img, gama_all, tgts_all, Gama_all, Tgt_all, K_gt, R_gt, C_gt] = synthetic_data_sph();

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
all_errs_views = {};
all_errs_no_badj_views = {};
all_errs_rt_views = {};
all_times_views = {};
nviews = size(R_gt, 3);
for v=1:2 % TODO XXX
  K_gt_inv = inv(K_gt(:,:,v));
  total_iter=0;
  all_errs = {};
  all_errs_no_badj = {};
  all_errs_rt = {};
  for tp = 1:n_theta_perts
    pert_errors = zeros(n_perturbs, nsamples_pool);
    tgt_pert = perturb_tangent(tgt_all_img(:,:,v), theta_perturbs_deg(tp)*pi/180);

    % transform to world coordinates
    tgt1_2d_pt = tgt_pert + gama_all_img(:,1:2,v);
    tgt1_2d_pt_normal = K_gt_inv*[tgt1_2d_pt ones(size(tgt1_2d_pt,1),1)]';
    tgt1_2d_pt_normal = tgt1_2d_pt_normal';
    tgt1_2d_pt_normal(:,1) = tgt1_2d_pt_normal(:,1) ./ tgt1_2d_pt_normal(:,3);
    tgt1_2d_pt_normal(:,2) = tgt1_2d_pt_normal(:,2) ./ tgt1_2d_pt_normal(:,3);
    tgt1_2d_pt_normal(:,3) = tgt1_2d_pt_normal(:,3) ./ tgt1_2d_pt_normal(:,3);
    tgt_pert = tgt1_2d_pt_normal - gama_all(:,:,v);

    ssum = sqrt(sum(tgt_pert.*tgt_pert, 2));
    norms =  [ssum ssum ssum];
    tgt_pert = tgt_pert ./ norms;
    if abs(sum(tgt_pert(:,3))) > 1e-7
      error('something is wrong with normalizing tgt pert');
    end

    tgt_pert(:,3) = zeros(size(tgt_pert,1),1);
    for p = 1:n_perturbs
      % perturb the points in the image.
      gama_pert_img = perturb(gama_all_img(:,:,v), perturb_levels(p));

      % transform to world coordinates
      gama_pert = K_gt_inv*[gama_pert_img ones(size(gama_pert_img,1),1)]';
      gama_pert = gama_pert';
      gama_pert(:,1) = gama_pert(:,1)./gama_pert(:,3);
      gama_pert(:,2) = gama_pert(:,2)./gama_pert(:,3);
      gama_pert(:,3) = gama_pert(:,3)./gama_pert(:,3);

      dThreshRansac = perturb_levels(p)+1;
      [Rot,Transl,bestResErr,bestResErrVec, solve_time] = rf_pose_from_point_tangents_ransac_fn(...
          ids1, gama_pert, tgt_pert, Gama_all(:,:,v), Tgt_all(:,:,v), ...
          K_gt(:,:,v), gama_pert_img, dThreshRansac);

      % We report reproj. errors on the entire perturbed ground truth:
      pert_errors_no_badj(p,:) = rf_reprojection_error(K_gt(:,:,v)*[Rot Transl],...
               gama_pert_img, Gama_all(:,:,v));

      times(p,:) = solve_time;

      disp('bundle adjustment');
      if b_adj
        % input for bundle adjustment.
        unix('rm *.txt');
        save('image_pts.txt','gama_pert_img','-ascii','-double');
        tmp = Gama_all(:,:,v);
        save('world_pts.txt','tmp','-ascii','-double');
        
        % save cam.
        RC = [Rot;(-Rot'*Transl)'];
        save('camera_RC.txt','RC','-ascii','-double');
        tmp = K_gt(:,:,v);
        save('camera_K.txt','tmp','-ascii','-double');

        % run bundle adjustment.
        retval = unix('$HOME/cprg/vxlprg/lemsvpe/lemsvxl-bin/bin/dbccl_refine_pose_app');  % lemsvxl/contrib/tpollard/dbccl
        if retval
          error('something wrong with refine pose app.');
        end

        % read results
        rc = load('camera_RC_refined.txt');
        Rot = rc(1:3,1:3);
        C = rc(4,:)';
        Transl = -Rot*C;
      end
      disp('done bundle adjustment');

      % We report reproj. errors on the entire perturbed ground truth:
      pert_errors(p,:) = rf_reprojection_error(K_gt(:,:,v)*[Rot Transl],...
               gama_pert_img, Gama_all(:,:,v));

      % Pose errors       
      T_gt = -R_gt(:,:,v)*C_gt(:,:,v);
      [rt_errors(p,1),rt_errors(p,2)] = rf_pose_error(R_gt(:,:,v), T_gt, Rot, Transl);

      total_iter = total_iter + 1;
      disp(['== finished pose computation: ' num2str(total_iter) '/'...
            num2str(n_perturbs*n_theta_perts) '('...
            num2str(100*total_iter/(n_perturbs*n_theta_perts)) '%)']);
    end
    all_errs{end+1} = pert_errors;
    all_errs_no_badj{end+1} = pert_errors_no_badj;
    all_errs_rt{end+1} = rt_errors;
    all_times{end+1} = times;
  end
  all_errs_views{end+1} = all_errs;
  all_errs_no_badj_views{end+1} = all_errs_no_badj_v;
  all_errs_rt_views{end+1} = all_errs_rt;
  all_times_views{end+1} = all_times;
end  % views

% usually drops into ~/lib/matlab
% also remember to save the workspace when hitting an excellent result
[stat,cpuinfo]=unix('cat /proc/cpuinfo');
[stat,gitinfo]=unix('git branch -v');
script_path = mfilename('fullpath');
timestamp = datetime('now');
script_txt=load(sript_path);
save(['all_pairs_experiment_perturb-maxcount_' num2str(maxcount) '-ransac-sph.mat'],...
      'all_errs_views',...
      'all_errs_nobadj_views', ...
      'all_errs_rt_views',...
      'all_times_views',...
      'ids1','perturb_levels','theta_perturbs_deg',...
      'script_path',...
      'script_txt',...
      'timestamp',...
      'gitinfo');
      
% % Raw plot ------------------------------------------------
% figure;
% clf;
% hold on;
% for tp = 1:n_theta_perts
%   for p=1:n_perturbs
%     mycolor = rand(1,3)*0.7;
%     mycolor(1) = min(mycolor(1)+rand()*0.4,1);
%     mycolor(2) = min(mycolor(2)+rand()*0.2,1);
%     mycolor(3) = min(mycolor(3)+rand()*0.1,1);
% 
%     h = plot(all_errs{tp}(p,:));
%     set(h,'color',mycolor);
%     set(h,'linewidth',2);
%   end
% end
% title('Error for different samples and noise levels');
% ylabel('reprojection error');
% xlabel('point-tangent id');
% 
% % Hist plot ------------------------------------------------
% 
% rf_all_pairs_experiment_perturb_histplot;
% 
% % Box plot ------------------------------------------------
% rf_all_pairs_experiment_perturb_boxplot;
