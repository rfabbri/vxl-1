% this is the boundary curve of a "comma"-like figure (like a ying-yang thingy)
% The curvature can be obtained by k_dataset2.m
function [fdata,data,theta,k,kdot,t,t_angle,nrm]=dataset2(npts)

ra1 = 20;
rb1 = 30;
dra2 = 4;


% compute radius of osc. circle
k_peak = rb1/(ra1*ra1);
rc = 1/k_peak;
yc  = rb1-rc;
ra2 = 5;
rb2 = (yc-rc+rb1)/2;
% ---

theta_final = 3*pi;   % you can reduce this if you want an open curve

step = theta_final / npts;

theta=-pi/8:step:(theta_final-step);
theta=theta';

data = zeros(size(theta,1),2);
fdata = data;
% ---


% (0,y0_el2) : center of 2nd ellipse
y0_el2 = rb1 - rb2 - 2*rc;

idx0 = find(theta < pi/4);
idx1 = find(theta >= pi/4  &  theta < pi/4 + pi);
idx2 = find(theta >= pi + pi/4  &  theta < 2*pi + pi/4);
idx3 = find(theta >= 2*pi + pi/4);  %&  theta < 2*pi + pi/4 + pi/2 + pi/4);

theta0 = 3*pi/4 - theta(idx0);
theta1 = theta(idx1) -pi/4 -pi/2;
theta2 = theta(idx2) - (pi + pi/4) +pi/2;
theta3 = 1.5*pi - (theta(idx3) - (2*pi + pi/4));

data(idx0,:) = [ (ra2+dra2)*cos(theta0),  rb2*sin(theta0) + y0_el2];
data(idx1,:) = [  rc*cos(theta1),   rc*sin(theta1) + yc];
data(idx2,:) = [ ra1*cos(theta2),  rb1*sin(theta2) ];
data(idx3,:) = [ ra2*cos(theta3),  rb2*sin(theta3) + y0_el2];
% ---

fdata = myquantize(data);
% --- 


% GEOMETRY


k = zeros(size(theta));
kdot = k;
t_angle = k;
t = zeros(max(size(theta)),2);
nrm = t;

[k0,kdot0,t0,t_angle0,nrm0] = kellipse(theta0,ra2+dra2,rb2);
k(idx0)= -k0;
kdot(idx0)= kdot0;
t(idx0,:) = -t0;
t_angle0(t_angle0 < pi) = t_angle0(t_angle0 < pi) + pi;
t_angle0(t_angle0 >= pi) = t_angle0(t_angle0 >= pi) - pi;
t_angle(idx0) = t_angle0;
nrm(idx0,:) = -nrm0;

[k0,kdot0,t0,t_angle0,nrm0] = kellipse(theta1,rc,rc);
k(idx1)= k0;
kdot(idx1)= kdot0;
t(idx1,:) = t0;
t_angle(idx1) = t_angle0;
nrm(idx1,:) = nrm0;

[k0,kdot0,t0,t_angle0,nrm0] = kellipse(theta2,ra1,rb1);
k(idx2)= k0;
kdot(idx2)= kdot0;
t(idx2,:) = t0;
t_angle(idx2) = t_angle0;
nrm(idx2,:) = nrm0;

[k0,kdot0,t0,t_angle0,nrm0] = kellipse(theta3,ra2,rb2);
k(idx3)= -k0;
kdot(idx3)= kdot0;
t(idx3,:) = -t0;
t_angle0(t_angle0 < pi) = t_angle0(t_angle0 < pi) + pi;
t_angle0(t_angle0 >= pi) = t_angle0(t_angle0 >= pi) - pi;
t_angle(idx3) = t_angle0;
nrm(idx3,:) = -nrm0;

