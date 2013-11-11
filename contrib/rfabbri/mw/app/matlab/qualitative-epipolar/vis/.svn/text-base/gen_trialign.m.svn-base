% Plot tri-alignment curve
%
yaw_correction = 0;
[ap0,ap1]=synthetic_data('30,60',yaw_correction);
[ap3,ap2]=synthetic_data('90,180',yaw_correction);

idx=770:1120;
ap0=ap0(idx,:);
ap1=ap1(idx,:);
ap2=ap2(idx,:);
s0=myarclength(ap0);
s1=myarclength(ap1);
s2=myarclength(ap2);

figure; h=cplot([s0 s1 s2],'r');
set(h,'linewidth',2);

axis equal;
grid on;
hold on;
box on;

myz = zeros(size(s0));
cplot([myz s1 s2]);
cplot([s0 myz s2]);
cplot([s0 s1 myz]);

xlabel('s_1');
ylabel('s_2');
zlabel('s_3');
