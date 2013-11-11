% Returns the epipole positions within two given polygons in correspondence. The
% positions are chosen as the one minimizing geometric error, but only a
% discrete sampling of the polygon is used, or gradient descent.
% 
% poly0,1: nx2 vector of points of polygon vertices
%
% pts0,1: nx2 vector of data points
% epi_s: geometric epipolar datastructure corresponding to e0_best and e1_best
% e0_v and e1_v : vectors of all evaluated epipolar positions
function [d,e0_best,e1_best,epi_s] = epi_cost_of_polygon(poly0,poly1,pts0,pts1)
  % Centroids

  c0 = [mean(poly0(:,1)); mean(poly0(:,2))];
  c1 = [mean(poly1(:,1)); mean(poly1(:,2))] ;

  ev_ini = [c0; c1];

  [d,e0_best,e1_best,epi_s] = epioptimize_function(pts0,pts1,ev_ini);

