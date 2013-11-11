% Gaussian smoothing with optional irregular sampling (e.g. arclength)
% Boundary condition:
%   This does not filter the endpoint elements.
%   but near them, it considers a partial gaussian normalized to unit area.
%
% do_rescale (default 1): rescales the signal to prevent shrinkage.
function fsm = gsmi(f,sigma,s,do_rescale)



alpha = 6*sigma;

nel = max(size(f));

if nargin == 2
  s = (1:nel)';
end

if nargin < 4
  do_rescale=1;
end

% make sure s is a column vector 
if size(s,2) > 1
  s = s';
end
if size(f,2) > 1
  f = f';
end

fsm = zeros(size(f));

% fix endpoints
fsm(1)   = f(1); 
fsm(nel) = f(nel);

delta_s  = zeros(size(s));

idx = 2:(nel-1);
delta_s(idx) = (s(idx+1) - s(idx-1)) * 0.5;
delta_s(1)   = (s(2) - s(1)) * 0.5;
delta_s(nel) = (s(nel) - s(nel-1)) * 0.5;


g_const1 = 1/(sigma*sqrt(2*pi));
g_const2 = -0.5/(sigma*sigma);

for i=2:(nel-1)

  % determine I=i_min:i_max where the gaussian will be applied

  %
  % imin
  %
  k = i-1;
  while ( s(i) - s(k) <= alpha )  &  ( k ~= 1 )
    k = k - 1;
  end

  if k ~= 1
    imin = k + 1;
  elseif s(i) - s(k) <= alpha
    imin = 1;
  else
    imin = 2;
  end

  %
  % imax
  %
  k = i + 1;

  while ( s(k) - s(i) <= alpha ) & ( k ~= nel)
    k = k + 1;
  end

  if k ~= nel
    imax = k - 1;
  elseif s(k) - s(i) <= alpha
    imax = nel;
  else
    imax = nel-1;
  end

  I = (imin:imax)';

  x = s(I) - s(i);
  g = g_const1*exp(g_const2*x.*x);

  g_area = g'*delta_s(I);
  g = g/g_area;

  fsm(i) = g'*(delta_s(I).*f(I));
end


if do_rescale
  % first smooth sigma=1.5
  % Then normalize
  fsm_no_quantize= gsmi(f,1.5,s,0);
  fsm=gnorm(fsm_no_quantize,fsm);
end
