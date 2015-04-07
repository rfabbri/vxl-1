clear all;

maxcount = 500;
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

rf_synthetic_point_tangent_curves;  

% for now:
gama_pert_img = gama_all_img;
gama_pert = gama_all;
tgt_pert = tgt_all;

% RANSAC
%rf_pose_from_point_tangents_ransac;
[Rot,Transl,ResErr] = rf_pose_from_point_tangents_ransac_fn(...
ids1, gama_pert, tgt_pert, Gama_all, Tgt_all, K_gt, gama_pert_img)
