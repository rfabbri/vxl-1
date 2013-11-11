function h=cplot(v,color)
  if (nargin ==2)
    h=plot3(v(:,1),v(:,2),v(:,3),color);
  else
    h=plot3(v(:,1),v(:,2),v(:,3));
  end
