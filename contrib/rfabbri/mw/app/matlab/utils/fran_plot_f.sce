scf();
a=gca();
a.clip_state='off'
n = 200
r = 1/2
x0 = 2
y0 = 1
eps = 0.01*rand(n,1,'gaussian');
theta = linspace(0,2*%pi,n);
theta = theta';

xx=(r*ones(eps) + eps).*cos(theta)+x0*ones(eps);
yy=(r*ones(eps) + eps).*sin(theta)+y0*ones(eps);

c = [xx yy];

cplot2(c);
a = gca();
a.isoview = 'on';
a.auto_clear = 'off';
a.data_bounds = [1.4 0.4; 2.6 1.6];
a.tight_limits='on';
[fc,cc] = circle_any_theta(theta, r, x0, y0)
//plot2d(cc(:,1),cc(:,2),2);
xs2svg(gcf(), '/tmp/fran_plot_f.svg');
