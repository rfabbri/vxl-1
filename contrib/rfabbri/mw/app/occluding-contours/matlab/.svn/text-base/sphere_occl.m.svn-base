function [Gamma,Gamma_center]=sphere_occl(c,rho,s0)
% s0 - sphere center
% rho - sphere radius
% c  - camera center

  nn=s0 - c;

  if (nn(3) == 0)
    disp ('Problem, c(3) zero');
    return;
  end

  theta = 0:0.1:(2*pi);
  theta = theta';


  norm_nn = norm(nn);

  y = norm_nn - rho*rho/norm_nn;

  Gamma_center = c + y*nn/norm_nn;


  vrad_direction = [cos(theta), sin(theta), -(nn(1)*cos(theta) + nn(2)*sin(theta))/nn(3)];

  vrad_norm = sqrt(sum(vrad_direction.*vrad_direction,2));
  
  aux = rho/(norm_nn);
  x = rho*sqrt(1 - aux*aux);

  vrad = x*vrad_direction./([vrad_norm vrad_norm vrad_norm]);
  

  Gamma = ones(size(theta,1),1)*Gamma_center' + vrad;
