% Modified by rfabbri Tue Jan 15 10:56:47 EST 2008
show_ransac_inliers = true;
if show_ransac_inliers
  figure(21) ; clf ;
  h = plotmatches(img1,img2,loc_1(1:2,:),loc_2(1:2,:), sift_matches') ;
  set(h,'Color','g');
  title([num2str(max(size(sift_matches))) ' matched points by nearest/next nearest neighbor method']);
  drawnow;

  figure(22) ; clf ;
  h = plotmatches(img1,img2,loc_1(1:2,:),loc_2(1:2,:), matches') ;
  set(h,'Color','g','Linewidth',1);
  title([num2str(max(size(matches))) ' inliers after RANSAC']);
  drawnow ;
else
  figure(1) ; clf ;
  h = plotmatches(img1,img2,loc_1(1:2,:),loc_2(1:2,:), sift_matches') ;
  title([num2str(max(size(sift_matches))) ' matched points by nearest/next nearest neighbor method' ]);
  drawnow ;

  % figure(2) ; clf ;
  % plotmatches(img1,img2,loc_1(1:2,:),loc_2(1:2,:), matches') ;
  % title('inliers after RANSAC');
  % drawnow ;
end
