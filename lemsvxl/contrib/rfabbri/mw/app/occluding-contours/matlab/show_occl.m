% Show sphere occluding contours obtained from C++

% 1 - read files


G=myreadv(['ct-spheres/dat/Gamma-occl' suf ext]);
G2=myreadv(['ct-spheres/dat/Gamma2-occl' suf ext]);

%TODO
% - read Gamma1 center, radius, and plane normal
% - same for Gamma2

v=myreadv2(['ct-spheres/dat/xi' suf ext]);
v2=myreadv2(['ct-spheres/dat/xi2' suf ext]);


% 2 - plot
figure(15);
clf
cplot(G);
axis equal;
hold on;
cplot(G2,'r');


figure(17);
clf
cplot2(v);
axis equal;
hold on;
cplot2(v,'.');
cplot2(v2,'r');
cplot2(v2,'r.');




