% Input: 
%     - polygons poly0 and poly1 : regions where corresponding epipoles might
%     be. These are given by a list of vertices of the outline.
%
%     - p0,p1 : nx2 vectors with p0(i,:) equal to i-th point in view 0, and
%     similarly p1 for view 1.
%
% Output:
%     - cost : a value that is higher for less likely polygons, lower for more
%     consistent polygons.
%
% Mon Sep 24 12:29:12 EDT 2007


function [cost] = four_line_poly_test(poly0,poly1,p0,p1) 

  
  c0 = [mean(poly0(:,1)); mean(poly0(:,2))];
  c1 = [mean(poly1(:,1)); mean(poly1(:,2))]; 
  [cost] = four_line_all_tests_geometric(c0,c1,p0,p1);
