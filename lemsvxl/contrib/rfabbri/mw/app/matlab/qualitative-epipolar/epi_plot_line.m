%
% Plot an epipolar line given in Euclidean representation on current figure
%
function epi_plot_line(epi,el,view)
  p0 = epi.e(view+1,:)'
  p1 = p0 + 1.5*el;
  p0 = p0 - 1.5*el;
  cplot2([p0'; p1'],'g');

