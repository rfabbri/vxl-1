% INPUT:
% t - theta array 
% 
% OUTPUT:
% t - tangent
% nrm - normal
function [k,kdot,t,t_angle,nrm] = kellipse(t,a,b)
  if (size(t,1) > 1)
    t = t';
  end
  ab =  a*b;
  cost = cos(t);
  cost2 = cost.*cost;
  b2=b*b;
  a2=a*a;

  k = ab./(b2*cost2 + a2*(1 - cost2)).^(1.5);

  % derivative:
  sint = sin(t);

  kdot = -3*ab*cost.*sint*(a2 - b2);

  g = sqrt(a2*sint.*sint + b2*cost2);

  denom = (b2*cost2 + a2*(1 - cost2)).^2.5;

  kdot = kdot./(denom.*g);

  % tangent:

  t = [(-a*sint./g)' (b*cost./g)'];

  t_angle = atan2(t(:,2),t(:,1));

  sz = size(t_angle,1)*size(t_angle,2);
  for i=1:sz
    if t_angle(i) < 0
      t_angle(i) = t_angle(i) + 2*pi;
    end
  end

  nrm = [-t(:,2) t(:,1)];
