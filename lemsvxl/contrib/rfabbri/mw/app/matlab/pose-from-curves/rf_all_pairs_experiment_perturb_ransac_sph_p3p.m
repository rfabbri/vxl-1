%clear all;
% to be called by this script _batch.m

for v=v_ini:v_f % 1:nviews
%  total_iter=0;
  all_errs = {};
  all_errs_no_badj = {};
  all_errs_rt = {};
  all_times = {};
  pert_errors = zeros(n_perturbs, nsamples_pool);
  pert_errors_no_badj = zeros(n_perturbs, nsamples_pool);
  rt_errors = zeros(n_perturbs,2); % 1 = R, 2 = T
  times = zeros(n_perturbs,N);
  for p = 1:n_perturbs
    % perturb the points in the image.
    gama_pert_img = perturb(gama_all_img(:,:,v), perturb_levels(p));

    % transform to world coordinates
    gama_pert = K_gt_inv*[gama_pert_img ones(size(gama_pert_img,1),1)]';
    gama_pert = gama_pert';
    gama_pert(:,1) = gama_pert(:,1)./gama_pert(:,3);
    gama_pert(:,2) = gama_pert(:,2)./gama_pert(:,3);
    gama_pert(:,3) = gama_pert(:,3)./gama_pert(:,3);

    % P3P ------------------------------------------------------------------------
    dThreshRansac = perturb_levels(p)+1;
    [Rot,Transl,bestResErr,bestResErrVec, solve_time] = rf_p3p_ransac_fn(ids1, gama_pert, Gama_all, K_gt, gama_pert_img, dThreshRansac);
    % P3P END --------------------------------------------------------------------

    % We report reproj. errors on the entire perturbed ground truth:
    pert_errors_no_badj(p,:) = rf_reprojection_error(K_gt*[Rot Transl], gama_pert_img, Gama_all);
    times(p,:) = solve_time;

    disp('bundle adjustment');
    if b_adj
      % input for bundle adjustment.
      signature = ['-view_' num2str(v) '-thetapert-' num2str(tp) '-pert-' num2str(p)];
      workdir_sig = [workdir signature];
      unix(['mkdir ' workdir_sig ' 2>/dev/null']);
      cd(workdir_sig);
      
      unix('rm *.txt');
      save('image_pts.txt','gama_pert_img','-ascii','-double');
      tmp = Gama_all;
      save('world_pts.txt','tmp','-ascii','-double');
      
      % save cam.
      RC = [Rot;(-Rot'*Transl)'];
      save('camera_RC.txt','RC','-ascii','-double');
      save('camera_K.txt','K_gt','-ascii','-double');

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
      cd('..');
      unix(['rm -rf ' workdir_sig]);
    end
    disp('done bundle adjustment');

    % We report reproj. errors on the entire perturbed ground truth:
    pert_errors(p,:) = rf_reprojection_error(K_gt*[Rot Transl],...
             gama_pert_img, Gama_all);

    % Pose errors       
    T_gt = -R_gt(:,:,v)*C_gt(:,v);
    [rt_errors(p,1),rt_errors(p,2)] = rf_pose_error(R_gt(:,:,v), T_gt, Rot, Transl);

%      total_iter = total_iter + 1;
%      disp(['== finished pose computation: ' num2str(total_iter) '/'...
%            num2str(n_perturbs*n_theta_perts) '('...
%            num2str(100*total_iter/(n_perturbs*n_theta_perts)) '%)']);
  end
    all_errs{tp} = pert_errors;
    all_errs_no_badj{tp} = pert_errors_no_badj;
    all_errs_rt{tp} = rt_errors;
    all_times{tp} = times;
  end
  all_errs_p3p_views{v} = all_errs;
  all_errs_p3p_no_badj_views{v} = all_errs_no_badj;
  all_errs_p3p_rt_views{v} = all_errs_rt;
  all_times_p3p_views{v} = all_times;
end  % views
disp(['finished loops for ' num2str(v_ini) '-' num2str(v_f)]);

% usually drops into ~/lib/matlab
% also remember to save the workspace when hitting an excellent result
[stat,cpuinfo]=unix('cat /proc/cpuinfo');
[stat,gitinfo]=unix('git branch -v');
script_path = mfilename('fullpath');
timestamp = datetime('now');
script_txt=load(script_path);
repname = ['all_pairs_experiment_perturb-maxcount_' num2str(maxcount) '-ransac-sph-p3p.mat'];
save(repname,...
      'all_errs_p3p_views',...
      'all_errs_p3p_nobadj_views', ...
      'all_errs_p3p_rt_views',...
      'all_p3p_times_views',...
      'ids1','perturb_levels','theta_perturbs_deg',...
      'script_path',...
      'script_txt',...
      'timestamp',...
      'gitinfo', 'v_ini', 'v_f');
disp(['saved data to' repname]);
      
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
