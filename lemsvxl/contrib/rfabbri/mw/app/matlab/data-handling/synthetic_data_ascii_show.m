% Data read ---------------------------------------------------------------------
clear
mydir="~/data/rfabbri/out-tmp/";
fname = dir(mydir + '*.extrinsic');
nviews = length(fname);
extrinsics = zeros(4,3,nviews);

for i=1:nviews
  extrinsics(:,:,i) = load(mydir + fname(i).name);
end

C = squeeze(extrinsics(4,:,:))';
R = extrinsics(1:3,:,:);

% Statistics --------------------------------------------------------------------

angles=zeros(nviews,nviews);
for i=1:nviews
  for j=1:nviews
    angles(i,j) = atan2d(norm(cross(C(i,:),C(j,:))),dot(C(i,:),C(j,:)));
  end
end
% This has to be the minimum in degrees in the data!
min(angles(angles>0))

% Display -----------------------------------------------------------------------
pts3d = load(mydir+'crv-3D-pts.txt');
cplot(pts3d,'.');
hold on
axis equal
%cplot(C,'o');


for i=1:nviews
  hh(i)=plotCamera('Location',C(i,:),'Orientation',R(:,:,i),'AxesVisible',true,'Opacity',0,'Size',50);
  % show z axes
  % cplot([C(i,:); C(i,:) + 5000*R(3,:,i)]);
end



