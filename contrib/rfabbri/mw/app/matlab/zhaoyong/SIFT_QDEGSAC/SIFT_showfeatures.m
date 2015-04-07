  iptsetpref( 'ImshowBorder', 'tight' ); %get rid of border
  figure(3) ; clf ;
  imshow(img1, 'InitialMagnification',100) ; colormap gray ;
  hold on ;
  h=plotsiftframe( loc_1 ) ; set(h,'LineWidth',1,'Color','g') ;
  drawnow ;

  iptsetpref( 'ImshowBorder', 'tight' ); %get rid of border
  figure(4); clf;
  imshow(img2, 'InitialMagnification',100) ; colormap gray ;
  hold on ;
  h = plotsiftframe( loc_2 ) ; set(h,'LineWidth',1,'Color','g') ;
  drawnow ;
