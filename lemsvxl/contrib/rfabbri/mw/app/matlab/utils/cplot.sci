function cplot(v,mycolor)
  // see test_cplot.sce
  [lhs, rhs] = argn(0);
  if rhs == 0 then
     error('Invalid number of arguments.');
  end

  if ~exists('mycolor','local')
     mycolor=-9 // circles o o o o 
  end

  param3d1(v(:,1), v(:,2), list(v(:,3), mycolor));
endfunction
