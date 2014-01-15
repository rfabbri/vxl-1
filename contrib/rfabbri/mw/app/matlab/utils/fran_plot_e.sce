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
