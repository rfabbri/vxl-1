% Assume defined f, t

% t=1:1000;
% f=randn(size(t));

fd =myderiv(f,t);
fdd=myderiv2(f,t);
f3d=myderiv3(f,t);

clf
clf
subplot 421
plot(f)
title f
subplot 423
plot(fd)
title '1st deriv'
subplot 425
plot(fdd)
title '2nd deriv'
subplot 427
plot(f3d)
title '3rd deriv'

fs   = gsmi(f,sigma,t',0)';
fsd  = myderiv(fs,t);
fsdd = myderiv2(fs,t);
fs3d = myderiv3(fs,t);

fs = mytrim(fs,sigma);
fsd = mytrim(fsd,sigma);
fsdd = mytrim(fsdd,sigma);
fs3d = mytrim(fs3d,sigma);

subplot 422
plot(fs)
subplot 424
plot(fsd)
subplot 426
plot(fsdd);
subplot 428
plot(fs3d);
