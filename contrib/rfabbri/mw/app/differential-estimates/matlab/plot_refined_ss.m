% plots results of gradient descent refinement,
% to be used after a suitable call to smooth_compute


nfig = 4;
clearfigs = 0;

cmap = colormap;

color = cmap(round((63)*rand())+1,:);

figure(nfig)
hold on
if (clearfigs), clf, end
title 'angle difference'

plot(t_angle_el_fine - t_angle_sm,'r');
plot(t_angle_el_fine - t_angle_ref,'Color',color);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure (nfig+1)
hold on
if (clearfigs), clf, end
title 'curvature derivative';


plot(len_ref,kdot_sm,'k');
plot(len_sm_ss,kdot_el_fine,'r');

plot(len_ref,kdot_ref,'Color',color);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure(nfig+2);
hold on
if (clearfigs), clf, end
title 'curvature'

plot(len_ref,k_sm,'k');
plot(len_sm_ss,k_el_fine,'r');

plot(len_ref,k_ref,'Color',color);
