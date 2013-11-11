
f=figure(40);
clf;

%---------------------------------
a=1;
bb=5;
transl=[0 30];

s_stepsize=(2*pi/760);

sineconic_define;

% camera center
c = [0 0];

s0_idx= 450;


%-----------------------

sineconic_compute;

hcirc=cplot2(Gamma);
hold on;
axis equal;
translate=0;
sineconic_draw;

sineconic_f;
