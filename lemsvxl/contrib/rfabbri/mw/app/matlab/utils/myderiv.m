% central finite differences  differentiation
%
function fd = myderiv(f,x)
  n = max(size(f));
  fd = zeros(size(f));
  idx = 2:(n-1);
  fd(idx) = (f(idx+1) - f(idx-1)) ./ (x(idx+1) - x(idx-1));

  fd(1) = ( f(2) - f(1) ) / (x(2) - x(1));
  fd(n) = ( f(n) - f(n-1) ) / (x(n) - x(n-1));
