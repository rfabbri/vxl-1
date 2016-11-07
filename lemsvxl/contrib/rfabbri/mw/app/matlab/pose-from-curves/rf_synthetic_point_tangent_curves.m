
[gama_all, gama_b_all, ematrix, ...
R_gt, T_gt, R_b_gt, T_b_gt, ...
tgt_all, tgt_b_all, ...
Gama_all, Tgt_all] = synthetic_data_r_t('36,77');

[gama_all_img, gama_b_all_img, fmatrix, ...
proj1, proj2,...
tgt_all_img, tgt_b_all_img, ...
Gama_all, Tgt_all] = synthetic_data('36,77');

K_gt =  vgg_KR_from_P(proj1);
s_fix= [1     0     0
        0    -1     0
        0     0     1];
K_gt = K_gt*(s_fix);
K_gt_inv = inv(K_gt);

tgt_all_img_v = [cos(tgt_all_img) sin(tgt_all_img)];

clear gama_b_all fmatrix ematrix R_b_gt T_b_gt tgt2_all

if exist('id1')
  gama1 = gama_all(id1,:)';
  gama2 = gama_all(id2,:)';

  tgt1 = tgt_all(id1,:)';
  tgt2 = tgt_all(id2,:)';

  Gama1 = Gama_all(id1,:)';
  Gama2 = Gama_all(id2,:)';

  Tgt1 = Tgt_all(id1,:)';
  Tgt2 = Tgt_all(id2,:)';
end
