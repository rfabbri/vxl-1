distinguish_outliers = true;
if distinguish_outliers
  % To be called after SIFT_compute*rfabbri.m
  %
  % Distinguishes inliers(green)/outliers(red)

  iptsetpref( 'ImshowBorder', 'tight' ); %get rid of border
  figure(11) ; clf ;
  imshow(img1, 'InitialMagnification',100) ; colormap gray ;
  hold on ;
  h=plotsiftframe( loc_1 ) ; set(h,'LineWidth',1,'Color','r') ;
  h=plotsiftframe( loc_1_inlier ) ; set(h,'LineWidth',2,'Color','g') ;
  drawnow ;

  iptsetpref( 'ImshowBorder', 'tight' ); %get rid of border
  figure(12); clf;
  imshow(img2, 'InitialMagnification',100) ; colormap gray ;
  hold on ;
  h = plotsiftframe( loc_2 ) ; set(h,'LineWidth',1,'Color','r') ;
  h = plotsiftframe( loc_2_inlier ) ; set(h,'LineWidth',2,'Color','g') ;
  drawnow ;

else

  iptsetpref( 'ImshowBorder', 'tight' ); %get rid of border
  figure(1) ; clf ;
  imshow(img1, 'InitialMagnification',100) ; colormap gray ;
  hold on ;
  h=plotsiftframe( loc_1 ) ; set(h,'LineWidth',1,'Color','g') ;
  drawnow ;

  iptsetpref( 'ImshowBorder', 'tight' ); %get rid of border
  figure(2); clf;
  imshow(img2, 'InitialMagnification',100) ; colormap gray ;
  hold on ;
  h = plotsiftframe( loc_2 ) ; set(h,'LineWidth',1,'Color','g') ;
  drawnow ;
end
