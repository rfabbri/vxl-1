% max of the minimal distance between two 3d point sets
% c1: nx3  c2: nx3
% dv: vector of minimum distances of each of c1's point to c2
function [d,dv]=dir_hausdorff_distance(c1,c2)

dv = zeros(size(c1,1),1);

d = 0;
for i=1:size(c1,1) 
  dmin = inf;
  p = c1(i,:);
  for k=1:size(c2,1)
    d_tmp = norm(p-c2(k,:));
    if (dmin > d_tmp)
      dmin = d_tmp;
    end
  end
  dv(i) = dmin;

  if (dmin > d)
    d = dmin;
  end
end

