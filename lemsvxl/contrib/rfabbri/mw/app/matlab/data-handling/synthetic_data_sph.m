[pts, tgts, pts3d, tgts3d, K, R, C] = synthetic_data_sph()
% returns 3D-2D points for all views, from 1 to 100
% all in image cooords

mydir='~/lib/data/synthcurves-multiview-3d-dataset/spherical-ascii-100_views-perturb-radius_sigma10-normal_sigma0_01rad-minsep_15deg-no_two_cams_colinear_with_object/'

% Data read --------------------------------------------------------------------
fname = dir(mydir + '*.extrinsic');
nviews = length(fname);
extrinsics = zeros(4,3,nviews);

for i=1:nviews
  extrinsics(:,:,i) = load(mydir + fname(i).name);
end

C = squeeze(extrinsics(4,:,:))';
R = extrinsics(1:3,:,:);

pts3d = load(mydir+'crv-3D-pts.txt');
npts = size(pts3d,1);
fname = dir(mydir + '*-pts-2D*');
pts = zeros(npts,2,nviews);
for i=1:npts
  pts(:,:,i) = load(mydir + fname(i).name);
end
fname = dir(mydir + '*-tgts-2D*');
tgts = zeros(npts,2,nviews);
for i=1:npts
  tgts(:,:,i) = load(mydir + fname(i).name);
end

K=load(mydir + 'calib.intrinsic');
