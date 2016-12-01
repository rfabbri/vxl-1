function h=cplot2_img(v,img_db,scale)
  if nargin == 2
      scale = 1
  end

%  h=plot3(v(:,1),v(:,2),color);
  npts = size(v,1);
  for i=1:npts
    x = v(i,1)
    y = v(i,2)
    h=imagesc(reshape(img_db(:,i),100,100),'Xdata',[x+1 x+3*scale],'Ydata',[y+1 y+3*scale],'Tag',num2str(i))
  end
  axis ij
  colormap(gray)
  axis equal
