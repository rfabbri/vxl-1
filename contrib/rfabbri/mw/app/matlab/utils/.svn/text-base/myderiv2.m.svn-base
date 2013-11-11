% 2nd derivative
function fd = myderiv2(f,x)
  n = max(size(f));
  idx = 2:(n-1);
  fd = zeros(size(f));

  h = x(2) - x(1);

  fd(idx) = (f(idx-1) -2*f(idx) + f(idx+1))/(h*h);

  fd(1) = fd(2);
  fd(n) = fd(n-1);
