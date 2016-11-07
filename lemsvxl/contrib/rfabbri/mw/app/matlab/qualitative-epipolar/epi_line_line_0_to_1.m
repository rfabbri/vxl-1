%
% Returns the epipolar line in view 1, given corresponding epipolar line in view
% 0 (el0) and geometric epipolar data structure (epi).
% el0 : column vector
%
function el1 = epi_line_line_0_to_1(epi,el0)
  M = epi.h*el0;
  el1 = [-M(2); M(1)];
  norm_el1 = norm(el1);
  el1 = el1*(epi.r(2)/norm_el1);
  if (norm(el1) < 1e-10)
    warning('Point might be too close to epipole - instabilities');
  end
