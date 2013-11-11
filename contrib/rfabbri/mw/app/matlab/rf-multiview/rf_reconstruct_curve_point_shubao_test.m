num_samples_per_point = 10
[pts1,pts2,fm,pr1,pr2,tgts1,tgts2,pts3d_gt,tgts3d_gt] = synthetic_data('0,2',0,1);
[pts2,pts3,fm,pr2,pr3,tgts2,tgts3,pts3d_gt,tgts3d_gt] = synthetic_data('2,4',0,1);

noiselevel = 0.3;
pts1 = perturb(pts1,noiselevel);
pts2 = perturb(pts2,noiselevel);
pts3 = perturb(pts3,noiselevel);


[pts_dummy,pts2_dummy,fm_dummy,pr_dummy,pr2_dummy,tgts_dymmy,dummy,p3d_gt,tgt3d_gt] = synthetic_data('2,4');

npts = size(pts1,1);

projmatrix{1} = pr1;
projmatrix{2} = pr2;
projmatrix{3} = pr3;

pts3d = zeros(npts*num_samples_per_point,3);
normals = zeros(npts*num_samples_per_point,3);
confs = zeros(npts*num_samples_per_point,4);

% reconstruct around each point of the central frame.
for i=1:npts
  edgels{1}.x = pts1(i,1);
  edgels{1}.y = pts1(i,2);
  edgels{1}.orientation = tgts1(i);

  edgels{2}.x = pts2(i,1);
  edgels{2}.y = pts2(i,2);
  edgels{2}.orientation = tgts2(i);

  edgels{3}.x = pts3(i,1);
  edgels{3}.y = pts3(i,2);
  edgels{3}.orientation = tgts3(i);

  [pts3d_i, normals_i, conf_i] = rf_reconstruct_curve_point_shubao(edgels,...
                                 projmatrix, num_samples_per_point);
  % append the 3D points to the global list.
  ioffset = (i-1)*(num_samples_per_point) + 1;
  range = ioffset:(ioffset+num_samples_per_point-1); 
  pts3d(range, :) = pts3d_i';
  normals(range, :) = normals_i';
  confs(range, :) = conf_i';
  
end

pts3d_lsq = torr_triangulate([pts1 pts3], 1, pr1, pr3);
pts3d_lsq = pts3d_lsq';
pts3d_lsq = pts3d_lsq(:,1:3)./[pts3d_lsq(:,4) pts3d_lsq(:,4) pts3d_lsq(:,4)];

clf
cplot(pts3d_lsq,'-');
%cplot(pts3d_lsq,'.');
hold on
%cplot(p3d_gt,'g-');
%cplot(p3d_gt,'g.');
%cplot(pts3d,'r+');
cplot(pts3d,'r');

hold on
xyz = mysphere([0 0 0]', 10);
cplot(xyz,'g.');
%quiver3(pts3d(:,1), pts3d(:,2), pts3d(:,3), normals(:,1), normals(:,2), normals(:,3),3);

axis equal
axis tight
grid on;
