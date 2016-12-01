scf()
n = 200

sig = (1/720)* [1281 638; 638 324];

cplot2(mvn([4,2],sig,n))
a = gca()
a.isoview = 'on'
a.auto_clear = 'off'
plot2d([0; 8], [0; 4],2);
xs2svg(gcf(), '/tmp/fran_plot_d.svg');
