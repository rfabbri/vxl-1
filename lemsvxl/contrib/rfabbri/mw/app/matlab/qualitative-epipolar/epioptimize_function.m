%
% how_precise: what to enforce: 'precision' or 'speed' . Defaults to 'speed'
%
%
function [d,e0_best,e1_best,epi_s] = epioptimize_function(pts0,pts1,ev_ini,how_precise)

  if nargin < 4
    opt = optimset('TolFun',1e-5,'MaxFunEvals',30,'MaxIter',100,'Display','off');
  else
    opt = optimset('TolFun',1e-7,'MaxFunEvals',200,'MaxIter',100,'Display','iter');
  end

  [ev_opt,res] = lsqnonlin(@(ev) ep_f_geometric(ev,pts0,pts1),ev_ini,[],[],opt);

  e0_best = ev_opt(1:2);
  e1_best = ev_opt(3:4);

  [cost,vv,n_inst,epi_s] = epi_estimate_h_3pt_epipoles(ev_opt(1:2),ev_opt(3:4),pts0,pts1);  
  d = cost;
