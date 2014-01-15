n = 200

cplot2(mvn([1,4],eye(2),n))
a = gca()
a.isoview = 'on'
a.auto_clear = 'off'

cplot2(mvn([5,1],eye(2),n))
