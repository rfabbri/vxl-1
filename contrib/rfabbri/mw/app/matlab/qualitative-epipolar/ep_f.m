function vv = ep_f(ev,p0,p1)
  [cost,vv] = four_line_all_tests(ev(1:2),ev(3:4),p0,p1);

  vv(vv==-1) = Inf;
