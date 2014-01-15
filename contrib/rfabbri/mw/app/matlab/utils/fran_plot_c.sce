scf()
n = 200

x = 2 + 4*rand(n,1)
y = 0.5*x + 0.1*rand(n,1,'gaussian');
a = gca()
a.isoview = 'on'
a.auto_clear = 'off'

c = [x y]
cplot2(c)

plot2d([0; 8], [0; 4],2);
