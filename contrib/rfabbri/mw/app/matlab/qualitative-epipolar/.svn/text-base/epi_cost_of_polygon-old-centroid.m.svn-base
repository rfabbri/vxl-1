% Returns the epipole positions within two given polygons in correspondence. The
% positions are chosen as the one minimizing geometric error, but only a
% discrete sampling of the polygon is used.
% 
% poly0,1: nx2 vector of points of polygon vertices
%
% pts0,1: nx2 vector of data points
% epi_s: geometric epipolar datastructure corresponding to e0_best and e1_best
%
function [d,e0_best,e1_best,epi_s] = epi_cost_of_polygon(poly0,poly1,pts0,pts1)
  
  % Centroids

  c0 = [mean(poly0(:,1)); mean(poly0(:,2))]
  c1 = [mean(poly1(:,1)); mean(poly1(:,2))] ;
  r0 = max(norm(c0),1);
  r1 = max(norm(c1),1);

  % Weighted centroids (sortof 1-step newton iteration)

  gcost = zeros(size(poly0,1));
  for i = 1:size(poly0,1)
    [cost,vv] = four_line_all_tests_geometric(poly0(i,:)',c1,pts0,pts1);
    gcost(i) = cost;
  end
  gcostn = gcost/sum(gcost);

  wc0 = [poly0(:,1)'*gcostn; poly0(:,2)'*gcostn]

  gcost = zeros(size(poly1,1));
  for i = 1:size(poly1,1)
    [cost,vv] = four_line_all_tests_geometric(c0,poly1(i,:)',pts0,pts1);
    gcost(i) = cost;
  end
  gcostn = gcost/sum(gcost);

  wc1 = [poly1(:,1)'*gcostn; poly1(:,2)'*gcostn];

  [dwc,vv,n_inst,epi_s_wc] = four_line_all_tests_geometric(wc0,wc1,pts0,pts1);
  dwc
  [dc, vv,n_inst,epi_s_c]   = four_line_all_tests_geometric(c0,c1,pts0,pts1);
  dc
  
  if dwc < dc
    disp ('Weighted centroid is better');
    d = dwc;
    epi_s = epi_s_wc;
    e0_best = wc0;
    e1_best = wc1;
  else
    disp ('Normal   centroid is better');
    d = dc;
    epi_s = epi_s_c;
    e0_best = c0;
    e1_best = c1;
  end
