% variant of gsmi, but at boundaries consider a partial gaussian normalized to
% unit area
% This variant modifies endpoints.
function fsm = gsmi(f,sigma,s)

alpha = 2*sigma;

nel = max(size(f,1));

if nargin == 2
  s = (1:nel)';
end

% make sure s is a column vector 
if size(s,2) > 1
  s = s';
end
if size(f,2) > 1
  f = f';
end

fsm = zeros(size(f));

% Define delta_s

delta_s = zeros(size(s));

idx = 2:(nel-1);
delta_s(idx) = (s(idx+1) - s(idx-1)) * 0.5;
delta_s(1)   = s(2) - s(1);
delta_s(nel) = s(nel) - s(nel-1);


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


% do the same things for i=1

i = 1;
imin = 1;
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


% do the same things for i=nel
i = nel;
imax = nel;
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

I = (imin:imax)';

x = s(I) - s(i);
g = g_const1*exp(g_const2*x.*x);

g_area = g'*delta_s(I);
g = g/g_area;

fsm(i) = g'*(delta_s(I).*f(I));
