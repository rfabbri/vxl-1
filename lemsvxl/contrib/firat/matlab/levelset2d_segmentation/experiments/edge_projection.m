% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/experiments/edge_projection.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 31, 2011


pe1 = [12.1 13.4 7.5; 21.16 18.8 7.1; 6.75 2.1 7.44];
ne1 = [1 2 3; 3 2 1; 3 1 2];

for i = 1:3
pe = pe1(i,:);
ne = ne1(i,:);
ne = ne/norm(ne);
he = ne*pe';

ps = [1 1 7];
ns = [0 0 1];
hs = ns*ps';

ce = (he - hs*(ne*ns')) / (1 - (ne*ns')^2);
cs = (hs - he*(ne*ns')) / (1 - (ne*ns')^2);

p1 = ce*ne + cs*ns;
p2 = cross(ne, ns);

lambda = (p2 * (pe - p1)') / (p2*p2');

projected_point = p1 + lambda*p2
end

 
