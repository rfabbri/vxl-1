% Plot curves of geometric error versus noise stdev

n_fig_npts_exp_plot = 44;
n_fig = n_fig_npts_exp_plot;

% figure(); plot(sigma_v,fv_maxcost_newalg,sigma_v,fv_maxcost_8pt);
% title 'max geometric error'
% figure(); plot(sigma_v,fv_meancost_newalg,sigma_v,fv_maxcost_8pt);
% title 'mean geometric error'

spec_newalg= 'b-';
spec_8pt = 'g--';

figure(n_fig);
x_pts = 8:max_npts;
plot(x_pts,fv_maxcost_newalg,spec_newalg,x_pts,fv_maxcost_8pt,spec_8pt);
title 'max geometric error'
xlabel('number of points');
ylabel('epipolar distance');
legend('new','8pt');

figure(n_fig+1); 
plot(x_pts,fv_meancost_newalg,spec_newalg,x_pts,fv_meancost_8pt,spec_8pt);
title 'mean geometric error'
xlabel('number of points');
ylabel('epipolar distance');
legend('new','8pt');

