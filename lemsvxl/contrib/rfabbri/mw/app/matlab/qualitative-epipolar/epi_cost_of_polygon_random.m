% Returns the epipole positions within two given polygons in correspondence. The
% positions are chosen as the one minimizing geometric error, but only a
% discrete sampling of the polygon is used, or gradient descent.
% 
% poly0,1: nx2 vector of points of polygon vertices
%
% pts0,1: nx2 vector of data points
% epi_s: geometric epipolar datastructure corresponding to e0_best and e1_best
% e0_v and e1_v : vectors of all evaluated epipolar positions
%
% dr: 1xn vector. Let i1 index pts1 and i0 index pts0. Then dr(i1) = min cost
% over all matches i0 for pts1(i1,:). 
% ir: 1xn vector.   ir(i1) = i0 st cost is minimum among all i0.
%
function [d,e0_best,e1_best,epi_s,e0_v,e1_v,dr,ir] = epi_cost_of_polygon_random(poly0,poly1,pts0,pts1)
  
  % Centroids

  c0 = [mean(poly0(:,1)); mean(poly0(:,2))];
  c1 = [mean(poly1(:,1)); mean(poly1(:,2))] ;



  rand_pts0 = random_polygon_points(poly0,50);
  rand_pts1 = random_polygon_points(poly1,50);


  e0_v = [c0'; poly0; rand_pts0];
  e1_v = [c1'; poly1; rand_pts1];

  

  r0 = max(norm(c0),1);
  r1 = max(norm(c1),1);



  nep_pts0 = size(e0_v,1);
  nep_pts1 = size(e1_v,1);
  gcost = zeros(nep_pts0,nep_pts1);
  epi_s_cell = cell(nep_pts0,nep_pts1);
  for i = 1:nep_pts0
    for k=1:nep_pts1
%      [cost,vv,n_inst,epi_s] = four_line_all_tests_geometric(e0_v(i,:)',e1_v(k,:)',pts0,pts1);
      [cost,vv,n_inst,epi_s] = epi_estimate_h_3pt_epipoles(e0_v(i,:)',e1_v(k,:)',pts0,pts1);
      gcost(i,k) = cost;
      epi_s_cell{i}{k} = epi_s;
      if i==1 && k == 1
        cost_ctroid = cost;
      else
%        if cost < cost_ctroid
%          disp ('New point is better');
%        end
      end
    end
  end

  [dr,ir] = min(gcost,[],1);
  [dmin,ic] = min(dr);
  ir = ir(ic);

  if gcost(ir,ic) ~= min(gcost(:))
    error('my assertion failed');
  end

%  [dmin,vv,n_inst,epi_s] = four_line_all_tests_geometric(e0_v(1,:)',e1_v(1,:)',pts0,pts1);

%  ir = 1; ic = 1;
  e0_best = e0_v(ir,:)';
  e1_best = e1_v(ic,:)';
  epi_s = epi_s_cell{ir}{ic};
  d = dmin;
