% Experiment to evaluate how determinant of four-line constraint changes with
% reference angle axis (x-axis).

%synth_point_data;

nfig_rot_exp = 200;

% Pick points from synthetic data
p4id = p_id([8 7 5 1]);

p4_0 = sp0(p4id,:);
p4_1 = sp1(p4id,:);

p4_e0 = e0;
p4_e1 = e1;



angles = 0:0.01:2*pi;
nangles = max(size(angles));
det_values = zeros(1,nangles);

% TODO:  3D surface where ref angle in image 1 is indep of angle in image 2.

for i=1:nangles
  % rotate points and epipoles
  det_values(i) = det_value_for_angle(angles(i),p4_e0,p4_e1,p4_0,p4_1);
end

% plot det_values versus angles in degrees

figure(nfig_rot_exp);clf;
plot(angles*(180/pi),det_values);

% mark point corresponding to average reference line


e1v = ones(4,1)*p4_e1;
dp1 = p4_1 - e1v;
ns = sqrt(sum(dp1.*dp1,2));
dp1./[ns ns];

dp_avg = sum(dp1);
dp_avg = dp_avg/norm(dp_avg);
avg_angle1 = -atan2(dp_avg(2),dp_avg(1)) + 2*pi;

dv_avg = det_value_for_angle(avg_angle1,p4_e0,p4_e1,p4_0,p4_1);

hold on;
plot(avg_angle1*(180/pi),dv_avg,'ro');

figure(n_fig_poly_corresp+1);
cplot2([e1+dp_avg*(-20000); e1+dp_avg*(20000)],'r');
