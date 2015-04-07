function [errors,errors_gt]=rf_pose_from_point_tangents_root_find_function(id1,id2)
% This function is to be ran inside a loop, for synthetic experiments where two
% samples id1 and id2 are drawn from a pool of synthetic data points
%
%clear errors_gt;
%clear errors_gt_nrm;
%clear errors;
%
% This is the main routine to find roots.
%

% dummy input -------------------------------

%gama1 = rand(1,3);
%gama1(3) = 1;
%gama2 = rand(1,3);
%gama2(3) = 1;
%tgt1 = rand(1,3);
%tgt1(3) = 0;
%tgt2 = rand(1,3);
%tgt2(3) = 0;

%Gama1 = rand(1,3);
%Gama2 = rand(1,3);
%Tgt1 = rand(1,3);
%Tgt1 = Tgt1/norm(Tgt1);
%Tgt2 = rand(1,3);
%Tgt2 = Tgt2/norm(Tgt2);

% !dummy input ------------------------------

% synthetic input ---------------------------

%id1 = 240;
%id2 = 1100;

%id1 = 101;
%id2 = 406;

%id1 = 497;
%id2 = 798;

%id1 = 169;
%id2 = 750;

 synthetic = true;
 rf_synthetic_point_tangent_curves;
% rf_synthetic_point_tangent_simple

%synthetic = false;
%rf_real_point_tangents;

% !synthetic input ----------------------------

% compute roots -------------------------------

t_vector=-1:0.001:1;
rf_pose_from_point_tangents_2;

root_ids = rf_find_bounded_root_intervals(t_vector);

% compute rhos, r, t --------------------------

[rhos1,rhos1_minus,rhos1_plus,rhos2,rhos2_minus,rhos2_plus, ts] = ...
    rf_rhos_from_root_ids(t_vector, root_ids)

rf_get_sigmas;

rf_get_r_t_from_rhos;

% compare to ground truth rho -----------------

% Measure reprojection error for each of the rotation, translation pairs,
% with respect to all points in the synth data.

if (synthetic)
  errors_gt = rf_reprojection_error(K_gt*[R_gt T_gt], gama_all_img, Gama_all);
  errors_gt_nrm = rf_reprojection_error([R_gt T_gt], gama_all(:,1:2), Gama_all);
  errors = {};
  for i=1:length(Rots)
    errors{end+1} = rf_reprojection_error(K_gt*[Rots{i} Transls{i}], gama_all_img, Gama_all);
  end
end

Gama1_cam = R_gt*Gama1 + T_gt;
rho1_gt = Gama1_cam(3);

Gama2_cam = R_gt*Gama2 + T_gt;
rho2_gt = Gama2_cam(3);

%Gama_all_cam = R_gt*Gama_all' + T_gt*ones(1,size(Gama_all,1));
%Gama_all_cam = Gama_all_cam';

%C_gt = -R_gt'*T_gt;

%if (synthetic)
%  rf_plot_synth_data;
%end

%id1, id2, R_gt, T_gt, gama1, gama2, tgt1, tgt2, Gama1, Gama2, Tgt1, Tgt2
