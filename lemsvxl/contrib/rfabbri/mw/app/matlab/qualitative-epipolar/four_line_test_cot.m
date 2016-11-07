% Same as four_line_test but uses cotangent parametrization of epipolar lines as
% priority, then switches to tangent if necessary.
%
% Input: 
%     - epipoles e_0 and e_1 in two images
%     - p0,p1 : nx2 vectors with p0(i,:) equal to i-th point in view 0, and
%     similarly p1 for view 1.
% Output:
%     - determinant of a constraint matrix. Det = 0 if the four points and the
%     epipoles are consistent.
%
% Mon Sep 24 12:29:12 EDT 2007


function [M,v] = four_line_test_cot(e0,e1,p0,p1) 

  
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

  % Make sure no epiline is horizontal

  % cotangent parametrization
  x_v = dp0(:,1)./dp0(:,2);
  y_v = dp1(:,1)./dp1(:,2);

  M = [ x_v.*y_v x_v y_v ones(4,1) ];

  if sum( abs(dp0(:,2)) < dp0(:,1)*0.01) || sum( abs(dp1(:,2)) < dp1(:,2)*0.01 )
    if sum( abs(dp0(:,1)) < 1000000*eps ) || sum( abs(dp1(:,1)) < 1000000*eps )
      warning('Vertical and horizontal lines - putative epipole is probably one of 4 points');
      v = -1;
      return;
    else 
    warning('Some epipolar lines are horizontal - using tangents');
      % Here we can use tangent parametrization
      
      x_v = dp0(:,2)./dp0(:,1);
      y_v = dp1(:,2)./dp1(:,1);

      M = [ x_v.*y_v x_v y_v ones(4,1) ];
      v = abs(det(M));
      return;

    end
%    error('Testing');
 
    v = Inf;
    return;
    % todo: handle vertical case with, perhaps, cotangent
  end
  v = abs(det(M));
