% Input: 
%     - epipoles e_0 and e_1 in two images
%     - p0,p1 : nx2 vectors with p0(i,:) equal to i-th point in view 0, and
%     similarly p1 for view 1.
% Output:
%     - Geometric cost - should be 0 if all points are perfectly consistent with the epipoles.
%       The geometric cost is just sum-of-squares over all points, without
%       normalization.
%     - n_inst - number of points for which cost was ignored
%     - epi_s
%


function [cost,vv,n_inst,epi_s] = four_line_all_tests_geometric(e0,e1,p0,p1) 


%  ctroid0 = [mean(p0(:,1)) mean(p0(:,2))]';
%  ctroid1 = [mean(p1(:,1)) mean(p1(:,2))]';


  % For all permutations i of 4 points on the set of p0 and p1
  %   - solve for homography
  %   - compute geometric distance of all the other points
  %   -   vv(i) = max of this distance.



% PERMUTATION APPROACH

%  cost = Inf;
%  ids1 = 1:npts;
%  for i1 = ids1
%    ids2 = ids1(ids1 > i1);
%    for i2 = ids2
%      ids3 = ids2(ids2 > i2);
%      for i3 = ids3
%        ids4 = ids3(ids3 > i3); 
%        for i4 = ids4
%          i4pt = [i1 i2 i3 i4];
%          [h,res] = epi_estimate_h_points(epi, p0(i4pt,:), p1(i4pt,:));
%          epi.h = h;

%          ipts = ids1(ids1 ~= i1 & ids1 ~= i2 & ids1 ~= i3 & ids1 ~= i4);

%          [d,retval] = epi_geometric_error(epi,p0(ipts,:),p1(ipts,:));
%          cost = min(d,cost);
%        end
%      end
%    end
%  end

[cost,vv,n_inst,epi_s] = epi_estimate_h_3pt_epipoles(e0,e1,p0,p1);


