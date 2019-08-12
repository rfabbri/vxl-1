clear all;

cd ('~/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/work')
%maxcount = 1100;  % how many ransac-like iterations (how many random samples)
%maxcount = 3;
b_adj = true;

% TODO rewrite to use my static dataset
%%%% Four comments %%%% = using the value loaded from .mat file
%%%%[gama_all_img, gama_b_all_img, fmatrix, ...
%%%%proj1, proj2,...
%%%%tgt_all_img, tgt_b_all_img, ...
%%%%Gama_all, Tgt_all] = synthetic_data('36,77');

load('~/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/working-state-synthetic-ransac_results-paper.mat')
% maxcount = 1000 from this load

nsamples_pool = max(size(gama_all));

if (maxcount > nsamples_pool)
  error('maxcount too high');
end

%%%%%clear gama_all_img gama_b_all_img fmatrix  proj1 proj2 tgt_all_img tgt_b_all_img Gama_all Tgt_all;

%%%%% Commented out to reproduce ECCV12 data to compare against p2pt -----------
%%%%% ids1 = zeros(maxcount,1);
%%%%% ids2 = zeros(maxcount,1);
%%%%% ids3 = zeros(maxcount,1);
%%%%% 
%%%%% scount=0;
%%%%% while scount < maxcount
%%%%%   % id1 represents a permutation of 1..nsamples_pool
%%%%%   id1 = randi([1 nsamples_pool],1,1);    
%%%%%   while (~isempty(find(ids1 == id1)))
%%%%%     id1 = randi([1 nsamples_pool],1,1);
%%%%%   end
%%%%%   scount = scount + 1;
%%%%%   ids1(scount) = id1;
%%%%% 
%%%%% %  id2 = randi([1 nsamples_pool],1,1);
%%%%% %  while (id2 ~= id1 & ~isempty(find(ids2 == id2)))
%%%%% %    id2 = randi([1 nsamples_pool],1,1);
%%%%% %  end
%%%%% %  ids2(scount) = id2;
%%%%% %  
%%%%% %  id3 = randi([1 nsamples_pool],1,1);
%%%%% %  while (id3 ~= id3 & ~isempty(find(ids3 == id3)))
%%%%% %    id3 = randi([1 nsamples_pool],1,1);
%%%%% %  end
%%%%% %  ids3(scount) = id3;
%%%%% end


% TODO use my rewrite to use txt dataset
%%%%% rf_synthetic_point_tangent_curves;  

%perturb_levels = [0 0.1 0.5 1 2];
%theta_perturbs_deg = [0 0.1 0.5 1 2 5 7 10];
%%%% perturb_levels = [0 0.5 1 2];

%%%%%n_perturbs = length(perturb_levels);
total_iter=0;
all_errs_p3p = {}; % same format as P2Pt
all_errs_p3p_no_badj = {};
pert_errors = zeros(n_perturbs, nsamples_pool);
for p = 1:n_perturbs
  % perturb the points in the image.
  gama_pert_img = perturb(gama_all_img, perturb_levels(p));

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

  if b_adj
    % input for bundle adjustment.
    unix('./clean');
    save('image_pts.txt','gama_pert_img','-ascii','-double');
    save('world_pts.txt','Gama_all','-ascii','-double');
    
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
  end
  
  % We report reproj. errors on the entire perturbed ground truth:
  pert_errors(p,:) = rf_reprojection_error(K_gt*[Rot Transl],...
           gama_pert_img, Gama_all);

  total_iter = total_iter + 1;
  disp(['== finished pose computation: ' num2str(total_iter) '/'...
        num2str(n_perturbs*n_theta_perts) '('...
        num2str(100*total_iter/(n_perturbs*n_theta_perts)) '%)']);
end
all_errs_p3p{end+1} = pert_errors; % used to store more than one entry for tangent pert
all_errs_p3p_no_badj{end+1} = pert_errors_no_badj;
all_errs = all_errs_p3p;
all_errs_no_badj = all_errs_p3p_no_badj;

save(['all_pairs_experiment_perturb-maxcount_' num2str(maxcount) '-ransac-p3p.mat'],...
      'all_errs','all_errs_no_badj','ids1','perturb_levels','theta_perturbs_deg','ids1');

% Raw plot ------------------------------------------------
%figure;
%clf;
%hold on;
%for tp = 1:n_theta_perts
%  for p=1:n_perturbs
%    mycolor = rand(1,3)*0.7;
%    mycolor(1) = min(mycolor(1)+rand()*0.4,1);
%    mycolor(2) = min(mycolor(2)+rand()*0.2,1);
%    mycolor(3) = min(mycolor(3)+rand()*0.1,1);

%    h = plot(all_errs_p3p{tp}(p,:));
%    set(h,'color',mycolor);
%    set(h,'linewidth',2);
%  end
%end
%title('Error for different samples and noise levels');
%ylabel('reprojection error');
%xlabel('point-tangent id');

% Hist plot ------------------------------------------------

%rf_all_pairs_experiment_perturb_histplot;

% Box plot ------------------------------------------------
%rf_all_pairs_experiment_perturb_boxplot;
