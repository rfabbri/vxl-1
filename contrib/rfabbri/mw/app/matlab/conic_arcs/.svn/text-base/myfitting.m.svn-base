% Toplevel script. Just loads a curve and perform fitting, as well as filtering
% by closest point.

mybasedir = '/home/rfabbri/lib/matlab/conic_arcs/'
crv =  myreadv2([mybasedir 'dat/curve2d-view0-tracer-tri.dat']);
%crv = perturb(crv,0.3);

[ss,endpts,cs]=fit_conics(crv,'conic',0.01,15,2);

clf;
cpts = ss;
plot_conic_arcs;
cplot2(crv,'k');
hold on;
cplot2(cpts,'.');
cplot2(cpts,'-');
axis equal;

