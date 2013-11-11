% Fit my model
s0 = (s0_idx-1)*s_stepsize;
Tbar = [a*s0 + transl(1), bb*sin(s0) + transl(2)];
Gamaprime = [a, bb*cos(s0)];
Gama2prime = [0, -bb*sin(s0)];
g = norm(Gamaprime);
gprime = -bb*bb*sin(s0)*cos(s0)/g;
tgt = Gamaprime/g;
nrm = [-tgt(2), tgt(1)];
kvector = (Gama2prime*g - Gamaprime*gprime)/(g*g*g);
k = nrm*kvector';
K = [k 0; 0 0];
Rbar = [tgt(1), nrm(1); tgt(2), nrm(2)];
ybarmax = 10;
xbarmax = min([sqrt(ybarmax*2/abs(k)),10]);
xbar = -xbarmax:0.01:xbarmax;
ybar = 0.5*k*(xbar.*xbar);
Gammabar_model = [xbar', ybar'];
Gamma_model = Rbar*Gammabar_model' + Tbar' * ones(1,size(xbar,2));

% Fit Shashua around s0_idx

% Select 4 points

if (s0_idx < 5)
  s0_idx = 5;
elseif (s0_idx > size(Gamma,1)-5)
  s0_idx = size(Gamma,1)-5;
end

if (s0_idx == 1);
    sm1_idx = size(Gamma,1); else sm1_idx = s0_idx - 1;end
if (sm1_idx == 1);
    sm2_idx = size(Gamma,1); else sm2_idx = sm1_idx - 1;end

if (s0_idx == size(Gamma,1));
    s1_idx = 1; else s1_idx = s0_idx + 1;end
if (s1_idx == size(Gamma,1));
    s2_idx = 1; else s2_idx = s1_idx + 1;end
if (s2_idx == size(Gamma,1));
    s3_idx = 1; else s3_idx = s2_idx + 1;end

pm2 = Gamma(sm2_idx,:);
pm1 = Gamma(sm1_idx,:);
p0 = Gamma(s0_idx,:);
p1 = Gamma(s1_idx,:);
p2 = Gamma(s2_idx,:);

A = [ pm2(1)*pm2(1), pm2(1)*pm2(2), pm2(2)*pm2(2), pm2(1)];
A = [A; [ pm1(1)*pm1(1), pm1(1)*pm1(2), pm1(2)*pm1(2), pm1(1)] ];
A = [A; [ p0(1)*p0(1), p0(1)*p0(2), p0(2)*p0(2), p0(1)]];
A = [A; [ p1(1)*p1(1), p1(1)*p1(2), p1(2)*p1(2), p1(1)]];
A = [A; [ p2(1)*p2(1), p2(1)*p2(2), p2(2)*p2(2), p2(1) ]];

b = [-pm2(2); -pm1(2); -p0(2); -p1(2); -p2(2);];
%b = [-pm2(2); -pm1(2); -p0(2); -p1(2);];
qc = A\b;

disp 'Residuals:'
(A*qc-b)'

disp 'Shashua Quadric Coefficients:'
disp([num2str(qc(1)) 'x^2  ' num2str(qc(2)) 'xy  ' num2str(qc(3)) 'y^2  '...
      num2str(qc(4)) 'x + y = 0']);

disp 'Paraboloid Coefficients:'
Kc = Rbar*K*(Rbar');
disp([num2str(Kc(1,1)) 'x^2  ' num2str(2*Kc(1,2)) 'xy  ' num2str(Kc(2,2)) 'y^2  '...
      num2str(-Rbar(1,2)) 'x  ' num2str(-Rbar(2,2)) 'y  ' num2str(Rbar(1,2)*Tbar(1))...
      '   ' num2str(Rbar(2,2)*Tbar(2)) ' = 0']);
disp  '   Actual Parameters: point p0, tangent t, curvature k (4 numbers)';
disp(['                                                     k = ' num2str(k)]);

% Applying to any p,
% f(p) =  [ pm2(1)*pm2(1), pm2(1)*pm2(2), pm2(2)*pm2(2), pm2(1)]*qc + pm2(2);

% Plot it as 0-contour:

xx = (min([Gamma(:,1); -2])-10): 0.5 : (max([Gamma(:,1); 2])+15);
yy = (min([Gamma(:,2); -2])-10): 0.5 : (max([Gamma(:,2); 2])+15);
zz = zeros(length(yy),length(xx));

for i=1:length(xx)
  for j = 1:length(yy)
    zz(j,i) = [ xx(i)*xx(i), xx(i)*yy(j), yy(j)*yy(j), xx(i)]*qc + yy(j);
  end
end
