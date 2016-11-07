% plots results of gradient descent refinement,
% to be used after a suitable call to smooth_compute


nfig = 7;
clearfigs = 0;

cmap = colormap;

color = cmap(round((63)*rand())+1,:);

figure(nfig)
if (clearfigs), clf, end
hold on
title 'angle difference'

plot(t_angle_el - t_angle_sm,'r');
plot(t_angle_el - t_angle_ref,'Color',color);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure (nfig+1)
if (clearfigs), clf, end
hold on
title 'curvature derivative';


plot(len_ref,kdot_sm,'k');
plot(len_sm,kdot_el,'r');

plot(len_ref,kdot_ref,'Color',color);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure(nfig+2);
if (clearfigs), clf, end
hold on
title 'curvature'

plot(len_ref,k_sm,'k');
plot(len_sm,k_el,'r');

plot(len_ref,k_ref,'Color',color);
