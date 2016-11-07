% Obtain synthetic data with rotation, translation and projected points in
% normalized image coordinates.
%
% Todo: multiply fmatrix out by K and inv(K) to output essential matrix.
%

function [pts1,pts2, fmatrix, R1, T1, R2, T2, tgts1, tgts2, pts3d, tgts3d] = synthetic_data_r_t(angles)

  [pts1,pts2, fmatrix, proj1, proj2, tgts1, tgts2, pts3d, tgts3d] = synthetic_data(angles);
  
  [K1, R1, C1] = vgg_KR_from_P(proj1);
  T1 = -R1*C1;

  s_fix= [1     0     0
          0    -1     0
          0     0     1];

  K1 = K1*(s_fix);
  R1 = -s_fix*R1;
  T1 = -s_fix*T1;

  [K2, R2, C2] = vgg_KR_from_P(proj2);
  T2 = -R2*C2;

  K2 = K2*(s_fix);
  R2 = -s_fix*R2;
  T2 = -s_fix*T2;

  % convert this output to normalized image coordinates

  K1inv = inv(K1);
  K2inv = inv(K2);

  tgt1_2d_pt = [cos(tgts1), sin(tgts1)] + pts1(:,1:2);
  tgt2_2d_pt = [cos(tgts2), sin(tgts2)] + pts2(:,1:2);

  pts1 = K1inv*[pts1 ones(size(pts1,1),1)]';
  pts1 = pts1';
  pts1(:,1) = pts1(:,1)./pts1(:,3);
  pts1(:,2) = pts1(:,2)./pts1(:,3);
  pts1(:,3) = 1;

  pts2 = K1inv*[pts2 ones(size(pts2,1),1)]';
  pts2 = pts2';
  pts2(:,1) = pts2(:,1)./pts2(:,3);
  pts2(:,2) = pts2(:,2)./pts2(:,3);
  pts2(:,3) = pts2(:,3)./pts2(:,3);;


  tgt1_2d_pt_normal = K1inv*[tgt1_2d_pt ones(size(tgt1_2d_pt,1),1)]';
  tgt1_2d_pt_normal = tgt1_2d_pt_normal';
  tgt1_2d_pt_normal(:,1) = tgt1_2d_pt_normal(:,1) ./ tgt1_2d_pt_normal(:,3);
  tgt1_2d_pt_normal(:,2) = tgt1_2d_pt_normal(:,2) ./ tgt1_2d_pt_normal(:,3);
  tgt1_2d_pt_normal(:,3) = tgt1_2d_pt_normal(:,3) ./ tgt1_2d_pt_normal(:,3);

  tgt2_2d_pt_normal = K2inv*[tgt2_2d_pt ones(size(tgt2_2d_pt,1),1)]';
  tgt2_2d_pt_normal = tgt2_2d_pt_normal';
  tgt2_2d_pt_normal(:,1) = tgt2_2d_pt_normal(:,1) ./ tgt2_2d_pt_normal(:,3);
  tgt2_2d_pt_normal(:,2) = tgt2_2d_pt_normal(:,2) ./ tgt2_2d_pt_normal(:,3);
  tgt2_2d_pt_normal(:,3) = tgt2_2d_pt_normal(:,3) ./ tgt2_2d_pt_normal(:,3);

   
  tgts1 = tgt1_2d_pt_normal - pts1;
  tgts2 = tgt2_2d_pt_normal - pts2;

  ssum = sqrt(sum(tgts1.*tgts1, 2));
  norms =  [ssum ssum ssum];
  tgts1 = tgts1 ./ norms;

  ssum = sqrt(sum(tgts2.*tgts2, 2));
  norms =  [ssum ssum ssum];
  tgts2 = tgts2 ./ norms;

