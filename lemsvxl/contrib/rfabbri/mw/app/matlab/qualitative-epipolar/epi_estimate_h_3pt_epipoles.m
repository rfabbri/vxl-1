% Estimate epipolar homogaphy from 2 epipoles and a minimum of 3 points. Returns least
% squares residual and geometric error as well.

function [d,cost_v,n_inst,epi_s,res] = epi_estimate_h_3pt_epipoles(e0,e1,p0,p1) 

%  [d,cost_v,n_inst,epi_s,res] = epi_estimate_h_3pt_epipoles_35(e0,e1,p0,p1) ;

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

  el0v = zeros(npts,2);
  el1v = zeros(npts,2);
  n_valid = 0;
  valid = zeros(npts,1);
  for i=1:npts
    [el0,retval0] = epi_line(epi_s,p0(i,:)',0);
    [el1,retval1] = epi_line(epi_s,p1(i,:)',1);
    if retval0 && retval1
      n_valid = n_valid + 1;   
      el0v(n_valid,:) = el0';
      el1v(n_valid,:) = el1';
      valid(i) = 1;
    end
  end

  if n_valid < 3
    error('epipolar homography needs 3 points');
  end
  ivalid = find(valid == 1);
  if n_valid < npts
    el0v = el0v(ivalid,:);
    el1v = el1v(ivalid,:);
  end
  npts = size(el0v,1);


  % Now select pair of 3 epilines that are most far apart as possible


  dmax = 0;
  for ia=1:npts;
    for ib=(ia+1):npts;
      d1 = norm(el0v(ia,:) - el0v(ib,:)) + norm(el1v(ia,:) - el1v(ib,:));
      d2 = norm(el0v(ia,:) + el0v(ib,:)) + norm(el1v(ia,:) + el1v(ib,:));
      d = min(d1,d2);

      if d > dmax
        dmax = d;
        iaf = ia;
        ibf = ib;
      end
    end
  end

  % Select 3rd as one that maximizes distance to dmax,dmin

  dmin=Inf;
  for ic=1:npts
    if ic == iaf || ic == ibf
      continue
    end

    d1 = norm(el0v(iaf,:) - el0v(ic,:)) + norm(el1v(iaf,:) - el1v(ic,:));
    d2 = norm(el0v(iaf,:) + el0v(ic,:)) + norm(el1v(iaf,:) + el1v(ic,:));
    d_ac = min(d1,d2);

    d1 = norm(el0v(ic,:) - el0v(ibf,:)) + norm(el1v(ic,:) - el1v(ibf,:));
    d2 = norm(el0v(ic,:) + el0v(ibf,:)) + norm(el1v(ic,:) + el1v(ibf,:));
    d_bc = min(d1,d2);

    if dmin > abs(d_ac - d_bc)
      dmin = abs(d_ac - d_bc);
      icf = ic;
    end
  end


  % estimate for iaf,ibf,icf
  id_f = [iaf ibf icf];
  [h,res] = epi_estimate_h_lines(epi_s,el0v(id_f,:),el1v(id_f,:));
  epi_s.h = h;

%  figure(1);
%  epi_plot_line(epi_s,el0v(iaf,:)',0,'b');
%  epi_plot_line(epi_s,el0v(icf,:)',0,'r');
%  epi_plot_line(epi_s,el0v(ibf,:)',0,'b');
%  figure(2);
%  epi_plot_line(epi_s,el1v(iaf,:)',1,'b');
%  epi_plot_line(epi_s,el1v(icf,:)',1,'r');
%  epi_plot_line(epi_s,el1v(ibf,:)',1,'b');

  [d,retval,n_inst,cost_v] = epi_geometric_error(epi_s,p0,p1);
%  disp(cost_v);
