% Input: 
%     - epipoles e_0 and e_1 in two images
%     - p0,p1 : nx2 vectors with p0(i,:) equal to i-th point in view 0, and
%     similarly p1 for view 1.
% Output:
%     - Vector of absolute value of determinants of a constraint matrix and its
%     vector norm. The norm should be 0 if all points are perfectly consistent with the epipoles.
%
% Mon Sep 24 12:29:12 EDT 2007


%function [vv,cost,rank_vv] = four_line_all_tests(e0,e1,p0,p1) 
function [cost,vv] = four_line_all_tests(e0,e1,p0,p1) 

  
  if ( (size(p0,1) < 4) || (size(p1,1) < 4))
    error('Input is required to have at least 4 points.');
  end

  if (size(e0,2) ~= 1)
    error('Epipoles required to be column vectors');
  end

  ctroid0 = [mean(p0(:,1)) mean(p0(:,2))]';
  ctroid1 = [mean(p1(:,1)) mean(p1(:,2))]';

  vv = zeros(size(p0,1)-3,1);
%  rank_vv = zeros(size(p0,1)-3,1);
  for i=1:size(p0,1)-3
    [m,v] = four_line_test_cos_radius(e0,e1,p0(i:i+3,:),p1(i:i+3,:),ctroid0,ctroid1);
    vv(i) = v;
    if (v == Inf)
      cost = Inf;
      return;
    end
%    rank_vv(i) = rank(m);
  end

  vv_valid = vv(vv~=-1);

  if (min(size(vv_valid)) == 0)
    cost = Inf;
    return;
  end
  cost = sum(vv_valid.*vv_valid);
