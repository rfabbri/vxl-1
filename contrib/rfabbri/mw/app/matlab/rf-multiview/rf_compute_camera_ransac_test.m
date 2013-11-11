%oxford_corridor_point_data
world_pts = pts3d(idx,:);
image_pts = ap0;

P = load([data_dir 'bt.000.P']);

errors = rf_reprojection_error(P, image_pts, world_pts);

disp('mean error:');
mean(errors)

disp('max error:');
max(errors)


% Now use the computed camera

%world_pts = world_pts(1:10,:);
%image_pts = image_pts(1:10,:);
myones = ones(size(world_pts,1),1);
Pc = rf_compute_camera_ransac([image_pts myones], [world_pts myones]);
errors = rf_reprojection_error(Pc, image_pts, world_pts);

disp('mean error computed:');
mean(errors)

disp('max error computed:');
max(errors)


% Now decompose to see if the intrinsics are alright
[Kc, Rc, Tc] = vgg_KR_from_P(Pc)
[K, R, T] = vgg_KR_from_P(P)


