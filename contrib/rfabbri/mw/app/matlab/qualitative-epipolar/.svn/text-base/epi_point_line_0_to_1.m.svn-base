% retval - false if stabilities occurred; true if not
%
%
function [el1,retval] = epi_point_line_0_to_1(epi,p0)

%  [el0,retval] = epi_line(epi,p0,0); 
% INLININE:
  dp = p0 - epi.e(1,:)';
  norm_dp = norm(dp);
  el0 = dp * (epi.r(1)/norm_dp);

  retval = true;
  if (norm_dp < 1e-10)
%    warning('Point too close to epipole - instabilities');
    retval = false;
    el1 = zeros(2,1);
    return;
  end

%  el1 = epi_line_line_0_to_1(epi,el0); 
% INLINE:
  M = epi.h*el0;
  el1 = [-M(2); M(1)];
  norm_el1 = norm(el1);
  el1 = el1*(epi.r(2)/norm_el1);
  if (norm(el1) < 1e-10)
    warning('Point might be too close to epipole - instabilities');
  end
