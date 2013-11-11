function vv = ep_f_geometric_polar_sep(thetas,rhos,p0,p1)

  [x,y] = pol2cart(thetas(1),rhos(1)^2);
  epipole_0 = [x;y];
  [x,y] = pol2cart(thetas(2),rhos(2)^2);
  epipole_1 = [x;y];

  [cost,vv,n_inst,epi_s] = epi_estimate_h_3pt_epipoles(epipole_0,epipole_1,p0,p1);
