% To be called after showpts

% reproject

pts3d_h = [pts3d,ones(size(pts3d,1),1)];
ptsnew_h = pts3d_h*cam';
ptsnew = [ptsnew_h(:,1)./ptsnew_h(:,3), ptsnew_h(:,2)./ptsnew_h(:,3)];

dp = ptsnew - pts2d;

derr = sqrt(sum(dp.*dp,2))


if display_images
  hold on;
  h=cplot2(pts2d,'red.');
  set(h,'MarkerSize',10);
  h=cplot2(ptsnew,'greeno');
  set(h,'MarkerSize',7);
  for i=1:size(pts2d,1)
    cplot2([pts2d(i,:); ptsnew(i,:)],'b');
  end
end


% 3D plot

figure(plotfig3d);
hold on;

h=cplot(pts3d,'red.');

mytag= [nblock ',' nview];
set(h,'tag',mytag);
set(h,'displayname',mytag);
h=cplot(c,'blueo');
set(h,'tag',mytag);
set(h,'displayname',mytag);
vlen = c - [mean(pts3d(:,1)),mean(pts3d(:,2)),mean(pts3d(:,3))];
vlen = sqrt(sum(vlen.*vlen));
h=cplot([c;c+vlen*fvec],'green');
set(h,'tag',mytag);
set(h,'displayname',mytag);
