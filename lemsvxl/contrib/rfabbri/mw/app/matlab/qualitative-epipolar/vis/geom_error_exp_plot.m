% Plot curves of geometric error versus noise stdev

n_fig_geom_err_plot = 4;
n_fig = n_fig_geom_err_plot;

% figure(); plot(sigma_v,fv_maxcost_newalg,sigma_v,fv_maxcost_8pt);
% title 'max geometric error'
% figure(); plot(sigma_v,fv_meancost_newalg,sigma_v,fv_maxcost_8pt);
% title 'mean geometric error'

spec_newalg= 'b-';
spec_8pt = 'g--';

figure(n_fig);
plot(sigma_v,fv_maxcost_newalg,spec_newalg,sigma_v,fv_maxcost_8pt,spec_8pt);
title 'max geometric error'
xlabel('\sigma');
ylabel('epipolar distance');
legend('new','8pt');

figure(n_fig+1); 
plot(sigma_v,fv_meancost_newalg,spec_newalg,sigma_v,fv_meancost_8pt,spec_8pt);
title 'mean geometric error'
xlabel('\sigma');
ylabel('epipolar distance');
legend('new','8pt');

