
f=figure(40);
clf;

%---------------------------------
r=10;
c_circ=[0 50];

s_stepsize=(2*pi/360);

conic_define;

% camera center
c = [0 0];

s0_idx= 275;


%-----------------------

conic_compute;

hcirc=cplot2(Gamma);
hold on;
axis equal;
translate=0;
conic_draw;

conic_f;
