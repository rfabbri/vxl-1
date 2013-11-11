% Input: 
%     - epipoles e_0 and e_1 in two images
%     - p0,p1 : nx2 vectors with p0(i,:) equal to i-th point in view 0, and
%     similarly p1 for view 1.
% Output:
%     - determinant of a constraint matrix. Det = 0 if the four points and the
%     epipoles are consistent.
%
% Tue Oct  2 09:22:45 EDT 2007


% This function attempts at normalizing the determinant in order to get a more
% stable cost
function [M,v] = four_line_test_cos(e0,e1,p0,p1) 

  
  if ( (size(p0,1) ~= 4) || (size(p1,1) ~= 4))
    error('Input is required to have exactly 4 points.');
  end

  if (size(e0,2) ~= 1)
    error('Epipoles required to be column vectors');
  end

  e0v = ones(4,1)*e0';
  dp0 = p0 - e0v;

  e1v = ones(4,1)*e1';
  dp1 = p1 - e1v;

  % Make sure no epiline is vertical

  rcos = dp0(:,1);
  rsin = dp0(:,2);
  ns = sqrt(sum(dp0.*dp0,2));
  rcos = rcos./ns;
  rsin = rsin./ns;
  rcos_bar = dp1(:,1);
  rsin_bar = dp1(:,2);
  ns = sqrt(sum(dp1.*dp1,2));
  rcos_bar = rcos_bar./ ns;
  rsin_bar = rsin_bar./ ns;

  M = [ rsin.*rsin_bar rsin.*rcos_bar rsin_bar.*rcos rcos.*rcos_bar];

  v = abs(det(M));
