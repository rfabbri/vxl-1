% plots reconstruction error for analytic sphere

% 1 - read occl contour params
% 2 - for each point in c3, compute distance, store in vector
% 3 - plot vector

[Gamma_center,Gamma_binormal,Gamma_radius] = read_occl();
n = size(c3,1);
v_error = zeros(n,1);
for i=1:n
  v_error(i) = d_pt_circle3(c3(i,:)',Gamma_radius, Gamma_center, Gamma_binormal);
end

figure(95);
clf
plot(v_error);
title('Reconstruction error (mm) for cameras 0-1');


[hd,dv12,dv21] = hdist(c3,c3_2);

figure(96);
clf
t12 = 1:size(c3,1);
t21 = 1:size(c3_2,1);
plot(t12,dv12,'-',t21,dv21,'-.');
title('Distances from reconstruction c12 to c23 and vice-versa');

figure(40);
title(['Hausdorff distance between 2 reconstructions: ' num2str(hd)]);
