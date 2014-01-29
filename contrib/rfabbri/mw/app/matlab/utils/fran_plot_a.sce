scf()
n = 200

c1 = mvn([1,4],eye(2)/4,n/2);
cplot2(c1)
a = gca()
a.isoview = 'on'
a.auto_clear = 'off'

c2 = mvn([5,1],eye(2)/4,n/2);
cplot2(c2)

c = [c1; c2]

xs2svg(gcf(), '/tmp/fran_plot_a0.svg');

// projection

cproj = (c - ones(n,1)*[3 2.5])* [4/5; -3/5]
cproj = gsort(cproj,'g','i')

scf()
plot2d(cproj,style=-9)
a = gca()
a.isoview = 'off'
//a.auto_clear = 'off'
xs2svg(gcf(), '/tmp/fran_plot_a1.svg');
