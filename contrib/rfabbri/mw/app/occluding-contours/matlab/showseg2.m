function showseg2(i,iteration,img,phi,phi2)
% Shows result of levelset segmentation
% i: slice number
% offset: figure offset
% img: 3D image
% phi: 3D initial levelet (plot in green)
% phi: 3D final levelset to be compared (plot in red)

plotoffset=10;
h=figure(plotoffset+i+iteration);
clf
imshow(img(:,:,i),[]);
scrsz = get(0,'ScreenSize');
set(h,'Position',[1 scrsz(4) scrsz(3) scrsz(4)])
hold on;
contour(phi(:,:,i),[0 0],'g');
contour(phi2(:,:,i),[0 0],'r');
title(['Iteration #' int2str(iteration) '; slice ' int2str(i) ' of ' int2str(size(img,3))]);

