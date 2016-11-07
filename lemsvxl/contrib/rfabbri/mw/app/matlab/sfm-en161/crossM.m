% this function computes the cross porduct matrix of the 3 vector 'a'.
function cpmat = crossM(a)

  cpmat = [  0   -a(3)   a(2)
           a(3)      0  -a(1)
          -a(2)   a(1)     0 ];
     