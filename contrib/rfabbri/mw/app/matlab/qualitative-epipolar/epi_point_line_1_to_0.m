% retval - false if stabilities occurred; true if not
%
%
function [el0,retval] = epi_point_line_1_to_0(epi,p1)

%  [el1,retval] = epi_line(epi,p1,1); 
  dp = p1 - epi.e(2,:)';
  norm_dp = norm(dp);
  el1 = dp * (epi.r(2)/norm_dp);

  retval = true;
  if (norm_dp < 1e-10)
%    warning('Point too close to epipole - instabilities');
    retval = false;
    el0 = zeros(2,1);
    return;
  end

%  el0 = epi_line_line_1_to_0(epi,el1); 
% INLINE:
  M = el1'*epi.h;
  el0 = [-M(2); M(1)];
  norm_el0 = norm(el0);
  el0 = el0*(epi.r(1)/norm_el0);
  if (norm(el0) < 1e-10)
    warning('Point might be too close to epipole - instabilities');
  end
