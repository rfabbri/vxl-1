clear all;

cd /home/rfabbri/cprg/vxlprg/lemsvxlsrc-git2/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/work
maxcount = 1100;
%maxcount = 3;
b_adj = true;

[gama_all_img, gama_b_all_img, fmatrix, ...
proj1, proj2,...
tgt_all_img, tgt_b_all_img, ...
Gama_all, Tgt_all] = synthetic_data('36,77');

nsamples_pool = max(size(gama_all_img));

if (maxcount > nsamples_pool)
  error('maxcount too high');
end

clear gama_all_img gama_b_all_img fmatrix  proj1 proj2 tgt_all_img tgt_b_all_img Gama_all Tgt_all;

ids1 = zeros(maxcount,1);
ids2 = zeros(maxcount,1);

scount=0;
while scount < maxcount
  id1 = randint(1,1,[1 nsamples_pool]);
  while (~isempty(find(ids1 == id1)))
    id1 = randint(1,1,[1 nsamples_pool]);
  end
  scount = scount + 1;
  ids1(scount) = id1;

  id2 = randint(1,1,[1 nsamples_pool]);
  while (id2 ~= id1 & ~isempty(find(ids2 == id2)))
    id2 = randint(1,1,[1 nsamples_pool]);
  end
  ids2(scount) = id2;
end



rf_synthetic_point_tangent_curves;  



%perturb_levels = [0 0.1 0.5 1 2];
%theta_perturbs_deg = [0 0.1 0.5 1 2 5 7 10];
perturb_levels = [0 0.5 1 2];
theta_perturbs_deg = [0 0.5 1 5 10];

n_perturbs = length(perturb_levels);
n_theta_perts = length(theta_perturbs_deg);
total_iter=0;

all_errs = {};
all_errs_no_badj = {};
for tp = 1:n_theta_perts
  pert_errors = zeros(n_perturbs, nsamples_pool);
  tgt_pert  = perturb_tangent(tgt_all_img_v, theta_perturbs_deg(tp)*pi/180);

  % transform to world coordinates

  tgt1_2d_pt = tgt_pert + gama_all_img(:,1:2);
  tgt1_2d_pt_normal = K_gt_inv*[tgt1_2d_pt ones(size(tgt1_2d_pt,1),1)]';
  tgt1_2d_pt_normal = tgt1_2d_pt_normal';
  tgt1_2d_pt_normal(:,1) = tgt1_2d_pt_normal(:,1) ./ tgt1_2d_pt_normal(:,3);
  tgt1_2d_pt_normal(:,2) = tgt1_2d_pt_normal(:,2) ./ tgt1_2d_pt_normal(:,3);
  tgt1_2d_pt_normal(:,3) = tgt1_2d_pt_normal(:,3) ./ tgt1_2d_pt_normal(:,3);

  tgt_pert = tgt1_2d_pt_normal - gama_all;

  ssum = sqrt(sum(tgt_pert.*tgt_pert, 2));
  norms =  [ssum ssum ssum];
  tgt_pert = tgt_pert ./ norms;

  if (abs(sum(tgt_pert(:,3))) > 1e-7)
    error('something is wrong with normalizing tgt pert');
  end

  tgt_pert(:,3) = zeros(size(tgt_pert,1),1);

  for p = 1:n_perturbs
    % perturb the points in the image.
    gama_pert_img = perturb(gama_all_img, perturb_levels(p));

    % transform to world coordinates
    gama_pert = K_gt_inv*[gama_pert_img ones(size(gama_pert_img,1),1)]';
    gama_pert = gama_pert';
    gama_pert(:,1) = gama_pert(:,1)./gama_pert(:,3);
    gama_pert(:,2) = gama_pert(:,2)./gama_pert(:,3);
    gama_pert(:,3) = gama_pert(:,3)./gama_pert(:,3);

    dThreshRansac = perturb_levels(p)+1;
    [Rot,Transl] = rf_pose_from_point_tangents_ransac_fn(...
    ids1, gama_pert, tgt_pert, Gama_all, Tgt_all, K_gt, gama_pert_img, dThreshRansac);

    % We report reproj. errors on the entire perturbed ground truth:
    pert_errors_no_badj(p,:) = rf_reprojection_error(K_gt*[Rot Transl],...
             gama_pert_img, Gama_all);

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
      retval = unix('dbccl_refine_pose_app');
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
  all_errs{end+1} = pert_errors;
  all_errs_no_badj{end+1} = pert_errors_no_badj;
end

% usually drops into ~/lib/matlab
save(['all_pairs_experiment_perturb-maxcount_' num2str(maxcount) '-ransac.mat'],...
      'all_errs','ids1','ids2','perturb_levels','theta_perturbs_deg','ids1','ids2');

% Raw plot ------------------------------------------------
figure;
clf;
hold on;
for tp = 1:n_theta_perts
  for p=1:n_perturbs
    mycolor = rand(1,3)*0.7;
    mycolor(1) = min(mycolor(1)+rand()*0.4,1);
    mycolor(2) = min(mycolor(2)+rand()*0.2,1);
    mycolor(3) = min(mycolor(3)+rand()*0.1,1);

    h = plot(all_errs{tp}(p,:));
    set(h,'color',mycolor);
    set(h,'linewidth',2);
  end
end
title('Error for different samples and noise levels');
ylabel('reprojection error');
xlabel('point-tangent id');

% Hist plot ------------------------------------------------

rf_all_pairs_experiment_perturb_histplot;
