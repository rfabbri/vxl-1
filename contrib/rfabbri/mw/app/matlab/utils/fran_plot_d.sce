scf()
n = 200

sig = (1/720)* [1281 638; 638 324];

cplot2(mvn([4,2],sig,n))
a = gca()
a.isoview = 'on'
a.auto_clear = 'off'
