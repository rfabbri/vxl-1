%
% epi : geometric epipolar datastructure
%
% Returns the epipolar line in view of point p in same view; basically joins p
% to epipole and compute our geometric representation, namely the tangent direction with
% certain radius.
%
% retval: 1 if stable, 0 if not
%
% view: 0 or 1
function [el,retval] = epi_line(epi,p,view)
  dp = p - epi.e(view+1,:)';
  norm_dp = norm(dp);
  el = dp * (epi.r(view+1)/norm_dp);

  retval = true;
  if (norm_dp < 1e-10)
%    warning('Point too close to epipole - instabilities');
    retval = false;
  end

