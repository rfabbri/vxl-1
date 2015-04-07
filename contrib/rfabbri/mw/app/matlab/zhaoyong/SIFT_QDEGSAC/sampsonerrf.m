function err = sampsonErrf( F, x, xp )
x(:,3) = 1;
xp(:,3) = 1;
L = F * x';
Lp = F' * xp';
num = sum(xp' .* L).^2;
den = L(1,:).^2 + L(2,:).^2 + Lp(1,:).^2 + Lp(2,:).^2;
err = sum(num ./ den) / size(x,1);




