function h=cplot2(v,color)
  if (nargin ==2)
    h=plot(v(:,1),v(:,2),color);
  else
    h=plot(v(:,1),v(:,2));
  end
