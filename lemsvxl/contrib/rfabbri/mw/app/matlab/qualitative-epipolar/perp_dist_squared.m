% XXX IN CONSTRUCTION TODO
%: Get the square of the perpendicular distance of a point to a line.
% This is just the homogeneous form of the familiar
% \f$ \frac{a x + b y + c}{\sqrt{a^2+b^2}} \f$ :
% \f[ d = \frac{(l^\top p)}{p_z\sqrt{l_x^2 + l_y^2}} \f]
% If either the point or the line are at infinity an error message is
% printed and Inf is returned.
%
function d = perp_dist_squared(p,l)

  if (l(1) == 0 && l(2) == 0) || p(3) == 0 
    warning('line or point at infinity.');
    d = Inf;
    return;
  end

  numerator = (sum(line.*point) / point(3))^2;
  if (numerator == 0) 
    d = 0; 
    return;
  end

  denominator = line(1)*line(1) + line(2)*line.b(2);

  d = numerator / denominator;
   
