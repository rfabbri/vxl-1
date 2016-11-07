% Gaussian smoothing with optional irregular sampling (e.g. arclength)
% Periodic Boundary condition
%
% do_rescale (default 1): rescales the signal to prevent shrinkage.
%
% OBS: The input "s" has one more element than f, the last element being the
% final abcissa of the initial point.
function fsm = gsmi_circ(f,sigma,s,do_rescale)

alpha = 3*sigma;

if (alpha > s(end))
  disp 'Warning: sigma too large for this implementation';
end

nel = max(size(f));

if nargin == 2; s = (1:nel)'; end
if nargin < 4; do_rescale=1; end

% make sure s is a column vector 
if size(s,2) > 1; s = s'; end
if size(f,2) > 1; f = f'; end


f = [f; f; f];

s = [s(1:nel); s(1:nel) + s(nel+1) ; s(1:nel) + 2*s(nel+1)];

fsm      = zeros(size(f));
delta_s  = zeros(size(s));

idx = (nel+1):(2*nel);

idx2=2:(3*nel-1);

delta_s(idx2) = (s(idx2+1) - s(idx2-1)) * 0.5;
delta_s(1) = (s(2) - s(1)) * 0.5;
delta_s(end) = (s(end) - s(end-1)) * 0.5;
clear idx2;


g_const1 = 1/(sigma*sqrt(2*pi));
g_const2 = -0.5/(sigma*sigma);

nel = 3*nel;
for i=idx

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

fsm = fsm(idx);

if do_rescale
  % first smooth sigma=1.5
  % Then normalize
  fsm_no_quantize= gsmi(f,1.5,s,0);
  fsm=gnorm(fsm_no_quantize,fsm);
end

disp 'Circular smoothing!...'
