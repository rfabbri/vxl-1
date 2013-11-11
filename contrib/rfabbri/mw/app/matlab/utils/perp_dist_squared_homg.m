%: Get the square of the perpendicular distance of a point to a line.
% This is just the homogeneous form of the familiar
% \f$ \frac{a x + b y + c}{\sqrt{a^2+b^2}} \f$ :
% \f[ d = \frac{(l^\top p)}{p_z\sqrt{l_x^2 + l_y^2}} \f]
% If either the point or the line are at infinity an error message is
% printed and Inf is returned.
%
%
% Both point and line are column vectors.
%
function d = perp_dist_squared_homg(point,line)

% for efficiency reasons, I removed this.

%  if (line(1) == 0 && line(2) == 0) || point(3) == 0 
%    warning('line or point at infinity.');
%    d = Inf;
%    return;
%  end

  numerator = (line'*point / point(3));
  if (numerator == 0)
    d = 0; 
    return;
  end

  d = (numerator*numerator) /(line(1)*line(1) + line(2)*line(2));
   
