% Estimate epipolar homogaphy from 2 epipoles and a minimum of 3 points. Returns least
% squares residual and geometric error as well.

function [dmin,cost_v_min,n_inst_min,epi_s_min,res_min] = epi_estimate_h_3pt_epipoles_35(e0,e1,p0,p1) 

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



  % For all choices of 3 points - this is a mini-ransac to avoid degeneracy

  n_valid = 0;
  dmin = Inf;
%  n_run=0;
  for i1 = 1:npts
    for i2 = (i1+1):npts
      for i3 = (i2+1):npts
%        n_run = n_run + 1;
%        n_run
        i123 = [i1 i2 i3]';
        [h,res,retval] = epi_estimate_h_points(epi_s, p0(i123,:), p1(i123,:));
        if ~retval
          continue;
        end
        epi_s_123   = epi_s;
        epi_s_123.h = h;

        [d,retval,n_inst,cost_v] = epi_geometric_error(epi_s_123,p0,p1);

        if d < dmin
          epi_s_min = epi_s_123;
          dmin = d;
          cost_v_min = cost_v;
          n_inst_min = n_inst;
          res_min = res;
%          i123
%          mean(cost_v)
%          if d < 4*npts
            n_valid = n_valid + 1;
%            if n_valid >= 2
%              return;
%            end
%          end
        end
      end
    end
  end

  if n_valid == 0
    msg = 'All combination of points have one point coinciding with an epipole';
    disp('All combination of points have one point coinciding with an epipole');
    keyboard;
    error('All combination of points have one point coinciding with an epipole');
  end
  

