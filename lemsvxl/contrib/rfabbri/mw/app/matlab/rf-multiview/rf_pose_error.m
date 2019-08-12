function [dR, dT] = rf_pose_error(R_gt, T_gt, R, T)
  skew2v =@(S) [S(3,2);S(1,3);S(2,1)]; % from skew matrix to vector
  dR = norm(skew2v(R*R_gt'));
  dT = norm(T-T_gt);
