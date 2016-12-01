function h=cplot2_img(v,img,scale_factor)
  if nargin == 2
    scale_factor = 0.1
  end

%  h=plot3(v(:,1),v(:,2),v(:,3),color);
  npts = size(v,1);
  for i=1:npts
    h=imagesc(im,'Xdata',[v(i,],'Ydata',)
  end
  colormap(gray)
