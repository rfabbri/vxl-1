% max of the minimal distance between two 3d point sets
% c1: nx3  c2: nx3
function [h,dv12,dv21]=hdist(c1,c2)
  [hd12,dv12] = dir_hausdorff_distance(c1,c2);
  [hd21,dv21] = dir_hausdorff_distance(c2,c1);
  h = max(hd12,hd21);
