function xyz=sphere_point(theta,phi)

r=1;
center = [0 0 0];

cosphi = cos(phi);
sinphi = sin(phi);
costheta = cos(theta);
sintheta = sin(theta);

xyz = [r*sinphi*costheta, r*sinphi*sintheta, r*cosphi] + center;
