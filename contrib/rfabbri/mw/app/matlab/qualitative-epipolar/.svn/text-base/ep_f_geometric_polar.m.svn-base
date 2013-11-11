function vv = ep_f_geometric_polar(ev_polar,p0,p1)
  % [cost,vv] = four_line_all_tests_geometric(ev(1:2),ev(3:4),p0,p1);
  % Transform back to cartesian coordinates

  % theta,rho
  [x,y] = pol2cart(ev_polar(1),ev_polar(2)^2);
  epipole_0 = [x;y];
  [x,y] = pol2cart(ev_polar(3),ev_polar(4)^2);
  epipole_1 = [x;y];

  [cost,vv,n_inst,epi_s] = epi_estimate_h_3pt_epipoles(epipole_0,epipole_1,p0,p1);
