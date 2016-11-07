function h=myquiver2(v,crv,step)
  idx=1:step:size(v,1);
  h=quiver(v(idx,1),v(idx,2),crv(idx,1),crv(idx,2))
