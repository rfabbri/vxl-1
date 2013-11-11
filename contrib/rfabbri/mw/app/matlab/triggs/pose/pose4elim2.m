% Pose of a calibrated camera from known 3D points. Lan-Quan version of
% elimination method --- runs entire elimination for each depth
% separately.

function u = pose4elim2(C,D)
   n = size(C,1);
   u = zeros(1,n);
   for i = 1:n
      perm = [i:n,1:i-1];
      u1 = pose4elim(C(perm,perm),D(perm,perm));
      if isempty(u1), u = zeros(0,n); return; end;
      u(i) = u1(1);
   end;
%end;
