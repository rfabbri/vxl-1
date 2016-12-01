scf()
a=gca()
a.clip_state='off'
n = 200

[fc,c] = circle_any_theta(2*%pi*rand(n,1), 0.5, 2, 1)

cplot2(c)
a = gca();
a.isoview = 'on'
a.auto_clear = 'off'
a.data_bounds = [1.4 0.4; 2.6 1.6]
a.tight_limits='on'

theta = linspace(0,2*%pi,n);
theta = theta';
[fc,cc] = circle_any_theta(theta, 0.5, 2, 1)
//plot2d(cc(:,1),cc(:,2),2);


xs2svg(gcf(), '/tmp/fran_plot_e.svg');
