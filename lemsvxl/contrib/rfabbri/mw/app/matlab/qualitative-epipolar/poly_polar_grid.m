% Generates grid wrapping the polygon
%
% xypoly: nx2 vector, vertices of polygon in cartesian representation
% pts_polar: nx2 vector, points in (theta,rho) representation
% use_centroid: to also consider an arc through the centroid
%
function [pts_polar,theta_p,theta_m,theta_a,rmax,rmin,rc,rmid] = poly_polar_grid(xypoly,pole,dtheta,c0,c1)

%max_npts = 50;
max_npts = 100;
%
% At level 1, we have rmin, rc, rmax, thetamin, thetac, thetamax crossed (9
% points)
%
% At level 2, we 
%

if nargin <= 2
  x = xypoly(:,1);
  y = xypoly(:,2);
  c = [mean(x) mean(y)];
  dtheta = 0.05236/4; % 3 degrees
end

% We will have 4 radius curves for now


[theta_p,theta_m,theta_a] = polybounds(x,y,pole(1),pole(2));
if theta_p < theta_m
  error('theta_p < theta_m!');
end

% For each radius curve, from max_npts we can get what is the min dtheta
%
% And pick final dtheta = max(min_dtheta,dtheta_user)

min_dtheta = ((theta_p - theta_m)/(max_npts-1))*3;
dtheta = max(min_dtheta,dtheta);

if size(pole,1) > 1
  pole = pole'
end

nv = size(x,1);

mpole = ones(nv,1)*pole;
dv = xypoly - mpole;
nrm = sqrt(sum(dv.*dv,2));
rmax = max(nrm);
rmin = min(nrm);


rc = norm(pole-c);

rbar_max = sqrt(rmax);
rbar_min = sqrt(rmin);

rbar_mid = (rbar_max + rbar_min)/2;
rbar_min2 = (rbar_mid + rbar_min)/2;
rbar_max2 = (rbar_max + rbar_mid)/2;

rmid = rbar_mid^2;
rmin2 = rbar_min2^2;
rmax2 = rbar_max2^2;


thetas = [theta_m:dtheta:theta_p];
if thetas(end) ~= theta_p
  thetas(end+1) = theta_p;
end

pts_polar = zeros(size(thetas,2)*4,2);
ip=1;
for th =thetas
%  for r=[rmin2 rmid rc rmax2];
  for r=[rmin2 rmid rmax2];
    pts_polar(ip,1) = th;
    pts_polar(ip,2) = r;
    ip = ip + 1;
  end
end

% Restrict output to only those points inside xypoly

flag_inpoly = zeros(size(pts_polar));
for i=1:max(size(pts_polar))
  [ptx,pty] = pol2cart(pts_polar(i,1),pts_polar(i,2));
  if inpolygon(ptx,pty,x,y)
    flag_inpoly(i) = 1;
  end
end
pts_polar = pts_polar(flag_inpoly==1,:);
