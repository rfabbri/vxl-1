% Input: 
%     - epi: epipoles e_0 and e_1 in two images, and radius r of reference
%     circle. The member h of the epipole will not be used (it is being
%     estimated).
%
%     - p0,p1 : nx2 vectors with p0(i,:) equal to i-th point in view 0, and
%     similarly p1 for view 1.
%
% Output:
%     - h = Coefficients a,b,c,d of alignment hyperbola axy + bx + cy + d = 0
%     estimated by least squares following the convention:
%              h = [ d b
%                    c a ];
%       where the equation can be written as
%              [yc ys]' * h * [xc xs] = 0
%       Where yc,ys and xc,xs are epipolar lines in our Euclidean formulation.
%
%     - The algebraic residual cost
%  returns false if too many points and epipole coincides
%
% Fri Sep 28 17:25:10 EDT 2007

function [h,res,retval] = epi_estimate_h_points(epi,p0,p1)

  % 1) For each point, build vector of epipolar lines el0, el1 in Euclidean-geometric
  % representation

  retval = true;
  npts = size(p0,1);

%  e0v = ones(npts,1)*epi.e(1,:);
%  e1v = ones(npts,1)*epi.e(2,:);

%  el0v = p0 - e0v;
%  ns = sqrt(sum(el0v.*el0v,2));
%  ns = epi.r(1)./ns;
%  el0v = el0v .* [ns ns];

%  el1v = p1 - e1v;
%  ns = sqrt(sum(el1v.*el1v,2));
%  ns = epi.r(2)./ns;
%  el1v = el1v .* [ns ns];

  el0v = zeros(npts,2);
  el1v = zeros(npts,2);
  n_valid = 0;
  valid = zeros(npts,1);
  for i=1:npts
    [el0,retval0] = epi_line(epi,p0(i,:)',0);
    [el1,retval1] = epi_line(epi,p1(i,:)',1);
    if retval0 && retval1
      n_valid = n_valid + 1;   
      el0v(n_valid,:) = el0';
      el1v(n_valid,:) = el1';
      valid(i) = 1;
    end
  end

  if n_valid < 3
    h=[];
    res = NaN;
    retval = false;
    return;
%    error('epipolar homography needs 3 points');
  end
  ivalid = find(valid == 1);
  if n_valid < npts
    el0v = el0v(ivalid,:);
    el1v = el1v(ivalid,:);
  end

  % 2) Build a coefficient matrix

%  figure(1001); hold on;
  % xxx debug
%  for i=1:npts
%    epi_plot_line(epi,el1v(i,:)',1);
%  end
%  hold off;

  [h,res] = epi_estimate_h_lines(epi,el0v,el1v);

