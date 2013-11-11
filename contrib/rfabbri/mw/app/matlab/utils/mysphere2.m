% analytical sphere
function xyz = mysphere2(center,r)

n = 20;
dtheta = (2*pi)/20;

theta = 0:dtheta:(2*pi - dtheta);
theta = theta';

dphi = dtheta/2;
phi = 0:dphi:(pi - dphi);
phi = phi';

cosphi = cos(phi);
sinphi = sin(phi);
costheta = cos(theta);
sintheta = sin(theta);

ntheta = size(theta,1);
nphi = size(phi,1);

xyz = zeros(nphi*ntheta,3);

k=1;
for itheta=1:ntheta
  for iphi=1:nphi
    xyz(k,:) = [r*sinphi(iphi)*costheta(itheta), r*sinphi(iphi)*sintheta(itheta), r*cosphi(iphi)] + center';
    k = k+1;
  end
end


