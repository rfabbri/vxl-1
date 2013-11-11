function vv = ep_f_geometric(ev,p0,p1)
  % [cost,vv] = four_line_all_tests_geometric(ev(1:2),ev(3:4),p0,p1);
  [cost,vv,n_inst,epi_s] = epi_estimate_h_3pt_epipoles(ev(1:2),ev(3:4),p0,p1);
