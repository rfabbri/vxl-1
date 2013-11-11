% Input: 
%     - epi: epipoles e_0 and e_1 in two images, and radius r of reference
%     circle. The member h of the epipole will not be used (it is being
%     estimated).
%
%     - el0,el1: nx2 vectors with el0(i,:) equal to i-th epipolar line in view 0, and
%     similarly el1 for view 1.
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
%
function [h,res] = epi_estimate_h_lines(epi,el0,el1)

  % Build a coefficient matrix

  if (size(el0,1) < 3)
    error('cannot compute epipolar homography for less than 3 lines');
  end

  xc = el0(:,1);
  xs = el0(:,2);

  yc = el1(:,1);
  ys = el1(:,2);

  A = [xc.*yc, xs.*yc, xc.*ys, xs.*ys];

  % Solve by least squares
  [u,s,v]=svd(A);

  h_v = v(:,end);

  h = [ h_v(1) h_v(2);
        h_v(3) h_v(4) ];

  c = size(A,2);

  if size(el0,1) > 3
    res = s(c,c);
  else
    res = 0;
  end
%  if (norm(A*h_v) > 10 || res > 10)
%     warning('Residual is very high');
%  end

  if (abs(res-norm(A*h_v)) > 0.01)
     error('Residual is wrong');
  end

