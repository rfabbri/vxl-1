% 3rd derivative
function fd = myderiv3(f,x)

fd = myderiv(myderiv2(f,x),x);

fd(1)=fd(2);
n = max(size(f));
fd(1:2)=fd(3);
fd(n-1:n)=fd(n-2);
