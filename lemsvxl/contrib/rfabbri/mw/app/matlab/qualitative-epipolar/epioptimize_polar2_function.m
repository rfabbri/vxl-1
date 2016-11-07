%
% how_precise: what to enforce: 'precision' or 'speed' . Defaults to 'speed'
%
% First optimize in theta, then in radius, then both
%
function [d,e0_best,e1_best,epi_s] = epioptimize_polar2_function(pts0,pts1,ev_ini,refpt,how_precise)

  if nargin < 4
    opt = optimset('TolFun',1e-5,'MaxFunEvals',30,'MaxIter',100,'Display','iter');
  else
    opt = optimset('TolFun',1e-7,'MaxFunEvals',200,'MaxIter',100,'Display','iter');
  end


  [theta0,rho0]= cart2pol(ev_ini(1),ev_ini(2));
  [theta1,rho1]= cart2pol(ev_ini(3),ev_ini(4));
  rho0 = sqrt(rho0); rho1 = sqrt(rho1);
  thetas_ini = [theta0;theta1];
  rhos_ini = [rho0;rho1];

  % optimize theta
  [thetas_opt1,res] = lsqnonlin(@(vth) ep_f_geometric_polar_sep(vth,rhos_ini,pts0,pts1),thetas_ini,[],[],opt);

  
  % optimize rho
  [rhos_opt1,res] = lsqnonlin(@(rth) ep_f_geometric_polar_sep(thetas_opt1,rth,pts0,pts1),rhos_ini,[],[],opt);


  % optimize both

  ev_ini = [thetas_opt1(1) rhos_opt1(1) thetas_opt1(2) rhos_opt1(2)]';
  [ev_opt,res] = lsqnonlin(@(ev) ep_f_geometric_polar(ev,pts0,pts1),ev_ini,[],[],opt);

%  ev_opt = [e0_polar e1_polar]';
  [x,y] = pol2cart(ev_opt(1),ev_opt(2)^2);
  e0_best = [x,y];
  [x,y] = pol2cart(ev_opt(3),ev_opt(4)^2);
  e1_best = [x,y];

  ev_opt = [e0_best e1_best]';

  [cost,vv,n_inst,epi_s] = epi_estimate_h_3pt_epipoles(ev_opt(1:2),ev_opt(3:4),pts0,pts1);  
  d = cost;
