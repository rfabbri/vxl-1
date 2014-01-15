scf();
a=gca();
a.clip_state='off'


// ellipse gaussian
n = 1000

sig = [1 0 ;0 0.2]
t = %pi/6
rot = [cos(t) -sin(t); sin(t) cos(t)]
sig = rot*sig*rot';

cplot2(mvn([8,9],sig,n))

a = gca()
a.isoview = 'on'
a.auto_clear = 'off'
a.data_bounds = [-3 -6; 15 13];
a.tight_limits='on';

// now line
n = 100
x = -0.5 + 3*rand(n,1)
y = 2*x + 0.2*rand(n,1,'gaussian') + 3;

c = [x y]
cplot2(c)

// now spiral

n = 40
x0 = 4
y0 = -1
eps = 0.1*rand(n,1,'gaussian');
theta = linspace(%pi/2,4*%pi,n);
theta = theta';
r=0.3*theta.^0.8;

xx=(r + eps).*cos(theta)+x0*ones(eps);
yy=(r + eps).*sin(theta)+y0*ones(eps);

c = [xx yy];
c1 = c;

//cplot2(c);

xx=(r + eps).*cos(theta)+x0*ones(eps);
yy=(r + eps).*sin(theta)+y0*ones(eps);

c = [xx yy];

c = -c + [2*x0*ones(n,1) 2*y0*ones(n,1)]

c = c + [4.3*ones(n,1) 0*ones(n,1)]
c2 = c;
//cplot2(c);

x0 = 5.8
y0 = 3

xx=(0.9*r + eps).*cos(theta)+x0*ones(eps);
yy=(r + eps).*sin(theta)+y0*ones(eps);

c = [xx yy];
c3 = c;

//cplot2(c);


t = -%pi/6
rot = [cos(t) -sin(t); sin(t) cos(t)]
cplot2([c1;c2;c3]*rot' + [2*ones(3*n,1) 4*ones(3*n,1)])
