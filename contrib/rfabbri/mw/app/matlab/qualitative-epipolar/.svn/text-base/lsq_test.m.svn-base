% Input: 
%     - epipoles e_0 and e_1 in two images
%     - p0,p1 : nx2 vectors with p0(i,:) equal to i-th point in view 0, and
%     similarly p1 for view 1.
%
% Output:
%     - Coefficients a,b,c,d of alignment hyperbola axy + bx + cy + d = 0
%     estimated by least squares
%     - The residual cost
%     - The geometric cost  (TODO put in another function - geometric cost given abcd and
%     epipoles
%
% Fri Sep 28 17:25:10 EDT 2007

function [abcd,res] = lsq_test(e0,e1,p0,p1)

  % Coefficient matrix
  %   A = [xy_i x_i y_i 1]
  %
  % Unknown vector x = [a b c d]
  %
  % Equation
  %   Ax = 0;
  %
  % Solve by least squares
  %
  %   [u,s,v]=svd(A);
  %
  %   abcd = v(:,end)
  %   
  %   c = size(A,2);
  %   res = s(c,c);
  %   if (abs(res-A*abcd) > eps*1e6)
  %      error('Residual is wrong');
  %    end
