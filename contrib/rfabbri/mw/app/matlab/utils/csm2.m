% Same as cs1 but starts from a given normal field
%
% c: digital curve
% psi: how much to evolve at a step
% nsteps: number of steps
%
function [cs]=csm2(c,psi,nrm)
  % fix endpts (or use last endpoint curvature!)

  cs = c + psi*nrm;
