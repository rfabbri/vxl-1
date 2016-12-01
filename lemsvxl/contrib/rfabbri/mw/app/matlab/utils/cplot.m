function h=cplot(v,color,alpha)
  if nargin == 3
    x = v(:,1)';
    y = v(:,2)';
    z = v(:,3)';

    xflip = [x(1 : end - 1) fliplr(x)];

    yflip = [y(1 : end - 1) fliplr(y)];

    zflip = [z(1 : end - 1) fliplr(z)];

    h = patch(xflip, yflip, zflip, 'r', 'EdgeAlpha', alpha, 'EdgeColor', color);
  elseif nargin ==2
    h=plot3(v(:,1),v(:,2),v(:,3),color);
  elseif nargin == 1
    h=plot3(v(:,1),v(:,2),v(:,3));
  end
