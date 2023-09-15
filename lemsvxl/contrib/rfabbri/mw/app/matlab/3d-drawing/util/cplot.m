function h=cplot(v,color)
    view_colors = distinguishable_colors(9);
    curColor = view_colors(5,:);
  if (nargin ==2)
    h=plot3(v(:,1),v(:,2),v(:,3),color);
  else
    h=plot3(v(:,1),v(:,2),v(:,3),'Color',curColor);
  end
