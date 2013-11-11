%
% Plot an epipolar line given in Euclidean representation on current figure
%
function h=plot_homg_line(l,radius)

  if nargin < 2
    radius = 1000;
  end

  N = [l(1) ; l(2)];
  l = l/norm(N);
  N = N / norm(N);

  pc_y = [0 -l(3)/N(2)]';
  pc_x = [-l(3)/N(1) 0]';

  if abs(pc_y(2)) < abs(pc_x(1))
    pc = pc_y;
  else
    pc = pc_x;
  end

  T = [-N(2); N(1)];

  r=radius;
  p1 = pc + r*T;
  p0 = pc - r*T;

  h=cplot2([p0'; p1'],'g');
  cplot2([pc'],'kx');
