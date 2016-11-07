%
% Returns the epipolar line in view 1, given corresponding epipolar line in view
% 0 (el0) and geometric epipolar data structure (epi).
% el0 : column vector
%
function el0 = epi_line_line_1_to_0(epi,el1)
  M = el1'*epi.h;
  el0 = [-M(2); M(1)];
  norm_el0 = norm(el0);
  el0 = el0*(epi.r(1)/norm_el0);
  if (norm(el0) < 1e-10)
    warning('Point might be too close to epipole - instabilities');
  end
