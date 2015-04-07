clear all;

maxcount = 1000;
%maxcount = 3;

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


%perturb_levels = [0 0.1 0.5 1 2];
%theta_perturbs_deg = [0 0.1 0.5 1 2 5 7 10];
perturb_levels = [0 0.5 1 2];
theta_perturbs_deg = [0 0.5 1 5 10];

n_perturbs = length(perturb_levels);
n_theta_perts = length(theta_perturbs_deg);

% pairs are generated; now run.

rf_synthetic_point_tangent_curves;  

total_iter=0;
all_errs = {};
all_dets = {};
for tp = 1:n_theta_perts
  avg_errors = zeros(n_perturbs,maxcount);
  dets = zeros(n_perturbs,maxcount);
%  tgt_pert  = perturb_tangent(tgt_all(:,1:2), theta_perturbs_deg(tp)*pi/180);
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

    allerrs = zeros(maxcount,nsamples_pool);
    for i=1:maxcount
      id1 = ids1(i);
      id2 = ids2(i);

      disp(['starting pose computation for id1=' num2str(id1), ' id2=', num2str(id2)]);

      [Rots, Transls, degen] = rf_pose_from_point_tangents_root_find_function_any(...
      gama_pert(id1,:)', tgt_pert(id1,:)', gama_pert(id2,:)', tgt_pert(id2,:)',...
      Gama_all(id1,:)', Tgt_all(id1,:)', Gama_all(id2,:)', Tgt_all(id2,:)');

      total_iter = total_iter + 1;
      disp(['finished pose computation: ' num2str(total_iter) '/'...
      num2str(n_perturbs*n_theta_perts*maxcount) '('...
      num2str(100*total_iter/(n_perturbs*n_theta_perts*maxcount)) '%)']);

      dets(p,i) = degen;

      errors = {};
      for ii=1:length(Rots)
        errors{end+1} = rf_reprojection_error(K_gt*[Rots{ii} Transls{ii}], gama_all_img, Gama_all);
      end
      
      if isempty(errors)
        allerrs(i,:) = -1;
        avg_errors(p,i) = -1;
      else
        % Accumulate the errors
        %   - Get best vector
        minsum = sum(errors{1});
        k_min = 1;
        for k=2:max(size(errors))
          minsum_tmp = sum(errors{k});
          if (minsum_tmp < minsum)
            minsum = minsum_tmp;
            k_min = k;
          end
        end
        allerrs(i,:) = errors{k_min};
        avg_errors(p,i) = median(allerrs(i,:));
      end
    end
  end
  all_errs{end+1} = avg_errors;
  all_dets{end+1} = dets;
end

% usually drops into ~/lib/matlab
save(['all_pairs_experiment_perturb-maxcount_' num2str(maxcount) '.mat'],...
      'all_errs','all_dets','ids1','ids2','perturb_levels','theta_perturbs_deg','ids1','ids2');

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
ylabel('average reprojection error');
xlabel('sample id');

% Hist plot ------------------------------------------------

rf_all_pairs_experiment_perturb_histplot;
