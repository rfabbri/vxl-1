function myquiver(v,crv,step)
  idx=1:step:size(v,1);
  quiver3(v(idx,1),v(idx,2),v(idx,3),crv(idx,1),crv(idx,2),crv(idx,3))
