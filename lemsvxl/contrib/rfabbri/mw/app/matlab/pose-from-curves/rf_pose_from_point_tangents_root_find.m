clear all;
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

% No computation:

% Tough guy: No perturb generates results. Real degeneracy?
%id1 = 188;
%id2 = 261;

%id1 = 915;
%id2 = 801;
% when id1=914, ts -0.4127   -0.3936    0.3756    0.3952

%id1 = 339;
%id2 = 267;

% Computation with high reprojection error
%id1 = 169;
%id2 = 309;

%id1 =  1109 ;
%id2 = 1105;

%id1 = 858;
%id2 = 1032;

% 800 : 
%Gama2 =

%   21.1557
%   -3.7524
%   -2.1282

%Tgt2 =

%   -0.1983
%    0.9404
%   -0.2763


%Gama2 =

%   21.2954
%   -4.4330
%   -1.9296

%Tgt2 =

%-0.1882
%0.9433
%-0.2734

%synthetic = true;
% rf_synthetic_point_tangent_curves
% rf_synthetic_point_tangent_simple

synthetic = false;
% rf_real_point_tangents;
rf_real_point_tangents_dino2;
% rf_real_point_tangents_capitol;

% !synthetic input ----------------------------

% test for geometric degeneracy -------------------------------

DGama = Gama1 - Gama2;
DGama = DGama/norm(DGama);
degen = det([DGama Tgt1 Tgt2]);
if (abs(degen) < 1e-3)
  error('data point not reliable');
end

% compute roots -------------------------------

t_vector=-1:0.001:1;
rf_pose_from_point_tangents_2;

[root_ids,sampled_poly] = rf_find_bounded_root_intervals(t_vector);

% compute rhos, r, t --------------------------

[rhos1,rhos1_minus,rhos1_plus,rhos2,rhos2_minus,rhos2_plus, ts] = ...
    rf_rhos_from_root_ids(t_vector, root_ids)

rf_get_sigmas;

rf_get_r_t_from_rhos;

% compare to ground truth rho -----------------

% Measure reprojection error for each of the rotation, translation pairs,
% with respect to all points in the synth data.

%if (synthetic)
  errors_gt = rf_reprojection_error(K_gt*[R_gt T_gt], gama_all_img, Gama_all);
%  errors_gt_nrm = rf_reprojection_error([R_gt T_gt], gama_all(:,1:2), Gama_all);
  errors = {};
  for i=1:length(Rots)
    errors{end+1} = rf_reprojection_error(K_gt*[Rots{i} Transls{i}], gama_all_img, Gama_all);
  end
%else
  % errors using dataset camera:
%  errors_data = [norm(gama1_img  - gama1_reproj) norm(gama2_img  - gama2_reproj)];

  % errors using newfound cameras:
%  errors = {};
%  for i=1:length(Rots)
%    errors{end+1} = rf_reprojection_error(K_gt*[Rots{i} Transls{i}], ...
%         [gama1_img(1:2)'; gama2_img(1:2)'], [Gama1'; Gama2']);
%  end
%end

Gama1_cam = R_gt*Gama1 + T_gt;
rho1_gt = Gama1_cam(3);

Gama2_cam = R_gt*Gama2 + T_gt;
rho2_gt = Gama2_cam(3);


C_gt = -R_gt'*T_gt;

if (synthetic)
  Gama_all_cam = R_gt*Gama_all' + T_gt*ones(1,size(Gama_all,1));
  Gama_all_cam = Gama_all_cam';

  rf_plot_synth_data;
end

%for i=1:length(t_vector);
%  yy(i) = rf_pose_from_point_tangents_2_fn_t(t_vector(i));
%end
yy = sampled_poly;

figure;
clf;
plot(t_vector, yy);
hold on;
plot(ts, zeros(size(ts)),'ro');
grid on;

disp('Det(A)');
det(A)
degen
%title(['data ids: ' num2str(id1) '-' num2str(id2)]);

%id1, id2, R_gt, T_gt, gama1, gama2, tgt1, tgt2, Gama1, Gama2, Tgt1, Tgt2
