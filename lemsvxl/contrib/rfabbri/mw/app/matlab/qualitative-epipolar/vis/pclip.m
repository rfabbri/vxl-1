%  To be used after plot_polys.
% clips the images to the image domains

figure(n_fig);
mypad = 50;
axis([0 - mypad; im0_xmax+ mypad; 0 - mypad ;im0_ymax+ mypad]);

figure(n_fig+1);
axis([0 - mypad; im1_xmax+ mypad; 0 - mypad ;im1_ymax+ mypad]);
