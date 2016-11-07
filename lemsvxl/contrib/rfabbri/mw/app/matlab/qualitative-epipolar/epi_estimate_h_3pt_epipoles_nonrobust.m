% Estimate epipolar homogaphy from 2 epipoles and a minimum of 3 points. Returns least
% squares residual and geometric error as well.

function [cost,cost_v,n_inst,epi_s,res] = epi_estimate_h_3pt_epipoles(e0,e1,p0,p1) 

  if ( (size(p0,1) < 3) || (size(p1,1) < 3))
    error('Input is required to have at least 3 points.');
  end

  if (size(e0,2) ~= 1)
    error('Epipoles required to be column vectors');
  end

  % Geometric cost is totally insensitive to reference point.
%  r0 = max(norm(e0-ctroid0'),1);
%  r1 = max(norm(e1-ctroid1'),1);
  r0 = max(norm(e0),1);
  r1 = max(norm(e1),1);

  epi_s = epi_init(e0,e1,r0,r1,zeros(2,2));


  npts = size(p0,1);


  [h,res] = epi_estimate_h_points(epi_s, p0, p1);
  epi_s.h = h;

  [d,retval,n_inst,cost_v] = epi_geometric_error(epi_s,p0,p1);
  cost = d;
