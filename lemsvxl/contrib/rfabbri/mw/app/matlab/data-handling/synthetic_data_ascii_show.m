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

selected_ids = [55 43 63]; % subtract 1 to get file id!

for i=1:nviews
  hh(i)=plotCamera('Location',C(i,:),'Orientation',R(:,:,i),'AxesVisible',false,'Opacity',0.8,'Size',30,'Label',num2str(i));
%  hh(i).Visible = false;
  % show z axes
  % cplot([C(i,:); C(i,:) + 5000*R(3,:,i)]);
  for sid=1:length(selected_ids)
    if i == selected_ids(sid)
      hh(i).Visible = true;
    end
  end
end
for sid=1:length(selected_ids)
  disp ['selecting' num2str(selected_ids(sid))]
  hh(selected_ids(sid)).Color = [0.5 0.8 0];
  % show z axes
  cplot([C(selected_ids(sid),:); C(selected_ids(sid),:) + 2500*R(3,:,selected_ids(sid))]);
end

%r
