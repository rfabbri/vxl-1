clear all;
% TODO: dataset does chdir to working results dir
rf_real_point_tangents_dino2;
%rf_real_point_tangents_capitol2;

dThreshRansac = max(errors_gt) + 0.5;
ids1 = (1:size(gama_all,1))';

[Rot,Transl] = rf_pose_from_point_tangents_ransac_fn(...
ids1, gama_all, tgt_all, Gama_all, Tgt_all, K_gt, gama_all_img, dThreshRansac);

errors = rf_reprojection_error(K_gt*[Rot Transl], gama_all_img, Gama_all);



% to bundle adjustment
unix('./clean');
save('world_pts.txt','Gama_all','-ascii','-double');
save('image_pts.txt','gama_all_img','-ascii','-double');

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


err_badj = rf_reprojection_error(K_gt*[Rot Transl], gama_all_img, Gama_all);

% for capitol; results used in paper  were from hard-coding initial cameras into C++
%err_badj=[
%1.21308
%2.30842
%1.36222
%0.480667
%0.398039
%0.468218
%0.961341
%1.79885
%0.645944
%0.104417
%0.192395
%1.64132
%0.207486
%1.55724
%1.52959
%0.706941
%0.622476
%1.26624
%0.37247
%0.521477
%0.455129
%0.140729
%0.626633
%0.905018
%0.441843
%0.254791
%0.0538325
%0.442664
%0.291859];

rf_pose_from_point_tangents_ransac_run_plot;
