% plots sphere, occl contour, camera center, occl contour center

r = 20;
s_center = [0 0 80]';
c = [15 20 90]';


s = mysphere(s_center,r);

[Gamma,Gamma_center]=sphere_occl(c,r,s_center);

figure(39)
clf
cplot(s,'og');
hold on
cplot(Gamma,'or');
cplot(Gamma,'.r');
cplot(Gamma_center','or');
cplot(c','xk');
axis equal;
