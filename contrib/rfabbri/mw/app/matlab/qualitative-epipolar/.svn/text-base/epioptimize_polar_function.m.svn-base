%
% how_precise: what to enforce: 'precision' or 'speed' . Defaults to 'speed'
%
%
function [d,e0_best,e1_best,epi_s] = epioptimize_polar_function(pts0,pts1,ev_ini,refpt,how_precise)

  if nargin < 4
    opt = optimset('TolFun',1e-5,'MaxFunEvals',30,'MaxIter',100,'Display','iter');
  else
    opt = optimset('TolFun',1e-7,'MaxFunEvals',200,'MaxIter',100,'Display','iter');
  end

  [theta,rho]= cart2pol(ev_ini(1),ev_ini(2));
  e0_polar = [theta, sqrt(rho)];
  [theta,rho]= cart2pol(ev_ini(3),ev_ini(4));
  e1_polar = [theta, sqrt(rho)];

  ev_ini = [e0_polar e1_polar]';
  [ev_opt,res] = lsqnonlin(@(ev) ep_f_geometric_polar(ev,pts0,pts1),ev_ini,[],[],opt);

%  ev_opt = [e0_polar e1_polar]';
  [x,y] = pol2cart(ev_opt(1),ev_opt(2)^2);
  e0_best = [x,y];
  [x,y] = pol2cart(ev_opt(3),ev_opt(4)^2);
  e1_best = [x,y];

  ev_opt = [e0_best e1_best]';

  [cost,vv,n_inst,epi_s] = epi_estimate_h_3pt_epipoles(ev_opt(1:2),ev_opt(3:4),pts0,pts1);  
  d = cost;
